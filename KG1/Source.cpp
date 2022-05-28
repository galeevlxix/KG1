#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "pipeline.h"

#define WINDOW_WIDTH 1024.0f
#define WINDOW_HEIGHT 800.0f

GLuint VBO;
GLuint gWorldLocation;

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

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
    glClearColor(0.53f, 0.33f, 0.75f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);

    static float Scale = 0.0f;
    Scale += 0.001f;

    Pipeline p;
   
    p.Scale(cos(Scale * 0.5), sinf(Scale * 0.5), 0.0f);
    p.WorldPos(sinf(Scale) / 2, cosf(Scale) / 2, 0.0f);
    p.Rotate(1.0f, 1.0f, 1.0f);

    p.PerspectiveProj(100.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 200.0f);
    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.getTransformation());

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_POLYGON, 0, 8);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();//
}

static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    double r = 0.5;
    double t = 1.0;
    const double PI = 3.141592653589793;

    glm::vec3 Vertices[8];

    for (int i = 0; i < 8; i++) {
        Vertices[i] = glm::vec3(r * cos(t + i * PI * 0.25), r * sin(t + i * PI * 0.25), 0.0f);
    }
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
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

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("rewrewregf");

    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    glutMainLoop();

    return 0;
}