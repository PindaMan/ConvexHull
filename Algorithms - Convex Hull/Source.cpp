#include "Polygon.h"
#include "Point.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define LINEDEBUG

//Initialise SDL
bool init();

//Quit SDL
void exit();


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
	std::cout << (double) ( p2.getY() - p1.getY() ) / ( p2.getX() - p1.getX() ) << std::endl;
	return (double) ( p2.getY() - p1.getY() ) / ( p2.getX() - p1.getX() );
}

Polygon merge( Polygon leftPolygon, Polygon rightPolygon )
{
	std::vector<Point> leftPoints = leftPolygon.getPoints();
	std::vector<Point> rightPoints = rightPolygon.getPoints();

#ifdef LINEDEBUG
	std::cout << "Left polygon:" << std::endl;
	leftPolygon.toString();
	std::cout << "Right polygon:" << std::endl;
	rightPolygon.toString();
	SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
	SDL_RenderClear( gRenderer );

	SDL_Event e;
	bool quit = false;
	while ( !quit )
	{
		while ( SDL_PollEvent( &e ) != 0 )
		{
			if ( e.type == SDL_KEYDOWN )
			{
				if ( e.key.keysym.sym == SDLK_SPACE )
				{
					quit = true;
				}
			}
		}
		SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
		leftPolygon.drawPolygon( gRenderer );
		rightPolygon.drawPolygon( gRenderer );
		SDL_RenderPresent( gRenderer );
	}
#endif

	int aHigher = 0;
	int aLower = leftPoints.size() - 1;

	int bHigher = rightPoints.size() - 1;
	int bLower = 0;

	int prevAHigher = aHigher;
	int prevBHigher = bHigher;

	int prevALower = aLower;
	int prevBLower = bLower;

	bool found = false;

	bool movingA = true;
	bool movingB = false;

	//Higher tangent
	while ( !found )
	{

		while ( movingA )
		{
			std::cout << "Moving A Higher" << std::endl;
			if ( aHigher < leftPoints.size() - 1 )
			{
				if ( gradient( leftPoints.at( aHigher ), rightPoints.at( bHigher ) ) <= gradient( leftPoints.at( aHigher + 1 ), rightPoints.at( bHigher ) ) ) //>=
				{
					aHigher++;
				}
				else
				{
					movingA = !movingA;
					movingB = !movingB;
				}
			}
			else
			{
				movingA = !movingA;
				movingB = !movingB;
			}

#ifdef LINEDEBUG
			SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
			SDL_RenderClear( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			leftPolygon.drawPolygon( gRenderer );
			rightPolygon.drawPolygon( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
			SDL_RenderPresent( gRenderer );
			SDL_Event e;
			bool quit = false;
			while ( !quit )
			{
				while ( SDL_PollEvent( &e ) != 0 )
				{
					if ( e.type == SDL_KEYDOWN )
					{
						if ( e.key.keysym.sym == SDLK_SPACE )
						{
							quit = true;
						}
					}
				}
			}
#endif

		}

		while ( movingB )
		{
			std::cout << "Moving B Higher" << std::endl;
			if ( bHigher > 0 )
			{
				if ( gradient( leftPoints.at( aLower ), rightPoints.at( bHigher ) ) > gradient( leftPoints.at( aLower ), rightPoints.at( bHigher - 1 ) ) ) //>
				{
					bHigher--;
				}
				else
				{
					movingA = !movingA;
					movingB = !movingB;
				}
			}
			else
			{
				movingA = !movingA;
				movingB = !movingB;
			}

#ifdef LINEDEBUG
			SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
			SDL_RenderClear( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			leftPolygon.drawPolygon( gRenderer );
			rightPolygon.drawPolygon( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
			SDL_RenderPresent( gRenderer );
			SDL_Event e;
			bool quit = false;
			while ( !quit )
			{
				while ( SDL_PollEvent( &e ) != 0 )
				{
					if ( e.type == SDL_KEYDOWN )
					{
						if ( e.key.keysym.sym == SDLK_SPACE )
						{
							quit = true;
						}
					}
				}
			}
#endif

		}

		if ( ( prevAHigher == aHigher ) && ( prevBHigher == bHigher ) )
		{
			found = true;
		}

		prevAHigher = aHigher;
		prevBHigher = bHigher;

	}

	found = false;

	movingA = false;
	movingB = true;

	//Lower tangent
	while ( !found )
	{

		while ( movingA )
		{
			std::cout << "Moving A Lower" << std::endl;
			if ( aLower > 0 )
			{
				if ( gradient( leftPoints.at( aLower ), rightPoints.at( bLower ) ) >= gradient( leftPoints.at( aLower - 1 ), rightPoints.at( bLower ) ) ) //<=
				{
					aLower--;
				}
				else
				{
					movingA = !movingA;
					movingB = !movingB;
				}
			}
			else
			{
				movingA = !movingA;
				movingB = !movingB;
			}

#ifdef LINEDEBUG
			SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
			SDL_RenderClear( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			leftPolygon.drawPolygon( gRenderer );
			rightPolygon.drawPolygon( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aLower ).getX(), leftPoints.at( aLower ).getY(), rightPoints.at( bLower ).getX(), rightPoints.at( bLower ).getY() );
			SDL_RenderPresent( gRenderer );
			SDL_Event e;
			bool quit = false;
			while ( !quit )
			{
				while ( SDL_PollEvent( &e ) != 0 )
				{
					if ( e.type == SDL_KEYDOWN )
					{
						if ( e.key.keysym.sym == SDLK_SPACE )
						{
							quit = true;
						}
					}
				}
			}
#endif

		}

		while ( movingB )
		{
			std::cout << "Moving B Lower" << std::endl;
			if ( bLower < rightPoints.size() - 1 )
			{
				if ( gradient( leftPoints.at( aHigher ), rightPoints.at( bLower ) ) < gradient( leftPoints.at( aHigher ), rightPoints.at( bLower + 1 ) ) ) //<
				{
					bLower++;
				}
				else
				{
					movingA = !movingA;
					movingB = !movingB;
				}
			}
			else
			{
				movingA = !movingA;
				movingB = !movingB;
			}
		

#ifdef LINEDEBUG
			SDL_SetRenderDrawColor( gRenderer, 0x66, 0x66, 0x66, SDL_ALPHA_OPAQUE );
			SDL_RenderClear( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			leftPolygon.drawPolygon( gRenderer );
			rightPolygon.drawPolygon( gRenderer );
			SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aHigher ).getX(), leftPoints.at( aHigher ).getY(), rightPoints.at( bHigher ).getX(), rightPoints.at( bHigher ).getY() );
			SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
			SDL_RenderDrawLine( gRenderer, leftPoints.at( aLower ).getX(), leftPoints.at( aLower ).getY(), rightPoints.at( bLower ).getX(), rightPoints.at( bLower ).getY() );
			SDL_RenderPresent( gRenderer );
			SDL_Event e;
			bool quit = false;
			while ( !quit )
			{
				while ( SDL_PollEvent( &e ) != 0 )
				{
					if ( e.type == SDL_KEYDOWN )
					{
						if ( e.key.keysym.sym == SDLK_SPACE )
						{
							quit = true;
						}
					}
				}
			}
#endif

		}

		if ( ( prevALower == aLower ) && ( prevBLower == bLower ) )
		{
			found = true;
		}

		prevALower = aLower;
		prevBLower = bLower;

	}

	Point aHigherPoint = leftPoints.at( aHigher );
	Point aLowerPoint = leftPoints.at( aLower );

	Point bHigherPoint = rightPoints.at( bHigher );
	Point bLowerPoint = rightPoints.at( bLower );

	Polygon mergedPolygon;

	for ( int i = 0; i <= aHigher; i++ )
	{
		mergedPolygon.addPoint( leftPoints.at( i ) );
	}
	for ( int i = bHigher; i <= bLower; i++ )
	{
		mergedPolygon.addPoint( rightPoints.at( i ) );
	}
	for ( int i = aLower; i < leftPoints.size(); i++ )
	{
		mergedPolygon.addPoint( leftPoints.at( i ) );
	}

	std::cout << "Merged" << std::endl;
	mergedPolygon.toString();
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

	outPoly.drawPolygon( gRenderer ); //Remove at the end

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
			//dcHull( polygons.at( i ) ).drawPolygon( gRenderer );
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