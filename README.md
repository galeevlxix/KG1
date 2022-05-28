# 3 и 4 лабораторные работы по компьютерной графике
## Выполнил Галеев Тимур МО-221
# Выполнено:
## Пространство камеры, управление камерой
### Рефакторинг класса Pipeline
```c++
void SetCamera(const my_Vector3f Pos, const my_Vector3f Target, const my_Vector3f Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }
```
```c++
struct {
        my_Vector3f Pos;
        my_Vector3f Target;
        my_Vector3f Up;
    } m_camera;
```
```c++
const Matrix4f* Pipeline::getTransformation()
{
    Matrix4f scaleTrans, rotateTrans, translationTrans, CameraTranslationTrans, CameraRotateTrans, persProjTrans;

    scaleTrans.InitScaleTransform(mScale.x, mScale.y, mScale.z);
    rotateTrans.InitRotateTransform(mRotateInfo.x, mRotateInfo.y, mRotateInfo.z);
    translationTrans.InitTranslationTransform(mWorldPos.x, mWorldPos.y, mWorldPos.z);
    CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
    CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);
    persProjTrans.InitPersProjTransform(mPersProj.FOV, mPersProj.width, mPersProj.height, mPersProj.zNear, mPersProj.zFar);
    
    mTransformation = persProjTrans * CameraRotateTrans * CameraTranslationTrans * translationTrans * rotateTrans * scaleTrans;
    return &mTransformation;
}
```
### Создание класса камеры
```c++
class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const my_Vector3f& Pos, const my_Vector3f& Target, const my_Vector3f& Up);

    bool OnKeyboard(int Key);

    void OnMouse(int x, int y);

    void OnRender();

    const my_Vector3f& GetPos() const
    {
        return m_pos;
    }

    const my_Vector3f& GetTarget() const
    {
        return m_target;
    }

    const my_Vector3f& GetUp() const
    {
        return m_up;
    }

private:

    void Init();
    void Update();

    my_Vector3f m_pos;
    my_Vector3f m_target;
    my_Vector3f m_up;

    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    my_Vector2i m_mousePos;
};
```
### Задание callback функций для обработки нажатий на клавиши и движения мыши
```c++
static void SpecialKeyboardCB(int Key, int x, int y)
{
    pGameCamera->OnKeyboard(Key);
}

static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
    case 'q':
        exit(0);
    }
}

static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}
```
## Основы наложения текстур
### Создание класса Texture
```c++
class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    bool Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    Image* m_pImage;
    Blob m_blob;
};
```
### Обновляем вершинный шейдер
```c++
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
}";
```
### Обновляем фрагментный шейдер
```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture2D(gSampler, TexCoord0.xy);                                  \n\
}";
```
### Теперь каждая вершина привязана к координате на картинке
```c++
struct Vertex
{
    glm::vec3 m_pos;
    glm::vec2 m_tex;

    Vertex() {}

    Vertex(glm::vec3 pos, glm::vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};
```
### Поменяем нашу фигуру
Теперь это куб, грани которого имеют разные текстуры 
### Изменим вершинный буфер
```c++
void CreateVertexBuffer(const unsigned int* pIndices, unsigned int IndexCount)
    {
        Vertex Vertices[24] = { //вершины куба
        Vertex(my_Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.499f, 0.6666f)),     // 0 0  верхняя левая ближняя 
        Vertex(my_Vector3f(1.0f, 1.0f, 1.0f), vec2(0.251f, 0.6666f)),      // 1 1  верхняя правая ближняя
        Vertex(my_Vector3f(-1.0f, -1.0f, 1.0f), vec2(0.499f, 1.0f)),    // 2 2  нижняя левая ближняя
        Vertex(my_Vector3f(1.0f, -1.0f, 1.0f), vec2(0.251f, 1.0f)),     // 3 3  нижняя правая ближняя

        Vertex(my_Vector3f(1.0f, 1.0f, 1.0f), vec2(0.499f, 0.6666f)),      // 1 4  верхняя правая ближняя
        Vertex(my_Vector3f(1.0f, 1.0f, -1.0f), vec2(0.251f, 0.6666f)),     // 5 5  верхняя правая дальняя
        Vertex(my_Vector3f(1.0f, -1.0f, 1.0f), vec2(0.499f, 1.0f)),     // 3 6  нижняя правая ближняя
        Vertex(my_Vector3f(1.0f, -1.0f, -1.0f), vec2(0.251f, 1.0f)),    // 7 7  нижняя правая дальняя

        Vertex(my_Vector3f(1.0f, 1.0f, -1.0f), vec2(0.499f, 0.6666f)),     // 5 8  верхняя правая дальняя
        Vertex(my_Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.251f, 0.6666f)),    // 4 9  верхняя левая дальняя
        Vertex(my_Vector3f(1.0f, -1.0f, -1.0f), vec2(0.499f, 1.0f)),    // 7 10  нижняя правая дальняя
        Vertex(my_Vector3f(-1.0f, -1.0f, -1.0f), vec2(0.251f, 1.0f)),   // 6 11  нижняя левая дальняя

        Vertex(my_Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.499f, 0.6666f)),    // 4 12  верхняя левая дальняя
        Vertex(my_Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.251f, 0.6666f)),     // 0 13  верхняя левая ближняя
        Vertex(my_Vector3f(-1.0f, -1.0f, -1.0f), vec2(0.499f, 1.0f)),   // 6 14  нижняя левая дальняя
        Vertex(my_Vector3f(-1.0f, -1.0f, 1.0f), vec2(0.251f, 1.0f)),    // 2 15  нижняя левая ближняя

        Vertex(my_Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.25f, 0.3333f)),    // 4 16  верхняя левая дальняя
        Vertex(my_Vector3f(1.0f, 1.0f, -1.0f), vec2(0.5f, 0.3333f)),     // 5 17  верхняя правая дальняя
        Vertex(my_Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.25f, 0.6666f)),     // 0 18  верхняя левая ближняя
        Vertex(my_Vector3f(1.0f, 1.0f, 1.0f), vec2(0.5f, 0.6666f)),      // 1 19  верхняя правая ближняя

        Vertex(my_Vector3f(-1.0f, -1.0f, 1.0f), vec2(1.0f, 0.3333f)),    // 2 20  нижняя левая ближняя
        Vertex(my_Vector3f(1.0f, -1.0f, 1.0f), vec2(0.75f, 0.3333f)),     // 3 21  нижняя правая ближняя
        Vertex(my_Vector3f(-1.0f, -1.0f, -1.0f), vec2(1.0f, 0.6666f)),   // 6 22  нижняя левая дальняя
        Vertex(my_Vector3f(1.0f, -1.0f, -1.0f), vec2(0.75f, 0.6666f)),    // 7 23  нижняя правая дальняя
        };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }
```
### Также изменим индексный буфер
```c++
void CreateIndexBuffer()
    {
        unsigned int Indices[] = { // грани куба
                        // ближняя
                        1, 3, 0,
                        0, 3, 2,

                        // дальняя
                        9, 11, 8,
                        8, 11, 10,

                        // левая
                        13, 15, 12,
                        12, 15, 14,

                        // правая
                        5, 7, 4,
                        4, 7, 6,

                        // верхняя
                        17, 19, 16,
                        16, 19, 18,

                        // нижняя
                        22, 20, 23,
                        23, 20, 21 };

        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    }
```
### Отрисовка куба в цикле рендера 
```c++
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    pTexture->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
```
### Добавим в функцию main методы для оптимизации текстурирования
```c++
glFrontFace(GL_FRONT);
glCullFace(GL_BACK);
glEnable(GL_CULL_FACE);
```
### Здесь мы устанавливаем индексы модулей текстуры, который мы собираемся использовать внутри сэмплера uniform-переменной в шейдере
```c++
glUniform1i(gSampler, 0);
```
### Здесь мы создаем объект текстуры и загружаем его
```c++
pTexture = new Texture(GL_TEXTURE_2D, "C:\\tnt.png");

    if (!pTexture->Load()) {
        return 1;
    }
```
## Результат
![16](https://github.com/galeevlxix/KG1/raw/laba34/16.png)

## Фоновое освещение
### Реорганизируем старый код по разным файлам, что позволит сделать добавление нового кода в будущем проще и удобнее
### Создание класса ICallbacks
```c++
class ICallbacks
{
public:
    virtual void SpecialKeyboardCB(int Key, int x, int y) = 0;
    virtual void KeyboardCB(unsigned char Key, int x, int y) = 0;
    virtual void PassiveMouseCB(int x, int y) = 0;
    virtual void RenderSceneCB() = 0;
    virtual void IdleCB() = 0;
};
```
### Почти весь код, затрагивающий GLUT, был перемещен в компоненту glut_backend.h
```c++

void GLUTBackendInit(int argc, char** argv);

bool GLUTBackendCreateWindow(unsigned int Width, unsigned int Height, unsigned int bpp, bool isFullScreen, const char* pTitle);

void GLUTBackendRun(ICallbacks* pCallbacks);
```
### glut_backend.cpp
```c++
#include "glut_backend.h"

static ICallbacks* s_pCallbacks = NULL;

static void SpecialKeyboardCB(int Key, int x, int y) {
    s_pCallbacks->SpecialKeyboardCB(Key, x, y);
}

static void KeyboardCB(unsigned char Key, int x, int y) {
    s_pCallbacks->KeyboardCB(Key, x, y);
}

static void PassiveMouseCB(int x, int y) {
    s_pCallbacks->PassiveMouseCB(x, y);
}

static void RenderSceneCB() {
    s_pCallbacks->RenderSceneCB();
}

static void IdleCB() {
    s_pCallbacks->IdleCB();
}

static void InitCallbacks() {
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(IdleCB);
    glutSpecialFunc(SpecialKeyboardCB);
    glutPassiveMotionFunc(PassiveMouseCB);
    glutKeyboardFunc(KeyboardCB);
}

void GLUTBackendInit(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
}

bool GLUTBackendCreateWindow(unsigned int Width, unsigned int Height, unsigned int bpp, bool isFullScreen, const char* pTitle) {
    if (isFullScreen) {
        char ModeString[64] = { 0 };
        snprintf(ModeString, sizeof(ModeString), "%dx%d@%d", Width, Height, bpp);
        glutGameModeString(ModeString);
        glutEnterGameMode();
    }
    else {
        glutInitWindowSize(Width, Height);
        glutCreateWindow(pTitle);
    }

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }

    return true;
}

void GLUTBackendRun(ICallbacks* pCallbacks) {
    if (!pCallbacks) {
        fprintf(stderr, "%s : callbacks not specified!\n", __FUNCTION__);
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_FRONT);
    glCullFace(GL_BACK);

    glEnable(GL_CULL_FACE);

    s_pCallbacks = pCallbacks;
    InitCallbacks();
    glutMainLoop();
}
```
### Создание класса Technique, выполняющего работу по компиляции и линковки шейдеров
```c++
class Technique
{
public:
    Technique();
    ~Technique();
    virtual bool Init();
    void Enable();

protected:
    bool AddShader(GLenum ShaderType, const char* pShaderText);
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);

private:
    GLuint m_shaderProg;
    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};
```
### Создание структуры DirectionLight для хранения 2 полей - цвета и фоновой интенсивности
```c++
struct DirectionLight
{
    my_Vector3f Color;
    float AmbientIntensity;
};
```
### Создание класса lighting_technique
```c++
class LightingTechnique : public Technique
{
public:
    LightingTechnique();
    virtual bool Init();

    void SetWVP(const Matrix4f* WVP);
    void SetTextureUnit(unsigned int TextureUnit);
    void SetDirectionalLight(const DirectionLight& Light);

private:
    GLuint m_WVPLocation;
    GLuint m_samplerLocation;
    GLuint m_dirLightColorLocation;
    GLuint m_dirLightAmbientIntensityLocation;
};
```
### Изменения во фрагментном шейдере
```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                vec4(gDirectionalLight.Color, 1.0f) *                               \n\
                gDirectionalLight.AmbientIntensity;                                 \n\
}";
```
### В цикл рендера добавляем
```c++
m_pEffect->SetWVP(p.getTransformation());
m_pEffect->SetDirectionalLight(directionalLight);
```
### Добавляем возможность менять интенсивность света окружающей среды через клавиатуру
```c++
case 'a':
    directionalLight.AmbientIntensity += 0.05f;
    break;

case 's':
    directionalLight.AmbientIntensity -= 0.05f;
    break;
```
## Результат
![17](https://github.com/galeevlxix/KG1/raw/laba34/17.png)
## Рассеянное освещение
### Появились 2 новых члена: направление в виде 3 вектора, указываемое в мировом пространстве, и интенсивность как вещественное число
```c++
struct DirectionLight
{
    my_Vector3f Color;
    float AmbientIntensity;
    my_Vector3f Direction;
    float DiffuseIntensity;
};
```
### Обновленный вершинный шейдер
```c++
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
    Normal0 = (gWorld * vec4(Normal, 0.0)).xyz;                                     \n\
}";
```
### Обновленный фрагментный шейдер
```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    vec3 Direction;                                                                 \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);    \n\
                                                                                    \n\
    vec4 DiffuseColor;                                                              \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        \n\
                       gDirectionalLight.DiffuseIntensity *                         \n\
                       DiffuseFactor;                                               \n\
    }                                                                               \n\
    else{                                                                           \n\
        DiffuseColor = vec4(0,0,0,0);                                               \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                (AmbientColor + DiffuseColor);                                      \n\
}";
```
### Функция SetDirectionalLight назначает параметры направленного света в шейдере
```c++
void LightingTechnique::SetDirectionalLight(const DirectionLight& Light)
{
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    my_Vector3f Direction = Light.Direction;
    Direction.Normalize();
    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}
```
### Обновленная структура Vertex теперь включает нормали
```c++
struct Vertex
{
    my_Vector3f m_pos;
    vec2 m_tex;
    my_Vector3f m_normal;

    Vertex() {}

    Vertex(my_Vector3f pos, vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = my_Vector3f(0.0f, 0.0f, 0.0f);
    }
};
```
### Функция CalcNormals принимает массив индексов, получает вершины треугольников, полагаясь на них, и вычисляет нормали
```c++
void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount,
        Vertex* pVertices, unsigned int VertexCount) {
        for (unsigned int i = 0; i < IndexCount; i += 3) {
            unsigned int Index0 = pIndices[i];
            unsigned int Index1 = pIndices[i + 1];
            unsigned int Index2 = pIndices[i + 2];
            my_Vector3f v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
            my_Vector3f v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
            my_Vector3f Normal = v1.Cross(v2);
            Normal.Normalize();

            pVertices[Index0].m_normal = pVertices[Index0].m_normal + Normal;
            pVertices[Index1].m_normal = pVertices[Index1].m_normal + Normal;
            pVertices[Index2].m_normal = pVertices[Index2].m_normal + Normal;
        }

        for (unsigned int i = 0; i < VertexCount; i++) {
            pVertices[i].m_normal.Normalize();
        }
    }
```
### Изменения в цикле рендера
```c++
virtual void RenderSceneCB()
    {
        pGameCamera->OnRender();
        glClearColor(0.53f, 0.33f, 0.75f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Scale += 0.001f;

        Pipeline p;
        
        p.Rotate(0.0f, Scale * 50, 20 * sinf(Scale));
        p.WorldPos(sinf(Scale), sinf(Scale) * sinf(Scale) - 2.0f, 5.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

        m_pEffect->SetWVP(p.GetWVPTrans());
        const Matrix4f& WorldTransformation = p.getTransformation();

        m_pEffect->SetWorldMatrix(WorldTransformation);

        m_pEffect->SetDirectionalLight(directionalLight);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        pTexture->Bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glutSwapBuffers();
    }
```
## Результат
![18](https://github.com/galeevlxix/KG1/raw/laba34/18.png)
## Отраженный свет
### Добавим 3 новых свойства у LightingTechnique - позиция глаза, интенсивность отражения и коэффициент материала
```c++
class LightingTechnique : public Technique
{
public:
    LightingTechnique();
    virtual bool Init();

    void SetWVP(const Matrix4f& WVP);
    void SetWorldMatrix(const Matrix4f& WVP);
    void SetTextureUnit(unsigned int TextureUnit);
    void SetDirectionalLight(const DirectionLight& Light);

    void SetEyeWorldPos(const my_Vector3f& EyeWorldPos);
    void SetMatSpecularIntensity(float Intensity);
    void SetMatSpecularPower(float Power);

private:
    GLuint m_WVPLocation;
    GLuint m_WorldMatrixLocation;
    GLuint m_samplerLocation;

    GLuint m_eyeWorldPosition;
    GLuint m_matSpecularIntensityLocation;
    GLuint m_matSpecularPowerLocation;

    struct {
        GLuint Color;
        GLuint AmbientIntensity;
        GLuint Direction;
        GLuint DiffuseIntensity;
    } m_dirLightLocation;
};
```
### Обновленный вершинный шейдер
```c++
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 WorldPos0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
    Normal0   = (gWorld * vec4(Normal, 0.0)).xyz;                                   \n\
    WorldPos0 = (gWorld * vec4(Position, 1.0)).xyz;                                 \n\
}";
```
### Обновленный фрагментный шейдер
```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    vec3 Direction;                                                                 \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
uniform DirectionalLight gDirectionalLight;                                         \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
uniform vec3 gEyeWorldPos;                                                          \n\
uniform float gMatSpecularIntensity;                                                \n\
uniform float gSpecularPower;                                                       \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *                       \n\
                        gDirectionalLight.AmbientIntensity;                         \n\
                                                                                    \n\
    vec3 LightDirection = -gDirectionalLight.Direction;                             \n\
    vec3 Normal = normalize(Normal0);                                               \n\
                                                                                    \n\
    float DiffuseFactor = dot(Normal, LightDirection);                              \n\
                                                                                    \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                          \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                          \n\
                                                                                    \n\
    if (DiffuseFactor > 0){                                                         \n\
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        \n\
                       gDirectionalLight.DiffuseIntensity *                         \n\
                       DiffuseFactor;                                               \n\
                                                                                    \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                     \n\
        vec3 LightReflect = normalize(reflect(gDirectionalLight.Direction, Normal));\n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                      \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                       \n\
                                                                                    \n\
        if (SpecularFactor > 0){                                                    \n\
            SpecularColor = vec4(gDirectionalLight.Color, 1.0f) *                   \n\
                            gMatSpecularIntensity *                                 \n\
                            SpecularFactor;                                         \n\
        }                                                                           \n\
    }                                                                               \n\
                                                                                    \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) *                                 \n\
                (AmbientColor + DiffuseColor + SpecularColor);                      \n\
}";
```
### Добавим в цикл рендера
```c++
m_pEffect->SetEyeWorldPos(pGameCamera->GetPos());
m_pEffect->SetMatSpecularIntensity(1.0f);
m_pEffect->SetMatSpecularPower(32);
```
## Точечный источник света
### Создание структуры BaseLight
```c++
struct BaseLight
{
    my_Vector3f Color;
    float AmbientIntensity;
    float DiffuseIntensity;

    BaseLight()
    {
        Color = my_Vector3f(0.0f, 0.0f, 0.0f);
        AmbientIntensity = 0.0f;
        DiffuseIntensity = 0.0f;
    }
};
```
### DirectionLight наследуется от BaseLight
```c++
struct DirectionLight : public BaseLight
{
    my_Vector3f Direction;

    DirectionLight()
    {
        Direction = my_Vector3f(0.0f, 0.0f, 0.0f);
    }
};
```
### Создание структуры PointLight, который наследуется от BaseLight
```c++
struct PointLight : public BaseLight
{
    my_Vector3f Position;

    struct
    {
        float Constant;
        float Linear;
        float Exp;
    } Attenuation;

    PointLight()
    {
        Position = my_Vector3f(0.0f, 0.0f, 0.0f);
        Attenuation.Constant = 1.0f;
        Attenuation.Linear = 0.0f;
        Attenuation.Exp = 0.0f;
    }
};
```
### Функция SetPointLights принимает массив структур PointLight и размер массива и обновляет соответствующие значения в шейдере
```c++
void SetPointLights(unsigned int NumLights, const PointLight* pLights);
```
Эта функция обновляет шейдер с значениями точечных источников через перебор элементов массива и передачи каждого атрибута элемента одного за другим.
```c++
void LightingTechnique::SetPointLights(unsigned int NumLights, const PointLight* pLights)
{
    glUniform1i(m_numPointLightsLocation, NumLights);

    for (unsigned int i = 0; i < NumLights; i++) {
        glUniform3f(m_pointLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
        glUniform1f(m_pointLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
        glUniform1f(m_pointLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
        glUniform3f(m_pointLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
        glUniform1f(m_pointLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
        glUniform1f(m_pointLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
        glUniform1f(m_pointLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
    }
}
```
### Обновленный фрагментный шейдер. 
Функция CalcLightInternal инкапсулирует общую часть между 2 типами света. Структура BaseLight хранит интенсивность и цвет. LightDirection подается отдельно по причине выше. Вершинная нормаль так же предоставляется потому, что мы нормируем ее только при входе в фрагментный шейдер и используем во многих вызовах в этой функции.
Функция CalcPointLight вычисления точечного источника света будет подсчитана для каждого настроенного точечного источника света, поэтому она принимает индекс света в качестве и использует его в глобальном массиве. Она вычисляет вектор из источника света в мировую позицию, полученную из вершинного шейдера.
```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
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
uniform int gNumPointLights;                                                                \n\
uniform DirectionalLight gDirectionalLight;                                                 \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          \n\
uniform sampler2D gSampler;                                                                 \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
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
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal); \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcPointLight(int Index, vec3 Normal)                                                 \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - gPointLights[Index].Position;                         \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(gPointLights[Index].Base, LightDirection, Normal);       \n\
    float Attenuation =  gPointLights[Index].Atten.Constant +                               \n\
                         gPointLights[Index].Atten.Linear * Distance +                      \n\
                         gPointLights[Index].Atten.Exp * Distance * Distance;               \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(i, Normal);                                            \n\
    }                                                                                       \n\
                                                                                            \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight;                             \n\
}";
```
### В цикле рендера
```c++
PointLight pl[3];
pl[0].DiffuseIntensity = 0.5f;
pl[0].Color = my_Vector3f(1.0f, 0.0f, 0.0f);
pl[0].Position = my_Vector3f(sinf(Scale) * 10, 1.0f, cosf(Scale) * 10);
pl[0].Attenuation.Linear = 0.1f;

pl[1].DiffuseIntensity = 0.5f;
pl[1].Color = my_Vector3f(0.0f, 1.0f, 0.0f);
pl[1].Position = my_Vector3f(sinf(Scale + 2.1f) * 10, 1.0f, cosf(Scale + 2.1f) * 10);
pl[1].Attenuation.Linear = 0.1f;

pl[2].DiffuseIntensity = 0.5f;
pl[2].Color = my_Vector3f(0.0f, 0.0f, 1.0f);
pl[2].Position = my_Vector3f(sinf(Scale + 4.2f) * 10, 1.0f, cosf(Scale + 4.2f) * 10);
pl[2].Attenuation.Linear = 0.1f;

m_pEffect->SetPointLights(3, pl);
```
## Результат
![20](https://github.com/galeevlxix/KG1/raw/laba34/20.png)
## Прожектор
### Обновленный фрагментный шейдер
Структура SpotLight, которая определяет прожектор, наследуется от точечного света и добавляет 3 новых параметра:структуру PointLight, вектор направления и значение отсекания.
Функция CalcPointLight теперь принимает экземпляр структуры PointLight в качестве параметра вместо обращения к глобальному массиву.

```c++
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
const int MAX_SPOT_LIGHTS = 2;                                                      \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
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
uniform sampler2D gSampler;                                                                 \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
                                                                                            \n\
vec4 CalcLightInternal( BaseLight Light, vec3 LightDirection, vec3 Normal)            \n\
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
vec4 CalcPointLight( PointLight l, vec3 Normal)                                       \n\
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
vec4 CalcSpotLight( SpotLight l, vec3 Normal)                                         \n\
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
    FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight;                             \n\
}";
```
### Функция SetSpotLights обновляет программу шейдера массивом структур SpotLight. 
```c++
void LightingTechnique::SetSpotLights(unsigned int NumLights, const SpotLight* pLights)
{
    glUniform1i(m_numSpotLightsLocation, NumLights);
 
    for (unsigned int i = 0 ; i < NumLights ; i++) {
        glUniform3f(m_spotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
        glUniform1f(m_spotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
        glUniform1f(m_spotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
        glUniform3f(m_spotLightsLocation[i].Position,  pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
        Vector3f Direction = pLights[i].Direction;
        Direction.Normalize();
        glUniform3f(m_spotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
        glUniform1f(m_spotLightsLocation[i].Cutoff, cosf(ToRadian(pLights[i].Cutoff)));
        glUniform1f(m_spotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
        glUniform1f(m_spotLightsLocation[i].Atten.Linear,   pLights[i].Attenuation.Linear);
        glUniform1f(m_spotLightsLocation[i].Atten.Exp,      pLights[i].Attenuation.Exp);
    }
}
```
### В цикле рендера
```c++
SpotLight sl[2];
sl[0].DiffuseIntensity = 15.0f;
sl[0].Color = my_Vector3f(1.0f, 1.0f, 0.7f);
sl[0].Position = my_Vector3f(0.0f, 0.0f, 0.0f);
sl[0].Direction = my_Vector3f(sinf(Scale), 0.0f, cosf(Scale));
sl[0].Attenuation.Linear = 0.1f;
sl[0].Cutoff = 20.0f;

sl[1].DiffuseIntensity = 5.0f;
sl[1].Color = my_Vector3f(0.0f, 1.0f, 1.0f);
sl[1].Position = pGameCamera->GetPos();
sl[1].Direction = pGameCamera->GetTarget();
sl[1].Attenuation.Linear = 0.1f;
sl[1].Cutoff = 10.0f;

m_pEffect->SetSpotLights(2, sl);
```       
## Результат
![21new](https://github.com/galeevlxix/KG1/raw/laba34/21new.png)
