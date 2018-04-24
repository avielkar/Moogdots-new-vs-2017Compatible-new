#pragma once

#ifndef GROD
#define GRID

#include "StdAfx.h"

/* Defines a structure of matrix element */
typedef struct ELEMENT   
{
	double ox, oy; //original point location
	double nx, ny; //new point location
}element;

// Grid class help to divide "original 3D drawing" display image by grid line 
// and each small quadrilateral will map to new quadrilateral by linear texture mapping.
class Grid
{
public:
	Grid(void);
	~Grid(void);
	double space,		//cm
		   lineWidth,	//pixel
		   x_offset,	//cm
		   y_offset,	//cm
		   screenWidth,		// Width of the screen (world coord).
		   screenHeight;	// Height of the screen (world coord).
	bool enable;	// if enable, dawn grid
	element** matrix; // draw grid depends on the matrix
	double shiftDistance;
	double cubicEqCoeff_X[10], cubicEqCoeff_Y[10];
	int FPcol; // fixation point have to alignment with middel vertical line in grid.
	
private:
	bool emptyMatrix; // default is true
	int oldrow; // for delete old 2D matrix array
	// create new matrix to store all coordinate of new and original points
	void CreateMatrix(void);
	// delete old matrix
	void DeleteMatrix(void);
public:	
	// return max possible column number
	int GetColNum(void);	
	// return max possible row number
	int GetRowNum(void);	
	// create new matrix to store all coordinate of new and original points
	// if there is old matrix, then delete it before create new matrix
	void SetupMatrix(void);	
};

#endif
