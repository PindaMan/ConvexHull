#pragma once
#include <SDL.h>

class Point
{
public:
	Point( int x, int y );
	Point();
	~Point();

	int getX();
	int getY();
	void setX( int x );
	void setY( int y );

	bool operator == ( Point& toCompare );
	void print();

	void drawPoint( SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a );

private:
	int xPos;
	int yPos;
};

