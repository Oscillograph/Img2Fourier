#include <savannah/proto-core.h>
#include <external/glad/include/glad/glad.h>
#include <external/glfw/glfw3.h>

namespace Savannah
{
	class OpenGLTexture2D
	{
	public:
		OpenGLTexture2D(const std::string& filename = "");
		~OpenGLTexture2D();
		bool LoadTextureFromFile(const std::string& filename);
		GLuint GetID();
		iVec2 GetSize();
		void SetData(void* data, uint32_t size);
		void Bind(uint32_t slot) const;
		void Unbind(uint32_t slot) const;
		
		inline bool operator==(const OpenGLTexture2D* other) const
		{
			return (m_TextureID == ((OpenGLTexture2D*)other)->GetID());
		}
	private:
		std::string m_Path = "";
		uint32_t m_TextureID = 0;
		iVec2 m_ImageSize = {0, 0};
		uint32_t m_InternalFormat = GL_RGB8;
		uint32_t m_DataFormat = GL_RGB8;
	};
}
