//def
#define STB_IMAGE_IMPLEMENTATION

//ext
#include "../external/cpp/inc/GL/glew.h"
#include "../external/cpp/inc/stb_image.h"

//tensegrity
#include "inc/GPU/Texture.hpp"

namespace gpu
{
	//constructors
	Texture::Texture(void) : m_id(0)
	{
		return;
	}
	Texture::Texture(const char* path) : m_id(0), m_path(path)
	{
		return;
	}

	//destructor
	Texture::~Texture(void)
	{
		if(glIsTexture(m_id))
		{
			glDeleteTextures(1, &m_id);
		}
	}

	//setup
	void Texture::setup(void)
	{
		if(glIsTexture(m_id))
		{
			glDeleteTextures(1, &m_id);
		}
		glGenTextures(1, &m_id);
	}

	//load
	void Texture::load(void)
	{
		//load
		int width, height, bpp;
		stbi_set_flip_vertically_on_load(1);
		const unsigned format[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
		unsigned char* data = stbi_load(m_path.c_str(), &width, &height, &bpp, 0);
		//check
		if(!data)
		{
			fprintf(stderr, "Error: unable to load texture from %s: %s\n", m_path.c_str(), stbi_failure_reason());
			exit(EXIT_FAILURE);
		}
		//handle
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format[bpp - 1], GL_UNSIGNED_BYTE, data);
		//setup
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//bind
		glBindTexture(GL_TEXTURE_2D, 0);
		//cleanup
		stbi_image_free(data);
	}

	//bind
	void Texture::bind(GLenum unit)
	{
		glActiveTexture(unit);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
	void Texture::unbind(void)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}