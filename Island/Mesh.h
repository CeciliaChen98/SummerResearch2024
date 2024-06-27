#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>
#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


class Mesh {
public:   
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
        //setupTexture();
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        /*
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "cubemap")
            {
                glUniform1i(glGetUniformLocation(shader.ID, "skybox"), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_CUBE_MAP, textures[i].id);
            }
            else 
            {
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to string
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to string
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to string

                // now set the sampler to the correct texture unit
                glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
        }
        */
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
        glBindVertexArray(0);
    }


    void setupTexture()
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            string name = textures[i].type;
            if (name == "cubemap")
            {
                continue;
            }
            // Stores the width, height, and the number of color channels of the image
            int widthImg, heightImg, numColCh;
            // Flips the image so it appears right side up
            stbi_set_flip_vertically_on_load(true);
            // Reads the image from a file and stores it in bytes
            unsigned char* bytes = stbi_load(textures[i].path.c_str(), &widthImg, &heightImg, &numColCh, 0);

            // Generates an OpenGL texture object
            glGenTextures(1, &textures[i].id);
            // Assigns the texture to a Texture Unit
            glActiveTexture(GL_TEXTURE0 + i);
      
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

            // Configures the type of algorithm that is used to make the image smaller or bigger
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Configures the way the texture repeats (if it does at all)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Extra lines in case you choose to use GL_CLAMP_TO_BORDER
            // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

            // Check what type of color channels the texture has and load it accordingly
            if (numColCh == 4)
                glTexImage2D
                (
                    GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    widthImg,
                    heightImg,
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    bytes
                );
            else if (numColCh == 3)
                glTexImage2D
                (
                    GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    widthImg,
                    heightImg,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    bytes
                );
            else if (numColCh == 1)
                glTexImage2D
                (
                    GL_TEXTURE_2D,
                    0,
                    GL_RGBA,
                    widthImg,
                    heightImg,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    bytes
                );
            // Generates MipMaps
            glGenerateMipmap(GL_TEXTURE_2D);

            // Deletes the image data as it is already in the OpenGL Texture object
            stbi_image_free(bytes);

            // Unbinds the OpenGL Texture object so that it can't accidentally be modified
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
};
#endif