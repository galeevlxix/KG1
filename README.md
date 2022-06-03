# 6 лабораторная работа по компьютерной графике
## Выполнил Галеев Тимур МО-221
# Billboarding и Геометрический шейдер
## Создание класса BillboardList
```c++
class BillboardList
{
public:
    BillboardList();
    ~BillboardList();

    bool Init(const std::string& TexFilename);

    void Render(const Matrix4f& VP, const Vector3f& CameraPos);

private:
    void CreatePositionBuffer();

    GLuint m_VB;
    Texture* m_pTexture;
    BillboardTechnique m_technique;
};
```
## Функция Render разрешает использование методов billboard, устанавливает требуемые переменные состояния OpenGL и рисует точки, которые будут переделаны в прямоугольник в GS
```c++
void BillboardList::Render(const Matrix4f& VP, const Vector3f& CameraPos)
{
    m_technique.Enable();
    m_technique.SetVP(VP);
    m_technique.SetCameraPosition(CameraPos);

    m_pTexture->Bind(COLOR_TEXTURE_UNIT);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, m_VB);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);   // position

    glDrawArrays(GL_POINTS, 0, NUM_ROWS * NUM_COLUMNS);

    glDisableVertexAttribArray(0);
}
```
## Интерфейс BillboardTechnique для метода billboard
```c++
class BillboardTechnique : public Technique
{
public:

    BillboardTechnique();

    virtual bool Init();

    void SetVP(const Matrix4f& VP);
    void SetCameraPosition(const Vector3f& Pos);
    void SetColorTextureUnit(unsigned int TextureUnit);

private:

    GLuint m_VPLocation;
    GLuint m_cameraPosLocation;
    GLuint m_colorMapLocation;
};
```
## Вершинный шейдер метода billboard
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = vec4(Position, 1.0);                                              \n\
}
```
## Геометрический шейдер метода billboard
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout(points) in;                                                                  \n\
layout(triangle_strip) out;                                                         \n\
layout(max_vertices = 4) out;                                                       \n\
                                                                                    \n\
uniform mat4 gVP;                                                                   \n\
uniform vec3 gCameraPos;                                                            \n\
                                                                                    \n\
out vec2 TexCoord;                                                                  \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec3 Pos = gl_in[0].gl_Position.xyz;                                            \n\
    vec3 toCamera = normalize(gCameraPos - Pos);                                    \n\
    vec3 up = vec3(0.0, 1.0, 0.0);                                                  \n\
    vec3 right = cross(toCamera, up);                                               \n\
                                                                                    \n\
    Pos -= (right * 0.5);                                                           \n\
    gl_Position = gVP * vec4(Pos, 1.0);                                             \n\
    TexCoord = vec2(0.0, 0.0);                                                      \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    Pos.y += 1.0;                                                                   \n\
    gl_Position = gVP * vec4(Pos, 1.0);                                             \n\
    TexCoord = vec2(0.0, 1.0);                                                      \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    Pos.y -= 1.0;                                                                   \n\
    Pos += right;                                                                   \n\
    gl_Position = gVP * vec4(Pos, 1.0);                                             \n\
    TexCoord = vec2(1.0, 0.0);                                                      \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    Pos.y += 1.0;                                                                   \n\
    gl_Position = gVP * vec4(Pos, 1.0);                                             \n\
    TexCoord = vec2(1.0, 1.0);                                                      \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    EndPrimitive();                                                                 \n\
}
```
## Фрагментный шейдер метода billboard
```c++
#version 330                                                                        \n\
                                                                                    \n\
uniform sampler2D gColorMap;                                                        \n\
                                                                                    \n\
in vec2 TexCoord;                                                                   \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture2D(gColorMap, TexCoord);                                     \n\
                                                                                    \n\
    if (FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0) {                 \n\
        discard;                                                                    \n\
    }                                                                               \n\
}
```
## Вызов billboard в цикле рендера
```c++
m_billboardList.Render(p.GetVPTrans(), pGameCamera->GetPos());
```
# Результат 
# Система частиц и Transform Feedback
## Класс системы частиц ParticleSystem инкапсулирует всю механику взаимодействия буферов transform feedback
```c++
class ParticleSystem
{
public:
    ParticleSystem();

    ~ParticleSystem();

    bool InitParticleSystem(const Vector3f& Pos);

    void Render(int DeltaTimeMillis, const Matrix4f& VP, const Vector3f& CameraPos);

private:

    void UpdateParticles(int DeltaTimeMillis);
    void RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos);

    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    GLuint m_particleBuffer[2];
    GLuint m_transformFeedback[2];
    PSUpdateTechnique m_updateTechnique;
    BillboardTechnique m_billboardTechnique;
    RandomTexture m_randomTexture;
    Texture* m_pTexture;
    int m_time;
};
```
## Каждая частица имеет структуру
```c++
struct Particle
{
    float Type;
    Vector3f Pos;
    Vector3f Vel;
    float LifetimeMillis;
};
```
## Инициализация системы частиц
```c++
bool ParticleSystem::InitParticleSystem(const Vector3f& Pos)
{
    Particle Particles[MAX_PARTICLES];
    ZERO_MEM(Particles);

    Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
    Particles[0].Pos = Pos;
    Particles[0].Vel = Vector3f(0.0f, 0.00001f, 0.0f);
    Particles[0].LifetimeMillis = 0.0f;

    glGenTransformFeedbacks(2, m_transformFeedback);
    glGenBuffers(2, m_particleBuffer);

    for (unsigned int i = 0; i < 2; i++) {
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[i]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_particleBuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles, GL_DYNAMIC_DRAW);
    }

    if (!m_updateTechnique.Init()) {
        return false;
    }

    m_updateTechnique.Enable();

    m_updateTechnique.SetRandomTextureUnit(RANDOM_TEXTURE_UNIT_INDEX);
    m_updateTechnique.SetLauncherLifetime(100.0f);
    m_updateTechnique.SetShellLifetime(10000.0f);
    m_updateTechnique.SetSecondaryShellLifetime(2500.0f);

    if (!m_randomTexture.InitRandomTexture(1000)) {
        return false;
    }

    m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);

    if (!m_billboardTechnique.Init()) {
        return false;
    }

    m_billboardTechnique.Enable();

    m_billboardTechnique.SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

    m_billboardTechnique.SetBillboardSize(0.01f);

    m_pTexture = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\james-p-rat-is-short.png");

    if (!m_pTexture->Load()) {
        return false;
    }

    return GLCheckError();
}
```
## Главная функция рендера класса ParticleSystem
```c++
void ParticleSystem::Render(int DeltaTimeMillis, const Matrix4f& VP, const Vector3f& CameraPos)
{
    m_time += DeltaTimeMillis;

    UpdateParticles(DeltaTimeMillis);

    RenderParticles(VP, CameraPos);

    m_currVB = m_currTFB;
    m_currTFB = (m_currTFB + 1) & 0x1;
}
```
## Функция обновления частиц UpdateParticles
```c++
void ParticleSystem::UpdateParticles(int DeltaTimeMillis)
{
    m_updateTechnique.Enable();
    m_updateTechnique.SetTime(m_time);
    m_updateTechnique.SetDeltaTimeMillis(DeltaTimeMillis);

    m_randomTexture.Bind(RANDOM_TEXTURE_UNIT);

    glEnable(GL_RASTERIZER_DISCARD);

    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currVB]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_transformFeedback[m_currTFB]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);         // position
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)16);        // velocity
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)28);          // lifetime

    glBeginTransformFeedback(GL_POINTS);

    if (m_isFirst) {
        glDrawArrays(GL_POINTS, 0, 1);

        m_isFirst = false;
    }
    else {
        glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currVB]);
    }

    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}
```
## Текущий рендер мы начинаем с разрешения метода billboarding и назначение в него параметров
```c++
void ParticleSystem::RenderParticles(const Matrix4f& VP, const Vector3f& CameraPos)
{
    m_billboardTechnique.Enable();
    m_billboardTechnique.SetCameraPosition(CameraPos);
    m_billboardTechnique.SetVP(VP);
    m_pTexture->Bind(COLOR_TEXTURE_UNIT);

    glDisable(GL_RASTERIZER_DISCARD);

    glBindBuffer(GL_ARRAY_BUFFER, m_particleBuffer[m_currTFB]);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)4);  // position

    glDrawTransformFeedback(GL_POINTS, m_transformFeedback[m_currTFB]);

    glDisableVertexAttribArray(0);
}
```
## Создание класса PSUpdateTechnique
```c++
class PSUpdateTechnique : public Technique
{
public:
    PSUpdateTechnique();

    virtual bool Init();

    void SetParticleLifetime(float Lifetime);

    void SetDeltaTimeMillis(float DeltaTimeMillis);

    void SetTime(int Time);

    void SetRandomTextureUnit(unsigned int TextureUnit);

    void SetLauncherLifetime(float Lifetime);

    void SetShellLifetime(float Lifetime);

    void SetSecondaryShellLifetime(float Lifetime);

private:
    GLuint m_deltaTimeMillisLocation;
    GLuint m_randomTextureLocation;
    GLuint m_timeLocation;
    GLuint m_launcherLifetimeLocation;
    GLuint m_shellLifetimeLocation;
    GLuint m_secondaryShellLifetimeLocation;
};
```
## Вершинный шейдер метода обновления частиц
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in float Type;                                                \n\
layout (location = 1) in vec3 Position;                                             \n\
layout (location = 2) in vec3 Velocity;                                             \n\
layout (location = 3) in float Age;                                                 \n\
                                                                                    \n\
