#ifndef ResourceStore_hpp
#define ResourceStore_hpp

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../rendering/RenderingDef.hpp"

class ResourceStore {
  public:
    explicit ResourceStore();
    ~ResourceStore(){};
    ResourceStore(const ResourceStore& temp_obj) = delete;
    ResourceStore& operator=(const ResourceStore& temp_obj) = delete;

    void addTexture(RenderingDef::TextureKey key, std::string texture_path, bool repeated = false);
    std::shared_ptr<sf::Texture> getTexture(RenderingDef::TextureKey key);
    void addShader(RenderingDef::ShaderKey key, std::string vertex_shader_path,
                   std::string fragment_shader_path);
    std::shared_ptr<sf::Shader> getShader(RenderingDef::ShaderKey key);
    void addFont(RenderingDef::FontKey key, std::string font_path);
    std::shared_ptr<sf::Font> getFont(RenderingDef::FontKey key);

  private:
    void load();

    std::map<RenderingDef::TextureKey, std::shared_ptr<sf::Texture>> m_textures;
    std::map<RenderingDef::ShaderKey, std::shared_ptr<sf::Shader>> m_shaders;
    std::map<RenderingDef::FontKey, std::shared_ptr<sf::Font>> m_fonts;
};

#endif /* ResourceStore_hpp */
