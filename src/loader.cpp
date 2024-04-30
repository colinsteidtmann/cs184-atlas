#pragma once
#include <vector>
#include <glad.h> 
#include <iostream>
#include <string>
#include "stb_image.h"
#include <loader.h>


GLuint Loader::loadTexture(const std::string& fileName) {
        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(("../resources/" + fileName + ".png").c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << fileName << ".png " << stbi_failure_reason() << std::endl;
            std::exit(-1);
        }

        return textureID;
    }   

RawModel Loader::loadToVAO(std::vector<float> positions, int dimensions) {
    RawModel model;
    model.vertexCount = positions.size() / dimensions;
    glGenVertexArrays(1, &model.VAO);
    //vaos.push_back(model.VAO);
    glBindVertexArray(model.VAO);

    // Store data in attribute array, via the VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    //vbos.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), &positions[0], GL_STATIC_DRAW);

    // Configure vertex position attribute
    glVertexAttribPointer(0, dimensions, GL_FLOAT, GL_FALSE, dimensions * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return model;
}
