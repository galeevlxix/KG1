#include "Object.h"

void Cube::CreateBuffer() {
    unsigned int Indices[] = { // ����� ����
                        // �������
                        9, 11, 8,
                        8, 11, 10,

                        // �������
                        1, 3, 0,
                        0, 3, 2,

                        // ������
                        5, 7, 4,
                        4, 7, 6,

                        // �����
                        13, 15, 12,
                        12, 15, 14,

                        // ������
                        22, 20, 23,
                        23, 20, 21,

                        // �������
                        17, 19, 16,
                        16, 19, 18 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    Vertex2 Vertices[24] = { //������� ����
        Vertex2(Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.499f, 0.6666f)),     // 0 0  ������� ����� ������� 
        Vertex2(Vector3f(1.0f, 1.0f, 1.0f), vec2(0.251f, 0.6666f)),      // 1 1  ������� ������ �������
        Vertex2(Vector3f(-1.0f, -1.0f, 1.0f), vec2(0.499f, 1.0f)),       // 2 2  ������ ����� �������
        Vertex2(Vector3f(1.0f, -1.0f, 1.0f), vec2(0.251f, 1.0f)),        // 3 3  ������ ������ �������

        Vertex2(Vector3f(1.0f, 1.0f, 1.0f), vec2(0.499f, 0.6666f)),      // 1 4  ������� ������ �������
        Vertex2(Vector3f(1.0f, 1.0f, -1.0f), vec2(0.251f, 0.6666f)),     // 5 5  ������� ������ �������
        Vertex2(Vector3f(1.0f, -1.0f, 1.0f), vec2(0.499f, 1.0f)),     // 3 6  ������ ������ �������
        Vertex2(Vector3f(1.0f, -1.0f, -1.0f), vec2(0.251f, 1.0f)),    // 7 7  ������ ������ �������

        Vertex2(Vector3f(1.0f, 1.0f, -1.0f), vec2(0.499f, 0.6666f)),     // 5 8  ������� ������ �������
        Vertex2(Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.251f, 0.6666f)),    // 4 9  ������� ����� �������
        Vertex2(Vector3f(1.0f, -1.0f, -1.0f), vec2(0.499f, 1.0f)),    // 7 10  ������ ������ �������
        Vertex2(Vector3f(-1.0f, -1.0f, -1.0f), vec2(0.251f, 1.0f)),   // 6 11  ������ ����� �������

        Vertex2(Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.499f, 0.6666f)),    // 4 12  ������� ����� �������
        Vertex2(Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.251f, 0.6666f)),     // 0 13  ������� ����� �������
        Vertex2(Vector3f(-1.0f, -1.0f, -1.0f), vec2(0.499f, 1.0f)),   // 6 14  ������ ����� �������
        Vertex2(Vector3f(-1.0f, -1.0f, 1.0f), vec2(0.251f, 1.0f)),    // 2 15  ������ ����� �������

        Vertex2(Vector3f(-1.0f, 1.0f, -1.0f), vec2(0.25f, 0.3333f)),    // 4 16  ������� ����� �������
        Vertex2(Vector3f(1.0f, 1.0f, -1.0f), vec2(0.5f, 0.3333f)),     // 5 17  ������� ������ �������
        Vertex2(Vector3f(-1.0f, 1.0f, 1.0f), vec2(0.25f, 0.6666f)),     // 0 18  ������� ����� �������
        Vertex2(Vector3f(1.0f, 1.0f, 1.0f), vec2(0.5f, 0.6666f)),      // 1 19  ������� ������ �������

        Vertex2(Vector3f(-1.0f, -1.0f, 1.0f), vec2(1.0f, 0.3333f)),    // 2 20  ������ ����� �������
        Vertex2(Vector3f(1.0f, -1.0f, 1.0f), vec2(0.75f, 0.3333f)),     // 3 21  ������ ������ �������
        Vertex2(Vector3f(-1.0f, -1.0f, -1.0f), vec2(1.0f, 0.6666f)),   // 6 22  ������ ����� �������
        Vertex2(Vector3f(1.0f, -1.0f, -1.0f), vec2(0.75f, 0.6666f)),    // 7 23  ������ ������ �������
    };

    unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(Vertices);
    CalcNormals(Indices, ARRAY_SIZE_IN_ELEMENTS(Indices), Vertices, VertexCount);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    texture = new Texture(GL_TEXTURE_2D, "C:\\tnt.png");
    texture->Load();
}

void Cube::Render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (const GLvoid*)12);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (const GLvoid*)20);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    texture->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Floor::CreateBuffer() {
    unsigned int Indices[] = {      // ����� ����
                        0, 3, 1,
                        2, 3, 0,
                        
                        4, 7, 6,
                        5, 7, 4
    };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    Vertex2 Vertices[8] = { 
        Vertex2(Vector3f(-15.0f, -1.0f, 15.0f), vec2(0.0f, 1.0f)),
        Vertex2(Vector3f(15.0f, -1.0f, 15.0f), vec2(1.0f, 1.0f)),
        Vertex2(Vector3f(-15.0f, -1.0f, -15.0f), vec2(0.0f, 0.0f)),
        Vertex2(Vector3f(15.0f, -1.0f, -15.0f), vec2(1.0f, 0.0f)),

        Vertex2(Vector3f(-15.0f, -1.0f, 15.0f), vec2(0.0f, 1.0f)),
        Vertex2(Vector3f(15.0f, -1.0f, 15.0f), vec2(1.0f, 1.0f)),
        Vertex2(Vector3f(-15.0f, -1.0f, -15.0f), vec2(0.0f, 0.0f)),
        Vertex2(Vector3f(15.0f, -1.0f, -15.0f), vec2(1.0f, 0.0f)),
    };

    unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS(Vertices);
    CalcNormals(Indices, ARRAY_SIZE_IN_ELEMENTS(Indices), Vertices, VertexCount);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    texture = new Texture(GL_TEXTURE_2D, "C:\\floor.png");
    texture->Load();
}

void Floor::Render() {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (const GLvoid*)12);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2), (const GLvoid*)20);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    texture->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Object :: CalcNormals(const unsigned int* pIndices, unsigned int IndexCount, Vertex2* pVertices, unsigned int VertexCount) {
    for (unsigned int i = 0; i < IndexCount; i += 3) {
        unsigned int Index0 = pIndices[i];
        unsigned int Index1 = pIndices[i + 1];
        unsigned int Index2 = pIndices[i + 2];
        Vector3f v1 = pVertices[Index1].m_pos - pVertices[Index0].m_pos;
        Vector3f v2 = pVertices[Index2].m_pos - pVertices[Index0].m_pos;
        Vector3f Normal = v1.Cross(v2);
        Normal.Normalize();

        pVertices[Index0].m_normal = pVertices[Index0].m_normal + Normal;
        pVertices[Index1].m_normal = pVertices[Index1].m_normal + Normal;
        pVertices[Index2].m_normal = pVertices[Index2].m_normal + Normal;
    }

    for (unsigned int i = 0; i < VertexCount; i++) {
        pVertices[i].m_normal.Normalize();
    }
}

