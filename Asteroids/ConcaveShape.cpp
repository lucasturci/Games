#include "include/ConcaveShape.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>

using namespace std;

int partition_sort_by_polar_angle(vector<sf::Vector2f>& points, int s, int e, sf::Vector2f pivot) {
	int i, j;
	sf::Vector2f right = rightvec;

	j = s;
	for(i = s; i < e; ++i) {
		if(angle(right, points[i] - pivot) < angle(right, points[e] - pivot)) { //se o produto escalar for maior, o angulo é menor
			swap(points[i], points[j]);
			j++;
		}
	}
	swap(points[j], points[e]);

	return j;
}

void sort_by_polar_angle(vector<sf::Vector2f>& points, int s, int e, sf::Vector2f pivot) {
	if(s > e) return;

	int p = partition_sort_by_polar_angle(points, s, e, pivot);
	sort_by_polar_angle(points, s, p-1, pivot);
	sort_by_polar_angle(points, p+1, e, pivot);
 }

ConcaveShape::ConcaveShape(vector<sf::Vector2f> points) {
	int i, j = 0;
	sf::Vector2f pivot, last, curr;
	vector<vector<sf::Vector2f>> convex_points;

	for(i = 0; i < points.size(); ++i) {
		if(points[i].y < points[j].y) j = i;
	}

	swap(points[0], points[j]);
	pivot = points[0];

	sort_by_polar_angle(points, 1, points.size() - 1, pivot);

	j = 0;
	convex_points.emplace_back();
	convex_points[j].push_back(points[0]);
	convex_points[j].push_back(points[1]);

	for(i = 2; i < points.size(); ++i) {
		curr = points[i] - points[i-1];
		last = points[i-1] - points[i-2];
		if(determinant(last, curr) < 0) { //o points[i-1] é um inner point
			convex_points.emplace_back();
			j++;
			convex_points[j].push_back(pivot);
			convex_points[j].push_back(points[i-1]);
		}
		convex_points[j].push_back(points[i]);
	}

	relConvexPoints = convex_points;
	absConvexPoints = convex_points;

	for(i = 0; i < convex_points.size(); ++i) {
		convexShapes.push_back(new sf::ConvexShape(convex_points[i].size()));
		for(j = 0; j < convex_points[i].size(); ++j) {
			convexShapes[i]->setPoint(j, convex_points[i][j]);
		}
	}

	vertices = points;
}

float ConcaveShape::getTop() {
	sf::FloatRect box;
	float top = 10000000; //min y
	for(int i = 0; i < convexShapes.size(); ++i) {
		box = convexShapes[i]->getGlobalBounds();
		if(box.top < top) top = box.top;
	}

	return top;
}

float ConcaveShape::getLeft() {
	sf::FloatRect box;
	float left = 10000000; //min x
	for(int i = 0; i < convexShapes.size(); ++i) {
		box = convexShapes[i]->getGlobalBounds();
		if(box.left < left) left = box.left;
	}

	return left;
}

float ConcaveShape::getBottom() {
	sf::FloatRect box;
	float bottom = -10000000; //max y
	for(int i = 0; i < convexShapes.size(); ++i) {
		box = convexShapes[i]->getGlobalBounds();
		if(box.top + box.height > bottom) bottom = box.top + box.height;
	}

	return bottom;
}

float ConcaveShape::getRight() {
	sf::FloatRect box;
	float right = -10000000; //max x
	for(int i = 0; i < convexShapes.size(); ++i) {
		box = convexShapes[i]->getGlobalBounds();
		if(box.left + box.width > right) right = box.left + box.width;
	}

	return right;
}


void ConcaveShape::setFillColor(sf::Color col) {
	for(int i = 0; i < convexShapes.size(); ++i) 
		convexShapes[i]->setFillColor(col);
}

void ConcaveShape::setRotation(float angle) {
	for(int i = 0; i < convexShapes.size(); ++i) 
		convexShapes[i]->setRotation(angle);	
}

void ConcaveShape::setPosition(sf::Vector2f pos) {
	for(int i = 0; i < convexShapes.size(); ++i) {
		convexShapes[i]->setPosition(pos);	
 		for(int j = 0; j < absConvexPoints[i].size(); ++j) absConvexPoints[i][j] = convexShapes[i]->getTransform().transformPoint(relConvexPoints[i][j]);
	}
}

ConcaveShape::~ConcaveShape() {
	int i;
	for(i = 0; i < convexShapes.size(); ++i) {
		delete convexShapes[i];
	}
}
