//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#include "FontManager.hpp"

#include <fmt/std.h>

std::unordered_map<std::string, sf::Font, FontManager::string_hash, std::equal_to<>> FontManager::fonts {};

void FontManager::addFont(const std::string &name, const std::filesystem::path &font_path){
    sf::Font font;
    if (!font.loadFromFile(font_path)){
        throw std::runtime_error { fmt::format("Failed to find the font '{}' at '{}'.", name, font_path) };
    }

    fonts.emplace(name, std::move(font));
}

bool FontManager::deleteFont(std::string_view name){
    auto it = fonts.find(name);
    if (it == fonts.end()){
        return false;
    }

    fonts.erase(it);
    return true;
}

sf::Font &FontManager::getFont(std::string_view name){
    auto it = fonts.find(name);
    if (it == fonts.end()){
        throw std::invalid_argument { fmt::format("The font with name '{}' not exists.", name) };
    }

    return it->second;
}
