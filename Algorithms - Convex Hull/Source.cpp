#include "Polygon.h"
#include "Point.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>

//#define MERGEDEBUG
//#define LINEDEBUG

//Initialise SDL
bool init();

//Quit SDL
void exit();

void pause();

bool wayToSort( Point &a, Point &b );

double gradient( Point p1, Point p2 );

Polygon merge( Polygon leftPolygon, Polygon rightPolygon );

//Convex hull of polygon
Polygon convexHull( Polygon input );

//Upper hull of polygon
std::vector<Point>upperHull( std::vector<Point> points );

//Lower hull of polygon
std::vector<Point>lowerHull( std::vector<Point> points );

//Check if 3 points make a right turn
bool rightTurn( Point p1, Point p2, Point p3 );


//Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

//Window
SDL_Window* gWindow = NULL;

//Renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialisation success
	bool success = true;

	//Initialise SDL
	if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
	{
		std::cout << "SDL failed to initialise. SDL error: " << SDL_GetError() << std::endl;
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "Convex Hull", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if ( gWindow == NULL )
		{
			std::cout << "SDL failed to create window. SDL error: " << SDL_GetError() << std::endl;
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if ( gRenderer == NULL )
			{
				std::cout << "SDL failed to create renderer. SDL error: " << SDL_GetError() << std::endl;
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
				SDL_RenderClear( gRenderer );
			}
		}
	}

	return success;
}

void exit()
{
	//Destroy and deallocate renderer
	SDL_DestroyRenderer( gRenderer );
	gRenderer = NULL;

	//Destroy and deallocate window
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void pause()
{
	std::cout << "Press any key to continue . . ." << std::endl;
	SDL_Event e;
	bool quit = false;
	while ( !quit )
	{
		while ( SDL_PollEvent( &e ) != 0 )
		{
			if ( e.type == SDL_KEYDOWN )
			{
				quit = true;
			}
			if ( e.type == SDL_QUIT )
			{
				quit = true;
				exit();
			}
		}
	}
}

bool wayToSort( Point& a, Point& b )
{
	if ( a.getX() != b.getX() )
	{
		return a.getX() < b.getX();
	}
	else
	{
		return a.getY() < b.getY();
	}
}

double gradient( Point p1, Point p2 )
{
	return (double) ( p2.getY() - p1.getY() ) / ( p2.getX() - p1.getX() );
}

double yIntercept( double m, Point p )
{
	return ( p.getY() - ( m * p.getX() ) );
}

double intersection( double x, Point p1, Point p2 )
{
	double m = gradient( p1, p2 );
	double c = yIntercept( m, p1 );
	return ( ( m*x ) + c );
}

Polygon merge( Polygon leftPolygon, Polygon rightPolygon )
{
	std::vector<Point> leftPoints = leftPolygon.getPoints();
	std::vector<Point> rightPoints = rightPolygon.getPoints();

	//Rightmost point of left polygon
	int aHigher = leftPolygon.getRightmostIndex();
	int aLower = aHigher;

	//Leftmost point of right polygon
	int bHigher = rightPolygon.getLeftmostIndex();
	int bLower = bHigher;

	//The line who's intersection we check
	double xToCheck = (double) ( leftPoints.at( aHigher ).getX() + rightPoints.at( bHigher ).getX() ) / 2;

	bool aMoved = false;
	bool bMoved = false;

	//Intersection with xToCheck;
	double ix = intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher ) );

#ifdef LINEDEBUG
	SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
	SDL_RenderClear( gRenderer );
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	leftPolygon.drawPolygon( gRenderer );
	rightPolygon.drawPolygon( gRenderer );
	SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
	SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
	SDL_RenderPresent( gRenderer );
	pause();
#endif

	//Higher tangent
	do
	{
		//If intersection increases at next b (clockwise(++)), move
		if ( bHigher > 0 )
		{
			if ( ix < intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher - 1 ) ) )
			{
				bHigher--;
				ix = intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher ) );
				bMoved = true;
			}
			else
			{
				bMoved = false;
			}
		}
		else
		{
			if ( ix < intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( rightPoints.size() - 1 ) ) )
			{
				bHigher = rightPoints.size() - 1;
				ix = intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher ) );
				bMoved = true;
			}
			else
			{
				bMoved = false;
			}
		}