out float Type0;                                                                    \n\
out vec3 Position0;                                                                 \n\
out vec3 Velocity0;                                                                 \n\
out float Age0;                                                                     \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    Type0 = Type;                                                                   \n\
    Position0 = Position;                                                           \n\
    Velocity0 = Velocity;                                                           \n\
    Age0 = Age;                                                                     \n\
}
```
## Геометрический шейдер метода обновления частиц
```c++
#version 330                                                                        \n\
                                                                                    \n\
layout(points) in;                                                                  \n\
layout(points) out;                                                                 \n\
layout(max_vertices = 30) out;                                                      \n\
                                                                                    \n\
in float Type0[];                                                                   \n\
in vec3 Position0[];                                                                \n\
in vec3 Velocity0[];                                                                \n\
in float Age0[];                                                                    \n\
                                                                                    \n\
out float Type1;                                                                    \n\
out vec3 Position1;                                                                 \n\
out vec3 Velocity1;                                                                 \n\
out float Age1;                                                                     \n\
                                                                                    \n\
uniform float gDeltaTimeMillis;                                                     \n\
uniform float gTime;                                                                \n\
uniform sampler1D gRandomTexture;                                                   \n\
uniform float gLauncherLifetime;                                                    \n\
uniform float gShellLifetime;                                                       \n\
uniform float gSecondaryShellLifetime;                                              \n\
                                                                                    \n\
