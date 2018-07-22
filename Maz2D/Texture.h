#ifndef H_TEXTURE
#define H_TEXTURE

class Texture
{
public:
	enum class WrapMode
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_EDGE,
		CLAMP_BORDER
	};

	enum class MagFilter
	{
		NEAREST,
		LINEAR
	};

	enum class MinFilter
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_NEAREST,
		LINEAR_MIPMAP_LINEAR
	};

public:
	Texture();
	~Texture();

	Texture(const Texture& texture) = delete;
	Texture& operator= (const Texture& texture) = delete;
	Texture(Texture&& texture) = delete;
	Texture& operator= (Texture&& texture) = delete;

	static void InitDevIL();

	bool loadTexture(const char* texturePath);
	unsigned int getTextureId();
	int getWidth();
	int getHeight();
	void setWrapMode(WrapMode wrapMode);
	void setMagFilter(MagFilter magFilter);
	void setMinFilter(MinFilter minFilter);
	void bind();
	void unbind();

private:
	void releaseTexture();
	void defaultTextureConfiguration();
	void applyWrapMode();
	void applyMagFilter();
	void applyMinFilter();

	unsigned int textureId = 0;
	int width = 0;
	int height = 0;
	WrapMode wrapMode = WrapMode::REPEAT;
	MagFilter magFilter = MagFilter::NEAREST;
	MinFilter minFilter = MinFilter::NEAREST;
};


#endif // !H_TEXTURE
