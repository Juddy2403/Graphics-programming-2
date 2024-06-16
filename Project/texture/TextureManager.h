#include <unordered_map>
#include <string>
#include "Texture.h"
#include "Singleton.h"

class TextureManager final : public Singleton<TextureManager> {
public:
    Texture LoadTexture(const VkCommandPool &commandPool, const std::string &path);
    void DestroyTextures();
private:
    friend class Singleton<TextureManager>;
    TextureManager() = default;

    std::unordered_map<std::string, Texture> m_TextureMap{};
};


