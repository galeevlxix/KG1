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

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 1024

using namespace glm;

struct Vertex
{
    vec3 m_pos;
    vec2 m_tex;

    Vertex() {}

    Vertex(vec3 pos, vec2 tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

class Main : public ICallbacks
{
public:

    Main()
    {
        pGameCamera = NULL;
        pTexture = NULL;
        m_pEffect = NULL;
        Scale = 0.0f;
        directionalLight.Color = my_Vector3f(1.0f, 1.0f, 1.0f);
        directionalLight.AmbientIntensity = 0.5f;
    }

    ~Main()
    {
        delete m_pEffect;
        delete pGameCamera;
        delete pTexture;
    }

    bool Init()
    {
        pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

        CreateVertexBuffer();
        CreateIndexBuffer();

        m_pEffect = new LightingTechnique();

        if (!m_pEffect->Init())
        {
            return false;
        }

        m_pEffect->Enable();

        m_pEffect->SetTextureUnit(0);

        pTexture = new Texture(GL_TEXTURE_2D, "C:\\tnt.png");

        if (!pTexture->Load()) {
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
        glClear(GL_COLOR_BUFFER_BIT);

        Scale += 0.001f;

        Pipeline p;
        p.Rotate(0.0f, Scale * 50, 20 * sinf(Scale));
        p.WorldPos(sinf(Scale), sinf(Scale) * sinf(Scale) - 2.0f, 5.0f);
        p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
        p.PerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

        m_pEffect->SetWVP(p.getTransformation());
        m_pEffect->SetDirectionalLight(directionalLight);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        pTexture->Bind(GL_TEXTURE0);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

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
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        pGameCamera->OnMouse(x, y);
    }

private:

    void CreateVertexBuffer()
    {
        Vertex Vertices[24] = { //вершины куба
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.499f, 0.6666f)),     // 0 0  верхн€€ лева€ ближн€€ 
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.251f, 0.6666f)),      // 1 1  верхн€€ права€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.499f, 1.0f)),    // 2 2  нижн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.251f, 1.0f)),     // 3 3  нижн€€ права€ ближн€€

        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.499f, 0.6666f)),      // 1 4  верхн€€ права€ ближн€€
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.251f, 0.6666f)),     // 5 5  верхн€€ права€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.499f, 1.0f)),     // 3 6  нижн€€ права€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.251f, 1.0f)),    // 7 7  нижн€€ права€ дальн€€

        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.499f, 0.6666f)),     // 5 8  верхн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.251f, 0.6666f)),    // 4 9  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.499f, 1.0f)),    // 7 10  нижн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.251f, 1.0f)),   // 6 11  нижн€€ лева€ дальн€€

        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.499f, 0.6666f)),    // 4 12  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.251f, 0.6666f)),     // 0 13  верхн€€ лева€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(0.499f, 1.0f)),   // 6 14  нижн€€ лева€ дальн€€
        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(0.251f, 1.0f)),    // 2 15  нижн€€ лева€ ближн€€

        Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec2(0.25f, 0.3333f)),    // 4 16  верхн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.5f, 0.3333f)),     // 5 17  верхн€€ права€ дальн€€
        Vertex(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec2(0.25f, 0.6666f)),     // 0 18  верхн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.6666f)),      // 1 19  верхн€€ права€ ближн€€

        Vertex(glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec2(1.0f, 0.3333f)),    // 2 20  нижн€€ лева€ ближн€€
        Vertex(glm::vec3(1.0f, -1.0f, 1.0f), glm::vec2(0.75f, 0.3333f)),     // 3 21  нижн€€ права€ ближн€€
        Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec2(1.0f, 0.6666f)),   // 6 22  нижн€€ лева€ дальн€€
        Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec2(0.75f, 0.6666f)),    // 7 23  нижн€€ права€ дальн€€
        };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    }

    void CreateIndexBuffer()
    {
        unsigned int Indices[] = { // грани куба
                        // ближн€€
                        1, 3, 0,
                        0, 3, 2,

                        // дальн€€
                        9, 11, 8,
                        8, 11, 10,

                        // лева€
                        13, 15, 12,
                        12, 15, 14,

                        // права€
                        5, 7, 4,
                        4, 7, 6,

                        // верхн€€
                        17, 19, 16,
                        16, 19, 18,

                        // нижн€€
                        22, 20, 23,
                        23, 20, 21 };

        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    }

    GLuint VBO;
    GLuint IBO;
    LightingTechnique* m_pEffect;
    Texture* pTexture;
    Camera* pGameCamera;
    float Scale;
    DirectionLight directionalLight;
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
