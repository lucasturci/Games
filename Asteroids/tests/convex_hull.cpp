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

class Vec2 {
public:
	double x, y;

	Vec2() { x = y = 0; }
	Vec2(double x, double y) {
		Vec2::x = x;
		Vec2::y = y;
	}
	/* Functions */
	double magnitude() { return sqrt(x * x + y * y); }
	Vec2 versor() { 
		Vec2 v(x*(1/magnitude()), y*(1/magnitude()));
		return v; 
	}

	void set(double x, double y) {
		Vec2::x = x;
		Vec2::y = y;
	}

	static Vec2 medium(Vec2 a, Vec2 b) {
		Vec2 m(a.x/2 + b.x/2, a.y/2 + b.y/2);
		return m;
	}
	static double determinant(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }
	static bool isZero(double n) { return n >= -1e-6 && n <= 1e-6; }
	static bool linearDependent(Vec2 a, Vec2 b) { return isZero(determinant(a, b)); }
	static double distance(Vec2 a, Vec2 b) { return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y)); }
	static double angle(Vec2 a, Vec2 b) {
		if(a == b) return 0;
		return acos((a * b) / (a.magnitude() * b.magnitude())); 
	}
	/* Operators */
	/////////////////////////////////////////////////////////////////
	Vec2 operator+ (Vec2 op) {
		Vec2 res(x + op.x, y + op.y);
		return res;
	}
	Vec2 operator- (Vec2 op) {
		Vec2 res(x - op.x, y - op.y);
		return res;
	}
	Vec2 operator- () {
		Vec2 res(-x, -y);
		return res;
	}
	Vec2 operator* (double factor) {
		Vec2 res(x*factor, y*factor);
		return res;
	}
	double operator* (Vec2 v) { return x * v.x + y * v.y; } 	//produto escalar
	bool operator== (Vec2 op) { return x == op.x && y == op.y; }
	bool operator!= (Vec2 op) { return x != op.x || y != op.y; }
	bool operator> (Vec2 op) { return magnitude() > op.magnitude(); }
	bool operator>= (Vec2 op) { return magnitude() >= op.magnitude(); }
	bool operator< (Vec2 op) { return magnitude() < op.magnitude(); }
	bool operator <= (Vec2 op) { return magnitude() <= op.magnitude(); }
};


int sort_polar_angle_partition(vector<Vec2>& points, Vec2 pivot, int s, int e) {
	int i, j, k;
	Vec2 right(1, 0);

	k = s;
	j = e;
	for(i = s; i <= e-1; ++i) {
		if(Vec2::angle(right, points[i] - pivot) < Vec2::angle(right, points[j] - pivot)) {
			swap(points[i], points[k]);
			k++;
		}
	}
	swap(points[j], points[k]);

	return k;
}

void sort_polar_angle(vector<Vec2>& points, Vec2 pivot, int s, int e) {
	if(s >= e) return;

	int p = sort_polar_angle_partition(points, pivot, s, e);
	sort_polar_angle(points, pivot, p+1, e);
	sort_polar_angle(points, pivot, s, p-1);

}

vector<Vec2> convex_hull(vector<Vec2> points) {
	vector<Vec2> hull;
	stack<Vec2> vertices;
	int i, j = 0;

	if(points.size() <= 3) return points;

	/* Find the minimum y point (pivot) in set of points */
	it(i, points.size())
		if(points[i].y < points[j].y) j = i;
	swap(points[j], points[0]);

	/* Sort by polar angle from pivot */
	sort_polar_angle(points, points[0], 1, points.size()-1);

	/* Grahan Scan */
	vertices.push(points[0]);
	vertices.push(points[1]);

	for(i = 2; i < points.size(); ++i) {

		while(vertices.size() > 1) {
			Vec2 vec, last_vec, last_point = vertices.top();
			vertices.pop();
			last_vec = last_point - vertices.top();
			vertices.push(last_point);

			vec = points[i] - last_point;

			if(Vec2::determinant(last_vec, vec) >= 0) break;
			else vertices.pop();
		}
		vertices.push(points[i]);
	}

	while(!vertices.empty()) {
		hull.pb(vertices.top());
		vertices.pop();
	}

	return hull;
}


int main(int argc, char * argv[]) {
	int i, j, k, m, n, p, q, r, t = 0;
	double x, y;
	vector<Vec2> points, hull;

	cin >> n;

	it(i, n) {
		scanf("%lf %lf", &x, &y);
		points.pb(Vec2(x, y));
	}

	hull = convex_hull(points);

	it(i, hull.size())
		printf("(%.2lf, %.2lf) ", hull[i].x, hull[i].y), t++;
	printf("\n");
	printf("%d\n", t);


	return 0;
}