#ifdef LINEDEBUG
		std::cout << "bHigher" << std::endl;
		SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
		SDL_RenderClear( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		leftPolygon.drawPolygon( gRenderer );
		rightPolygon.drawPolygon( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
		SDL_RenderPresent( gRenderer );
		pause();
#endif

		//If intersection increases at next a (anti-clockwise(--)), move
		if ( aHigher < leftPoints.size() - 1 )
		{
			if ( ix < intersection( xToCheck, leftPoints.at( aHigher + 1 ), rightPoints.at( bHigher ) ) )
			{
				aHigher++;
				ix = intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher ) );
				aMoved = true;
			}
			else
			{
				aMoved = false;
			}
		}
		else
		{
			if ( ix < intersection( xToCheck, leftPoints.at( 0 ), rightPoints.at( bHigher ) ) )
			{
				aHigher = 0;
				ix = intersection( xToCheck, leftPoints.at( aHigher ), rightPoints.at( bHigher ) );
				aMoved = true;
			}
			else
			{
				aMoved = false;
			}
		}

#ifdef LINEDEBUG
		std::cout << "aHigher" << std::endl;
		SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
		SDL_RenderClear( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		leftPolygon.drawPolygon( gRenderer );
		rightPolygon.drawPolygon( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
		SDL_RenderPresent( gRenderer );
		pause();
#endif

	}
	while ( aMoved || bMoved );

	ix = intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower ) );
	//Lower tangent
	do
	{
		//If intersection decreases at next b (anti-clockwise(--)), move
		if ( bLower < rightPoints.size() - 1 )
		{
			if ( ix > intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower + 1 ) ) )
			{
				bLower++;
				ix = intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower ) );
				bMoved = true;
			}
			else
			{
				bMoved = false;
			}
		}
		else
		{
			if ( ix > intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( 0 ) ) )
			{
				bLower = 0;
				ix = intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower ) );
				bMoved = true;
			}
			else
			{
				bMoved = false;
			}
		}

#ifdef LINEDEBUG
		std::cout << "bLower" << std::endl;
		SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
		SDL_RenderClear( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		leftPolygon.drawPolygon( gRenderer );
		rightPolygon.drawPolygon( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aLower ).getX(), leftPoints.at( aLower ).getY(), rightPoints.at( bLower ).getX(), rightPoints.at( bLower ).getY() );
		SDL_RenderPresent( gRenderer );
		pause();
#endif

		//If intersection decreases at next a (clockwise(++)), move
		if ( aLower > 0 )
		{
			if ( ix > intersection( xToCheck, leftPoints.at( aLower - 1 ), rightPoints.at( bLower ) ) )
			{
				aLower--;
				ix = intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower ) );
				aMoved = true;
			}
			else
			{
				aMoved = false;
			}
		}
		else
		{
			if ( ix > intersection( xToCheck, leftPoints.at( leftPoints.size() - 1 ), rightPoints.at( bLower ) ) )
			{
				aLower = leftPoints.size() - 1;
				ix = intersection( xToCheck, leftPoints.at( aLower ), rightPoints.at( bLower ) );
				aMoved = true;
			}
			else
			{
				aMoved = false;
			}
		}

#ifdef LINEDEBUG
		std::cout << "aLower" << std::endl;
		SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
		SDL_RenderClear( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
		leftPolygon.drawPolygon( gRenderer );
		rightPolygon.drawPolygon( gRenderer );
		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
		SDL_RenderDrawLine( gRenderer, leftPoints.at( aLower ).getX(), leftPoints.at( aLower ).getY(), rightPoints.at( bLower ).getX(), rightPoints.at( bLower ).getY() );
		SDL_RenderPresent( gRenderer );
		pause();
#endif

	}
	while ( aMoved || bMoved );

	Polygon mergedPolygon;

	mergedPolygon.addPoint( leftPoints.at( aLower ) );
	int rightCount = bLower;
	if ( bLower != bHigher )
	{
		do
		{
			mergedPolygon.addPoint( rightPoints.at( rightCount ) );
			if ( rightCount < rightPoints.size() - 1 )
			{
				rightCount++;
			}
			else
			{
				rightCount = 0;
			}
		}
		while ( rightCount != bHigher );
	}

	mergedPolygon.addPoint( rightPoints.at( rightCount ) );

	int leftCount = aHigher;
	if ( aHigher != aLower )
	{
		do
		{
			mergedPolygon.addPoint( leftPoints.at( leftCount ) );
			if ( leftCount < leftPoints.size() - 1 )
			{
				leftCount++;
			}
			else
			{
				leftCount = 0;
			}
		}
		while ( leftCount != aLower );
	}

