#include "include/Collision.hpp"

bool has_gap(vector<sf::Vector2f> points1, vector<sf::Vector2f> points2) {
	int i, j, n, m;
	sf::Vector2f base, origin, projection;
	double corners1[2], corners2[2];
	bool found_gap = false;
	double dist;

	n = points1.size();
	m = points2.size();

	origin = sf::Vector2f(0.0, 0.0);

	points1.push_back(points1.front());
	points2.push_back(points2.front());

	for(i = 1; i <= n; ++i) {
		base = versor(perpendicular(points1[i] - points1[i-1]));

		projection = base * dotProduct(points1[0], base);
		corners1[0] = corners1[1] = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
		for(j = 0; j < n; ++j) {
			projection = base * dotProduct(points1[j], base);

			dist = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
			if(dist < corners1[0]) corners1[0] = dist;
			if(dist > corners1[1]) corners1[1] = dist;
		}

		projection = base * dotProduct(points2[0], base);
		corners2[0] = corners2[1] = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
		for(j = 0; j < m; ++j) {
			projection = base * dotProduct(points2[j], base);

			dist = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
			if(dist < corners2[0]) corners2[0] = dist;
			if(dist > corners2[1]) corners2[1] = dist;
		}

		if(corners1[0] > corners2[1] or corners1[1] < corners2[0]) found_gap = true;
	}

	return found_gap;
}

bool isColliding(vector<sf::Vector2f> points1, vector<sf::Vector2f> points2) {
	if(points1.size() < 3 and points2.size() < 3) printf("%s\n", "There is nothing I can do, you did it wrong");
	if(points1.size() >= 3 and points2.size() >= 3) return !has_gap(points1, points2) and !has_gap(points2, points1);
	else if(points1.size() < 3) return !has_gap(points2, points1);
	else return !has_gap(points1, points2);
}


bool has_gap_with_circle(vector<sf::Vector2f> points, sf::Vector2f pos, float radius) {
	int i, j, n;
	sf::Vector2f base, origin, projection;
	double corners[2];
	double dist;

	n = points.size();

	origin = sf::Vector2f(0.0, 0.0);
	points.push_back(points.front());

	for(i = 1; i <= n; ++i) {
		base = versor(perpendicular(points[i] - points[i-1]));

		projection = base * dotProduct(points[0], base);
		corners[0] = corners[1] = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
		for(j = 0; j < n; ++j) {
			projection = base * dotProduct(points[j], base);

			dist = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);
			if(dist < corners[0]) corners[0] = dist;
			if(dist > corners[1]) corners[1] = dist;
		}
		
		projection = base * dotProduct(pos, base);
		dist = (dotProduct(projection, base) >= 0)? magnitude(projection) : -magnitude(projection);

		if(corners[0] > dist + radius or corners[1] < dist - radius) return true;
	}
	return false;
}

bool isCollidingCirclePolygon(sf::Vector2f pos, float radius, vector<sf::Vector2f> points) {
	if(points.size() < 3) printf("%s\n", "Polygon with less than 3 points?");
	return !has_gap_with_circle(points, pos, radius); 
}
