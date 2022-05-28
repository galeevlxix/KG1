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
        mScale(Vector3f(1.0f, 1.0f, 1.0f)),
        mWorldPos(Vector3f(0.0f, 0.0f, 0.0f)),
        mRotateInfo(Vector3f(0.0f, 0.0f, 0.0f)){}

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
    const Matrix4f& getTransformation();

    void SetCamera(const Vector3f Pos, const Vector3f Target, const Vector3f Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }

    const Matrix4f& GetWVPTrans();

private:
    Vector3f mScale;
    Vector3f mWorldPos;
    Vector3f mRotateInfo;
    Matrix4f mTransformation;
    Matrix4f m_WVPtransformation;

    struct {
        float FOV;
        float width;
        float height;
        float zNear;
        float zFar;
    } mPersProj;

    struct {
        Vector3f Pos;
        Vector3f Target;
        Vector3f Up;
    } m_camera;
};
#endif