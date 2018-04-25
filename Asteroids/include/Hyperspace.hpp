#ifndef HYPERSPACE_HPP
#define HYPERSPACE_HPP

#include "vectorutils.hpp"

typedef struct rect {
	float left, right, top, bottom;
	float area;
	float w, h;

	rect() { left = right = top = bottom = w = h = area = 0; }
	rect(float x, float y, float r, float b) {
		left = x;
		top = y;
		this->w = r - x;
		this->h = b - y;
		right = r;
		bottom = b;
		area = w*h;
	}
} rect;

rect findBestRect(rect * rects, int n, float width, float height);


#endif