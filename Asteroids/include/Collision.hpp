#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <vector>
#include "vectorutils.hpp"

using namespace std;

bool has_gap(vector<sf::Vector2f> points1, vector<sf::Vector2f> points2);
bool isColliding(vector<sf::Vector2f> points1, vector<sf::Vector2f> points2);
bool has_gap_with_circle(vector<sf::Vector2f> points, sf::Vector2f pos, float radius);
bool isCollidingCirclePolygon(sf::Vector2f pos, float radius, vector<sf::Vector2f> points);

#endif