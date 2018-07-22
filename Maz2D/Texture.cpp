#include "Texture.h"
#include <glad/glad.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include "globals.h"

Texture::Texture()
{
}


Texture::~Texture()
{
	releaseTexture();
}


void Texture::InitDevIL()
{
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
}


bool Texture::loadTexture(const char* texturePath)
{
	ILuint imageID;				// Create an image ID as a ULuint
	ILboolean success;			// Create a flag to keep track of success/failure
	ILenum error;				// Create a flag to keep track of the IL error state
	
	ilGenImages(1, &imageID); 				// Generate the image ID
	ilBindImage(imageID); 					// Bind the image
	success = ilLoadImage(texturePath); 	// Load the image file

	if (!success)
	{
		// If we failed to open the image file in the first place...
		error = ilGetError();
		LOGGER("ERROR (Texture::loadTexture): Image load failed - IL reports error: %u - %s", error, iluErrorString(error));
	}
	else
	{
		// If we managed to load the image, then we can start to do things with it...

		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			LOGGER("ERROR (Texture::loadTexture): Image conversion failed - IL reports error: %u - %s", error, iluErrorString(error));
		}
		else
		{
			// Release previous texture
			releaseTexture();

			// Store image information
			width = ilGetInteger(IL_IMAGE_WIDTH);
			height = ilGetInteger(IL_IMAGE_HEIGHT);

			// Generate a new texture
			glGenTextures(1, &textureId);

			// Bind the texture to a name
			glBindTexture(GL_TEXTURE_2D, textureId);

			// Specify the texture specification
			glTexImage2D(GL_TEXTURE_2D, 		// Type of texture
				0,				// Pyramid level (for mip-mapping) - 0 is the top level
				ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
				width,			// Image width
				height,			// Image height
				0,				// Border width in pixels (can either be 1 or 0)
				ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
				GL_UNSIGNED_BYTE,				// Image data type
				ilGetData()						// The actual image data itself
			);

			glBindTexture(GL_TEXTURE_2D, GL_NONE);

			// Configure the newly created texture
			defaultTextureConfiguration();
		}
	}

	ilBindImage(0);
	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	return success;
}


unsigned int Texture::getTextureId()
{
	return textureId;
}


int Texture::getWidth()
{
	return width;
}


int Texture::getHeight()
{
	return height;
}


void Texture::setWrapMode(WrapMode wrapMode)
{
	if (this->wrapMode != wrapMode)
	{
		this->wrapMode = wrapMode;
		applyWrapMode();
	}
}


void Texture::setMagFilter(MagFilter magFilter)
{
	if (this->magFilter != magFilter)
	{
		this->magFilter = magFilter;
		applyMagFilter();
	}
}


void Texture::setMinFilter(MinFilter minFilter)
{
	if (this->minFilter != minFilter)
	{
		this->minFilter = minFilter;
		applyMinFilter();
	}
}


void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
}


void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::releaseTexture()
{
	glDeleteTextures(1, &textureId);
	textureId = 0;
	width = 0;
	height = 0;
	wrapMode = WrapMode::REPEAT;
	magFilter = MagFilter::NEAREST;
	minFilter = MinFilter::NEAREST;
}

void Texture::defaultTextureConfiguration()
{
	applyWrapMode();
	applyMagFilter();
	applyMinFilter();
}

void Texture::applyWrapMode()
{
	GLenum option;
	switch (this->wrapMode)
	{
	case Texture::WrapMode::REPEAT:
		option = GL_REPEAT;
		break;
	case Texture::WrapMode::MIRRORED_REPEAT:
		option = GL_MIRRORED_REPEAT;
		break;
	case Texture::WrapMode::CLAMP_EDGE:
		option = GL_CLAMP_TO_EDGE;
		break;
	case Texture::WrapMode::CLAMP_BORDER:
		option = GL_CLAMP_TO_BORDER;
		break;
	}
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, option);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, option);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::applyMagFilter()
{
	GLenum option;
	switch (this->magFilter)
	{
	case Texture::MagFilter::NEAREST:
		option = GL_NEAREST;
		break;
	case Texture::MagFilter::LINEAR:
		option = GL_LINEAR;
		break;
	}
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, option);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::applyMinFilter()
{
	GLenum option;
	switch (this->minFilter)
	{
	case Texture::MinFilter::NEAREST:
		option = GL_NEAREST;
		break;
	case Texture::MinFilter::LINEAR:
		option = GL_LINEAR;
		break;
	case Texture::MinFilter::NEAREST_MIPMAP_NEAREST:
		option = GL_NEAREST_MIPMAP_NEAREST;
		break;
	case Texture::MinFilter::NEAREST_MIPMAP_LINEAR:
		option = GL_NEAREST_MIPMAP_LINEAR;
		break;
	case Texture::MinFilter::LINEAR_MIPMAP_NEAREST:
		option = GL_LINEAR_MIPMAP_NEAREST;
		break;
	case Texture::MinFilter::LINEAR_MIPMAP_LINEAR:
		option = GL_LINEAR_MIPMAP_LINEAR;
		break;
	}
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, option);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}
