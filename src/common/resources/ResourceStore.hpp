#ifndef ResourceStore_hpp
#define ResourceStore_hpp

#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

class ResourceStore {
  public:
    explicit ResourceStore();
    ~ResourceStore(){};
    ResourceStore(const ResourceStore& temp_obj) = delete;
    ResourceStore& operator=(const ResourceStore& temp_obj) = delete;

    void addTexture(std::string key, std::string texture_path);
    sf::Texture& getTexture(std::string key);
    void addShader(std::string key, std::string vertex_shader_path,
                   std::string fragment_shader_path);
    std::shared_ptr<sf::Shader> getShader(std::string key);

  private:
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> mTextures;
    std::unordered_map<std::string, std::shared_ptr<sf::Shader>> mShaders;
};

#endif /* ResourceStore_hpp */