#ifdef MERGEDEBUG
	std::cout << "\nMerged:" << std::endl;
	mergedPolygon.toString();
	std::cout << "\nLeftmost" << std::endl;
	mergedPolygon.getPoints().at( mergedPolygon.getLeftmostIndex() ).print();
	std::cout << "\nRightmost" << std::endl;
	mergedPolygon.getPoints().at( mergedPolygon.getRightmostIndex() ).print();
	SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, 0xFF );
	SDL_RenderClear( gRenderer );
	SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );
	mergedPolygon.drawPolygon( gRenderer );
	SDL_RenderPresent( gRenderer );
	pause();
#endif
	return mergedPolygon;
}

Polygon dcHull( Polygon input )
{
	size_t minSize = 4;
	std::vector<Point> sortedPoints = input.getPoints();
	std::sort( sortedPoints.begin(), sortedPoints.end(), wayToSort );

	if ( sortedPoints.size() <= minSize )
	{
		return convexHull( Polygon( sortedPoints ) );
	}
	else
	{
		//split
		size_t const nHalf = sortedPoints.size() / 2;
		std::vector<Point> l1( sortedPoints.begin(), sortedPoints.begin() + nHalf );
		std::vector<Point> l2( sortedPoints.begin() + nHalf, sortedPoints.end() );

		return merge( dcHull( Polygon( l1 ) ), dcHull( Polygon( l2 ) ) );
	}
	//return input;
}

Polygon convexHull( Polygon input )
{
	std::vector<Point> sortedPoints = input.getPoints();
	std::vector<Point> lUpper;
	std::vector<Point> lLower;

	//Sort all points on x value
	std::sort( sortedPoints.begin(), sortedPoints.end(), wayToSort );

	lUpper = upperHull( sortedPoints );
	lLower = lowerHull( sortedPoints );

	lLower.erase( lLower.begin() );
	lLower.pop_back();

	lUpper.insert( lUpper.end(), lLower.begin(), lLower.end() );

	std::vector<int> outX;
	std::vector<int> outY;

	for ( size_t i = 0; i < lUpper.size(); i++ )
	{
		outX.push_back( lUpper.at( i ).getX() );
		outY.push_back( lUpper.at( i ).getY() );
	}

	Polygon outPoly = Polygon( outX, outY );

	return outPoly;
}

std::vector<Point> upperHull( std::vector<Point> points )
{
	std::vector<Point> lUpper;

	//Add first 2 points to lUpper
	lUpper.push_back( points.at( 0 ) );
	lUpper.push_back( points.at( 1 ) );

	for ( size_t i = 2; i < points.size(); i++ )
	{
		//Add point to lUpper
		lUpper.push_back( points.at( i ) );

		//more than 2 points and doesn't make a right turn
		while ( ( lUpper.size() > 2 ) && !rightTurn( lUpper.at( lUpper.size() - 3 ), lUpper.at( lUpper.size() - 2 ), lUpper.at( lUpper.size() - 1 ) ) )
		{
			//Remove second last element
			lUpper.erase( lUpper.end() - 2 );
		}
	}

	return lUpper;
}

std::vector<Point> lowerHull( std::vector<Point> points )
{
	std::vector<Point> lLower;

	//Add last 2 points to lLower
	lLower.push_back( points.at( points.size() - 1 ) );
	lLower.push_back( points.at( points.size() - 2 ) );

	for ( int i = ( points.size() - 3 ); i >= 0; i-- )
	{
		//Add point to lUpper
		lLower.push_back( points.at( i ) );

		//more than 2 points and doesn't make a right turn
		while ( ( lLower.size() > 2 ) && !rightTurn( lLower.at( lLower.size() - 3 ), lLower.at( lLower.size() - 2 ), lLower.at( lLower.size() - 1 ) ) )
		{
			//Remove second last element
			lLower.erase( lLower.end() - 2 );
		}
	}

	return lLower;
}

