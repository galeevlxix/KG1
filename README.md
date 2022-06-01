# Урок 22 - Загрузка моделей через Assimp
## Создание класса Mesh
```c++
class Mesh
{
public:
    Mesh();
 
    ~Mesh();
 
    bool LoadMesh(const std::string& Filename);
 
    void Render();
 
private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();
 
#define INVALID_MATERIAL 0xFFFFFFFF
 
    struct MeshEntry {
        MeshEntry();
 
        ~MeshEntry();
 
        bool Init(const std::vector<Vertex>& Vertices,
        const std::vector<unsigned int>& Indices);
 
        GLuint VB;
        GLuint IB;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };
 
    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
};
```
## Функция LoadMesh - начальная точка загрузки меша
```c++
bool Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
 
    bool Ret = false;
    Assimp::Importer Importer;
 
    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
 
    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }
 
    return Ret;
}
```
## Функция InitFromScene
```c++
bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
 
    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }
 
    return InitMaterials(pScene, Filename);
}
```
## Инициализация меша в функции InitMesh 
```c++
void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        const aiVector3D* pTangent = &(paiMesh->mTangents[i]);

        Vertex v(Vector3f(pPos->x, pPos->y, pPos->z),
            vec2(pTexCoord->x, pTexCoord->y),
            Vector3f(pNormal->x, pNormal->y, pNormal->z),
            Vector3f(pTangent->x, pTangent->y, pTangent->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}
```
## Функция InitMaterials загружает все текстуры, которые используются в модели
```c++
bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }
    }

    return Ret;
}
```
## Эта функция Render инкапсулирует рендер меша и выделяет его из центра приложения
```c++
void Mesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
 
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);
 
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
 
        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }
 
        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }
 
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
```
# Урок 23 - 24 - Карта теней 
## Создание класса ShadowMapFBO
```c++ 
class ShadowMapFBO
{
public:
    ShadowMapFBO();

    ~ShadowMapFBO();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading(GLenum TextureUnit);

private:
    GLuint m_fbo;
    GLuint m_shadowMap;
};
```
## Инициализация класса в функции Init
```c++
bool ShadowMapFBO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Создаем FBO
    glGenFramebuffers(1, &m_fbo);

    // Создаем буфер глубины
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        m_shadowMap, 0);

    // Отключаем запись в буфер цвета
    glDrawBuffer(GL_NONE);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    return true;
}
```
## Функции BindForWriting и BindForReading, чтобы переключаться между рендером в карту теней и в стандартный буфер кадра.
```c++
void ShadowMapFBO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void ShadowMapFBO::BindForReading(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}
```
## Создание класса ShadowMapTechnique
```c++
class ShadowMapTechnique : public Technique {

public:
    ShadowMapTechnique();
    virtual bool Init();
    void SetWVP(const Matrix4f& WVP);
    void SetTextureUnit(unsigned int TextureUnit);

private:

    GLuint m_WVPLocation;
    GLuint m_textureLocation;
};
```
## Вершинный шейдер теней
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec2 TexCoordOut;                                                               \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoordOut = TexCoord;                                                         \n\
}
```
## Фрагментный шейдер теней
```c++
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoordOut;                                                                \n\
uniform sampler2D gShadowMap;                                                       \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    float Depth = texture(gShadowMap, TexCoordOut).x;                               \n\
    Depth = 1.0 - (1.0 - Depth) * 25.0;                                             \n\
    FragColor = vec4(Depth);                                                        \n\
}
```
## Инициализация класса
```c++
bool ShadowMapTechnique::Init()
{
    if (!Technique::Init()) {
        return false;
    }

    if (!AddShader(GL_VERTEX_SHADER, pVS)) {
        return false;
    }

    if (!AddShader(GL_FRAGMENT_SHADER, pFS)) {
        return false;
    }

    if (!Finalize()) {
        return false;
    }

    m_WVPLocation = GetUniformLocation("gWVP");
    m_textureLocation = GetUniformLocation("gShadowMap");

    if (m_WVPLocation == INVALID_UNIFORM_LOCATION ||
        m_textureLocation == INVALID_UNIFORM_LOCATION) {
        return false;
    }

    return true;
}
```
## Рефакторинг цикла рендера
```c++
virtual void RenderSceneCB()
{
    m_pGameCamera->OnRender();
    m_scale += 0.05f;
 
    ShadowMapPass();
    RenderPass();
 
    glutSwapBuffers();
}
```
## Функция ShadowMapPass
```c++
virtual void ShadowMapPass()
{
    m_shadowMapFBO.BindForWriting();
 
    glClear(GL_DEPTH_BUFFER_BIT);
 
    Pipeline p;
    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, m_scale, 0.0f);
    p.WorldPos(0.0f, 0.0f, 5.0f);
    p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
    p.SetPerspectiveProj(20.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
    m_pShadowMapTech->SetWVP(p.GetWVPTrans());
 
    m_pMesh->Render();
 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
```
## Функция RenderPass
```c++
virtual void RenderPass()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    m_pShadowMapTech->SetTextureUnit(0);
    m_shadowMapFBO.BindForReading(GL_TEXTURE0);
 
    Pipeline p;
    p.Scale(5.0f, 5.0f, 5.0f);
    p.WorldPos(0.0f, 0.0f, 10.0f);
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
    m_pShadowMapTech->SetWVP(p.GetWVPTrans());
    m_pQuad->Render();
}
```
## Классу света требуется набор новых свойств. Матрица WVP, которая вычисляется из позиции источника света, и модуль текстуры для карты теней
```c++
void SetLightWVP(const Matrix4f& LightWVP);
void SetShadowMapTextureUnit(unsigned int TextureUnit);

