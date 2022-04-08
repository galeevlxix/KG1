# KG1
## Первая лабораторная работа по компьютерной графике.
## Выполнил Галеев Тимур МО-221
## Что делает это программное обеспечение:
### Рисует в окне с помощью метода drawPolygon восьмиугольник с заданными программистом размером и углом отклонения
```c++
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/vec3.hpp"
  
GLuint VBO;

using namespace glm;

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_POLYGON, 0, 8);
    glDisableVertexAttribArray(0);
    glutSwapBuffers();
}

static void createWindow() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("eeewdewewd");
    glutDisplayFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    double r = 0.5;
    double t = 1.0;
    const double PI = 3.141592653589793;

    vec3 Vertices[8];

    for (int i = 0; i < 8; i++) {
        Vertices[i] = vec3(r * cos(t + i * PI * 0.25), r * sin(t + i * PI * 0.25), 0.0f);
    }
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    createWindow();

    GLenum res = glewInit();

    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    glutMainLoop();

    return 0;
}
```
## Обработка отображения:
```c++
glutDisplayFunc(renderScene);
```
## Указатель вершинного буфера:
```c++
GLuint VBO;
```
## Обработка буфера:
```c++
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
```
## Инициализация окна:
```c++
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
glutInitWindowSize(1000, 1000);
glutInitWindowPosition(100, 100);
glutCreateWindow("eeewdewewd");
```
