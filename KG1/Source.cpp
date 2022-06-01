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
#include "Mesh.h"
#include "engine_common.h"

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
        Scale = 0.0f;

        m_pSphereMesh = NULL;
        m_pTexture = NULL;
        m_pNormalMap = NULL;
        m_pTrivialNormalMap = NULL;

        directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = -0.5f;
        directionalLight.DiffuseIntensity = 0.75f;
        directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);

        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

        m_bumpMapEnabled = false;
    }

    ~Main()
    {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(pGameCamera);
        SAFE_DELETE(m_pSphereMesh);
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
       
        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            printf("Error initializing the lighting technique\n");
            return false;
        }


        m_pEffect->Enable();
        m_pEffect->SetDirectionalLight(directionalLight);
        m_pEffect->SetColorTextureUnit(0);
        m_pEffect->SetNormalMapTextureUnit(2);

        m_pSphereMesh = new Mesh();

        if (!m_pSphereMesh->LoadMesh("C:\\Users\\Lenovo\\Desktop\\source\\Stylizedground_sphere.fbx")) {
            return false;
        }

        m_pTexture = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\source\\Stylizedground_basecolor.png");

        if (!m_pTexture->Load()) {
            return false;
        }

        m_pTexture->Bind(COLOR_TEXTURE_UNIT);

        m_pNormalMap = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\source\\Pinkish_N_0.png");

        if (!m_pNormalMap->Load()) {
            return false;
        }

        m_pTrivialNormalMap = new Texture(GL_TEXTURE_2D, "C:\\Users\\Lenovo\\Desktop\\source\\white.png");

        if (!m_pTrivialNormalMap->Load()) {
            return false;
        }

        return true;

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Scale += 0.001f;
        m_pEffect->Enable();

        pl[2].DiffuseIntensity = 2.5f;
        pl[2].Color = Vector3f(1.0f, 1.0f, 1.0f);
        pl[2].Position = Vector3f(sinf(Scale) * 10, 18.0f, cosf(Scale) * 10);
        pl[2].Attenuation.Linear = 0.1f;

        pl[1].DiffuseIntensity = 2.5f;
        pl[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
        pl[1].Position = Vector3f(sinf(Scale + 2.1f) * 15, 18.0f, cosf(Scale + 2.1f) * 10);
        pl[1].Attenuation.Linear = 0.1f;

        pl[0].DiffuseIntensity = 2.5f;
        pl[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
        pl[0].Position = Vector3f(sinf(Scale + 4.2f) * 10, 18.0f, cosf(Scale + 4.2f) * 10);
        pl[0].Attenuation.Linear = 0.1f;

        m_pEffect->SetPointLights(3, pl);

        Pipeline p;
        
        p.Scale(0.1, 0.1, 0.1);
        p.Rotate(90.0f, Scale * 30, 0.0f);
        p.WorldPos(0.0f, 15.0f, 3.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(m_persProjInfo);
        m_pTexture->Bind(COLOR_TEXTURE_UNIT);

        if (m_bumpMapEnabled)
        {
            m_pNormalMap->Bind(NORMAL_TEXTURE_UNIT);
        }
        else {
            m_pTrivialNormalMap->Bind(NORMAL_TEXTURE_UNIT);
        }

        m_pEffect->SetWVP(p.GetWVPTrans());
        m_pEffect->SetWorldMatrix(p.getTransformation());

        m_pSphereMesh->Render();
        
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
        case 'p':
            if (m_bumpMapEnabled) { 
                m_bumpMapEnabled = false; 
            }
            else { 
                m_bumpMapEnabled = true; 
            }
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

    PointLight pl[3];

    Mesh* m_pSphereMesh;
    Texture* m_pTexture;
    Texture* m_pNormalMap;
    Texture* m_pTrivialNormalMap;
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
