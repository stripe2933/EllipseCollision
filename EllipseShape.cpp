//
// Created by LEE KYOUNGHEON on 2023/03/08.
//

#include "EllipseShape.hpp"

#include <numbers>

#include "optimization.hpp"

namespace{
    template <typename T>
    Eigen::Vector2<T> toEigenVector(const sf::Vector2<T> &v) noexcept { return { v.x, v.y }; }
};

EllipseShape::EllipseShape(float semi_major, float semi_minor, std::size_t point_count) : semi_major_axis { semi_major }, semi_minor_axis { semi_minor }, point_count { point_count } {
    if (point_count < 3){
        throw std::invalid_argument { "Number of points must be at least 3." };
    }
}

bool EllipseShape::contains(sf::Vector2f point) const {
    // For an ellipse E and the corresponding ellipse matrix A,
    // x is inside in E if (x-x0)^T * A * (x-x0) <= 1, where x0 is the center of E.

    auto offset = toEigenVector(getInverseTransform().transformPoint(point)); // x-x0

    return offset.transpose() * getEllipseMatrix() * offset <= 1.f;
}

bool EllipseShape::isIntersectWith(const EllipseShape &lhs, const EllipseShape &rhs) {
    /**
     * Refers explanation at https://math.stackexchange.com/questions/1114879/detect-if-two-ellipses-intersect
     */

    auto inv_t1 = Eigen::Map<const Eigen::Matrix4f> { lhs.getTransform().getMatrix() }.topLeftCorner<2, 2>().inverse(),
            inv_t2 = Eigen::Map<const Eigen::Matrix4f> { rhs.getTransform().getMatrix() }.topLeftCorner<2, 2>().inverse();

    auto x1 = toEigenVector(lhs.getPosition()),
            x2 = toEigenVector(rhs.getPosition());
    auto m1 = inv_t1.transpose() * lhs.getEllipseMatrix() * inv_t1,
            m2 = inv_t2.transpose() * rhs.getEllipseMatrix() * inv_t2;

    Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::Matrix2f> es { m1.inverse(), m2.inverse() };
    auto lambdas = es.eigenvalues();
    auto phi = es.eigenvectors();
    auto v_squared = (phi.transpose() * (x1 - x2)).array().square();

    auto k_function = [&](float s){
        return 1.f - (v_squared * (s * (1.f - s) / (1.f + s * (lambdas.array() - 1.f)))).sum();
    };

    auto [_, min_k] = optimize(0.5f, k_function);
    return min_k > 0.f;
}

size_t EllipseShape::getPointCount() const {
    return point_count;
}

sf::Vector2f EllipseShape::getPoint(std::size_t index) const {
    float angle = 2.f * std::numbers::pi_v<float> * index / point_count;
    return { semi_major_axis * std::cos(angle), semi_minor_axis * std::sin(angle) };
}

float EllipseShape::getSemiMajorAxis() const noexcept {
    return semi_major_axis;
}

void EllipseShape::setSemiMajorAxis(float value) noexcept {
    semi_major_axis = value;
    sf::Shape::update();
}

float EllipseShape::getSemiMinorAxis() const noexcept {
    return semi_minor_axis;
}

void EllipseShape::setSemiMinorAxis(float value) noexcept {
    semi_minor_axis = value;
    sf::Shape::update();
}

Eigen::Matrix2f EllipseShape::getEllipseMatrix() const {
    return Eigen::DiagonalMatrix<float, 2> { std::pow(semi_major_axis, -2.f), std::pow(semi_minor_axis, -2.f) };
}
