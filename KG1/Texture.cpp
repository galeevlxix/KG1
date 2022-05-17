#include "Texture.h"
#include <iostream>

Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;        //позиция (мы используем GL_TEXTURE_2D)
    m_fileName = FileName;                  //имя файла
    m_pImage = NULL;
}

bool Texture::Load()
{
    try {
        m_pImage = new Image(m_fileName);
        m_pImage->write(&m_blob, "RGBA");
    }
    catch (Error& Error) {
        std::cout << "Error loading texture '" << m_fileName << "': " << Error.what() << std::endl;
        return false;
    }

    glGenTextures(1, &m_textureObj);            //Она генерирует указанное число объектов текстур и помещает их в указатель на массив GLuint (второй параметр)
    glBindTexture(m_textureTarget, m_textureObj);//Она сообщает OpenGL объект текстуры, который относится ко всем вызовам, связанным с текстурами, до тех пор, пока новый объект текстур не будет передан
    glTexImage2D(m_textureTarget, 0, GL_RGB, m_pImage->columns(), m_pImage->rows(), -0.5, GL_RGBA, GL_UNSIGNED_BYTE, m_blob.data());
    glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}