#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 \n\
#define PARTICLE_TYPE_SHELL 1.0f                                                    \n\
#define PARTICLE_TYPE_SECONDARY_SHELL 2.0f                                          \n\
                                                                                    \n\
vec3 GetRandomDir(float TexCoord)                                                   \n\
{                                                                                   \n\
     vec3 Dir = texture(gRandomTexture, TexCoord).xyz;                              \n\
     Dir -= vec3(0.5, 0.5, 0.5);                                                    \n\
     return Dir;                                                                    \n\
}                                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    float Age = Age0[0] + gDeltaTimeMillis;                                         \n\
                                                                                    \n\
    if (Type0[0] == PARTICLE_TYPE_LAUNCHER) {                                       \n\
        if (Age >= gLauncherLifetime) {                                             \n\
            Type1 = PARTICLE_TYPE_SHELL;                                            \n\
            Position1 = Position0[0];                                               \n\
            vec3 Dir = GetRandomDir(gTime/1000.0);                                  \n\
            Dir.y = max(Dir.y, 0.5);                                                \n\
            Velocity1 = normalize(Dir) / 20.0;                                      \n\
            Age1 = 0.0;                                                             \n\
            EmitVertex();                                                           \n\
            EndPrimitive();                                                         \n\
            Age = 0.0;                                                              \n\
        }                                                                           \n\
                                                                                    \n\
        Type1 = PARTICLE_TYPE_LAUNCHER;                                             \n\
        Position1 = Position0[0];                                                   \n\
        Velocity1 = Velocity0[0];                                                   \n\
        Age1 = Age;                                                                 \n\
        EmitVertex();                                                               \n\
        EndPrimitive();                                                             \n\
    }                                                                               \n\
    else {                                                                          \n\
        float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;                           \n\
        float t1 = Age0[0] / 1000.0;                                                \n\
        float t2 = Age / 1000.0;                                                    \n\
        vec3 DeltaP = DeltaTimeSecs * Velocity0[0];                                 \n\
        vec3 DeltaV = vec3(DeltaTimeSecs) * (0.0, -9.81, 0.0);                      \n\
                                                                                    \n\
        if (Type0[0] == PARTICLE_TYPE_SHELL)  {                                     \n\
	        if (Age < gShellLifetime) {                                             \n\
	            Type1 = PARTICLE_TYPE_SHELL;                                        \n\
	            Position1 = Position0[0] + DeltaP;                                  \n\
	            Velocity1 = Velocity0[0] + DeltaV;                                  \n\
	            Age1 = Age;                                                         \n\
	            EmitVertex();                                                       \n\
	            EndPrimitive();                                                     \n\
	        }                                                                       \n\
            else {                                                                  \n\
                for (int i = 0 ; i < 10 ; i++) {                                    \n\
                     Type1 = PARTICLE_TYPE_SECONDARY_SHELL;                         \n\
                     Position1 = Position0[0];                                      \n\
                     vec3 Dir = GetRandomDir((gTime + i)/1000.0);                   \n\
                     Velocity1 = normalize(Dir) / 20.0;                             \n\
                     Age1 = 0.0f;                                                   \n\
                     EmitVertex();                                                  \n\
                     EndPrimitive();                                                \n\
                }                                                                   \n\
            }                                                                       \n\
        }                                                                           \n\
        else {                                                                      \n\
            if (Age < gSecondaryShellLifetime) {                                    \n\
                Type1 = PARTICLE_TYPE_SECONDARY_SHELL;                              \n\
                Position1 = Position0[0] + DeltaP;                                  \n\
                Velocity1 = Velocity0[0] + DeltaV;                                  \n\
                Age1 = Age;                                                         \n\
                EmitVertex();                                                       \n\
                EndPrimitive();                                                     \n\
            }                                                                       \n\
        }                                                                           \n\
    }                                                                               \n\
}
```
## Создание класса RandomTexture для обеспечения случайных данных в шейдере
```c++
class RandomTexture
{
public:
    RandomTexture();

    ~RandomTexture();

    bool InitRandomTexture(unsigned int Size);

