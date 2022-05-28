#ifndef OBJECT_H
#define OBJECT_H

#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "camera.h"
#include "pipeline.h"
#include "texture.h"
#include "Utils.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

using namespace glm;
using namespace std;

struct Vertex
{
    vec3 m_pos;
    vec2 m_tex;
    vec3 m_normal;
    

    Vertex() {}

    Vertex(vec3 pos, vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
        m_normal = vec3(0.0f, 0.0f, 0.0f);
    }
};

class Object
{
public:
    GLuint VBO;
    GLuint IBO;
    Texture* texture;

    void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount);
};

class Cube : public Object {
public:
    void CreateBuffer();
    void Render();
};

class Floor : public Object {
public:
    void CreateBuffer();
    void Render();
};
#endif
