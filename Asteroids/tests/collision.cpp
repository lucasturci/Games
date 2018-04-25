#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

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
	Vec2 versor() { return Vec2(x, y)*(1/magnitude()); }
	Vec2 perpendicular() { return Vec2(-y, x); }

	/* Operators */
	/////////////////////////////////////////////////////////////////
	Vec2 operator+ (Vec2 op) { return Vec2(x + op.x, y + op.y); }
	Vec2 operator- (Vec2 op) { return Vec2(x - op.x, y - op.y); }
	Vec2 operator- () { return Vec2(-x, -y); }
	Vec2 operator* (double factor) { return Vec2(x*factor, y*factor); }
	double operator* (Vec2 op) { return x*op.x + y*op.y; }
	bool operator== (Vec2 op) { return x == op.x && y == op.y; }
	bool operator!= (Vec2 op) { return x != op.x || y != op.y; }
	bool operator> (Vec2 op) { return magnitude() > op.magnitude(); }
	bool operator>= (Vec2 op) { return magnitude() >= op.magnitude(); }
	bool operator< (Vec2 op) { return magnitude() < op.magnitude(); }
	bool operator<= (Vec2 op) { return magnitude() <= op.magnitude(); }

};

using namespace std;


bool has_gap(vector<Vec2> points1, vector<Vec2> points2) {
	int i, j, n, m;
	Vec2 base, origin, projection;
	double corners1[2], corners2[2];
	bool found_gap = false;
	double dist;

	n = points1.size();
	m = points2.size();

	origin = Vec2(0.0, 0.0);

	points1.push_back(points1.front());
	points2.push_back(points2.front());

	for(i = 1; i <= n; ++i) {
		base = (points1[i] - points1[i-1]).perpendicular();
		base = base.versor();

		projection = base.versor()*(points1[0]*base);
		corners1[0] = corners1[1] = (projection*base >= 0)? projection.magnitude() : -projection.magnitude();
		for(j = 0; j < n; ++j) {
			projection = base.versor()*(points1[j]*base);

			dist = (projection*base >= 0)? projection.magnitude() : -projection.magnitude();
			if(dist < corners1[0]) corners1[0] = dist;
			if(dist > corners1[1]) corners1[1] = dist;
		}

		projection = base.versor()*(points2[0]*base);
		corners2[0] = corners2[1] = (projection*base >= 0)? projection.magnitude() : -projection.magnitude();
		for(j = 0; j < m; ++j) {
			projection = base.versor()*(points2[j]*base);

			dist = (projection*base >= 0)? projection.magnitude() : -projection.magnitude();
			if(dist < corners2[0]) corners2[0] = dist;
			if(dist > corners2[1]) corners2[1] = dist;
		}

		printf("%lf %lf\n", base.x, base.y);
		if(corners1[0] > corners2[1] or corners1[1] < corners2[0]) {
			printf("Has gap\n");
			found_gap = true;
		}
		else printf("No gap\n");
	}

	return found_gap;
}

bool isColiding(vector<Vec2> points1, vector<Vec2> points2) {
	if(points1.size() >= 3 and points2.size() >= 3) return !has_gap(points1, points2) and !has_gap(points2, points1);
	else if(points1.size() < 3) return !has_gap(points2, points1);
	else return !has_gap(points1, points2);
}


int main(int argc, char * argv[]) {
	int n, m, i;
	double x, y;
	vector<Vec2> points1, points2;
	cin >> n >> m;

	if(n < 3 and m < 3) {
		printf("You cannot form a polygon with less then 3 points\n");
		return 0;
	}

	for(i = 0; i < n; ++i) {
		scanf("%lf %lf", &x, &y);
		points1.emplace_back(x, y);
	} 

	for(i = 0; i < m; ++i) {
		scanf("%lf %lf", &x, &y);
		points2.emplace_back(x, y);
	} 

	if(isColiding(points1, points2)) printf("They are colliding\n");
	else printf("The poligons are not colliding\n");

	return 0;
}