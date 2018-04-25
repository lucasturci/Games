#include "include/Hyperspace.hpp"
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iostream>

using namespace std;

bool xcomp(rect a, rect b) {return a.left < b.left;}

bool collides(rect a, rect b) {
	return a.right > b.left && a.left < b.right && a.bottom > b.top && a.top < b.bottom;
}

rect bestRect(rect * rects, int n) {
	int i, j = 0;
	for(i = 0; i < n; ++i)
		if(rects[i].area > rects[j].area) j = i;
	return rects[j];
}

rect find_rect(rect r, rect * rects, int s, int e) {
	int i, k = 0;
	rect results[4];
	int first = -1, last;

	if(r.left >= r.right || r.top >= r.bottom) {
		return r;
	}

	rect * coliding = new rect[e-s+1];

	for(i = s; i <= e; ++i) {
		if(collides(rects[i], r)) {
			coliding[k++] = rects[i];
			if(first < 0) first = i;
			last = i;
		}
	}
	if(k) {
		results[0] = find_rect(rect(coliding[k/2].right + eps, r.top, r.right, r.bottom), rects, first, last);
		results[1] = find_rect(rect(r.left, r.top, coliding[k/2].left - eps, r.bottom), rects, first, last);

		results[2] = find_rect(rect(r.left, r.top, r.right, coliding[k/2].top - eps), rects, first, last);
		results[3] = find_rect(rect(r.left, coliding[k/2].bottom + eps, r.right, r.bottom), rects, first, last);
		delete[] coliding;
		return bestRect(results, 4);
	}

	delete[] coliding;
	return r;
}

rect findBestRect(rect * rects, int n, float width, float height) {
	sort(rects, rects + n, xcomp);
	return find_rect(rect(0, 0, width, height), rects, 0, n-1);
} 
