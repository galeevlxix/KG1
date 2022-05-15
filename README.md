# KG2 
## Вторая лабораторная работа по компьютерной графике.
## Выполнил Галеев Тимур МО-221
# Выполнено:
## Перемещение восьмиугольника:
Создаём глобальную переменную gWorldLocation:
```c++
GLuint gWorldLocation;
```
Добавляем код шейдера:
```c++
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";
```
В функцию отрисовки добавляем:
```c++
static float Scale = 0.0f;
Scale += 0.001f;

glm::mat4  World;

World[0][0] = cosf(Scale); World[0][1] = -sinf(Scale); World[0][2] = 0.0f; World[0][3] = sinf(0.0f);
World[1][0] = sinf(Scale); World[1][1] = cosf(Scale); World[1][2] = 0.0f; World[1][3] = 0.0f;
World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = 1.0f; World[2][3] = 0.0f;
World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;

glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World[0][0]); 
```
Добавляем две новые функции:
```c++
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}
```
## Вращение треугольника вокруг оси OZ:
В функции отрисовки добавляеем матрицу rotZ:
```c++
glm::mat4 rotZ;

rotZ[0][0] = 1.0f; rotZ[0][1] = 0.0f; rotZ[0][2] = 0.0f; rotZ[0][3] = -sinf(Scale) / 2;

rotZ[1][0] = 0.0f; rotZ[1][1] = 1.0f; rotZ[1][2] = 0.0f; rotZ[1][3] = cosf(Scale) / 2;

rotZ[2][0] = 0.0f; rotZ[2][1] = 0.0f; rotZ[2][2] = 1.0f; rotZ[2][3] = 0.0f;

rotZ[3][0] = 0.0f; rotZ[3][1] = 0.0f; rotZ[3][2] = 0.0f; rotZ[3][3] = 1.0f;
```
## Вращение треугольника вокруг оси OX:
В функции отрисовки добавляеем матрицу rotX:
```c++
glm::mat4 rotX;

rotX[0][0] = 1.0f; rotX[0][1] = 0.0f; rotX[0][2] = 0.0f; rotX[0][3] = 0.0f;

rotX[1][0] = 0.0f; rotX[1][1] = 1.0f; rotX[1][2] = 0.0f; rotX[1][3] = cosf(Scale) / 2;

rotX[2][0] = 0.0f; rotX[2][1] = 0.0f; rotX[2][2] = 1.0f; rotX[2][3] = -sinf(Scale) / 2;

rotX[3][0] = 0.0f; rotX[3][1] = 0.0f; rotX[3][2] = 0.0f; rotX[3][3] = 1.0f;
```
## Преобразования масштаба:
В функции отрисовки добавляеем матрицу scalematrix:
```c++
glm::mat4 scalematrix;

scalematrix[0][0] = (sinf(Scale) + 1.5) / 3; scalematrix[0][1] = 0.0f; scalematrix[0][2] = 0.0f; scalematrix[0][3] = 0.0f;

scalematrix[1][0] = 0.0f; scalematrix[1][1] = (sinf(Scale) + 1.5) / 3; scalematrix[1][2] = 0.0f; scalematrix[1][3] = 0.0f;

scalematrix[2][0] = 0.0f; scalematrix[2][1] = 0.0f; scalematrix[2][2] = 1.0f; scalematrix[2][3] = 0.0f;

scalematrix[3][0] = 0.0f; scalematrix[3][1] = 0.0f; scalematrix[3][2] = 0.0f; scalematrix[3][3] = 1.0f;
```
## Объединение преобразований
Создадим класс с пайплайном преобразований:
```c++
class Pipeline {
public:
    Pipeline() :
        mScale(glm::vec3(1.0f, 1.0f, 1.0f)),
        mWorldPos(glm::vec3(0.0f, 0.0f, 0.0f)),
        mRotateInfo(glm::vec3(0.0f, 0.0f, 0.0f)),
        mTransformation(glm::mat4()) {}
    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
        mScale[0] = ScaleX;
        mScale[1] = ScaleY;
        mScale[2] = ScaleZ;
    }
    void WorldPos(float x, float y, float z)
    {
        mWorldPos[0] = x;
        mWorldPos[1] = y;
        mWorldPos[2] = z;
    }
    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
        mRotateInfo[0] = RotateX;
        mRotateInfo[1] = RotateY;
        mRotateInfo[2] = RotateZ;
    }
    const glm::mat4* getTransformation();

private:
    void InitScaleTransform(glm::mat4& m) const;
    void InitRotateTransform(glm::mat4& m) const;
    void InitTranslationTransform(glm::mat4& m) const;

    glm::vec3 mScale;
    glm::vec3 mWorldPos;
    glm::vec3 mRotateInfo;
    glm::mat4 mTransformation;
};
```
Реализация: 
```c++
void Pipeline::InitScaleTransform(glm::mat4& m) const
{
    m[0][0] = mScale[0]; m[0][1] = 0.0f;      m[0][2] = 0.0f;      m[0][3] = 0.0f;
    m[1][0] = 0.0f;      m[1][1] = mScale[1]; m[1][2] = 0.0f;      m[1][3] = 0.0f;
    m[2][0] = 0.0f;      m[2][1] = 0.0f;      m[2][2] = mScale[2]; m[2][3] = 0.0f;
    m[3][0] = 0.0f;      m[3][1] = 0.0f;      m[3][2] = 0.0f;      m[3][3] = 1.0f;
}

void Pipeline::InitRotateTransform(glm::mat4& m) const
{
    glm::mat4 rx, ry, rz;

    const float x = ToRadian(mRotateInfo[0]);
    const float y = ToRadian(mRotateInfo[1]);
    const float z = ToRadian(mRotateInfo[2]);

    rx[0][0] = 1.0f;        rx[0][1] = 0.0f;       rx[0][2] = 0.0f;        rx[0][3] = 0.0f;
    rx[1][0] = 0.0f;        rx[1][1] = cosf(x);    rx[1][2] = -sinf(x);    rx[1][3] = 0.0f;
    rx[2][0] = 0.0f;        rx[2][1] = sinf(x);    rx[2][2] = cosf(x);     rx[2][3] = 0.0f;
    rx[3][0] = 0.0f;        rx[3][1] = 0.0f;       rx[3][2] = 0.0f;        rx[3][3] = 1.0f;

    ry[0][0] = cosf(y);     ry[0][1] = 0.0f;       ry[0][2] = -sinf(y);    ry[0][3] = 0.0f;
    ry[1][0] = 0.0f;        ry[1][1] = 1.0f;       ry[1][2] = 0.0f;        ry[1][3] = 0.0f;
    ry[2][0] = sinf(y);     ry[2][1] = 0.0f;       ry[2][2] = cosf(y);     ry[2][3] = 0.0f;
    ry[3][0] = 0.0f;        ry[3][1] = 0.0f;       ry[3][2] = 0.0f;        ry[3][3] = 1.0f;

    rz[0][0] = cosf(z);     rz[0][1] = -sinf(z);   rz[0][2] = 0.0f;        rz[0][3] = 0.0f;
    rz[1][0] = sinf(z);     rz[1][1] = cosf(z);    rz[1][2] = 0.0f;        rz[1][3] = 0.0f;
    rz[2][0] = 0.0f;        rz[2][1] = 0.0f;       rz[2][2] = 1.0f;        rz[2][3] = 0.0f;
    rz[3][0] = 0.0f;        rz[3][1] = 0.0f;       rz[3][2] = 0.0f;        rz[3][3] = 1.0f;

    m = rz * ry * rx;
}

void Pipeline::InitTranslationTransform(glm::mat4& m) const
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = mWorldPos.x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = mWorldPos.y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = mWorldPos.z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


const glm::mat4* Pipeline::getTransformation()
{
    glm::mat4 scaleTrans, rotateTrans, translationTrans, persProjTrans;

    InitScaleTransform(scaleTrans);
    InitRotateTransform(rotateTrans);
    InitTranslationTransform(translationTrans);

    mTransformation = translationTrans * rotateTrans * scaleTrans;
    return &mTransformation;
}
```
В функции отрисовки вместо ручного задания матрицы World используем пайплайн:
```c++
Pipeline p;
p.Scale(-(sinf(Scale) + 1.5) / 3, (sinf(Scale) + 1.5) / 3, (sinf(Scale) + 1.5) / 3);
p.WorldPos(sinf(Scale), 0.0f, 0.0f);
p.Rotate(-sinf(Scale) / 2, cos(Scale) * 90.0f, -sinf(Scale) * 90.0f);
glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.getTransformation());
```
## Проекция перспективы
В main.cpp зададим параметры окна:
```c++
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
```
В пайплайн доба цвляем вспомогательные методы и переменную:
```c++
void initPerspectiveProj(glm::mat4 &m) const;

struct {
    float FOV
    float width;
    float height;
    float zNear;
    float zFar;
} mPersProj;

void perspectiveProj(float FOV, float width, float height, float zNear, float zFar) {
    mPersProj.FOV = FOV;
    mPersProj.width = width;
    mPersProj.height = height;
    mPersProj.zNear = zNear;
    mPersProj.zFar = zFar;
}
```
В методе getTransformation добавляем в умножение матрицу перспективы:
```c++
mTransformation = persProjTrans * translationTrans * rotateTrans * scaleTrans;
```
Изменим преобразования матрицы:
```c++
// Создаём pipeline для трансформаций
Pipeline p;
// Меняем масштаб
p.scale(0.1f, 0.1f, 0.1f);
// Вращаем фигуру
p.rotate(0, Scale, 0);
// Устанавливаем положение фигуры
p.worldPos(0.0f, 0.0f, 100.0f);
// Задаём проекцию перспективы
p.perspectiveProj(90.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 10.0f, 10000.0f);
```
