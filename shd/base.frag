#version 460 core

in vec3 vert_color;

out vec4 frag_color;

void main(void)
{
	frag_color = vec4(vert_color, 1);
}