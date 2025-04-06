#pragma once
#include <shader.h>

class Plane
{
public:
    Plane(glm::mat4 transform = glm::mat4(1.0f)) : mTransofrm(transform)
    {
        create();
    }

    void create()
    {
        float vertices[] = {
            //  Position               Normal         TexCoords
            -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

            -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

    void draw(Shader &shader)
    {
        shader.setMat4("model", mTransofrm);
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mTransofrm))));
        shader.setUpMaterial(mMaterial);
        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void setTransform(const glm::mat4 &transform)
    {
        mTransofrm = transform;
    }

    void setMaterial(const PbrMaterial &material)
    {
        mMaterial = material;
    }

    unsigned int mVAO;

private:
    PbrMaterial mMaterial;
    glm::mat4 mTransofrm;
};