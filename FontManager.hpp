//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#ifndef ELLIPSECOLLISION_FONTMANAGER_HPP
#define ELLIPSECOLLISION_FONTMANAGER_HPP

#include <unordered_map>

#include <SFML/Graphics/Font.hpp>

class FontManager{
public:
    static void addFont(const std::string &name, const std::filesystem::path &font_path);

    /**
     * Delete font from stored data.
     * @param name Specified name of font when called \p addFont.
     * @return \p true if \p name is the key of stored fonts therefore success to delete, \p false otherwise.
     */
    static bool deleteFont(std::string_view name);

    static sf::Font &getFont(std::string_view name);

private:
    struct string_hash // why isn't std::hash<std::string> this exact same thing?
    {
        using hash_type = std::hash<std::string_view>;

        // don't forget the next line
        // this is what causes the lookup methods of unordered containers
        // to select template keys as opposed to constructing the defined key type
        using is_transparent = void;

        size_t operator()(const char* str) const noexcept { return hash_type{}(str); }
        size_t operator()(std::string_view str) const noexcept { return hash_type{}(str); }
        size_t operator()(std::string const& str) const noexcept { return hash_type{}(str); }
    };

    static std::unordered_map<std::string, sf::Font, string_hash, std::equal_to<>> fonts;
};

#endif //ELLIPSECOLLISION_FONTMANAGER_HPP
