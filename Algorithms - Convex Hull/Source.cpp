#include "Polygon.h"
#include "Point.h"

#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>


//===========================================//
//================CONVEX HULL================//
//sort by x coordinate then y coordinate
bool wayToSort( Point &a, Point &b );

//calculate the gradient of the line between 2 points
double gradient( Point p1, Point p2 );
//claculate y intercept of line equation
double yIntercept( double m, Point p );
//calculate intersection of a line at a given x
double intersection( double x, Point p1, Point p2 );
//Check if 3 points make a right turn
bool rightTurn( Point p1, Point p2, Point p3 );

//Divide and conquer convex hull
Polygon dcHull( std::vector<Point> sortedPoints );
//Merge two polygons to create convex hull
Polygon merge( Polygon leftPolygon, Polygon rightPolygon );

//Convex hull of polygon
Polygon convexHull( std::vector<Point> sortedPoints );
//Upper hull of polygon
std::vector<Point>upperHull( std::vector<Point> points );
//Lower hull of polygon
std::vector<Point>lowerHull( std::vector<Point> points );
//================CONVEX HULL================//
//===========================================//


//===========================================//
//===============SDL FUNCTIONS===============//
//Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
//Window
SDL_Window* gWindow = NULL;
//Renderer
SDL_Renderer* gRenderer = NULL;

//Initialise SDL
bool init();
//Quit SDL
void close();
//===============SDL FUNCTIONS===============//
//===========================================//


//===========================================//
//=================DEBUGGING=================//
//#define MERGEDEBUG
//#define LINEDEBUG
//#define POINTDEBUG
#if defined MERGEDEBUG || defined LINEDEBUG
void pause();
#endif
//=================DEBUGGING=================//
//===========================================//

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
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	return success;
}

void close()
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

#if defined MERGEDEBUG || defined LINEDEBUG
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
#endif

