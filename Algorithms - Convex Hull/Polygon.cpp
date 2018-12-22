#include "Polygon.h"

Polygon::Polygon( std::vector<int> xP, std::vector<int> yP )
{
	xPoints = xP;
	yPoints = yP;
	fillPoints();
}

Polygon::Polygon( std::vector<Point> points )
{
	for ( size_t i = 0; i < points.size(); i++ )
	{
		allPoints.push_back( points.at( i ) );
	}
}

Polygon::Polygon()
{

}

Polygon::~Polygon()
{
}

std::vector<Point> Polygon::getPoints()
{
	return allPoints;
}

void Polygon::drawPolygon( SDL_Renderer * renderer )
{
	for ( int i = 1; i < nPoints; i++ )
	{
		//Draw each line
		SDL_RenderDrawLine( renderer, allPoints.at( i - 1 ).getX(), allPoints.at( i - 1 ).getY(), allPoints.at( i ).getX(), allPoints.at( i ).getY() );
	}
	//Final line
	SDL_RenderDrawLine( renderer, allPoints.at( nPoints - 1 ).getX(), allPoints.at( nPoints - 1 ).getY(), allPoints.at( 0 ).getX(), allPoints.at( 0 ).getY() );
}

//Translate the polygon by (dx, dy)
void Polygon::translate( int dx, int dy )
{
	for ( int i = 0; i < nPoints; i++ )
	{
		allPoints.at( i ).setX( allPoints.at( i ).getX() + dx );
		allPoints.at( i ).setY( allPoints.at( i ).getY() + dy );
	}
}

//Fill allPoints vector with each point in the polygon
void Polygon::fillPoints()
{
	for ( size_t i = 0; i < xPoints.size(); i++ )
	{
		allPoints.push_back( Point( xPoints.at( i ), yPoints.at( i ) ) );
	}
	nPoints = allPoints.size();
}

//Add a specific point to the polygon
void Polygon::addPoint( Point p )
{
	allPoints.push_back( p );
}