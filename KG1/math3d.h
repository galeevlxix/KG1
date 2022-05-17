#ifndef MATH3D_H
#define MATH3D_H
#include <stdio.h>
#include <math.h>
#define M_PI       3.14159265358979323846

#define ToRadian(x) ((x) * M_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / M_PI)

struct my_Vector2i
{
    float x;
    float y;
};

struct my_Vector3f
{
    float x;
    float y;
    float z;

    my_Vector3f()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    my_Vector3f(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    my_Vector3f Cross(const my_Vector3f& v) const;

    my_Vector3f& Normalize();

    void Rotate(float Angle, const my_Vector3f& Axis);

    void Print() const
    {
        printf("(%.02f, %.02f, %.02f", x, y, z);
    }
};

inline my_Vector3f operator+(const my_Vector3f& l, const my_Vector3f& r)
{
    my_Vector3f Ret(l.x + r.x,
        l.y + r.y,
        l.z + r.z);

    return Ret;
}

inline my_Vector3f operator-(const my_Vector3f& l, const my_Vector3f& r)
{
    my_Vector3f Ret(l.x - r.x,
        l.y - r.y,
        l.z - r.z);

    return Ret;
}

inline my_Vector3f operator*(const my_Vector3f& l, float f)
{
    my_Vector3f Ret(l.x * f,
        l.y * f,
        l.z * f);

    return Ret;
}

class Matrix4f
{
public:
    float m[4][4];

    Matrix4f()
    {
    }


    inline void InitIdentity()
    {
        m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
        m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
        m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    }

    inline Matrix4f operator*(const Matrix4f& Right) const
    {
        Matrix4f Ret;

        for (unsigned int i = 0; i < 4; i++) {
            for (unsigned int j = 0; j < 4; j++) {
                Ret.m[i][j] = m[i][0] * Right.m[0][j] +
                    m[i][1] * Right.m[1][j] +
                    m[i][2] * Right.m[2][j] +
                    m[i][3] * Right.m[3][j];
            }
        }

        return Ret;
    }

    void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
    void InitRotateTransform(float RotateX, float RotateY, float RotateZ);
    void InitTranslationTransform(float x, float y, float z);
    void InitCameraTransform(const my_Vector3f& Target, const my_Vector3f& Up);
    void InitPersProjTransform(float FOV, float Width, float Height, float zNear, float zFar);
};

struct Quaternion
{
    float x, y, z, w;

    Quaternion(float _x, float _y, float _z, float _w);

    void Normalize();

    Quaternion Conjugate();
};

Quaternion operator*(const Quaternion& l, const Quaternion& r);

Quaternion operator*(const Quaternion& q, const my_Vector3f& v);

#endif