#include "grid.h"
#include "GlobalDefs.h"

Grid::Grid(void)
{
	space = 10.0,		//cm
	lineWidth = 3.0,	//pixel
    x_offset = 1.0,		//cm
	y_offset = 1.0,		//cm
	screenWidth = 127.0,	// Width of the screen (world coord).
	screenHeight = 149.0;	// Height of the screen (world coord).
	enable = true;
	oldrow = 0; 
	matrix = NULL; 
	emptyMatrix = true;
	shiftDistance = 0.1; //cm
	memset(cubicEqCoeff_X, 0.0, 10 * sizeof(double));
	memset(cubicEqCoeff_Y, 0.0, 10 * sizeof(double));
	FPcol=0;
}

Grid::~Grid(void)
{
}

int Grid::GetColNum(void)
{
	int n = (int)(screenWidth/space);
	if((screenWidth-n*space)>0.0) n++;

	// return max possible column number
	return n+1;
}

int Grid::GetRowNum(void)
{
	int n = (int)(screenHeight/space);
	if((screenHeight-n*space)>0.0) n++;

	// return max possible row number
	return n+1;
}


void Grid::CreateMatrix(void)
{
	int r = GetRowNum();
	int c = GetColNum();

	matrix = new element*[r+1];
	for(int i=0;i<r+1;i++) matrix[i]=new element[c+1];

	emptyMatrix=false;

	x_offset=fmod(screenWidth/2.0, space);
	FPcol=(int)(screenWidth/2.0/space)+1;
}

void Grid::DeleteMatrix(void)
{
	for(int i=0;i<oldrow+1;i++) 
		delete[] *(matrix+i);
	delete[] matrix;
}

void Grid::SetupMatrix(void)
{
	int row = GetRowNum();
	int col = GetColNum();
	int r, c;
	

	if(!emptyMatrix) DeleteMatrix();
	
	oldrow = row;
	CreateMatrix();

	double mx=0.0, my=0.0;
	if(x_offset>0.0) mx=x_offset-space;
	if(y_offset>0.0) my=y_offset-space;
	for(r=0; r<=row; r++){	//y-direction
		for(c=0; c<=col; c++){	//x-direction
			matrix[r][c].ox=matrix[r][c].nx=(double)c*space+mx;
			matrix[r][c].oy=matrix[r][c].ny=(double)r*space+my;
		}
	}

	for(c=0; c<=col; c++)	// modify if last row is greater than screenHigh
		if(matrix[row][c].oy>screenHeight) matrix[row][c].oy=matrix[row][c].ny=screenHeight;

	for(r=0; r<=row; r++) // modify if last column is greater that screenWidth
		if(matrix[r][col].ox>screenWidth) matrix[r][col].ox=matrix[r][col].nx=screenWidth;

	for(c=0; c<=col; c++)	// modify if first row is less than zero
		if(matrix[0][c].oy<0.0) matrix[0][c].oy=matrix[0][c].ny=0.0;

	for(r=0; r<=row; r++) // modify if first column is less that zero
		if(matrix[r][0].ox<0.0) matrix[r][0].ox=matrix[r][0].nx=0.0;
}

