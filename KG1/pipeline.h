#ifndef PIPLINE_H
#define PIPLINE_H
#include <cmath>
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/fwd.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define M_PI       3.14159265358979323846

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

using namespace glm;

class Pipeline {
public:
    Pipeline() :
        mScale(vec3(1.0f, 1.0f, 1.0f)),
        mWorldPos(vec3(0.0f, 0.0f, 0.0f)),
        mRotateInfo(vec3(0.0f, 0.0f, 0.0f)){}

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

    void SetCamera(const vec3 Pos, const vec3 Target, const vec3 Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }
    const mat4& getTransformation();
    const mat4& GetWVPTrans();
private:
    vec3 mScale;// Масштаб
    vec3 mWorldPos;// Позиция
    vec3 mRotateInfo;// Вращение
    mat4 mTransformation;
    mat4 m_WVPtransformation;

    struct {
        float FOV;// Поле зрения (в градусах)
        float width;// Ширина экрана
        float height;// Высота экрана
        float zNear;// Ближайшая глубина зрения
        float zFar;// Дальнейшая глубина зрения
    } mPersProj;

    struct {
        vec3 Pos;// Позиция камеры
        vec3 Target;// Направление камеры
        vec3 Up;// "Вверх" камеры
    } m_camera;
};
#endif