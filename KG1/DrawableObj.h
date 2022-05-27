#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include "math3d.h"
#include <math.h>

#include "Texture.h"

using namespace std;
using namespace glm;
#pragma once

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

class Drawable
{
public:
	GLuint VBO;
    int drawType;

    glm::mat4 translateTransform;
    glm::mat4 rotateTransform;
    glm::mat4 scaleTransform;

	Drawable();

    virtual void paint();
    virtual void paintMid() = 0;
    virtual void update() = 0;
    virtual glm::mat4 getTrans();
};

class Point : public Drawable {
public:

	Point(glm::vec3& v);


	virtual void update() override;
	virtual void paintMid() override;
};

class Smth : public Drawable {

public:

	int pointNum;
	Smth(std::vector<glm::vec3>& v, int drawType_ = GL_TRIANGLE_FAN);


	virtual void update() override;
	virtual void paintMid() override;
};

class SmthIndex : public Drawable {
public:
	GLuint IBO; // Èíäåêñû âåðøèí

	Texture* texture;



	int drawType;
	int pointNum;
	int indexNum;
	SmthIndex(std::vector<Vertex>& v, std::vector<int>& indices, int drawType_ = GL_TRIANGLE_FAN);


	virtual void update() override;
	virtual void paintMid() override;
};


static class Creator {
public:
	static Drawable* circle(float radX, float radY, int cnt = 20);
	static Drawable* Obeme();
};

class DrawableGroup : public Drawable {
public:
	vector<Drawable*> objs;
	void add(Drawable* obj);
	void paintMid() override;
	void update() override;
};



class Sphere1 : public DrawableGroup {
public:
	Sphere1();
	void update() override;
};
#endif