GLuint m_LightWVPLocation;
GLuint m_shadowMapLocation;
```
## Обновленный векторный шейдер класса LightingTechnique
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
layout (location = 3) in vec3 Tangent;                                              \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gLightWVP;                                                             \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec4 LightSpacePos;                                                             \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 WorldPos0;                                                                 \n\
out vec3 Tangent0;                                                                  \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position   = gWVP * vec4(Position, 1.0);                                     \n\
    LightSpacePos = gLightWVP * vec4(Position, 1.0);                                \n\
    TexCoord0     = TexCoord;                                                       \n\
    Normal0       = (gWorld * vec4(Normal, 0.0)).xyz;                               \n\
    Tangent0      = (gWorld * vec4(Tangent, 0.0)).xyz;                              \n\
    WorldPos0     = (gWorld * vec4(Position, 1.0)).xyz;                             \n\
}
```
## Обновленный фрагментный шейдер класса LightingTechnique
```c++
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
const int MAX_SPOT_LIGHTS = 3;                                                      \n\
                                                                                    \n\
in vec4 LightSpacePos;                                                              \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
in vec3 Tangent0;                                                                   \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct BaseLight                                                                    \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    BaseLight Base;                                                                 \n\
    vec3 Direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
struct Attenuation                                                                  \n\
{                                                                                   \n\
    float Constant;                                                                 \n\
    float Linear;                                                                   \n\
    float Exp;                                                                      \n\
};                                                                                  \n\
                                                                                    \n\
struct PointLight                                                                           \n\
{                                                                                           \n\
    BaseLight Base;                                                                         \n\
    vec3 Position;                                                                          \n\
    Attenuation Atten;                                                                      \n\
};                                                                                          \n\
                                                                                            \n\
struct SpotLight                                                                            \n\
{                                                                                           \n\
    PointLight Base;                                                                        \n\
    vec3 Direction;                                                                         \n\
    float Cutoff;                                                                           \n\
};                                                                                          \n\
                                                                                            \n\
uniform int gNumPointLights;                                                                \n\
uniform int gNumSpotLights;                                                                 \n\
uniform DirectionalLight gDirectionalLight;                                                 \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          \n\
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                             \n\
uniform sampler2D gColorMap;                                                                \n\
uniform sampler2D gShadowMap;                                                               \n\
uniform sampler2D gNormalMap;                                                               \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
                                                                                            \n\
float CalcShadowFactor(vec4 LightSpacePos)                                                  \n\
{                                                                                           \n\
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  \n\
    vec2 UVCoords;                                                                          \n\
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;                                                  \n\
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;                                                  \n\
    float Depth = texture(gShadowMap, UVCoords).x;                                          \n\
    if (Depth <= (ProjCoords.z + 0.005))                                                    \n\
        return 0.5;                                                                         \n\
    else                                                                                    \n\
        return 1.0;                                                                         \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal,                   \n\
                       float ShadowFactor)                                                  \n\
{                                                                                           \n\
    vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;                   \n\
    float DiffuseFactor = dot(Normal, -LightDirection);                                     \n\
                                                                                            \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  \n\
                                                                                            \n\
    if (DiffuseFactor > 0) {                                                                \n\
        DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;    \n\
                                                                                            \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             \n\
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     \n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                              \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                               \n\
        if (SpecularFactor > 0) {                                                           \n\
            SpecularColor = vec4(Light.Color, 1.0f) *                                       \n\
                            gMatSpecularIntensity * SpecularFactor;                         \n\
        }                                                                                   \n\
    }                                                                                       \n\
                                                                                            \n\
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));                  \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                      \n\
{                                                                                                \n\
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal, 1.0);  \n\
}                                                                                                \n\
                                                                                            \n\
vec4 CalcPointLight(PointLight l, vec3 Normal, vec4 LightSpacePos)                          \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - l.Position;                                           \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
    float ShadowFactor = CalcShadowFactor(LightSpacePos);                                   \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal, ShadowFactor);           \n\
    float Attenuation =  l.Atten.Constant +                                                 \n\
                         l.Atten.Linear * Distance +                                        \n\
                         l.Atten.Exp * Distance * Distance;                                 \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcSpotLight(SpotLight l, vec3 Normal, vec4 LightSpacePos)                            \n\
{                                                                                           \n\
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);                             \n\
    float SpotFactor = dot(LightToPixel, l.Direction);                                      \n\
                                                                                            \n\
    if (SpotFactor > l.Cutoff) {                                                            \n\
        vec4 Color = CalcPointLight(l.Base, Normal, LightSpacePos);                         \n\
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   \n\
    }                                                                                       \n\
    else {                                                                                  \n\
        return vec4(0,0,0,0);                                                               \n\
    }                                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
vec3 CalcBumpedNormal()                                                                     \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec3 Tangent = normalize(Tangent0);                                                     \n\
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);                           \n\
    vec3 Bitangent = cross(Tangent, Normal);                                                \n\
    vec3 BumpMapNormal = texture(gNormalMap, TexCoord0).xyz;                                \n\
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);                              \n\
    vec3 NewNormal;                                                                         \n\
    mat3 TBN = mat3(Tangent, Bitangent, Normal);                                            \n\
    NewNormal = TBN * BumpMapNormal;                                                        \n\
    NewNormal = normalize(NewNormal);                                                       \n\
    return NewNormal;                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = CalcBumpedNormal();                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(gPointLights[i], Normal, LightSpacePos);               \n\
    }                                                                                       \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumSpotLights ; i++) {                                            \n\
        TotalLight += CalcSpotLight(gSpotLights[i], Normal, LightSpacePos);                 \n\
    }                                                                                       \n\
                                                                                            \n\
    vec4 SampledColor = texture2D(gColorMap, TexCoord0.xy);                                 \n\
    FragColor = SampledColor * TotalLight;                                                  \n\
}
```
# Урок 25 - Скайбокс
## Класс CubemapTexture включает в себя реализацию кубической текстуры
```c++
class CubemapTexture
{
public:
 
    CubemapTexture(const string& Directory,
        const string& PosXFilename,
        const string& NegXFilename,
        const string& PosYFilename,
        const string& NegYFilename,
        const string& PosZFilename,
        const string& NegZFilename);
 
    ~CubemapTexture();
 
    bool Load();
 
    void Bind(GLenum TextureUnit);
 
private:
 
    string m_fileNames[6];
    GLuint m_textureObj;
};
```
## Функция CubemapTexture, которая загружает текстуру, начинается с генерации объекта текстуры
```c++
bool CubemapTexture::Load()
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
 
    Magick::Image* pImage = NULL;
    Magick::Blob blob;
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(types) ; i++) {
        pImage = new Magick::Image(m_fileNames[i]);
 
        try {
            pImage->write(&blob, "RGBA");
        }
        catch (Magick::Error& Error) {
            cout << "Error loading texture '" << m_fileNames[i] << "': " << Error.what() << endl;
            delete pImage;
            return false;
        }
 
        glTexImage2D(types[i], 0, GL_RGB, pImage->columns(), pImage->rows(), 0, GL_RGBA,
            GL_UNSIGNED_BYTE, blob.data());
 
        delete pImage;
    }
 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
 
    return true;
}
```
## Эта функция Bind должна быть вызвана до того, как мы будем использовать текстуру для рисования скайбокса
```c++
void CubemapTexture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}
```
## Создание класса SkyboxTechnique
```c++
class SkyboxTechnique : public Technique {
public:
 
    SkyboxTechnique();
 
    virtual bool Init();
 
    void SetWVP(const Matrix4f& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
 
private:
 
    GLuint m_WVPLocation;
    GLuint m_textureLocation;
};
```
## Это вершинный шейдер для скайбокса
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec3 TexCoord0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 WVP_Pos = gWVP * vec4(Position, 1.0);                                      \n\
    gl_Position = WVP_Pos.xyww;                                                     \n\
    TexCoord0   = Position;                                                         \n\
}
```
## Это фрагментный шейдер для скайбокса
```c++
#version 330                                                                        \n\
                                                                                    \n\
