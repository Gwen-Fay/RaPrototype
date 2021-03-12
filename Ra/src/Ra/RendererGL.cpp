/*
 * Sets up and renders the scene.
 * This should contain all of the openGL commands.
 */

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "RendererGL.h"
#include "EntitySDF.h"
#include "Math.h"

#include <iostream>
#include <fstream>

    void RendererGL::setUniforms(SceneData sd)
    {
        getRotMat3(sd.camRot, sd.pitch, sd.yaw, sd.roll);

        glUniform2f(resLoc, *sd.res, *(sd.res + 1));
        glUniform4f(sunLoc, *sd.sun, *(sd.sun + 1), *(sd.sun + 2), *(sd.sun + 3));
        glUniform3f(camPosLoc, *sd.camPos, *(sd.camPos + 1), *(sd.camPos + 2));
        glUniformMatrix3fv(camRotLoc, 1, false, *sd.camRot);

        glUniform1f(mandelPowLoc, *sd.mandelData);
        glUniform1f(mandelLoopLoc, *(sd.mandelData + 1));
        glUniform1f(isJuliaLoc, *(sd.mandelData + 2));
        glUniform3f(juliaLoc, *(sd.mandelData + 3), *(sd.mandelData + 4), *(sd.mandelData + 5));

        glUniform3f(backColor1Loc, *(sd.backColor1), *(sd.backColor1 + 1), *(sd.backColor1 + 2));
        glUniform3f(backColor2Loc, *(sd.backColor2), *(sd.backColor2 + 1), *(sd.backColor2 + 2));

        glUniform1iv(rootsLoc, sizeof(int) * 200, tree.getRoots());
        glUniform1iv(childrenLoc, sizeof(int) * 100 * 20, tree.getChildrenArray());

        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(NodeSDF) * tree.getIndexSize(), tree.getNodes());

        calcTransMatricies();
    }

    void RendererGL::genUniforms(unsigned int shader)
    {
        resLoc = glGetUniformLocation(shader, "uRes");
        sunLoc = glGetUniformLocation(shader, "uSun");
        camPosLoc = glGetUniformLocation(shader, "uCamPos");
        camRotLoc = glGetUniformLocation(shader, "uCamRot");
        mandelPowLoc = glGetUniformLocation(shader, "uMandelPower");
        mandelLoopLoc = glGetUniformLocation(shader, "uMandelLoop");
        isJuliaLoc = glGetUniformLocation(shader, "uIsJulia");
        juliaLoc = glGetUniformLocation(shader, "uJulia");
        rootsLoc = glGetUniformLocation(shader, "uRoots");
        childrenLoc = glGetUniformLocation(shader, "uChildren");
        backColor1Loc = glGetUniformLocation(shader, "uBackColor1");
        backColor2Loc = glGetUniformLocation(shader, "uBackColor2");

        nodeSDFLoc = glGetUniformBlockIndex(shader, "NodeBlock");
        glUniformBlockBinding(shader, nodeSDFLoc, 0);
    }


    std::string RendererGL::readFile(const char* filePath) {
        std::string content;
        std::ifstream fileStream(filePath, std::ios::in);

        if (!fileStream.is_open()) {
            std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
            return "";
        }

        std::string line = "";
        while (!fileStream.eof()) {
            std::getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();
        return content;

    }

    unsigned int RendererGL::compileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = new char[length];
            glGetShaderInfoLog(id, length, &length, message);

            std::cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
                << " Shader failed to Compile!" << std::endl;
            std::cout << message << std::endl;

            delete[] message;

            glDeleteShader(id);
            return 0;
        }
        return id;
    }

    unsigned int RendererGL::createShader(const std::string& vertShader, const std::string& fragShader)
    {
        unsigned int program = glCreateProgram();
        unsigned int vs = compileShader(GL_VERTEX_SHADER, vertShader);
        unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragShader);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);
        return program;
    }

    void RendererGL::genScreenBuffer(unsigned int& buffer)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), screenVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        glEnableVertexAttribArray(0);
    }

    void RendererGL::genUboNodeSDF(unsigned int& buffer)
    {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(NodeSDF) * 200, NULL, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, nodeSDFLoc, buffer);

    }

    unsigned int RendererGL::setupShaders(const char* vertFile, const char* fragFile)
    {
        std::string vertShader = readFile(vertFile);
        std::string fragShader = readFile(fragFile);
        unsigned int shader = createShader(vertShader, fragShader);
        glUseProgram(shader);
        genUniforms(shader);
        return shader;
    }

    RendererGL::RendererGL(const int* const res, const char* vertShader, const char* fragShader)
    {
        /* Initialize the library */
        if (!glfwInit())
        {
            std::cout << "glfw failed to init" << std::endl;
            error = -1;
            return;
        }

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(*res, *(res + 1), "Ra - a Ray Marching Renderer", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            std::cout << "failed to create window" << std::endl;
            error = -1;
            return;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        /* Initalize GLEW */
        if (glewInit() != GLEW_OK)
        {
            std::cout << "glewInit Failed" << std::endl;
            glfwTerminate();
            error = -1;
            return;
        }

        genScreenBuffer(buffer);
        shaderProgram = setupShaders(vertShader, fragShader);
        genUboNodeSDF(UBO);
    }

    RendererGL::~RendererGL()
    {
        glfwTerminate();
    }

    void RendererGL::calcTransMatricies()
    {
        for (EntitySDF* entity : EntitySDF::entities)
        {
            entity->calcTransMatrix();
        }
    }

    void RendererGL::render()
    {
        /* Poll for and process events */
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);


    }
    bool RendererGL::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }
