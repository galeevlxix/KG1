# Третья лабораторная работа по компьютерной графике
## Выполнил Галеев Тимур МО-221
# Выполнено:
## Пространство камеры
### Рефакторинг класса Pipeline
```c++
void SetCamera(const my_Vector3f Pos, const my_Vector3f Target, const my_Vector3f Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }
```
```c++
struct {
        my_Vector3f Pos;
        my_Vector3f Target;
        my_Vector3f Up;
    } m_camera;
```
```c++
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
```
### Создание класса камеры
```c++
class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const my_Vector3f& Pos, const my_Vector3f& Target, const my_Vector3f& Up);

    bool OnKeyboard(int Key);

    void OnMouse(int x, int y);

    void OnRender();

    const my_Vector3f& GetPos() const
    {
        return m_pos;
    }

    const my_Vector3f& GetTarget() const
    {
        return m_target;
    }

    const my_Vector3f& GetUp() const
    {
        return m_up;
    }

private:

    void Init();
    void Update();

    my_Vector3f m_pos;
    my_Vector3f m_target;
    my_Vector3f m_up;

    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    my_Vector2i m_mousePos;
};
```
### Задание callback функций для обработки нажатий на клавиши и движения мыши
```c++
static void SpecialKeyboardCB(int Key, int x, int y)
{
    pGameCamera->OnKeyboard(Key);
}

static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
    case 'q':
        exit(0);
    }
}

static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}
```
### Создание класса Texture
```c++
class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    bool Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    Image* m_pImage;
    Blob m_blob;
};
```
### 
