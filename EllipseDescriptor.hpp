//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#ifndef ELLIPSECOLLISION_ELLIPSEDESCRIPTOR_HPP
#define ELLIPSECOLLISION_ELLIPSEDESCRIPTOR_HPP

#include <SFML/Graphics/Text.hpp>

#include "EllipseShape.hpp"

class EllipseDescriptor : public sf::Drawable{
public:
    explicit EllipseDescriptor(EllipseShape &target);
    EllipseDescriptor(const EllipseDescriptor&) = default;
    EllipseDescriptor &operator=(const EllipseDescriptor &source) { target = source.target; return *this; }
    EllipseDescriptor(EllipseDescriptor&&) noexcept = default;
    EllipseDescriptor &operator=(EllipseDescriptor &&source) noexcept { target = std::move(source.target); return *this; }

    /**
     * Call when target ellipse's semi major or minor radius changed.
     */
    void update();

protected:
    void draw(sf::RenderTarget &target, const sf::RenderStates &states) const override;

private:
    EllipseShape &target;

    sf::VertexArray guidelines;
    sf::Font &text_font;
};

#endif //ELLIPSECOLLISION_ELLIPSEDESCRIPTOR_HPP
