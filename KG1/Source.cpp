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
        m_pEffect = NULL;
        m_pMesh = NULL;
        floor = NULL;
        Scale = 0.0f;
        directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = 0.5f;
        directionalLight.DiffuseIntensity = 0.75f;
        directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);
    }

    ~Main()
    {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(pGameCamera);
        SAFE_DELETE(m_pMesh);
        SAFE_DELETE(floor);
    }

    bool Init()
    {
        Vector3f Pos(0.0f, 15.0f, -3.0f);
        Vector3f Target(0.0f, 0.0f, 1.0f);
        Vector3f Up(0.0, 1.0f, 0.0f);

        pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        obj1.CreateBuffer();
        obj2.CreateBuffer();

        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        m_pEffect->Enable();

        m_pEffect->SetTextureUnit(0);

        m_pMesh = new Mesh();

        return m_pMesh->LoadMesh("C:\\scene\\scene.fbx");
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


        Pipeline p;
        /*p.Rotate(0.0f, Scale * 50, 20 * sinf(Scale));
        p.WorldPos(sinf(Scale), sinf(Scale) * sinf(Scale) - 2.0f, 5.0f);*/
        p.Rotate(-90.0f, 0.0f, 0.0f);

        p.WorldPos(0.0f, 5.0f, 0.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);
        
        sl[0].DiffuseIntensity = 40.0f;
        sl[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[0].Position = Vector3f(sinf(Scale * 2) , 0.0f, 9.0f);
        sl[0].Direction = Vector3f(-sinf(Scale * 2), 1.0f, -1.1f);
        sl[0].Attenuation.Linear = 0.01f;
        sl[0].Cutoff = 20.0f;
        
        m_pEffect->SetSpotLights(1, sl);
        
        m_pEffect->SetWVP(p.GetWVPTrans());
        const Matrix4f& WorldTransformation = p.getTransformation();

        m_pEffect->SetWorldMatrix(WorldTransformation);
        m_pEffect->SetDirectionalLight(directionalLight);
        
        m_pEffect->SetEyeWorldPos(pGameCamera->GetPos());

        m_pEffect->SetMatSpecularIntensity(10.0f);
        m_pEffect->SetMatSpecularPower(30);

        m_pMesh->Render();

        glutSwapBuffers();
    }

    /*virtual void RenderSceneCB()
    {
        pGameCamera->OnRender();
        Scale += 0.001f;

        SpotLight sl[1];
        sl[0].DiffuseIntensity = 3.0f;
        sl[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
        sl[0].Position = Vector3f(sinf(Scale * 2) * 3, 2.0f, 0.0f);
        sl[0].Direction = Vector3f(-sinf(Scale * 2), 1.0f, 0.0f);
        sl[0].Attenuation.Linear = 0.01f;
        sl[0].Cutoff = 20.0f;
        m_pEffect->SetSpotLights(1, sl);

        ShadowMapPass();
        RenderPass();

        glutSwapBuffers();
    }*/

    /*virtual void ShadowMapPass()
    {
        shadowMapFBO.BindForWriting();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Pipeline p;
        p.Scale(0.2f, 0.2f, 0.2f);
        p.Rotate(0.0f, Scale, 0.0f);
        p.WorldPos(0.0f, 0.0f, 5.0f);
        p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
        p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
        shadowMapTechnique->SetWVP(p.GetWVPTrans());
        obj2.Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    virtual void RenderPass()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadowMapTechnique->SetTextureUnit(0);
        shadowMapFBO.BindForReading(GL_TEXTURE0);

        Pipeline p;
        p.Scale(5.0f, 5.0f, 5.0f);
        p.WorldPos(0.0f, 0.0f, 10.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
        shadowMapTechnique->SetWVP(p.GetWVPTrans());
        obj1.Render();
    }*/

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
    SpotLight sl[1];
    Mesh* m_pMesh;
    Mesh* floor;


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
