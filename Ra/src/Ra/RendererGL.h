/*
 * Sets up and renders the scene.  
 * Main and RendererGL should be the only thing including this
 */

#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "NodeSDF.h"
#include "SceneData.h"

    class RendererGL
    {
    public:
        void setUniforms(SceneData sceneData);
    private:
        void genUniforms(unsigned int shader);

        int resLoc = 0;
        int sunLoc = 0;
        int camPosLoc = 0;
        int camRotLoc = 0;
        int mandelPowLoc = 0;
        int mandelLoopLoc = 0;
        int isJuliaLoc = 0;
        int juliaLoc = 0;
        int nodeSDFLoc = 0;
        int rootsLoc = 0;
        int childrenLoc = 0;
        int backColor1Loc = 0;
        int backColor2Loc = 0;

        GLFWwindow* window;
        unsigned int buffer;
        unsigned int UBO = 0;
        unsigned int shaderProgram;
        const float screenVerts[8] =
        {
        -1.0,1.0,
         1.0,1.0,
        -1.0,-1.0,
         1.0,-1.0
        };
        std::string readFile(const char* filePath);
        unsigned int compileShader(unsigned int type, const std::string& source);
        unsigned int createShader(const std::string& vertShader, const std::string& fragShader);
        void genScreenBuffer(unsigned int& buffer);
        unsigned int setupShaders(const char* vertShader, const char* fragShader);
        void genUboNodeSDF(unsigned int& buffer);
        void calcTransMatricies();

    public:
        TreeSDF tree = TreeSDF();
        int error = 0;
        RendererGL(const int* const res, const char* vertShader, const char* fragShader);
        ~RendererGL();
        void render();
        bool shouldClose();
    };