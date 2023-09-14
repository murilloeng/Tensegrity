#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//std
#include <string>
#include <gl/GL.h>

//shaders
const char* shader_name(GLenum);
void load_file(const char*, std::string&);
void setup_shader(GLuint, GLenum, const char*);
void setup_program(GLuint&, const char*, const char*);

//buffers
void move(float*, unsigned, unsigned, unsigned, const float*);
void rotate(float*, unsigned, unsigned, unsigned, const float*, const float*);