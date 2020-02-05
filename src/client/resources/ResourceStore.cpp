
#include <iostream>

#include "../../common/util/game_settings.hpp"
#include "ResourceStore.hpp"

ResourceStore::ResourceStore() {
    load();
};

void ResourceStore::load() {
    addTexture(RenderingDef::TextureKey::collision, "resources/images/spark.png");
    addTexture(RenderingDef::TextureKey::mine_pattern, "resources/images/mine_pattern.png", true);
    addTexture(RenderingDef::TextureKey::dotted_background,
               "resources/images/dotted_background.png", true);
    addTexture(RenderingDef::TextureKey::a_letter, "resources/images/a_letter.png");
    addTexture(RenderingDef::TextureKey::m_letter, "resources/images/m_letter.png");
    addTexture(RenderingDef::TextureKey::e_letter, "resources/images/e_letter.png");
    addTexture(RenderingDef::TextureKey::n_letter, "resources/images/n_letter.png");

    addShader(RenderingDef::ShaderKey::noop, "resources/shaders/noop.vert",
              "resources/shaders/noop.frag");
    addShader(RenderingDef::ShaderKey::voronoi_counts, "resources/shaders/noop.vert",
              "resources/shaders/voronoi_counts.frag");

    addFont(RenderingDef::FontKey::monogram, "resources/fonts/monogram.ttf");
}

void ResourceStore::addTexture(RenderingDef::TextureKey key, std::string texture_path,
                               bool repeated) {
    auto texture = std::shared_ptr<sf::Texture>(new sf::Texture());
    texture->setRepeated(repeated);
    if (!texture->loadFromFile(texture_path)) {
        throw std::runtime_error("Error loading texture from " + texture_path);
    }
    m_textures[key] = std::move(texture);
}

std::shared_ptr<sf::Texture> ResourceStore::getTexture(RenderingDef::TextureKey key) {
    if (m_textures.count(key) == 0) {
        throw std::runtime_error("Texure with used key doesn't exist");
    }
    return m_textures[key];
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
    m_shaders[key] = shader;
}

std::shared_ptr<sf::Shader> ResourceStore::getShader(RenderingDef::ShaderKey key) {
    if (key == RenderingDef::ShaderKey::none) {
        return nullptr;
    } else {
        if (m_shaders.count(key) == 0) {
            throw std::runtime_error("Shader with used key doesn't exist");
        }
        return m_shaders[key];
    }
}

void ResourceStore::addFont(RenderingDef::FontKey key, std::string font_path) {
    auto font = std::shared_ptr<sf::Font>(new sf::Font());
    if (!font->loadFromFile(font_path)) {
        throw std::runtime_error("Error loading font from " + font_path);
    }
    m_fonts[key] = font;
}

std::shared_ptr<sf::Font> ResourceStore::getFont(RenderingDef::FontKey key) {
    if (m_fonts.count(key) == 0) {
        throw std::runtime_error("Font with used key doesn't exist");
    }
    return m_fonts[key];
}