#ifndef PIPLINE_H
#define PIPLINE_H
#include "math3d.h"
#include <cmath>
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/fwd.hpp>

class Pipeline {
public:
    Pipeline() :
        mScale(my_Vector3f(1.0f, 1.0f, 1.0f)),
        mWorldPos(my_Vector3f(0.0f, 0.0f, 0.0f)),
        mRotateInfo(my_Vector3f(0.0f, 0.0f, 0.0f)){}

    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
        mScale.x = ScaleX;
        mScale.y = ScaleY;
        mScale.z = ScaleZ;
    }
    void WorldPos(float x, float y, float z)
    {
        mWorldPos.x = x;
        mWorldPos.y = y;
        mWorldPos.z = z;
    }
    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
        mRotateInfo.x = RotateX;
        mRotateInfo.y = RotateY;
        mRotateInfo.z = RotateZ;
    }
    void PerspectiveProj(float FOV, float width, float height, float zNear, float zFar) {
        mPersProj.FOV = FOV;
        mPersProj.width = width;
        mPersProj.height = height;
        mPersProj.zNear = zNear;
        mPersProj.zFar = zFar;
    }
    const Matrix4f* getTransformation();

    void SetCamera(const my_Vector3f Pos, const my_Vector3f Target, const my_Vector3f Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }

private:
    my_Vector3f mScale;
    my_Vector3f mWorldPos;
    my_Vector3f mRotateInfo;
    Matrix4f mTransformation;

    struct {
        float FOV;
        float width;
        float height;
        float zNear;
        float zFar;
    } mPersProj;

    struct {
        my_Vector3f Pos;
        my_Vector3f Target;
        my_Vector3f Up;
    } m_camera;
};
#endif