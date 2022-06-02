#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <windows.h>
#include <cstring>


#include "engine_common.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "math3d.h"
#include "Utils.h"
#include "Mesh.h"
#include "ParticleSystem.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

using namespace glm;

class Main : public ICallbacks
{
public:
    Main()
    {
        pGameCamera = NULL;
        m_pLightingTechnique = NULL;
        obj1 = NULL;
        obj2 = NULL;
        obj3 = NULL;
        m_pTexture = NULL;
        m_pNormalMap = NULL;
        m_pTrivialNormalMap = NULL;

        Scale = 0.0f;
        stepfloat = 0.001f;

        directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = 0.5f;
        directionalLight.DiffuseIntensity = 0.75f;
        directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;
    }

    ~Main()
    {
        SAFE_DELETE(m_pLightingTechnique);
        SAFE_DELETE(pGameCamera);
        SAFE_DELETE(obj1);
        SAFE_DELETE(obj2);
        SAFE_DELETE(obj3);

        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pNormalMap);
        SAFE_DELETE(m_pTrivialNormalMap);
    }

    bool Init()
    {
        Vector3f Pos(0.0f, 15.0f, -3.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

       
        m_pLightingTechnique = new LightingTechnique();

        if (!m_pLightingTechnique->Init())
        {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pLightingTechnique->Enable();
        m_pLightingTechnique->SetDirectionalLight(directionalLight);
        m_pLightingTechnique->SetColorTextureUnit(0);
        m_pLightingTechnique->SetNormalMapTextureUnit(2);

        obj1 = new Mesh();

        if (!obj1->LoadMesh("models/Elf01_posed/Elf01_posed.obj")) {
            return false;
        }

        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }

    virtual void RenderSceneCB()
    {
        pGameCamera->OnRender();
        glClearColor(0.53f, 0.33f, 0.75f, 0.0f);


        Scale += stepfloat;
        /////////////////////////////////////////////// LIGHT
        /*pl[2].DiffuseIntensity = 2.5f;
        pl[2].Color = Vector3f(1.0f, 0.0f, 0.0f);
        pl[2].Position = Vector3f(sinf(Scale) * 10, 2.0f, cosf(Scale) * 10);
        pl[2].Attenuation.Linear = 0.1f;
        pl[1].DiffuseIntensity = 2.5f;
        pl[1].Color = Vector3f(0.0f, 1.0f, 0.0f);
        pl[1].Position = Vector3f(sinf(Scale + 2.1f) * 10, 2.0f, cosf(Scale + 2.1f) * 10);
        pl[1].Attenuation.Linear = 0.1f;
        pl[0].DiffuseIntensity = 2.5f;
        pl[0].Color = Vector3f(0.0f, 0.0f, 1.0f);
        pl[0].Position = Vector3f(sinf(Scale + 4.2f) * 10, 2.0f, cosf(Scale + 4.2f) * 10);
        pl[0].Attenuation.Linear = 0.1f;
        m_pLightingTechnique->SetPointLights(3, pl);*/

        sl[0].AmbientIntensity = 0.1f;
        sl[0].DiffuseIntensity = 3.0f;
        sl[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[0].Position = Vector3f(sinf(Scale * 2) * 3, 15.0f, -10.0f);
        sl[0].Direction = Vector3f(-sinf(Scale * 2), 1.0f, 1.0f);
        sl[0].Attenuation.Linear = 0.01f;
        sl[0].Cutoff = 20.0f;

        sl[1].AmbientIntensity = 0.1f;
        sl[1].DiffuseIntensity = 3.0f;
        sl[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[1].Position = pGameCamera->GetPos() /** Vector3f(1.0f, -1.0f, 1.0f)*/;
        sl[1].Direction = pGameCamera->GetTarget() /** Vector3f(0.5f, -2.0f, 0.5f)*/;
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 20.0f;

        m_pLightingTechnique->SetSpotLights(2, sl);

        m_pLightingTechnique->SetDirectionalLight(directionalLight);
        m_pLightingTechnique->SetMatSpecularIntensity(5.0f);
        m_pLightingTechnique->SetMatSpecularPower(5);
        /////////////////////////////////////////////// RenderPass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingTechnique->Enable();

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(0.0f, Scale * 30, 0.0f);
        p.WorldPos(0.0f, 10.0f, 0.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.SetPerspectiveProj(m_persProjInfo);
      
        m_pLightingTechnique->SetWVP(p.GetWVPTrans());
        m_pLightingTechnique->SetWorldMatrix(p.GetWorldTrans());

        obj1->Render();

        glutSwapBuffers();
    }
   

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        pGameCamera->OnKeyboard(Key);
    }

    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;

        case 'a':
            directionalLight.AmbientIntensity += 0.05f;
            break;

        case 's':
            directionalLight.AmbientIntensity -= 0.05f;
            break;
        case 'z':
            directionalLight.DiffuseIntensity += 0.05f;
            break;

        case 'x':
            directionalLight.DiffuseIntensity -= 0.05f;
            break;
        case 'o':
            stepfloat *= 2.0f;
            break;
        case 'p':
            stepfloat /= 2.0f;
            break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        pGameCamera->OnMouse(x, y);
    }

private:
    LightingTechnique* m_pLightingTechnique;
    Camera* pGameCamera;
    float Scale;
    DirectionalLight directionalLight;

    Mesh* obj1;
    Mesh* obj2;
    Mesh* obj3;

    Texture* m_pTexture;
    Texture* m_pNormalMap;
    Texture* m_pTrivialNormalMap;

    PointLight pl[3];
    SpotLight sl[2];
    float stepfloat;

    PersProjInfo m_persProjInfo;
};


int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "KG1")) {
        return 1;
    }

    Main* pApp = new Main();

    if (!pApp->Init()) {
        return 1;
    }
    pApp->Run();

    delete pApp;

    return 0;
}
