#pragma once
#include "Point.h"
#include <vector>

class Polygon
{
public:
	Polygon( std::vector<int> xP, std::vector<int> yP );
	Polygon( std::vector<Point> points);
	Polygon();
	~Polygon();

	void translate( int dx, int dy );
	std::vector<Point> getPoints();

	void drawPolygon( SDL_Renderer* renderer );
	void addPoint( Point p );
	void toString();

private:
	void fillPoints();

	std::vector<int> xPoints;
	std::vector<int> yPoints;
	std::vector<Point> allPoints;
	int nPoints;
};

