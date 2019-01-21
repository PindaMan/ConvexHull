#include "Polygon.h"

Polygon::Polygon( std::vector<int> xP, std::vector<int> yP )
{
	rightmostIndex = 0;
	leftmostIndex = 0;
	nPoints = 0;
	xPoints = xP;
	yPoints = yP;
	fillPoints();
}

Polygon::Polygon( std::vector<Point> points )
{
	rightmostIndex = 0;
	leftmostIndex = 0;
	for ( size_t i = 0; i < points.size(); i++ )
	{
		allPoints.push_back( points.at( i ) );
		//Update leftmost and rightmost point if necessary
		if ( points.at( i ).getX() > allPoints.at( rightmostIndex ).getX() )
		{
			rightmostIndex = allPoints.size() - 1;
		}
		if ( points.at( i ).getX() < allPoints.at( leftmostIndex ).getX() )
		{
			leftmostIndex = allPoints.size() - 1;
		}
	}
	nPoints = allPoints.size();
}

Polygon::Polygon()
{
	rightmostIndex = 0;
	leftmostIndex = 0;
	nPoints = 0;
}

Polygon::~Polygon()
{
}

std::vector<Point> Polygon::getPoints()
{
	return allPoints;
}

int Polygon::getRightmostIndex()
{
	return rightmostIndex;
}

int Polygon::getLeftmostIndex()
{
	return leftmostIndex;
}

void Polygon::drawPolygon( SDL_Renderer * renderer )
{
	for ( int i = 1; i < nPoints; i++ )
	{
		//Draw each line
		SDL_RenderDrawLine( renderer, allPoints.at( i - 1 ).getX(), allPoints.at( i - 1 ).getY(), allPoints.at( i ).getX(), allPoints.at( i ).getY() );
	}
	//Final line
	if ( nPoints > 1 )
	{
		SDL_RenderDrawLine( renderer, allPoints.at( nPoints - 1 ).getX(), allPoints.at( nPoints - 1 ).getY(), allPoints.at( 0 ).getX(), allPoints.at( 0 ).getY() );
	}
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
		//Update leftmost and rightmost point if necessary
		if ( xPoints.at( i ) > allPoints.at( rightmostIndex ).getX() )
		{
			rightmostIndex = allPoints.size() - 1;
		}
		if ( xPoints.at( i ) < allPoints.at( leftmostIndex ).getX() )
		{
			leftmostIndex = allPoints.size() - 1;
		}
	}
	nPoints = allPoints.size();
}

//Add a specific point to the polygon
void Polygon::addPoint( Point p )
{
	allPoints.push_back( p );
	//Update leftmost and rightmost point if necessary
	if ( p.getX() > allPoints.at( rightmostIndex ).getX() )
	{
		rightmostIndex = allPoints.size() - 1;
	}
	if ( p.getX() < allPoints.at( leftmostIndex ).getX() )
	{
		leftmostIndex = allPoints.size() - 1;
	}
	nPoints++;
}

void Polygon::toString()
{
	for ( size_t i = 0; i < allPoints.size(); i++ )
		allPoints.at( i ).print();
}