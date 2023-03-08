#include <array>
#include <list>

#include <SFML/Graphics.hpp>

#include "EllipseShape.hpp"
#include "EllipseDescriptor.hpp"
#include "FontManager.hpp"

struct EllipsePanningOperation{
    EllipseShape *target;
    sf::Vector2f prev_coord;
};

struct EllipseGenerateOperation{
    enum class CurrentState{
        SemiMajorGeneration,
        SemiMinorGeneration,
    };

    EllipseShape *target;
    CurrentState current_state;
    sf::Vector2f initial_coord;
    sf::Vector2f semi_major_end;

    EllipseGenerateOperation(EllipseShape *target, sf::Vector2f initial_coord) : target { target }, current_state { CurrentState::SemiMajorGeneration }, initial_coord { initial_coord } { }
    EllipseGenerateOperation(const EllipseGenerateOperation&) = default;
    EllipseGenerateOperation &operator=(const EllipseGenerateOperation&) = default;
    EllipseGenerateOperation(EllipseGenerateOperation&&) = default;
    EllipseGenerateOperation &operator=(EllipseGenerateOperation&&) = default;
};

/**
 * Generate combination pairs of given \p [start, last] range.
 * @param start A start iterator of range.
 * @param last A end iterator of range.
 * @return A list of pairs.
 * @example
 * @code
 * std::list<int> nums { 1, 2, 3, 4, 5 };
 * auto num_pairs = comb2(nums.cbegin(), nums.cend());
 * for (auto [pe1, pe2]: num_pairs){
 *     fmt::print("{},{} ", *pe1, *pe2);
 * }
 *
 * // Result: 1,2 1,3 1,4 1,5 2,3 2,4 2,5 3,4 3,5 4,5
 * @endcode
 */
template <std::forward_iterator It>
constexpr std::list<std::pair<It, It>> comb2(It start, It last){
    std::list<std::pair<It, It>> result;

    for (auto it1 = start, end1 = std::prev(last, 1); it1 != end1; ++it1){
        for (auto it2 = std::next(it1, 1); it2 != last; ++it2){
            result.emplace_back(std::make_pair(it1, it2));
        }
    }
    return result;
}

int main() {
    FontManager font_manager;
    font_manager.addFont("Menlo", "/System/Library/Fonts/Menlo.ttc");

    sf::RenderWindow window { sf::VideoMode { { 640U, 640U } }, "Ellipse Collision" };
    window.setVerticalSyncEnabled(true);

    std::list<EllipseShape> ellipses;
    std::unordered_map<const EllipseShape*, EllipseDescriptor> descriptors;

    std::variant<std::monostate, EllipsePanningOperation, EllipseGenerateOperation> drag_mode;

    while (window.isOpen()){
        for (sf::Event event{}; window.pollEvent(event);){
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonPressed:{
                    if (event.mouseButton.button == sf::Mouse::Button::Left){

                        if (std::holds_alternative<std::monostate>(drag_mode)){
                            auto coord = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

                            auto it = std::find_if(ellipses.rbegin(), ellipses.rend(), [=](const auto &ellipse) { return ellipse.contains(coord); });
                            if (it != ellipses.rend()){
                                drag_mode = EllipsePanningOperation { &*it, coord };
                            }
                            else{
                                // Draw new ellipse that centered at coord.
                                EllipseShape ellipse { 1.f, 1.f };
                                ellipse.setPosition(coord);
                                ellipse.setFillColor(sf::Color::Transparent);
                                ellipse.setOutlineColor(sf::Color::White);
                                ellipse.setOutlineThickness(1.f);

                                auto &target = ellipses.emplace_back(std::move(ellipse));
                                descriptors.insert_or_assign(&target, EllipseDescriptor { target });
                                drag_mode = EllipseGenerateOperation { &target, coord };
                            }
                        }
                    }
                    break;
                }

                case sf::Event::MouseMoved:{
                    auto current_coord = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });

                    if (std::holds_alternative<EllipsePanningOperation>(drag_mode)){
                        auto &operation = std::get<EllipsePanningOperation>(drag_mode);
                        operation.target->move(current_coord - operation.prev_coord);
                        operation.prev_coord = current_coord;
                    }
                    else if (std::holds_alternative<EllipseGenerateOperation>(drag_mode)){
                        auto &operation = std::get<EllipseGenerateOperation>(drag_mode);

                        if (operation.current_state == EllipseGenerateOperation::CurrentState::SemiMajorGeneration){
                            auto initial_current_offset = current_coord - operation.initial_coord;
                            operation.target->setRotation(initial_current_offset.angle());
                            operation.target->setSemiMajorRadius(initial_current_offset.length());
                            descriptors.at(operation.target).update();
                        }
                        else{ // operation.operation == EllipseGenerateOperation::CurrentState::SemiMinorGeneration
                            auto semi_major_end_current_offset = current_coord - operation.semi_major_end;
                            auto perpendicular_offset = semi_major_end_current_offset - semi_major_end_current_offset.projectedOnto(operation.semi_major_end - operation.initial_coord);
                            operation.target->setSemiMinorRadius(perpendicular_offset.length());
                            descriptors.at(operation.target).update();
                        }
                    }

                    break;
                }

                case sf::Event::MouseButtonReleased:{
                    if (std::holds_alternative<EllipseGenerateOperation>(drag_mode)){
                        auto &operation = std::get<EllipseGenerateOperation>(drag_mode);
                        if (operation.current_state == EllipseGenerateOperation::CurrentState::SemiMajorGeneration){
                            operation.current_state = EllipseGenerateOperation::CurrentState::SemiMinorGeneration;
                            operation.semi_major_end = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

                            break;
                        }
                    }

                    drag_mode = std::monostate{};
                    break;
                }

                default:
                    break;
            }
        }

        // Calculate colliding ellipses.
        for (auto &e : ellipses){
            e.setOutlineColor(sf::Color::White);
        }

        auto ellipse_pairs = comb2(ellipses.begin(), ellipses.end());
        for (auto [pe1, pe2] : ellipse_pairs){
            if (EllipseShape::isIntersectWith(*pe1, *pe2)){
                pe1->setOutlineColor(sf::Color::Red);
                pe2->setOutlineColor(sf::Color::Red);
            }
        }

        window.clear();
        for (const auto &ellipse : ellipses){
            window.draw(ellipse);
        }
        for (const auto &[_, value] : descriptors){
            window.draw(value);
        }
        window.display();
    }

    return 0;
}
