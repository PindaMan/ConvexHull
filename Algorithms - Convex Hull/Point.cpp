#include "Point.h"
#include <iostream>

Point::Point( int x, int y )
{
	xPos = x;
	yPos = y;
}

Point::Point()
{
	xPos = 0;
	yPos = 0;
}

Point::~Point()
{
}

int Point::getX()
{
	return xPos;
}

int Point::getY()
{
	return yPos;
}

void Point::setX( int x )
{
	xPos = x;
}

void Point::setY( int y )
{
	yPos = y;
}

//Point comparison
bool Point::operator == ( Point& toCompare )
{
	if ( getX() == toCompare.getX() )
	{
		return getY() == toCompare.getY();
	}
	else
	{
		return false;
	}
}

void Point::print()
{
	std::cout << "(" << getX() << ", " << getY() << ")" << std::endl;
}

//Draw this point as a rectangle
void Point::drawPoint( SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a )
{
	SDL_SetRenderDrawColor( renderer, r, g, b, a );
	SDL_Rect rect{ getX() - 1, getY() - 1, 3, 3 };
	SDL_RenderFillRect( renderer, &rect );
}