#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/fwd.hpp>

#define ToRadian(x) ((x) * M_PI / 180.0f)

class Pipeline {
public:
    Pipeline() :
        mScale(glm::vec3(1.0f, 1.0f, 1.0f)),
        mWorldPos(glm::vec3(0.0f, 0.0f, 0.0f)),
        mRotateInfo(glm::vec3(0.0f, 0.0f, 0.0f)),
        mPersProj(),
        mTransformation(glm::mat4()) {}

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
    const glm::mat4* getTransformation();

private:
    void InitScaleTransform(glm::mat4& m) const;
    void InitRotateTransform(glm::mat4& m) const;
    void InitTranslationTransform(glm::mat4& m) const;
    void InitPerspectiveProj(glm::mat4& m) const;

    glm::vec3 mScale;
    glm::vec3 mWorldPos;
    glm::vec3 mRotateInfo;
    glm::mat4 mTransformation;

    struct {
        float FOV;
        float width;
        float height;
        float zNear;
        float zFar;
    } mPersProj;
};
