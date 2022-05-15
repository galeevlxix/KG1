#include "pipeline.h"

const Matrix4f* Pipeline::getTransformation()
{
    Matrix4f scaleTrans, rotateTrans, translationTrans, CameraTranslationTrans, CameraRotateTrans, persProjTrans;

    scaleTrans.InitScaleTransform(mScale.x, mScale.y, mScale.z);
    rotateTrans.InitRotateTransform(mRotateInfo.x, mRotateInfo.y, mRotateInfo.z);
    translationTrans.InitTranslationTransform(mWorldPos.x, mWorldPos.y, mWorldPos.z);
    CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
    CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);
    persProjTrans.InitPersProjTransform(mPersProj.FOV, mPersProj.width, mPersProj.height, mPersProj.zNear, mPersProj.zFar);
    
    mTransformation = persProjTrans * CameraRotateTrans * CameraTranslationTrans * translationTrans * rotateTrans * scaleTrans;
    return &mTransformation;
}