// LPTInterfaceTest.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include "LPTController.h"

using namespace LPTInterface;
using namespace std;

int main(int ** args)
{
	LPTCOntroller* lptController = new LPTCOntroller();

	lptController->Connect();


	lptController->Write(0xb100, 0);

	lptController->Read(0xb100);

	cout << lptController->Read(0xb100);

	return 0;
}