in vec3 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform samplerCube gCubemapTexture;                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture(gCubemapTexture, TexCoord0);                                \n\
}
```
## Создание класса скайбокса
```c++
class SkyBox
{
public:
    SkyBox(const Camera* pCamera, const PersProjInfo& p);

    ~SkyBox();

    bool Init(const string& Directory,
        const string& PosXFilename,
        const string& NegXFilename,
        const string& PosYFilename,
        const string& NegYFilename,
        const string& PosZFilename,
        const string& NegZFilename);

    void Render();

private:
    SkyboxTechnique* m_pSkyboxTechnique;
    const Camera* m_pCamera;
    CubemapTexture* m_pCubemapTex;
    Mesh* m_pMesh;
    PersProjInfo m_persProjInfo;
};
```
## Функция Render рендерит скайбокс
```c++
void SkyBox::Render()
{
    m_pSkyboxTechnique->Enable();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

    Pipeline p;
    p.Scale(20.0f, 20.0f, 20.0f);
    p.Rotate(0.0f, 0.0f, 0.0f);
    p.WorldPos(m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);
    p.SetCamera(m_pCamera->GetPos(), m_pCamera->GetTarget(), m_pCamera->GetUp());
    p.PerspectiveProj(m_persProjInfo);
    m_pSkyboxTechnique->SetWVP(p.GetWVPTrans());
    m_pCubemapTex->Bind(GL_TEXTURE0);
    m_pMesh->Render();

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
}
```
## Инициализация класса скайбокс
```c++
bool SkyBox::Init(const string& Directory,
    const string& PosXFilename,
    const string& NegXFilename,
    const string& PosYFilename,
    const string& NegYFilename,
    const string& PosZFilename,
    const string& NegZFilename)
{
    m_pSkyboxTechnique = new SkyboxTechnique();

    if (!m_pSkyboxTechnique->Init()) {
        printf("Error initializing the skybox technique\n");
        return false;
    }

    m_pSkyboxTechnique->Enable();
    m_pSkyboxTechnique->SetTextureUnit(0);

    m_pCubemapTex = new CubemapTexture(Directory,
        PosXFilename,
        NegXFilename,
        PosYFilename,
        NegYFilename,
        PosZFilename,
        NegZFilename);

    if (!m_pCubemapTex->Load()) {
        return false;
    }

    m_pMesh = new Mesh();

    return m_pMesh->LoadMesh("C:\\sphere.obj");
}
```
# Урок 26 - Карта нормалей
## Cтруктура для вершины с дополнительным вектором тангентом 
```c++
struct Vertex
{
    Vector3f m_pos;
    vec2 m_tex;
    Vector3f m_normal;
    Vector3f m_tangent;

