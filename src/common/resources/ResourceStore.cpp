
#include <iostream>

#include "../util/game_settings.hpp"
#include "ResourceStore.hpp"

ResourceStore::ResourceStore() {
    addTexture("collision", "resources/images/spark.png");
    addTexture("mine_pattern", "resources/images/mine_pattern.png", true);
    addShader(RenderingDef::ShaderKey::noop, "resources/shaders/noop.vert",
              "resources/shaders/noop.frag");
    addShader(RenderingDef::ShaderKey::voronoi, "resources/shaders/noop.vert",
              "resources/shaders/voronoi.frag");
};

void ResourceStore::addTexture(std::string key, std::string texture_path, bool repeated) {
    auto texture = std::shared_ptr<sf::Texture>(new sf::Texture());
    texture->setRepeated(repeated);
    if (!texture->loadFromFile(texture_path)) {
        throw std::runtime_error("Error loading texture from " + texture_path);
    }
    m_Textures[key] = std::move(texture);
}

std::shared_ptr<sf::Texture> ResourceStore::getTexture(std::string key) {
    if (m_Textures.count(key) == 0) {
        throw std::runtime_error("Texure doesn't exist with key " + key);
    }
    return m_Textures[key];
}

void ResourceStore::addShader(RenderingDef::ShaderKey key, std::string vertex_shader_path,
                              std::string fragment_shader_path) {
    if (!sf::Shader::isAvailable()) {
        throw std::runtime_error("Shaders are not available");
    }
    auto shader = std::shared_ptr<sf::Shader>(new sf::Shader());
    if (!shader->loadFromFile(vertex_shader_path, sf::Shader::Vertex)) {
        throw std::runtime_error("Error loading vertex shader from " + vertex_shader_path);
    }
    if (!shader->loadFromFile(fragment_shader_path, sf::Shader::Fragment)) {
        throw std::runtime_error("Error loading fragment shader file " + fragment_shader_path);
    }
    shader->setUniform("u_resolution", sf::Vector2f(WINDOW_RESOLUTION));
    m_Shaders[key] = shader;
}

std::shared_ptr<sf::Shader> ResourceStore::getShader(RenderingDef::ShaderKey key) {
    if (key == RenderingDef::ShaderKey::none) {
        return nullptr;
    } else {
        if (m_Shaders.count(key) == 0) {
            throw std::runtime_error("Shader with used key doesn't exist");
        }
        return m_Shaders[key];
    }
}