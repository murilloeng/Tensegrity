#version 460 core

out vec3 vert_color;

layout (location = 1) in vec3 color;
layout (location = 0) in vec3 position;

void main(void)
{
	vert_color = color;
	gl_Position = vec4(position, 1);
}