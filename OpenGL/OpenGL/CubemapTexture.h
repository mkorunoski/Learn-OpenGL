#ifndef CUBEMAP_TEXTURE_H
#define CUBEMAP_TEXTURE_H

#include <string>
#include <GL/glew.h>
#include <SOIL/SOIL.h>

class CubemapTexture
{
private:
	GLuint texture;

public:
	CubemapTexture(const std::string& baseName, const std::string& format)
	{
		std::string sides[] = { "right", "left", "top", "bottom", "back", "front" };
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

		for (int i = 0; i < 6; ++i)
		{
			int textureWidth, textureHeight;
			unsigned char* image = SOIL_load_image((baseName + sides[i] + "." + format).c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);

			if (image == NULL)
				std::cout << "Unable to load texture" << std::endl;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

			SOIL_free_image_data(image);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void Use()
	{		
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}

	void Unuse()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	~CubemapTexture()
	{
		glDeleteTextures(1, &texture);
	}
};

#endif