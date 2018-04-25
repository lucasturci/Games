#include "include/vectorutils.hpp"

float magnitude(sf::Vector2f vec) { return sqrt(vec.x*vec.x + vec.y*vec.y); }
sf::Vector2f versor(sf::Vector2f vec) { return vec/magnitude(vec); }
float degrees(float rad) { return rad*(180.0f/acos(-1)); }
bool isZero(float f) { return f >= -eps and f <= eps; }
float dotProduct(sf::Vector2f a, sf::Vector2f b) { return a.x*b.x + a.y*b.y; }
float determinant(sf::Vector2f a, sf::Vector2f b) { return a.x * b.y - a.y * b.x; }
float angle(sf::Vector2f a, sf::Vector2f b) {
	if(a == b) return 0;
	return acos((dotProduct(a, b)) / (magnitude(a) * magnitude(b))); 
}
sf::Vector2f perpendicular(sf::Vector2f v) { return sf::Vector2f(-v.y, v.x); }