    Vertex() {}

    Vertex(const Vector3f& pos, const vec2& tex, const Vector3f& normal, const Vector3f& Tangent)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = normal;
        m_tangent = Tangent;

    }
};
```
## Обновленный метод рендера мэша
```c++
void Mesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);                 // position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12); // texture coordinate
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20); // normal
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)32); // tangent

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(COLOR_TEXTURE_UNIT);
        }

        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}
```
## Обновленный вершинный шейдер
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
layout (location = 3) in vec3 Tangent;                                              \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gLightWVP;                                                             \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec4 LightSpacePos;                                                             \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 WorldPos0;                                                                 \n\
out vec3 Tangent0;                                                                  \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position   = gWVP * vec4(Position, 1.0);                                     \n\
    LightSpacePos = gLightWVP * vec4(Position, 1.0);                                \n\
    TexCoord0     = TexCoord;                                                       \n\
    Normal0       = (gWorld * vec4(Normal, 0.0)).xyz;                               \n\
    Tangent0      = (gWorld * vec4(Tangent, 0.0)).xyz;                              \n\
    WorldPos0     = (gWorld * vec4(Position, 1.0)).xyz;                             \n\
}
```
## Обновленный фрагментный шейдер
```c++
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
const int MAX_SPOT_LIGHTS = 3;                                                      \n\
                                                                                    \n\
in vec4 LightSpacePos;                                                              \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
in vec3 Tangent0;                                                                   \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct BaseLight                                                                    \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    BaseLight Base;                                                          \n\
    vec3 Direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
struct Attenuation                                                                  \n\
{                                                                                   \n\
    float Constant;                                                                 \n\
    float Linear;                                                                   \n\
    float Exp;                                                                      \n\
};                                                                                  \n\
                                                                                    \n\
struct PointLight                                                                           \n\
{                                                                                           \n\
    BaseLight Base;                                                                  \n\
    vec3 Position;                                                                          \n\
    Attenuation Atten;                                                                      \n\
};                                                                                          \n\
                                                                                            \n\
struct SpotLight                                                                            \n\
{                                                                                           \n\
    PointLight Base;                                                                 \n\
    vec3 Direction;                                                                         \n\
    float Cutoff;                                                                           \n\
};                                                                                          \n\
                                                                                            \n\
uniform int gNumPointLights;                                                                \n\
uniform int gNumSpotLights;                                                                 \n\
uniform DirectionalLight gDirectionalLight;                                                 \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          \n\
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                             \n\
uniform sampler2D gColorMap;                                                                \n\
uniform sampler2D gShadowMap;                                                               \n\
uniform sampler2D gNormalMap;                                                               \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
                                                                                            \n\
float CalcShadowFactor(vec4 LightSpacePos)                                                  \n\
{                                                                                           \n\
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;                                  \n\
    vec2 UVCoords;                                                                          \n\
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;                                                  \n\
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;                                                  \n\
    float Depth = texture(gShadowMap, UVCoords).x;                                          \n\
    if (Depth <= (ProjCoords.z + 0.005))                                                    \n\
        return 0.5;                                                                         \n\
    else                                                                                    \n\
        return 1.0;                                                                         \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal,            \n\
                       float ShadowFactor)                                                  \n\
{                                                                                           \n\
    vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;                   \n\
    float DiffuseFactor = dot(Normal, -LightDirection);                                     \n\
                                                                                            \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  \n\
                                                                                            \n\
    if (DiffuseFactor > 0) {                                                                \n\
        DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;    \n\
                                                                                            \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             \n\
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     \n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                              \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                               \n\
        if (SpecularFactor > 0) {                                                           \n\
            SpecularColor = vec4(Light.Color, 1.0f) *                                       \n\
                            gMatSpecularIntensity * SpecularFactor;                         \n\
        }                                                                                   \n\
    }                                                                                       \n\
                                                                                            \n\
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));                  \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                      \n\
{                                                                                                \n\
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal, 1.0);  \n\
}                                                                                                \n\
                                                                                            \n\
vec4 CalcPointLight(PointLight l, vec3 Normal, vec4 LightSpacePos)                   \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - l.Position;                                           \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
    float ShadowFactor = CalcShadowFactor(LightSpacePos);                                   \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal, ShadowFactor);           \n\
    float Attenuation =  l.Atten.Constant +                                                 \n\
                         l.Atten.Linear * Distance +                                        \n\
                         l.Atten.Exp * Distance * Distance;                                 \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcSpotLight(SpotLight l, vec3 Normal, vec4 LightSpacePos)                     \n\
{                                                                                           \n\
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);                             \n\
    float SpotFactor = dot(LightToPixel, l.Direction);                                      \n\
                                                                                            \n\
    if (SpotFactor > l.Cutoff) {                                                            \n\
        vec4 Color = CalcPointLight(l.Base, Normal, LightSpacePos);                         \n\
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   \n\
    }                                                                                       \n\
    else {                                                                                  \n\
        return vec4(0,0,0,0);                                                               \n\
    }                                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
vec3 CalcBumpedNormal()                                                                     \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec3 Tangent = normalize(Tangent0);                                                     \n\
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);                           \n\
    vec3 Bitangent = cross(Tangent, Normal);                                                \n\
    vec3 BumpMapNormal = texture(gNormalMap, TexCoord0).xyz;                                \n\
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);                              \n\
    vec3 NewNormal;                                                                         \n\
    mat3 TBN = mat3(Tangent, Bitangent, Normal);                                            \n\
    NewNormal = TBN * BumpMapNormal;                                                        \n\
    NewNormal = normalize(NewNormal);                                                       \n\
    return NewNormal;                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = CalcBumpedNormal();                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(gPointLights[i], Normal, LightSpacePos);               \n\
    }                                                                                       \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumSpotLights ; i++) {                                            \n\
        TotalLight += CalcSpotLight(gSpotLights[i], Normal, LightSpacePos);                 \n\
    }                                                                                       \n\
                                                                                            \n\
    vec4 SampledColor = texture2D(gColorMap, TexCoord0.xy);                                 \n\
    FragColor = SampledColor * TotalLight;                                                  \n\
}
```
