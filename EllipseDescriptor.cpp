//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#include "EllipseDescriptor.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "FontManager.hpp"

EllipseDescriptor::EllipseDescriptor(EllipseShape &target) : target(target), guidelines { sf::PrimitiveType::LineStrip, 3 }, text_font { FontManager::getFont("Menlo") } {
    update();
}

void EllipseDescriptor::update() {
    guidelines[0].position = { target.getSemiMajorAxis(), 0.f };
    guidelines[1].position = { 0.f, 0.f };
    guidelines[2].position = { 0.f, target.getSemiMinorAxis() };
}

void EllipseDescriptor::draw(sf::RenderTarget &target, const sf::RenderStates &states) const {
    sf::RenderStates guideline_renderstate = states;
    guideline_renderstate.transform *= this->target.getTransform();
    target.draw(guidelines, guideline_renderstate);

    sf::Text semi_major_text { std::to_string(static_cast<int>(this->target.getSemiMajorAxis())), text_font, 10U };
    semi_major_text.setOrigin({ semi_major_text.getLocalBounds().width / 2, -2.f });
    semi_major_text.setPosition(this->target.getTransform().transformPoint({ this->target.getSemiMajorAxis() / 2, 0.f }));
    semi_major_text.setRotation(this->target.getRotation());

    sf::Text semi_minor_text { std::to_string(static_cast<int>(this->target.getSemiMinorAxis())), text_font, 10U };
    semi_minor_text.setOrigin({ semi_minor_text.getLocalBounds().width / 2, -2.f });
    semi_minor_text.setPosition(this->target.getTransform().transformPoint({ 0.f, this->target.getSemiMinorAxis() / 2 }));
    semi_minor_text.setRotation(this->target.getRotation() + sf::degrees(90));

    target.draw(semi_major_text, states);
    target.draw(semi_minor_text, states);
}
