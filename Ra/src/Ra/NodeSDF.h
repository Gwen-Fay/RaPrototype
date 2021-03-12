/*
 *  Structs used to send data from cpu to gpu, from C++ to GLSL.
 *  NodeSDF contains all of the data for one SDF primitive,
 *  and TreeSDF is an array with all of the nodes to be rendered.
 * 
 *  This file also has a basic Stack implimentation to help TreeSDF
 *  to quickly add and remove nodes, in a way that should reduce the amount
 *  of copying the renderer has to do from the cpu to gpu every frame. 
 */

#pragma once

#include <iostream>

    struct NodeSDF
    {
        float trans[4][4] =
        {
            {1.0, 0.0, 0.0, 0.0},
            {0.0, 1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0, 0.0},
            {0.0, 0.0, 0.0, 1.0}
        };
        int funcID = 0;
        int childArrayID = -1;
        int padding = -1;
        int parentID = -1;
        float color[4] = { 1.0, 1.0, 1.0, 1.0 };
        float sdfData[4] = { 1.0, 1.0, 1.0, 1.0 };
        float elong[4] = { 0.0, 0.0, 0.0, 0.0 };
        float repeat[4] = { 0.0, 0.0, 0.0, 0.0 };
        float roundRad = 0.0;
        float onionRad = 0.0;
        int combineID = 0.0;
        float combineRad = 0.0;

        float distortRot1[4] = { 0.0, 0.0, 0.0, 0.0 };
        float distortRot2[4] = { 0.0, 0.0, 0.0, 0.0 };
        float distortRotStart[4] = { -1.0, -1.0, -1.0, -1.0 };
        float distortRotEnd[4] = { 1.0, 1.0, 1.0, 1.0 };
        float distortScale1[4] = { 0.0, 0.0, 0.0, 0.0 };
        float distortScale2[4] = { 0.0, 0.0, 0.0, 0.0 };
        float distortScaleStart[4] = { -1.0, -1.0, -1.0, -1.0 };
        float distortScaleEnd[4] = { 1.0, 1.0, 1.0, 1.0 };
        float distortSinAmp[4] = { 0.0, 0.0, 0.0, 0.0 };
        float distortSinFreq[4] = { 1.0, 1.0, 1.0, 1.0 };
        float distortSinPhase[4] = { 0.0, 0.0, 0.0, 0.0 };

    };

    struct Stack
    {
        Stack();
        void push(int n);
        int pop();
        int peak();
        bool full();
        bool empty();
        int current = 0;

    private:
        int stack[200];
    };

    struct TreeSDF
    {
    public:
        TreeSDF();
        ~TreeSDF();
        NodeSDF* newNode();
        NodeSDF* newChild(NodeSDF* parent);
        void delNode(NodeSDF* node);
        NodeSDF* getNodes();
        int getIndexSize();
        int* getRoots();
        int* getChildrenArray();
        const int arraySize = 200;
        Stack stack = Stack();

    private:
        int emptyIndex = 0;
        int nodeCount = 0;
        NodeSDF* nodes;
        int* children;
        bool* isUsed;
        int* roots;
        int lastRoot = 0;
        void delNode(int node);
    };
