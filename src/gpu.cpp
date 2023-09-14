//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/gpu.hpp"
#include "inc/math.hpp"

//shaders
const char* shader_name(GLenum type)
{
	return type == GL_VERTEX_SHADER ? "Vertex Shader" : "Fragment Shader";
}
void load_file(const char* path, std::string& source)
{
	//open
	FILE* file = fopen(path, "r");
	//check
	if(!file)
	{
		fprintf(stderr, "Unable to load file: %s\n", path);
		exit(EXIT_FAILURE);
	}
	//read
	source.clear();
	while(!feof(file)) source += fgetc(file);
	source.back() = '\0';
	//close
	fclose(file);
}
void setup_shader(GLuint program, GLenum type, const char* path)
{
	//source
	std::string source;
	load_file(path, source);
	//create
	GLuint id = glCreateShader(type);
	if(id == 0)
	{
		fprintf(stderr, "Error: Unable to create shader! (%s)\n", shader_name(type));
		exit(EXIT_FAILURE);
	}
	//source
	const GLchar* p = source.c_str();
	glShaderSource(id, 1, &p, nullptr);
	//compile
	GLint status;
	GLchar log[1024];
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		glGetShaderInfoLog(id, sizeof(log), nullptr, log);
		fprintf(stderr, "Error: Unable to compile shader (%s): %s\n", shader_name(type), log);
		exit(EXIT_FAILURE);
	}
	//attach
	glAttachShader(program, id);
}
void setup_program(GLuint& id, const char* path_vert, const char* path_frag)
{
	//create
	id = glCreateProgram();
	//check
	if(id == 0)
	{
		fprintf(stderr, "Error: Unable to create shader program!\n");
		exit(EXIT_FAILURE);
	}
	//shaders
	setup_shader(id, GL_VERTEX_SHADER, path_vert);
	setup_shader(id, GL_FRAGMENT_SHADER, path_frag);
	//link
	GLint status;
	GLchar log[1024];
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	if(status == 0)
	{
		glGetProgramInfoLog(id, sizeof(log), nullptr, log);
		fprintf(stderr, "Error: Unable to link shader program: %s\n", log);
		exit(EXIT_FAILURE);
	}
	//validate
	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &status);
	if(status == 0)
	{
		glGetProgramInfoLog(id, sizeof(log), nullptr, log);
		fprintf(stderr, "Error validating shader program: %s\n", log);
		exit(EXIT_FAILURE);
	}
	//use
	glUseProgram(id);
}

//buffers
void move(float* vbo_data, unsigned start, unsigned stride, unsigned count, const float* dx)
{
	for(unsigned i = start; i < start + count; i++)
	{
		for(unsigned j = 0; j < 3; j++)
		{
			vbo_data[stride * i + j] += dx[j];
		}
	}
}
void rotate(float* vbo_data, unsigned start, unsigned stride, unsigned count, const float* xc, const float* tr)
{
	for(unsigned i = start; i < start + count; i++)
	{
		rotate(vbo_data + stride * i, xc, tr);
	}
}