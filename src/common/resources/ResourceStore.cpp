
#include <iostream>

#include "ResourceStore.hpp"

ResourceStore::ResourceStore() {
    addTexture("collision", "resources/images/collision_expanding_circle.png");
    addShader("default", "resources/shaders/default.vert", "resources/shaders/default.frag");
    addShader("voronoi", "resources/shaders/default.vert", "resources/shaders/voronoi.frag");
};

void ResourceStore::addTexture(std::string key, std::string texture_path) {
    auto texture = std::unique_ptr<sf::Texture>(new sf::Texture());
    if (!texture->loadFromFile(texture_path)) {
        throw std::runtime_error("Error loading texture from " + texture_path);
    }
    mTextures[key] = std::move(texture);
}

sf::Texture& ResourceStore::getTexture(std::string key) { return *mTextures[key]; }

void ResourceStore::addShader(std::string key, std::string vertex_shader_path,
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

    mShaders[key] = shader;
}

std::shared_ptr<sf::Shader> ResourceStore::getShader(std::string key) { return mShaders[key]; }