/*
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
*/
bool wayToSort( Point& a, Point& b )
{
	return a.getX() < b.getX();
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

#ifdef POINTDEBUG
	for ( int i = 0; i < leftPoints.size() - 1; i++ )
	{
		for ( int j = i + 1; j < leftPoints.size(); j++ )
		{
			if ( leftPoints.at( i ) == leftPoints.at( j ) )
			{
				std::cout << "Left polygon contains overlapping points" << std::endl;
			}
		}
	}

	for ( int i = 0; i < rightPoints.size() - 1; i++ )
	{
		for ( int j = i + 1; j < rightPoints.size(); j++ )
		{
			if ( rightPoints.at( i ) == rightPoints.at( j ) )
			{
				std::cout << "Right polygon contains overlapping points" << std::endl;
			}
		}
	}
#endif

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

#ifdef MERGEDEBUG
	std::cout << "\nLeft Polygon: " << std::endl;
	for ( int i = 0; i < leftPoints.size(); i++ )
	{
		leftPoints.at( i ).print();
	}
	std::cout << "\nRight Polygon: " << std::endl;
	for ( int i = 0; i < rightPoints.size(); i++ )
	{
		rightPoints.at( i ).print();
	}
#endif

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
		//If intersection increases at next b (clockwise(--)), move to next b
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
			//loop back to end
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

		//If intersection increases at next a (anti-clockwise(++)), move to next a
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
			//loop back to start
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
		//If intersection decreases at next b (anti-clockwise(++)), move to next b
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
			//loop back to start
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

		//If intersection decreases at next a (clockwise(--)), move to next a
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
			//loop back to end
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

	//output
	Polygon mergedPolygon;

	//add lower tangent of left polygon
	mergedPolygon.addPoint( leftPoints.at( aLower ) );

	//add points of right polygon (start at lower tangent, working outside to higher tangent)
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

	//add points of left polygon (start at higher tangent, working outside to lower tangent)
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

Polygon dcHull( std::vector<Point> sortedPoints )
{
	size_t minSize = 4;

	if ( sortedPoints.size() <= minSize )
	{
		//no further iteration required
		return convexHull( sortedPoints );
	}
	else
	{
		//split sortedPoints into 2 vectors
		size_t const nHalf = sortedPoints.size() / 2;
		std::vector<Point> l1( sortedPoints.begin(), sortedPoints.begin() + nHalf );
		std::vector<Point> l2( sortedPoints.begin() + nHalf, sortedPoints.end() );

		//next iteration
		return merge( dcHull( l1 ), dcHull( l2 ) );
	}
}

Polygon convexHull( std::vector<Point> sortedPoints )
{
	std::vector<Point> lUpper;
	std::vector<Point> lLower;

	//generate upper and lower hulls
	lUpper = upperHull( sortedPoints );
	lLower = lowerHull( sortedPoints );

	//remove first point of lower hull (shared with upper hull)
	lLower.erase( lLower.begin() );
	lLower.pop_back();

	//add all points from lower hull to upper hull to create final output polygon
	std::vector<Point> outPolyPoints = lUpper;
	outPolyPoints.insert( outPolyPoints.end(), lLower.begin(), lLower.end() );

	return Polygon(outPolyPoints);
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
	//Rearranged gradient equation to avoid division by zero
	int g1 = ( p2.getY() - p1.getY() ) * ( p3.getX() - p2.getX() );
	int g2 = ( p3.getY() - p2.getY() ) * ( p2.getX() - p1.getX() );

	return g1 < g2;
}

int main( int argc, char* args[] )
{
	if( !init() )
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
		polygons.push_back( Polygon( { {2, 3, 5, 9, 18, 48, 71, 76, 80, 79, 90, 99, 89, 83, 55, 37, 31, 30, 17, 16}, {53, 14, 18, 28, 49, 35, 11, 19, 17, 44, 50, 87, 85, 77, 57, 60, 68, 69, 91, 79} } ) );
		polygons.push_back( Polygon( { {2, 3, 5, 9, 18, 48, 71, 76, 80, 79, 90, 99, 89, 83, 55, 37, 31, 30, 17, 16}, {53, 14, 18, 28, 49, 35, 11, 19, 17, 44, 50, 87, 85, 77, 57, 60, 68, 69, 91, 79} } ) );
		polygons.push_back( Polygon( { {2, 3, 5, 9, 18, 48, 71, 76, 80, 79, 90, 99, 89, 83, 55, 37, 31, 30, 17, 16}, {53, 14, 18, 28, 49, 35, 11, 19, 17, 44, 50, 87, 85, 77, 57, 60, 68, 69, 91, 79} } ) );
		
		polygons.at( 0 ).translate( 230, 160 );
		polygons.at( 1 ).translate( 400, 50 );
		polygons.at( 2 ).translate( 400, 160 );

		SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE );
		
		for( int i = 0; i < polygons.size(); i++ )
		{
			polygons.at(i).drawPolygon( gRenderer );
		}

		//Create points a, b and c
		Point a = { 200, 200 };
		Point b = { 380, 190 };
		Point c = { 540, 170 };

		//add points to respective polygons
		polygons.at( 0 ).addPoint( a );
		polygons.at( 0 ).addPoint( b );

		polygons.at( 1 ).addPoint( b );
		polygons.at( 1 ).addPoint( c );

		polygons.at( 2 ).addPoint( b );
		polygons.at( 2 ).addPoint( c );

		//Calculate convex hull for each polygon
		for( int i = 0; i < polygons.size(); i++ )
		{
			std::vector<Point> sortedPoints = polygons.at(i).getPoints();
			std::sort( sortedPoints.begin(), sortedPoints.end(), wayToSort );

			//set draw colour to make different paths clear
			if( i == 0 ) SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0xFF, SDL_ALPHA_OPAQUE );
			if( i == 1 ) SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, SDL_ALPHA_OPAQUE );
			if( i == 2 ) SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
			dcHull( sortedPoints ).drawPolygon( gRenderer );

			for( size_t j = 0; j < polygons.at(i).getPoints().size(); j++ )
			{
				polygons.at(i).getPoints().at( j ).drawPoint( gRenderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE );
			}

		}

		//Draw points
		a.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
		b.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );
		c.drawPoint( gRenderer, 0x00, 0x00, 0xFF, SDL_ALPHA_OPAQUE );

		//Update screen
		SDL_RenderPresent( gRenderer );

		while( isRunning )
		{
			//Handle events
			while( SDL_PollEvent( &e ) != 0 )
			{
				//Close window button pressed
				if( e.type == SDL_QUIT )
				{
					isRunning = false;
				}
			}
		}

		//Quit SDL
		close();
	}
	return 0;
}