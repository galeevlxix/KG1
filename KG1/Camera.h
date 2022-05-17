#ifndef CAMERA_H
#define	CAMERA_H

#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/fwd.hpp>
#include "math3d.h"

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

    my_Vector2i m_mousePos;
};

#endif	/* CAMERA_H */
