#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "math3d.h"
#include "Utils.h"
#include "Object.h"
#include "ShadowMapFBO.h"
#include "ShadowMapTechnique.h"
#include "SkyBox.h"

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

using namespace glm;

class Main : public ICallbacks
{
public:
    Main()
    {
        pGameCamera = NULL;
        m_pEffect = NULL;
        m_pShadowMapEffect = NULL;
        Scale = 0.0f;

        directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = -0.5f;
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
        delete m_pEffect;
        delete pGameCamera;
        SAFE_DELETE(m_pShadowMapEffect);
    }

    bool Init()
    {
        Vector3f Pos(0.0f, 15.0f, -3.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        if (!m_shadowMapFBO.Init(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            return false;
        }

        pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

       
        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();
        m_pEffect->SetTextureUnit(0);
        m_pEffect->SetShadowMapTextureUnit(1);

        m_pShadowMapEffect = new ShadowMapTechnique();
        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }

        m_pSkyBox = new SkyBox(pGameCamera, m_persProjInfo);

        if (!m_pSkyBox->Init(".",
            "C:\\skybox\\right.png",
            "C:\\skybox\\left.png",
            "C:\\skybox\\up.png",
            "C:\\skybox\\down.png",
            "C:\\skybox\\back.png",
            "C:\\skybox\\front.png"
            )) {
            return false;
        }

        obj1.CreateBuffer();
        obj2.CreateBuffer();

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
        

        Scale += 0.001f;
        /////////////////////////////////////////////// LIGHT
        pl[2].DiffuseIntensity = 2.5f;
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

        m_pEffect->SetPointLights(3, pl);

        sl[0].AmbientIntensity = 0.1f;
        sl[0].DiffuseIntensity = 3.0f;
        sl[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[0].Position = Vector3f(sinf(Scale * 2) * 3, 2.0f, 0.0f);
        sl[0].Direction = Vector3f(-sinf(Scale * 2), 1.0f, 0.0f);
        sl[0].Attenuation.Linear = 0.01f;
        sl[0].Cutoff = 20.0f;

        sl[1].AmbientIntensity = 0.1f;
        sl[1].DiffuseIntensity = 6.0f;
        sl[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[1].Position = pGameCamera->GetPos() * Vector3f(1.0f, -1.0f, 1.0f);
        sl[1].Direction = pGameCamera->GetTarget() * Vector3f(0.5f, -2.0f, 0.5f);
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 5.0f;

        m_pEffect->SetSpotLights(2, sl);

        m_pEffect->SetDirectionalLight(directionalLight);
        m_pEffect->SetMatSpecularIntensity(5.0f);
        m_pEffect->SetMatSpecularPower(5);
        /////////////////////////////////////////////// ShadowMapPass

        m_shadowMapFBO.BindForWriting();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_pShadowMapEffect->Enable();

        Pipeline p1;        
        p1.Scale(0.1f, 0.1f, 0.1f);
        p1.Rotate(0.0f, Scale * 50, 20 * sinf(Scale * 2));
        p1.WorldPos(sinf(Scale * 2), sinf(Scale * 2) * sinf(Scale * 2) + 10.0f, 0.0f);
        p1.SetCamera(sl[0].Position, sl[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p1.PerspectiveProj(m_persProjInfo);
        m_pShadowMapEffect->SetWVP(p1.GetWVPTrans());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        /////////////////////////////////////////////// RenderPass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pEffect->Enable();
        m_shadowMapFBO.BindForReading(GL_TEXTURE0);

        Pipeline p;
        p.PerspectiveProj(m_persProjInfo);
        p.Rotate(0.0f, 0.0f, 0.0f);
        p.WorldPos(0.0f, 10.0f, 0.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());

      
        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.getTransformation());
        p.SetCamera(sl[0].Position, sl[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pEffect->SetLightWVP(p.GetWVPTrans());        
        m_pEffect->SetEyeWorldPos(pGameCamera->GetPos());
        
        obj2.Render();
        ///////////////////////////
        p.PerspectiveProj(m_persProjInfo);
        p.Rotate(0.0f, Scale * 50, 20 * sinf(Scale * 2));
        p.WorldPos(sinf(Scale * 2), sinf(Scale * 2) * sinf(Scale * 2) + 10.0f, 0.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());

        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.getTransformation());
        p.SetCamera(sl[0].Position , sl[0].Direction , Vector3f(0.0f, 1.0f, 0.0f));
        m_pEffect->SetLightWVP(p.GetWVPTrans());

        obj1.Render();

        m_pSkyBox->Render();
        
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
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        pGameCamera->OnMouse(x, y);
    }

private:
    LightingTechnique* m_pEffect;
    Camera* pGameCamera;
    float Scale;
    DirectionalLight directionalLight;
    Cube obj1;
    Floor obj2;
    PersProjInfo m_persProjInfo;

    PointLight pl[3];
    SpotLight sl[2];
    SkyBox* m_pSkyBox;

    ShadowMapFBO m_shadowMapFBO;
    ShadowMapTechnique* m_pShadowMapEffect;
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
