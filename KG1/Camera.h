#ifndef CAMERA_H
#define	CAMERA_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#define M_PI       3.14159265358979323846

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

using namespace glm;

class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const vec3& Pos, const vec3& Target, const vec3& Up);

    bool OnKeyboard(int Key);

    void OnMouse(int x, int y);

    void OnRender();

    const vec3& GetPos() const
    {
        return m_pos;
    }

    const vec3& GetTarget() const
    {
        return m_target;
    }

    const vec3& GetUp() const
    {
        return m_up;
    }

private:

    void Init();
    void Update();

    vec3 m_pos;
    vec3 m_target;
    vec3 m_up;

    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    vec2 m_lastMousePos;
};

#endif	/* CAMERA_H */
