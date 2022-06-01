#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "engine_common.h"
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
#include "Mesh.h"

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
        wall = NULL;
        m_pTexture = NULL;
        m_pNormalMap = NULL;
        m_pTrivialNormalMap = NULL;

        m_pShadowMapEffect = NULL;
        Scale = 0.0f;

        directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = 0.5f;
        directionalLight.DiffuseIntensity = 0.75f;
        directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

        m_bumpMapEnabled = true;
    }

    ~Main()
    {
        SAFE_DELETE(m_pLightingTechnique);
        SAFE_DELETE(pGameCamera);
        SAFE_DELETE(wall);
        SAFE_DELETE(m_pTexture);
        SAFE_DELETE(m_pNormalMap);
        SAFE_DELETE(m_pTrivialNormalMap);
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

        m_pShadowMapEffect = new ShadowMapTechnique();
        if (!m_pShadowMapEffect->Init()) {
            printf("Error initializing the shadow map technique\n");
            return false;
        }

        wall = new Mesh();

        if (!wall->LoadMesh("C:\\Users\\Lenovo\\Desktop\\sketch\\sourceimages\\final_v01.obj")) {
            return false;
        }

        m_pTexture = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\sketch\\sourceimages\\all.png");
        
        if (!m_pTexture->Load()) {
            return false;
        }

        m_pTexture->Bind(COLOR_TEXTURE_UNIT);
       
        

        m_pNormalMap = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\source\\Stylizedground_height.jpg");

        if (!m_pNormalMap->Load()) {
            return false;
        }

        m_pTrivialNormalMap = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\source\\Stylizedground_height.jpg");

        if (!m_pTrivialNormalMap->Load()) {
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
        

        Scale += 0.001f;
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
        sl[0].Position = Vector3f(sinf(Scale * 2) * 3, 0.0f, 10.0f);
        sl[0].Direction = Vector3f(-sinf(Scale * 2), 1.0f, -1.0f);
        sl[0].Attenuation.Linear = 0.01f;
        sl[0].Cutoff = 20.0f;

        sl[1].AmbientIntensity = 0.1f;
        sl[1].DiffuseIntensity = 6.0f;
        sl[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[1].Position = pGameCamera->GetPos() /** Vector3f(1.0f, -1.0f, 1.0f)*/;
        sl[1].Direction = pGameCamera->GetTarget() /** Vector3f(0.5f, -2.0f, 0.5f)*/;
        sl[1].Attenuation.Linear = 0.1f;
        sl[1].Cutoff = 5.0f;

        m_pLightingTechnique->SetSpotLights(2, sl);

        m_pLightingTechnique->SetDirectionalLight(directionalLight);
        m_pLightingTechnique->SetMatSpecularIntensity(5.0f);
        m_pLightingTechnique->SetMatSpecularPower(5);
        /////////////////////////////////////////////// ShadowMapPass

        m_shadowMapFBO.BindForWriting();
        glClear(GL_DEPTH_BUFFER_BIT);

        m_pShadowMapEffect->Enable();

        Pipeline p1;        
        p1.Scale(0.1f, 0.1f, 0.1f);
        p1.Rotate(0.0f, Scale * 50, 20 * sinf(Scale * 2));
        p1.WorldPos(sinf(Scale * 2), sinf(Scale * 2) * sinf(Scale * 2) + 12.0f, 0.0f);
        p1.SetCamera(sl[0].Position, sl[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p1.PerspectiveProj(m_persProjInfo);

        m_pShadowMapEffect->SetWVP(p1.GetWVPTrans());

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        /////////////////////////////////////////////// RenderPass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_pLightingTechnique->Enable();
        m_shadowMapFBO.BindForReading(GL_TEXTURE0);

        Pipeline p;
        p.Scale(0.1f, 0.1f, 0.1f);
        p.Rotate(0.0f, 0.0f, 0.0f);
        p.WorldPos(0.0f, 10.0f, 0.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(m_persProjInfo);

        
        m_pTexture->Bind(COLOR_TEXTURE_UNIT);
        

        if (m_bumpMapEnabled)
        {
            m_pNormalMap->Bind(NORMAL_TEXTURE_UNIT);
        }
        else
        {
            m_pTrivialNormalMap->Bind(NORMAL_TEXTURE_UNIT);
        }

        m_pLightingTechnique->SetWVP(p.GetWVPTrans());
        m_pLightingTechnique->SetWorldMatrix(p.getTransformation());
        p.SetCamera(sl[0].Position, sl[0].Direction, Vector3f(0.0f, 1.0f, 0.0f));
        m_pLightingTechnique->SetEyeWorldPos(pGameCamera->GetPos());

        wall->Render();
        
        ///////////////////////////
        
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
    LightingTechnique* m_pLightingTechnique;
    Camera* pGameCamera;
    float Scale;
    DirectionalLight directionalLight;

    Mesh* wall;

    Texture* m_pTexture;
    Texture* m_pNormalMap;
    Texture* m_pTrivialNormalMap;

    PointLight pl[3];
    SpotLight sl[2];

    ShadowMapFBO m_shadowMapFBO;
    ShadowMapTechnique* m_pShadowMapEffect;

    PersProjInfo m_persProjInfo;
    bool m_bumpMapEnabled;
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
