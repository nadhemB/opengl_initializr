#pragma once
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "pbrmaterial.h"

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------

    Shader() {}
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr,
           const char *tesselationControlPath = nullptr, const char *tesselationEvaluationPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::string tcCode;
        std::string teCode;

        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;

        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }

            if (tesselationControlPath != nullptr)
            {
                tcShaderFile.open(tesselationControlPath);
                std::stringstream tcShaderStream;
                tcShaderStream << tcShaderFile.rdbuf();
                tcShaderFile.close();
                tcCode = tcShaderStream.str();
            }

            if (tesselationEvaluationPath != nullptr)
            {
                teShaderFile.open(tesselationEvaluationPath);
                std::stringstream teShaderStream;
                teShaderStream << teShaderFile.rdbuf();
                teShaderFile.close();
                teCode = teShaderStream.str();
            }
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char *gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        unsigned int tc;
        unsigned int te;

        if (tesselationControlPath != nullptr)
        {
            const char *tcShaderCode = tcCode.c_str();
            tc = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tc, 1, &tcShaderCode, NULL);
            glCompileShader(tc);
            checkCompileErrors(tc, "TESSELATION CONTROL");

            const char *teShaderCode = teCode.c_str();
            te = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(te, 1, &teShaderCode, NULL);
            glCompileShader(te);
            checkCompileErrors(te, "TESSELATION EVALUATION");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        if (tesselationControlPath != nullptr)
        {
            glAttachShader(ID, tc);
            glAttachShader(ID, te);
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);
        if (tesselationControlPath != nullptr)
        {
            glDeleteShader(tc);
            glDeleteShader(te);
        }
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }

    void begin(const Camera &camera)
    {
        setMat4("view", camera.GetViewMatrix());
        setMat4("projection", camera.GetProjectionMatrix());
        setVec3("camPos", camera.Position);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        if (glGetUniformLocation(ID, name.c_str()) >= 0)
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
    }

    void setUpMaterial(PbrMaterial material)
    {
        setVec3("u_albedo", material.baseColor);
        setFloat("u_metallness", material.metalness);
        setFloat("u_roughness", material.roughness);
        setFloat("u_ao", material.ao);
        setFloat("u_opacity", material.opacity);
        int samplerMask = 0;
        int slot = 10;
        if (material.baseColorMap >= 0)
        {
            samplerMask |= ALBEDO_MASK;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, material.baseColorMap);
            setInt("albedoMap", slot++);
        }

        if (material.normalMap >= 0)
        {
            samplerMask |= NORMAL_MASK;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, material.normalMap);
            setInt("normalMap", slot++);
        }
        if (material.metallicMap >= 0)
        {
            samplerMask |= METALLIC_MASK;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, material.metallicMap);
            setInt("metallicMap", slot++);
        }
        if (material.roughnessMap >= 0)
        {
            samplerMask |= ROUGHNESS_MASK;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, material.roughnessMap);
            setInt("roughnessMap", slot++);
        }
        if (material.aoMap >= 0)
        {
            samplerMask |= AO_MASK;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, material.aoMap);
            setInt("aoMap", slot++);
        }
        setInt("SAMPLER_MASK", samplerMask);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