    void Bind(GLenum TextureUnit);

private:
    GLuint m_textureObj;
};
```
```c++
bool RandomTexture::InitRandomTexture(unsigned int Size)
{
    Vector3f* pRandomData = new Vector3f[Size];
    for (unsigned int i = 0; i < Size; i++) {
        pRandomData[i].x = RandomFloat();
        pRandomData[i].y = RandomFloat();
        pRandomData[i].z = RandomFloat();
    }

    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_1D, m_textureObj);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, Size, 0.0f, GL_RGB, GL_FLOAT, pRandomData);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    delete[] pRandomData;

    return GLCheckError();
}
```
# Результат
# Vertex Array Objects
## Рефакторинг класса Mesh
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
    void InitMesh(const aiMesh* paiMesh,
        std::vector<Vector3f>& Positions,
        std::vector<Vector3f>& Normals,
        std::vector<Vector2f>& TexCoords,
        std::vector<unsigned int>& Indices);

    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3       

    GLuint m_VAO;
    GLuint m_Buffers[4];

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
};
```
## Измененный метод LoadMesh
```c++
bool Mesh::LoadMesh(const string& Filename)
{
    // Удаляем предыдущую загруженную модель (если есть)
    Clear();

    // Создание VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Создание буферов для атрибутов вершин
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

    // Удостоверимся, что VAO не изменится из внешнего кода
    glBindVertexArray(0);

    return Ret;
}
```
## Измененный метод InitFromScene
```c++
bool Mesh::InitFromScene(const aiScene* pScene, const string& Filename){
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
 
    // Подготавливаем вектора для вершинных атрибутов и индексов
    vector<vector3f> Positions;
    vector<vector3f> Normals;
    vector<vector2f> TexCoords;
    vector<unsigned int=""> Indices;
 
    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;
 
    // Подсчитываем количество вершин и индексов
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++){
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;
 
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices+= m_Entries[i].BaseIndex;
    }
 
    // Резервируем пространство в векторах для атрибутов вершин и индексов
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Indices.reserve(NumIndices);
 
    // Инициализируем меши в сцене один за другим
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++){
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Normals, TexCoords, Indices);
    }
 
    if (!InitMaterials(pScene, Filename)) {
        return false;
    }
 
    // Создаем и заполняем буферы с вершинными атрибутами и индексами
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
 
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
 
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
 
    return true;
}
```
## Функция InitMesh отвечает за загрузку каждой структуры aiMesh, которую содержит aiScene
```c++
void Mesh::InitMesh(const aiMesh* paiMesh,
        vector<vector3f>& Positions,
        vector<vector3f>& Normals,
        vector<vector2f>& TexCoords,
        vector<unsigned int="">& Indices){
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
 
    // Заполняем векторы вершинных атрибутов
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos= &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
 
        Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
        Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
    }
 
    // Заполняем буфер индексов
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}
```
## Функция рендера 
```c++
void Mesh::Render()
{
    glBindVertexArray(m_VAO);
 
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
      const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
 
      assert(MaterialIndex < m_Textures.size());
 
      if (m_Textures[MaterialIndex]){
              m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
      }
 
      glDrawElementsBaseVertex(GL_TRIANGLES,
                              m_Entries[i].NumIndices,
                              GL_UNSIGNED_INT,
                              (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
                              m_Entries[i].BaseVertex);
    }
 
    // Убедитесь, что VAO не изменен снаружи
    glBindVertexArray(0);
}
```
# Результат
# Дублирующий рендер (Instanced Rendering)
## Это измененный класс Mesh
```c++
class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void Render(unsigned int NumInstances, const Matrix4f* WVPMats, const Matrix4f* WorldMats);

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(const aiMesh* paiMesh,
        std::vector<Vector3f>& Positions,
        std::vector<Vector3f>& Normals,
        std::vector<Vector2f>& TexCoords,
        std::vector<unsigned int>& Indices);

    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3    
#define WVP_MAT_VB   4
#define WORLD_MAT_VB 5

    GLuint m_VAO;
    GLuint m_Buffers[6];

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    std::vector<MeshEntry> m_Entries;
    std::vector<Texture*> m_Textures;
};
```
## Обновленный метод InitFromScene
```c++
bool Mesh::InitFromScene(const aiScene* pScene, const string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<unsigned int> Indices;

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    // Count the number of vertices and indices
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Normals, TexCoords, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);

    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(WVP_LOCATION + i);
        glVertexAttribPointer(WVP_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WVP_LOCATION + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);

    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(WORLD_LOCATION + i);
        glVertexAttribPointer(WORLD_LOCATION + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WORLD_LOCATION + i, 1);
    }

    return GLCheckError();
}
```
## Это обновленная функция Render() класса Mesh
```c++
void Mesh::Render(unsigned int NumInstances, const Matrix4f* WVPMats, const Matrix4f* WorldMats)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WVP_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WVPMats, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[WORLD_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4f) * NumInstances, WorldMats, GL_DYNAMIC_DRAW);

    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < m_Textures.size());

        if (m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

        glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
            m_Entries[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
            NumInstances,
            m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside    
    glBindVertexArray(0);
}
```
## Обновленный вершинный шейдер
```c++
#version 410                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
layout (location = 3) in mat4 WVP;                                                  \n\
layout (location = 7) in mat4 World;                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 WorldPos0;                                                                 \n\
flat out int InstanceID;                                                            \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = WVP * vec4(Position, 1.0);                                        \n\
    TexCoord0   = TexCoord;                                                         \n\
    Normal0     = (World * vec4(Normal, 0.0)).xyz;                                  \n\
    WorldPos0   = (World * vec4(Position, 1.0)).xyz;                                \n\
    InstanceID = gl_InstanceID;                                                     \n\
}
```
## Обновленный фрагментный шейдер
```c++
#version 410                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 2;                                                     \n\
const int MAX_SPOT_LIGHTS = 2;                                                      \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
flat in int InstanceID;                                                             \n\
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
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
uniform vec4 gColor[4];                                                                     \n\
                                                                                            \n\
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)                   \n\
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
    return (AmbientColor + DiffuseColor + SpecularColor);                                   \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                      \n\
{                                                                                           \n\
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcPointLight(PointLight l, vec3 Normal)                                              \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - l.Position;                                           \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal);                         \n\
    float Attenuation =  l.Atten.Constant +                                                 \n\
                         l.Atten.Linear * Distance +                                        \n\
                         l.Atten.Exp * Distance * Distance;                                 \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcSpotLight(SpotLight l, vec3 Normal)                                                \n\
{                                                                                           \n\
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);                             \n\
    float SpotFactor = dot(LightToPixel, l.Direction);                                      \n\
                                                                                            \n\
    if (SpotFactor > l.Cutoff) {                                                            \n\
        vec4 Color = CalcPointLight(l.Base, Normal);                                        \n\
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   \n\
    }                                                                                       \n\
    else {                                                                                  \n\
        return vec4(0,0,0,0);                                                               \n\
    }                                                                                       \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(gPointLights[i], Normal);                              \n\
    }                                                                                       \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumSpotLights ; i++) {                                            \n\
        TotalLight += CalcSpotLight(gSpotLights[i], Normal);                                \n\
    }                                                                                       \n\
                                                                                            \n\
    FragColor = texture(gColorMap, TexCoord0.xy) * TotalLight * gColor[InstanceID % 4];     \n\
}
```
## Вызов обновленной функции Mesh::Render() в главной функции рендера
```c++
Pipeline p;
p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
p.SetPerspectiveProj(m_persProjInfo);
p.Rotate(0.0f, 90.0f, 0.0f);
p.Scale(0.03f, 0.03f, 0.03f);

Matrix4f WVPMatrics[NUM_INSTANCES];
Matrix4f WorldMatrices[NUM_INSTANCES];

for (unsigned int i = 0; i < NUM_INSTANCES; i++) {
    Vector3f Pos(m_positions[i]);
    Pos.y += sinf(Scale) * m_velocity[i];
    p.WorldPos(Pos - Vector3f(0.0f, 20.0f, 0.0f));
    WVPMatrics[i] = p.GetWVPTrans().Transpose();
    WorldMatrices[i] = p.GetWorldTrans().Transpose();
}

m_pMesh->Render(NUM_INSTANCES, WVPMatrics, WorldMatrices);
```
# Результат 
# Deferred Shading - Часть 1
## Создание класса GBuffer
```c++
class GBuffer
{
public:
 
    enum GBUFFER_TEXTURE_TYPE {
    GBUFFER_TEXTURE_TYPE_POSITION,
            GBUFFER_TEXTURE_TYPE_DIFFUSE,
    GBUFFER_TEXTURE_TYPE_NORMAL,
            GBUFFER_TEXTURE_TYPE_TEXCOORD,
            GBUFFER_NUM_TEXTURES
    };
 
    GBuffer();
 
    ~GBuffer();
 
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
 
    void BindForWriting();
 
    void BindForReading();
 
private:
 
    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
};
```
## Инициализируем G буффер в функции Init
```c++
bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Создаем FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
 
    // Создаем текстуры gbuffer
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
    glGenTextures(1, &m_depthTexture);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures) ; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
 
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, m_textures[i], 0);
    }
 
    // глубина
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                                            NULL);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
 
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
 
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
 
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
 
    if (Status != GL_FRAMEBUFFER_COMPLETE) {
            printf("FB error, status: 0x%x\n", Status);
            return false;
    }
 
    // возвращаем стандартный FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
 
    return true;
}
```
## Главная функция рендера
```c++
virtual void RenderSceneCB()
{
    CalcFPS();
 
    m_scale += 0.05f;
 
    m_pGameCamera->OnRender();
 
    DSGeometryPass();
    DSLightPass();
 
    RenderFPS();
 
    glutSwapBuffers();
}
```
## Функция DSGeometryPass
```c++
void DSGeometryPass()
{
    m_DSGeomPassTech.Enable();
 
    m_gbuffer.BindForGeometryPass();
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    Pipeline p;
    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, m_scale, 0.0f);
    p.WorldPos(-0.8f, -1.0f, 12.0f);
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_persProjInfo);
    m_DSGeomPassTech.SetWVP(p.GetWVPTrans());
    m_DSGeomPassTech.SetWorldMatrix(p.GetWorldTrans());
    m_mesh.Render();
}
```
## Функция DSLightPass
```c++
void DSLightPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    m_gbuffer.BindForReading();
 
    GLsizei HalfWidth = (GLsizei)(WINDOW_WIDTH / 2.0f);
    GLsizei HalfHeight = (GLsizei)(WINDOW_HEIGHT / 2.0f);
 
    m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
 
    m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, HalfHeight, HalfWidth, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
 
    m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, HalfHeight, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
 
    m_gbuffer.SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, HalfWidth, 0, WINDOW_WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
```
## Это файл эффекта для геометрического прохода
```c++
struct VSInput
{
    vec3 Position;
    vec2 TexCoord;
    vec3 Normal;
};
 
interface VSOutput
{
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 Normal;
};
 
uniform mat4 gWVP;
uniform mat4 gWorld;
 
shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{
    gl_Position = gWVP * vec4(VSin.Position, 1.0);
    VSout.TexCoord   = VSin.TexCoord;
    VSout.Normal     = (gWorld * vec4(VSin.Normal, 0.0)).xyz;
    VSout.WorldPos   = (gWorld * vec4(VSin.Position, 1.0)).xyz;
};
 
struct FSOutput
{
    vec3 WorldPos;
    vec3 Diffuse;
    vec3 Normal;
    vec3 TexCoord;
};
 
uniform sampler2D gColorMap;
 
shader FSmain(in VSOutput FSin, out FSOutput FSout)
{
    FSout.WorldPos = FSin.WorldPos;
    FSout.Diffuse  = texture(gColorMap, FSin.TexCoord).xyz;
    FSout.Normal   = normalize(FSin.Normal);
    FSout.TexCoord = vec3(FSin.TexCoord, 0.0);
};
 
program GeometryPass
{
    vs(410)=VSmain();
    fs(410)=FSmain();
};
```
## Функции для изменения состояния G буффера
```c++
void GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}
 
void GBuffer::BindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}
 
void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}
```
# Deferred Shading - Часть 2
## Обновленная функция рендера
```c++
virtual void RenderSceneCB()
{
    CalcFPS();
    m_scale += 0.05f;
    m_pGameCamera->OnRender();
    DSGeometryPass();
 
        BeginLightPasses();
        DSPointLightsPass();
        DSDirectionalLightPass();
 
    RenderFPS();
    glutSwapBuffers();
}
```
## Изменения в геометрическом проходе
```c++
void DSGeometryPass()
{
    m_DSGeomPassTech.Enable();
 
    m_gbuffer.BindForWriting();
 
    // Only the geometry pass updates the depth buffer
        glDepthMask(GL_TRUE);
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
 
    Pipeline p;
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_persProjInfo);
    p.Rotate(0.0f, m_scale, 0.0f);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_boxPositions) ; i++) {
        p.WorldPos(m_boxPositions[i]);
        m_DSGeomPassTech.SetWVP(p.GetWVPTrans());
        m_DSGeomPassTech.SetWorldMatrix(p.GetWorldTrans());
        m_box.Render();
    }
 
    // When we get here the depth buffer is already populated and the stencil pass
    // depends on it, but it does not write to it.
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
}
```
## Вызывем функцию BeginLightPasses, прежде чем мы начнет проход света
```c++
void BeginLightPasses()
{
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
 
    m_gbuffer.BindForReading();
    glClear(GL_COLOR_BUFFER_BIT);
}
```
## Для точечного света мы просто рендерим сферу для каждого источника
```c++
void DSPointLightsPass()
{
    m_DSPointLightPassTech.Enable();
    m_DSPointLightPassTech.SetEyeWorldPos(m_pGameCamera->GetPos());
 
    Pipeline p;
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_persProjInfo);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLight); i++) {
        m_DSPointLightPassTech.SetPointLight(m_pointLight[i]);
        p.WorldPos(m_pointLight[i].Position);
        float BSphereScale = CalcPointLightBSphere(m_pointLight[i].Color, m_pointLight[i].DiffuseIntensity);
        p.Scale(BSphereScale, BSphereScale, BSphereScale);
        m_DSPointLightPassTech.SetWVP(p.GetWVPTrans());
        m_bsphere.Render();
    }
}
```
## Функция вычисляет размер сферы для указанного источника света
```c++
float CalcPointLightBSphere(const Vector3f& Color, float Intensity)
{
    float MaxChannel = fmax(fmax(Color.x, Color.y), Color.z);
    float c = MaxChannel * Intensity;
    return (8.0f * sqrtf(c) + 1.0f);
}
```
## Функция DSDirectionalLightPass, чтобы обрабатывать направленный свет
```c++
void DSDirectionalLightPass()
{
    m_DSDirLightPassTech.Enable();
    m_DSDirLightPassTech.SetEyeWorldPos(m_pGameCamera->GetPos());
    Matrix4f WVP;
    WVP.InitIdentity();
    m_DSDirLightPassTech.SetWVP(WVP);
    m_quad.Render();
}
```
## Векторный шейдер
```c++
shader VSmain(in vec3 Position)
{
    gl_Position = gWVP * vec4(Position, 1.0);
}
```
## Фрагментный шейдер для направленного и точечного света
```c++
shader FSmainDirLight(out vec4 FragColor)
{
        vec2 TexCoord = CalcTexCoord();
    vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
    vec3 Color = texture(gColorMap, TexCoord).xyz;
    vec3 Normal = texture(gNormalMap, TexCoord).xyz;
    Normal = normalize(Normal);
 
    FragColor = vec4(Color, 1.0) * CalcDirectionalLight(WorldPos, Normal);
}
 
shader FSmainPointLight(out vec4 FragColor)
{
        vec2 TexCoord = CalcTexCoord();
    vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
    vec3 Color = texture(gColorMap, TexCoord).xyz;
    vec3 Normal = texture(gNormalMap, TexCoord).xyz;
    Normal = normalize(Normal);
 
    FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal);
}
```
## Класс CalcTexCoord
```c++
vec2 CalcTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}
```
## Добавления в инициализации G буфера
```c++
bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);    
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
	glGenTextures(1, &m_depthTexture);

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures) ; i++) {
    	glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }

	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

   	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, 
						     GL_COLOR_ATTACHMENT1,
						     GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}
```
## Функция BindForReading
```c++
void GBuffer::BindForReading()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
		glActiveTexture(GL_TEXTURE0 + i);		
		glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
	}
}
```
# Deferred Shading - Часть 3
## Главная функция рендера
```c++
virtual void RenderSceneCB()
{
    CalcFPS();
 
    m_scale += 0.05f;
 
    m_pGameCamera->OnRender();
 
    m_gbuffer.StartFrame();
 
    DSGeometryPass();
 
    // Для того, что бы обновился буфер трафарета нужно его активировать,
    // так же он потребуется и в проходе света, так как свет рендерится
    // только при успешном проходе трафарета.
    glEnable(GL_STENCIL_TEST);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLight); i++) {
        DSStencilPass(i);
        DSPointLightPass(i);
    }
 
    // Направленному свету не требуется трафарет
    // так как его действие не ограничено расстоянием.
    glDisable(GL_STENCIL_TEST);
 
    DSDirectionalLightPass();
 
    DSFinalPass();
 
    RenderFPS();
 
    glutSwapBuffers();
}
```
## Изменения в геометрическом проходе
```c++
void DSGeometryPass()
{
    m_DSGeomPassTech.Enable();
 
        m_gbuffer.BindForGeomPass();
 
    // Только геометрический проход обновляет тест глубины
    glDepthMask(GL_TRUE);
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
    glEnable(GL_DEPTH_TEST);
 
    Pipeline p;
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_persProjInfo);
    p.Rotate(0.0f, m_scale, 0.0f);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_boxPositions) ; i++) {
        p.WorldPos(m_boxPositions[i]);
        m_DSGeomPassTech.SetWVP(p.GetWVPTrans());
        m_DSGeomPassTech.SetWorldMatrix(p.GetWorldTrans());
        m_box.Render();
    }
 
    // К этому моменту буфер глубины уже заполнен и, хоть проход трафарета
    // и основывается на нем, запись не потребуется.
    glDepthMask(GL_FALSE);
}
```
## Функция DSStencilPass
```c++
void DSStencilPass(unsigned int PointLightIndex)
{
    m_nullTech.Enable();
 
    // Отключаем запись цвета / глубины и включаем трафарет
    m_gbuffer.BindForStencilPass();
    glEnable(GL_DEPTH_TEST);
 
        glDisable(GL_CULL_FACE);
 
    glClear(GL_STENCIL_BUFFER_BIT);
 
    // Нам нужен тест трафарета, но мы хотим, что бы он всегда
    // успешно проходил. Важен только тест глубины.
    glStencilFunc(GL_ALWAYS, 0, 0);
 
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
 
    Pipeline p;
    p.WorldPos(m_pointLight[PointLightIndex].Position);
        float BBoxScale = CalcPointLightBSphere(m_pointLight[PointLightIndex].Color,
                                                m_pointLight[PointLightIndex].DiffuseIntensity);
    p.Scale(BBoxScale, BBoxScale, BBoxScale);
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
 
    m_nullTech.SetWVP(p.GetWVPTrans());
    m_bsphere.Render();
}
```
## Функция прохода света DSPointLightPass
```c++
void DSPointLightPass(unsigned int PointLightIndex)
{
    m_gbuffer.BindForLightPass();
 
    m_DSPointLightPassTech.Enable();
    m_DSPointLightPassTech.SetEyeWorldPos(m_pGameCamera->GetPos());
 
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
 
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
 
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
 
    Pipeline p;
    p.WorldPos(m_pointLight[PointLightIndex].Position);
    float BBoxScale = CalcPointLightBSphere(m_pointLight[PointLightIndex].Color,
                                            m_pointLight[PointLightIndex].DiffuseIntensity);
    p.Scale(BBoxScale, BBoxScale, BBoxScale);
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    p.SetPerspectiveProj(m_persProjInfo);
    m_DSPointLightPassTech.SetWVP(p.GetWVPTrans());
    m_DSPointLightPassTech.SetPointLight(m_pointLight[PointLightIndex]);
    m_bsphere.Render();
    glCullFace(GL_BACK);
 
    glDisable(GL_BLEND);
}
```
## В итоговом проходе мы блиттим (blit) из буфера цвета в G буфере на экран
```c++
void DSFinalPass()
{
    m_gbuffer.BindForFinalPass();
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
                      0, 0, WINDOW_WIDTH,
                      WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}
```
## Мы добавили итоговую текстуру в класс GBuffer для цвета, а также слегка перегруппировали API с прошлого урока
```c++
class GBuffer
{
public:
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_NUM_TEXTURES
    };
 
    GBuffer();
 
    ~GBuffer();
 
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
 
    void StartFrame();
    void BindForGeomPass();
    void BindForStencilPass();
    void BindForLightPass();
    void BindForFinalPass();
 
private:
    GLuint m_fbo;
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
    GLuint m_depthTexture;
 
        GLuint m_finalTexture;
};
```
## Инициализация G буфера
```c++
bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);    
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
    glGenTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);

	glGenTextures(1, &m_depthTexture);

	glGenTextures(1, &m_finalTexture);

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures) ; i++) {
    	glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }

	// depth
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	// final
	glBindTexture(GL_TEXTURE_2D, m_finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, m_finalTexture, 0);	

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        return false;
    }

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}
```
Перед началом кажого кадра мы должны очистить итоговую текстуру, которая привязана под номером 4
```c++
void GBuffer::StartFrame()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}
```
До этого момента FBO в G буфере был статическим (в терминах его конфигурации) и создавался заранее, поэтому мы его только привязывали для записи только в геометрическом проходе. Теперь для изменения FBO мы задаем буферы для записи каждый раз.
```c++
void GBuffer::BindForGeomPass()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
 
    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
                                GL_COLOR_ATTACHMENT1,
                                GL_COLOR_ATTACHMENT2 };
 
    glDrawBuffers(ARRAY_SIZE_IN_ELEMENTS(DrawBuffers), DrawBuffers);
}
```
Как объяснялось ранее, в тесте трафарета мы не записываем в буфер цвета, только в буфер трафарета. Даже с пустым FS. Хотя, в таком случае цвет по-умолчанию черный. Что бы не захламлять итоговый буфер черным изображением сферы мы полностью отключаем рисование в буфер.
```c++
void GBuffer::BindForStencilPass()
{
    // должны отключить буфер цвета
    glDrawBuffer(GL_NONE);
}
```
Проход света очевиден. Назначаем целью итоговый буфер и привязываем аттрибуты буфера как источник.
```c++
void GBuffer::BindForLightPass()
{
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
 
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}
```
Когда мы дойдем до последнего прохода наш итоговый буфер заполнен финальным изображением. Здесь же мы настраиваем блиттинг, который осуществляется в главном коде. FBO по-умолчанию ставим целью, а FBO G буфера - источником.
```c++
void GBuffer::BindForFinalPass()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}
```
# Результат
# Скелетная анимация
## Обновленный класс Mesh
```c++
bool Mesh::LoadMesh(const string& Filename)
{
    // Очищаем данные прошлого меша (если был загружен)
    Clear();
 
    // Создаем VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
 
    // Создаем буферы для аттрибутов вершин
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
 
    bool Ret = false;
 
        m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
 
    if (m_pScene) {
 
            m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
            m_GlobalInverseTransform.Inverse();
 
        Ret = InitFromScene(m_pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }
 
    // Убедимся, что VAO не изменится из вне
    glBindVertexArray(0);
 
    return Ret;
}
```
## Структура VertexBoneData содержит все, что нам потребуется на уровне вершин
```c++
struct VertexBoneData
{
    uint IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];
}
```
## Функция инициализации Mesh
```c++
bool Mesh::InitFromScene(const aiScene* pScene, const string& Filename)
{  
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<VertexBoneData> Bones;
    vector<uint> Indices;
       
    uint NumVertices = 0;
    uint NumIndices = 0;
    
    // Count the number of vertices and indices
    for (uint i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;        
        m_Entries[i].NumIndices    = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex    = NumVertices;
        m_Entries[i].BaseIndex     = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
        
    // Initialize the meshes in the scene one by one
    for (uint i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);    
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    return GLCheckError();
}
```
## Функция LoadBones загружает информацию о кости для одного объекта aiMesh
```c++
void Mesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);
 
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
        }
        else {
            BoneIndex = m_BoneMapping[BoneName];
        }
 
        m_BoneMapping[BoneName] = BoneIndex;
        m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
 
        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = m_Entries[MeshIndex].BaseVertex +
                            pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}
```
## Функция AddBoneData находит свободные слоты в структуре VertexBoneData и размещает внутри id и вес кости
```c++
void Mesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }
    }
 
    // Никогда не должны оказаться здесь - костей больше, чем мы рассчитывали
    assert(0);
}
```
## Вычисление преобразования кости, которое будет загружаться в шейдер каждый кадр, происходит в функции BoneTransform
```c++
Matrix4f Mesh::BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
{
    Matrix4f Identity;
    Identity.InitIdentity();
 
    float TicksPerSecond = m_pScene->mAnimations[0]->mTicksPerSecond != 0 ?
                          m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
 
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[0]->mDuration);
 
    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
 
    Transforms.resize(m_NumBones);
 
    for (uint i = 0 ; i < m_NumBones ; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}
```
## Функция ReadNodeHeirarchy обходит листы дерева и генерирует итоговое преобразование для каждого листа / кости согласно указанному времени анимации
```c++
void Mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
    string NodeName(pNode->mName.data);
 
    const aiAnimation* pAnimation = m_pScene->mAnimations[0];
 
    Matrix4f NodeTransformation(pNode->mTransformation);
 
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
 
    if (pNodeAnim) {
        // Интерполируем масштабирование и генерируем матрицу преобразования масштаба
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
 
        // Интерполируем вращение и генерируем матрицу вращения
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
 
        //  Интерполируем смещение и генерируем матрицу смещения
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
 
        // Объединяем преобразования
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
 
    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
 
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
 
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform *
                                                    GlobalTransformation *
                                                    m_BoneInfo[BoneIndex].BoneOffset;
    }
 
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}
```
## Этот метод интерполирует кватернион вращения указанного канала согласно времени анимации
```c++
void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // для интерполирования требуется не менее 2 значений...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
 
    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                      pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}
```
## Метод находит ключевое вращение непосредственно перед временем анимации
```c++
uint Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);
 
    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }
 
    assert(0);
}
```
## Теперь, когда мы закончили с классом меша давайте рассмотрим, что нам нужно на уровне шейдера. 
Для начала мы добавим массивы ID костей и веса в структуру VSInput. Затем появился новый uniform массив, который содержит все преобразования костей. В самом шейдере мы вычисляем итоговое преобразование кости как комбинацию матриц вершины и веса. Итоговая матрица используется для преобразования позиции и нормали их пространства кости в локальное. Дальше все как обычно.
```c++
struct VSInput
{
    vec3  Position;
    vec2  TexCoord;
    vec3  Normal;
 
        vec4 BoneIDs;
        vec4  Weights;
};
 
interface VSOutput
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 WorldPos;
};
 
const int MAX_BONES = 100;
 
uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gBones[MAX_BONES];
 
shader VSmain(in VSInput VSin:0, out VSOutput VSout)
{
    mat4 BoneTransform = gBones[VSin.BoneIDs[0]] * VSin.Weights[0];
    BoneTransform     += gBones[VSin.BoneIDs[1]] * VSin.Weights[1];
    BoneTransform     += gBones[VSin.BoneIDs[2]] * VSin.Weights[2];
    BoneTransform     += gBones[VSin.BoneIDs[3]] * VSin.Weights[3];
 
    vec4 PosL      = BoneTransform * vec4(VSin.Position, 1.0);
    gl_Position    = gWVP * PosL;
    VSout.TexCoord = VSin.TexCoord;
    vec4 NormalL   = BoneTransform * vec4(VSin.Normal, 0.0);
    VSout.Normal   = (gWorld * NormalL).xyz;
    VSout.WorldPos = (gWorld * PosL).xyz;
}
```
## Функция CalcRunningTime() возвращает прошедшее время в секундах от начала запуска приложения
```c++
float RunningTime = CalcRunningTime();
 
m_mesh.BoneTransform(RunningTime, Transforms);
 
for (uint i = 0 ; i < Transforms.size() ; i++) {
    m_pEffect->SetBoneTransform(i, Transforms[i]);
}
```
