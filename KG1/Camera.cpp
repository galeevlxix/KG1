#include "Camera.h"
#include <GL/freeglut.h>

const static float STEP_SCALE = 0.2f;

Camera::Camera(int WindowWidth, int WindowHeight)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Vector3f(0.0f, 0.0f, 0.0f);
    m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_target.Normalize();
    m_up = Vector3f(0.0f, 1.0f, 0.0f);

    Init();
}


Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize();

    m_up = Up;
    m_up.Normalize();

    Init();
}


void Camera::Init()
{
    Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize();

    if (HTarget.z >= 0.0f)
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
        }
    }
    else
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = ToDegree(asin(-HTarget.z));
        }
        else
        {
            m_AngleH = 90.0f + ToDegree(asin(-HTarget.z));
        }
    }

    m_AngleV = -ToDegree(asin(m_target.y));

    m_lastMousePos.x = m_windowWidth / 2;
    m_lastMousePos.y = m_windowHeight / 2;

    glutWarpPointer(m_lastMousePos.x, m_lastMousePos.y);
}


bool Camera::OnKeyboard(int Key)
{
    bool Ret = false;

    switch (Key) {

    case GLUT_KEY_UP:
    {
        m_pos = m_pos + (m_target * STEP_SCALE);
        Ret = true;
    }
    break;

    case GLUT_KEY_DOWN:
    {
        m_pos = m_pos - (m_target * STEP_SCALE);
        Ret = true;
    }
    break;

    case GLUT_KEY_LEFT:
    {
        Vector3f Left = m_target.Cross(m_up);
        Left.Normalize();
        Left = Left * STEP_SCALE;
        m_pos = m_pos + Left;
        Ret = true;
    }
    break;

    case GLUT_KEY_RIGHT:
    {
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right = Right * STEP_SCALE;
        m_pos = m_pos + Right;
        Ret = true;
    }
    break;


    }

    return Ret;
}


void Camera::OnMouse(int x, int y)
{
    const int DeltaX = x - m_lastMousePos.x;
    const int DeltaY = y - m_lastMousePos.y;

    m_lastMousePos.x = x;
    m_lastMousePos.y = y;

    if(DeltaX == 0 && DeltaY == 0) return;

    m_AngleH += (float)DeltaX / 2.0f;
    m_AngleV += (float)DeltaY / 2.0f;

    Update();
}

void Camera::OnRender()
{
    bool ShouldUpdate = false;

    if (ShouldUpdate) {
        Update();
    }
}

void Camera::Update()
{
    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_AngleH, Vaxis);
    View.Normalize();

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f Haxis = Vaxis.Cross(View);
    Haxis.Normalize();
    View.Rotate(m_AngleV, Haxis);

    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(Haxis);
    m_up.Normalize();
}