#include "Camera.h"


const static float STEP_SCALE = 0.1f;

Camera::Camera(int WindowWidth, int WindowHeight)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = vec3(0.0f, 0.0f, 0.0f);
    m_target = vec3(0.0f, 0.0f, 1.0f);
    normalize(m_target);
    m_up = vec3(0.0f, 1.0f, 0.0f);

    Init();
}


Camera::Camera(int WindowWidth, int WindowHeight, const vec3& Pos, const vec3& Target, const vec3& Up)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    normalize(m_target);

    m_up = Up;
    normalize(m_up);

    Init();
}


void Camera::Init()
{
    vec3 HTarget(m_target.x, 0.0, m_target.z);
    normalize(HTarget);

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
        vec3 Left = normalize(cross(m_target, m_up));
        Left = Left * STEP_SCALE;
        m_pos = m_pos + Left;
        Ret = true;
    }
    break;

    case GLUT_KEY_RIGHT:
    {
        vec3 Right = normalize(cross(m_up, m_target));
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
    const vec3 Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    vec3 View(1.0f, 0.0f, 0.0f);

    View = rotate(View, radians(m_AngleH), Vaxis);
    View = normalize(View);

    // Rotate the view vector by the vertical angle around the horizontal axis
    vec3 Haxis = normalize(cross(Vaxis,View));
    View = rotate(View, radians(m_AngleV), Haxis);

    m_target = normalize(View);

    m_up = normalize(cross(m_target, Haxis));
}