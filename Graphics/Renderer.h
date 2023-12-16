#pragma once
#include <glad/glad.h>
#include <gl/GL.h>
#include <assert.h>
#include<stdint.h>

#ifdef _DEBUG
#define GLCall(x) GLClearError();\
    x;\
    assert(GLCheckError())
#else
#define GLCall(x) x
#endif

void GLClearError();
bool GLCheckError();

