
#include <iostream>

#include "ResourceStore.hpp"

void ResourceStore::load() {
    addTexture("collision", "resources/images/collision_expanding_circle.png");
}

void ResourceStore::addTexture(std::string key, std::string texture_path) {
    auto texture = std::unique_ptr<sf::Texture>(new sf::Texture());
    if (!texture->loadFromFile(texture_path)) {
        throw std::runtime_error("Error loading texture file from " + texture_path);
    }
    mTextures[key] = std::move(texture);
}

sf::Texture& ResourceStore::getTexture(std::string key) { return *mTextures[key]; }