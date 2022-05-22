#include "pipeline.h"

const Matrix4f& Pipeline::getTransformation()
{
    Matrix4f scaleTrans, rotateTrans, translationTrans, CameraTranslationTrans, CameraRotateTrans, persProjTrans;

    scaleTrans.InitScaleTransform(mScale.x, mScale.y, mScale.z);
    rotateTrans.InitRotateTransform(mRotateInfo.x, mRotateInfo.y, mRotateInfo.z);
    translationTrans.InitTranslationTransform(mWorldPos.x, mWorldPos.y, mWorldPos.z);
    
    mTransformation = translationTrans * rotateTrans * scaleTrans;
    return mTransformation;
}

const Matrix4f& Pipeline::GetWVPTrans()
{
    getTransformation();

    Matrix4f CameraTranslationTrans, CameraRotateTrans, PersProjTrans;

    CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
    CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);
    PersProjTrans.InitPersProjTransform(mPersProj.FOV, mPersProj.width, mPersProj.height, mPersProj.zNear, mPersProj.zFar);

    m_WVPtransformation = PersProjTrans * CameraRotateTrans * CameraTranslationTrans * mTransformation;
    return m_WVPtransformation;
}