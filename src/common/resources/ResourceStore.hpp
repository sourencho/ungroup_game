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

    void addTexture(std::string key, std::string texture_path, bool repeated = false);
    std::shared_ptr<sf::Texture> getTexture(std::string key);
    void addShader(RenderingDef::ShaderKey key, std::string vertex_shader_path,
                   std::string fragment_shader_path);
    std::shared_ptr<sf::Shader> getShader(RenderingDef::ShaderKey key);

  private:
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> mTextures;
    std::map<RenderingDef::ShaderKey, std::shared_ptr<sf::Shader>> mShaders;
};

#endif /* ResourceStore_hpp */
