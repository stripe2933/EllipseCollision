//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#ifndef ELLIPSECOLLISION_ELLIPSESHAPE_HPP
#define ELLIPSECOLLISION_ELLIPSESHAPE_HPP

#include <array>

#include <SFML/Graphics/Shape.hpp>
#include <Eigen/Dense>

class EllipseShape : public sf::Shape{
public:
    //////////////////////////////////////////////////
    /// Constructors and destructor.
    //////////////////////////////////////////////////

    EllipseShape(float semi_major, float semi_minor, std::size_t point_count = 36);
    EllipseShape(const EllipseShape&) = default;
    EllipseShape &operator=(const EllipseShape&) = default;
    EllipseShape(EllipseShape&&) noexcept = default;
    EllipseShape &operator=(EllipseShape&&) noexcept = default;

    ~EllipseShape() noexcept = default;

    //////////////////////////////////////////////////
    /// Member functions.
    //////////////////////////////////////////////////

    bool contains(sf::Vector2f point) const;

    float getSemiMajorAxis() const noexcept;
    void setSemiMajorAxis(float value) noexcept;
    float getSemiMinorAxis() const noexcept;
    void setSemiMinorAxis(float value) noexcept;

    //////////////////////////////////////////////////
    /// Overridden member functions.
    //////////////////////////////////////////////////

    size_t getPointCount() const override;

    sf::Vector2f getPoint(std::size_t index) const override;

    //////////////////////////////////////////////////
    /// Static functions.
    //////////////////////////////////////////////////

    static bool isIntersectWith(const EllipseShape &lhs, const EllipseShape &rhs);

private:
    std::size_t point_count;
    float semi_major_axis, semi_minor_axis;

    Eigen::Matrix2f getEllipseMatrix() const;
};

#endif //ELLIPSECOLLISION_ELLIPSESHAPE_HPP