bool rightTurn( Point p1, Point p2, Point p3 )
{
	//double g1 = (double) ( p2.getY() - p1.getY() ) / ( p2.getX() - p1.getX() );
	//double g2 = (double) ( p3.getY() - p2.getY() ) / ( p3.getX() - p2.getX() );

	int g1 = ( p2.getY() - p1.getY() ) * ( p3.getX() - p2.getX() );
	int g2 = ( p3.getY() - p2.getY() ) * ( p2.getX() - p1.getX() );

	return g1 < g2;
}

int main( int argc, char* args[] )
{
	if ( !init() )
	{
		std::cout << "Failed to initialise." << std::endl;
	}
	else
	{
		//Main loop flag
		bool isRunning = true;

		//Event handler
		SDL_Event e;

		//Create polygons
		std::vector<Polygon> polygons;
		polygons.push_back( Polygon( { {5, 3, 2, 79, 90, 80, 55, 71, 17, 17, 99, 89, 76, 30, 37, 9, 16, 31, 48, 83}, {18, 14, 53, 44, 50, 17, 57, 11, 91, 49, 87, 85, 19, 69, 60, 28, 79, 68, 35, 77} } ) );
		polygons.push_back( Polygon( { {12, 45, 5, 74, 8, 85, 7, 78, 17, 21, 70, 64, 50, 79, 91, 10, 87, 11, 2, 66}, {82, 89, 65, 97, 52, 69, 16, 56, 74, 85, 35, 51, 46, 99, 31, 83, 12, 6, 8, 84} } ) );
		polygons.push_back( Polygon( { {55, 12, 42, 6, 81, 36, 10, 76, 41, 73, 59, 60, 3, 16, 65, 47, 9, 40, 24, 21}, {87, 61, 100, 31, 63, 75, 21, 89, 8, 35, 96, 49, 74, 93, 64, 56, 22, 11, 66, 98} } ) );

		//Translate polygons to desired location
		polygons.at( 0 ).translate( 230, 160 );
		polygons.at( 1 ).translate( 400, 50 );
		polygons.at( 2 ).translate( 400, 160 );

		//Create points a, b, and c
		Point a = { 200, 200 };
		Point b = { 380, 190 };
		Point c = { 540, 170 };

		//Draw each polygon as its convex hull filled with inner points (Green lines, red points)
		for ( size_t i = 0; i < polygons.size(); i++ )
		{
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			//convexHull( polygons.at( i ) ).drawPolygon( gRenderer );
			dcHull( polygons.at( i ) ).drawPolygon( gRenderer );
		}

		//Add start and end points to the polygons
		polygons.at( 0 ).addPoint( a );
		polygons.at( 0 ).addPoint( b );

		polygons.at( 1 ).addPoint( b );
		polygons.at( 1 ).addPoint( c );

		polygons.at( 2 ).addPoint( b );
		polygons.at( 2 ).addPoint( c );

		//Draw each path as the convex hull of its polygon (Cyan)
		for ( size_t i = 0; i < polygons.size(); i++ )
		{
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0xFF, SDL_ALPHA_OPAQUE );
			//convexHull( polygons.at( i ) ).drawPolygon( gRenderer );
			dcHull( polygons.at( i ) ).drawPolygon( gRenderer );

			//Draw each point in the polygon
			for ( size_t j = 0; j < polygons.at( i ).getPoints().size(); j++ )
			{
				polygons.at( i ).getPoints().at( j ).drawPoint( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			}
		}

		//Draw start and end points (Blue)
		a.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
		b.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
		c.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );

		//Update screen
		SDL_RenderPresent( gRenderer );


		while ( isRunning )
		{
			while ( SDL_PollEvent( &e ) )
			{
				//X button
				if ( e.type == SDL_QUIT )
				{
					isRunning = false;
				}
			}

		}
	}

	exit();

	return 0;
}