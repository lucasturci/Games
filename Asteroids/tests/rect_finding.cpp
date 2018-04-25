#include <algorithm>
#include <assert.h>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory.h>
#include <numeric>
#include <set>
#include <stack>
#include <sstream>
#include <utility>
#include <vector>
#include <array>
#include <queue>

#define N 1000007
#define it(i, n) for(i = 0; i < n; ++i)
#define pb push_back
#define eps 1e-9

using namespace std;
typedef long long ll;

typedef struct rect {
	double left, top, right, bottom;
	double w, h;
	double area;

	rect() { left = top = right = bottom = w = h = area = 0.0; }

	rect(double l, double t, double r, double b) {
		left = l;
		right = r;
		top = t;
		bottom = b;
		w = r-l;
		h = b-t;
		area = w*h;
	}
} rect;

bool xcomp(rect a, rect b) {return a.left < b.left;}
bool ycomp(rect a, rect b) {return a.top < b.top;}

rect * rects;
int n;

bool collides(rect a, rect b) {
	return a.right > b.left && a.left < b.right && a.bottom > b.top && a.top < b.bottom;
}

rect bestRect(rect * rects, int n) {
	int i, j = 0;
	it(i, n)
		if(rects[i].area > rects[j].area) j = i;
	return rects[j];
}

rect find_rect(rect r, int s, int e) {
	int i, k = 0;
	rect results[4];
	int first = -1, last;

	if(r.left >= r.right || r.top >= r.bottom) return r;

	rect * coliding = new rect[e-s+1];

	for(i = s; i <= e; ++i) {
		if(collides(rects[i], r)) {
			coliding[k++] = rects[i];
			if(first < 0) first = i;
			last = i;
		}
	}
	if(k) {
		results[0] = find_rect(rect(coliding[k/2].right + eps, r.top, r.right, r.bottom), first, last);
		results[1] = find_rect(rect(r.left, r.top, coliding[k/2].left - eps, r.bottom), first, last);

		results[2] = find_rect(rect(r.left, r.top, r.right, coliding[k/2].top - eps), first, last);
		results[3] = find_rect(rect(r.left, coliding[k/2].bottom + eps, r.right, r.bottom), first, last);
		delete[] coliding;
		return bestRect(results, 4);
	}

	delete[] coliding;
	return r;
}

int main(int argc, char * argv[]) {
	int i, j, k, m, p, q, r;
	double width, height;
	double x, y, w, h;
	rect res;
	
	cin >> width >> height;
	cin >> n;

	rects = new rect[n];

	it(i, n) {
		scanf("%lf %lf %lf %lf", &x, &y, &w, &h);
		rects[i] = rect(x, y, x+w, y+h);
	}

	sort(rects, rects + n, xcomp);
	res = find_rect(rect(0.0, 0.0, width, height), 0, n-1);

	printf("%lf %lf %lf %lf\n", res.left, res.top, res.w, res.h);
	delete[] rects;

	return 0;
}