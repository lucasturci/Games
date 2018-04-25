#ifndef VECTORUTILS_HPP
#define VECTORUTILS_HPP

#include <SFML/System.hpp>
#include <cmath>

#define leftvec sf::Vector2f(-1.0f, 0.0f)
#define rightvec sf::Vector2f(1.0f, 0.0f)
#define upvec sf::Vector2f(0.0f, 1.0f)
#define downvec sf::Vector2f(0.0f, -1.0f)
#define zerovec sf::Vector2f(0.0f, 0.0f)
#define eps 1e-6

float magnitude(sf::Vector2f vec);
sf::Vector2f versor(sf::Vector2f vec);
float degrees(float rad);
bool isZero(float f);
float dotProduct(sf::Vector2f a, sf::Vector2f b);
float determinant(sf::Vector2f a, sf::Vector2f b);
float angle(sf::Vector2f a, sf::Vector2f b);
sf::Vector2f perpendicular(sf::Vector2f v);

#endif
