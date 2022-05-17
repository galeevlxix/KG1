#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

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


GLuint VBO;
GLuint IBO;
GLuint gWVPLocation;
GLuint gSampler;
Texture* pTexture = NULL;
Camera* pGameCamera = NULL;

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

static void RenderSceneCB()
{
    pGameCamera->OnRender();
    glClearColor(0.53f, 0.33f, 0.75f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

    Scale += 0.001f;

    Pipeline p;
    p.Rotate(0.0f, Scale * 50 , 20 * sinf(Scale));
    p.WorldPos(sinf(Scale), sinf(Scale) * sinf(Scale), 5.0f);
    p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
    p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.getTransformation());

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

    glutSwapBuffers();
}


static void SpecialKeyboardCB(int Key, int x, int y)
{
    pGameCamera->OnKeyboard(Key);
}


static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
    case 'q':
        glutLeaveMainLoop();
    }
}


static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
    glutSpecialFunc(SpecialKeyboardCB);
    glutPassiveMotionFunc(PassiveMouseCB);
    glutKeyboardFunc(KeyboardCB);
}


static void CreateVertexBuffer()
{
    Vertex Vertices[24] = { 

        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),     // 0 0  верхн€€ лева€ ближн€€ 
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),      // 1 1  верхн€€ права€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),    // 2 2  нижн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),     // 3 3  нижн€€ права€ ближн€€

        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),      // 1 4  верхн€€ права€ ближн€€
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),     // 5 5  верхн€€ права€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),     // 3 6  нижн€€ права€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f)),    // 7 7  нижн€€ права€ дальн€€

        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),     // 5 8  верхн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),    // 4 9  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f)),    // 7 10  нижн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f)),   // 6 11  нижн€€ лева€ дальн€€

        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),    // 4 12  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),     // 0 13  верхн€€ лева€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f)),   // 6 14  нижн€€ лева€ дальн€€
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),    // 2 15  нижн€€ лева€ ближн€€

        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(1.0f, 0.0f)),    // 4 16  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.0f, 0.0f)),     // 5 17  верхн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),     // 0 18  верхн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)),      // 1 19  верхн€€ права€ ближн€€

        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),    // 2 20  нижн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),     // 3 21  нижн€€ права€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 1.0f)),   // 6 22  нижн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.0f, 1.0f)),    // 7 23  нижн€€ права€ дальн€€
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}


static void CreateIndexBuffer()
{
    unsigned int Indices[] = { // грани куба
                        // ближн€€
                        1, 3, 0,
                        0, 3, 2,

                        // дальн€€
                        9, 11, 8,
                        8, 11, 10,

                        // лева€
                        13, 15, 12,
                        12, 15, 14,

                        // права€
                        5, 7, 4,
                        4, 7, 6,

                        // верхн€€
                        17, 19, 16,
                        16, 19, 18,

                        // нижн€€
                        22, 20, 23,
                        23, 20, 21 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}


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

    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);
    gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    assert(gSampler != 0xFFFFFFFF);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 16");
    glutGameModeString("1280x1024@32");
    glutEnterGameMode();

    InitializeGlutCallbacks();

    pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFrontFace(GL_FRONT);
    glCullFace(GL_BACK);

    glEnable(GL_CULL_FACE);

    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    glUniform1i(gSampler, 0);

    pTexture = new Texture(GL_TEXTURE_2D, "C:\\PickerChan.png");

    if (!pTexture->Load()) {
        return 1;
    }

    glutMainLoop();

    return 0;
}
