// CB_Tools.h: interface for the CCB_Tools class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#ifndef CBTOOLS
#define CBTOOLS

#include "StdAfx.h"

class CCB_Tools  
{
public:
	int Get8255BaseAddr(int boardNum, int chipNum);
	int GetBoardNum(char *boardName);
	int DIO_board_num;
	int DIO_base_address;
	CCB_Tools();
	virtual ~CCB_Tools();

};

#endif
