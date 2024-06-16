#include "Texture.h"
#include "DescriptorPool.h"

class TextureLoader
{
public:
    static Texture m_DefaultTexture;
    int m_HasNormalMap = 0;

    explicit TextureLoader(DescriptorPool& descriptorPool) : m_DescriptorPool(descriptorPool) {
        m_DescriptorPool.SetAlbedoImageView(m_DefaultTexture.GetTextureImageView());
        m_DescriptorPool.SetNormalImageView(m_DefaultTexture.GetTextureImageView());
        m_DescriptorPool.SetGlossImageView(m_DefaultTexture.GetTextureImageView());
        m_DescriptorPool.SetSpecularImageView(m_DefaultTexture.GetTextureImageView());
    }
    static void LoadDefaultTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadAlbedoTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadNormalTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadGlossTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadSpecularTexture(VkCommandPool const &commandPool, const std::string &path);

private:
    Texture m_AlbedoTexture{};
    Texture m_NormalTexture{};
    Texture m_GlossTexture{};
    Texture m_SpecularTexture{};

    DescriptorPool& m_DescriptorPool;
};


