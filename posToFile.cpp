#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include "posToFile.h"
#include "moveGeneral.h"

extern int xpos;
extern int ypos;
extern int zpos;
extern int apos;

void posToFile(cmdHandler*xtplPrinter)
{
	std::ofstream axisPosData;
	axisPosData.open("axisPosData.txt");
	/*if (axisPosData.good() != TRUE) {
		std::cout << "V#: Failed to open axisPosData" << std::endl;
		return CANNOT_OPEN_FILE;
	}*/
	axisPosData << xpos << std::endl << ypos << std::endl << zpos << std::endl << apos << std::endl;;
	axisPosData.close();
}

void posToFileAutoDispensing(cmdHandler*xtplPrinter)
{
	std::ofstream axisPosData;
	axisPosData.open("axisPosDataAutoDispensing.txt");
	/*if (axisPosData.good() != TRUE) {
	std::cout << "V#: Failed to open axisPosData" << std::endl;
	return CANNOT_OPEN_FILE;
	}*/
	axisPosData << xpos << std::endl << ypos << std::endl << zpos << std::endl;
	axisPosData.close();
}

void posToFileAutoPrinting(cmdHandler*xtplPrinter)
{
	std::ofstream axisPosData;
	axisPosData.open("axisPosDataAutoPrinting.txt");
	/*if (axisPosData.good() != TRUE) {
	std::cout << "V#: Failed to open axisPosData" << std::endl;
	return CANNOT_OPEN_FILE;
	}*/
	axisPosData << xpos << std::endl << ypos << std::endl << zpos << std::endl;
	axisPosData.close();
}

void fileToPosAutoDispensing(cmdHandler*xtplPrinter) {
	//std::ofstream axisPosData;
	//axisPosData.open("axisPosData.txt");
	std::string line;
	std::ifstream axisPosData("axisPosDataAutoDispensing.txt");
	if (axisPosData.is_open())
	{
		getline(axisPosData, line);
		xpos = atoi(line.c_str());

		getline(axisPosData, line);
		ypos = atoi(line.c_str());

		getline(axisPosData, line);
		zpos = atoi(line.c_str());

		std::cout << "Loaded position: " << xpos << " " << ypos << " " << zpos << std::endl;
		axisPosData.close();
	}
	else {
		printf("Can't open file!\n");
	}

}

void fileToPosAutoPrinting(cmdHandler*xtplPrinter) {
	//std::ofstream axisPosData;
	//axisPosData.open("axisPosData.txt");
	std::string line;
	std::ifstream axisPosData("axisPosDataAutoPrinting.txt");
	if (axisPosData.is_open())
	{
		getline(axisPosData, line);
		xpos = atoi(line.c_str());

		getline(axisPosData, line);
		ypos = atoi(line.c_str());

		getline(axisPosData, line);
		zpos = atoi(line.c_str());

		std::cout << "Loaded position: " << xpos << " " << ypos << " " << zpos << std::endl;
		axisPosData.close();
	}
	else {
		printf("Can't open file!\n");
	}

}

void fileToPos(cmdHandler*xtplPrinter) {
	//std::ofstream axisPosData;
	//axisPosData.open("axisPosData.txt");
	std::string line;
	std::ifstream axisPosData("axisPosData.txt");
	if (axisPosData.is_open())
	{
		getline(axisPosData, line);		
		xpos = atoi(line.c_str());
		SetCurrPosition(xtplPrinter, 'x', xpos);

		getline(axisPosData, line);
		ypos = atoi(line.c_str());
		SetCurrPosition(xtplPrinter, 'y', ypos);

		getline(axisPosData, line);
		zpos = atoi(line.c_str());
		SetCurrPosition(xtplPrinter, 'z', zpos);

		getline(axisPosData, line);
		apos = atoi(line.c_str());
		SetCurrPosition(xtplPrinter, 'a', apos);

		std::cout << "Loaded position: " << xpos << " " << ypos << " " << zpos << " " << apos << std::endl;
		axisPosData.close();
	}	else {
		printf("Can't open file!\n");
	}
	
}

void moveToPos(cmdHandler*xtplPrinter)
{
	std::string mvpos;
	std::ifstream moveTo("goToPos.txt");
	if (moveTo.is_open())
	{
		getline(moveTo, mvpos);
		Move(xtplPrinter, 'x', atoi(mvpos.c_str()), 2500);
		getline(moveTo, mvpos);
		Move(xtplPrinter, 'y', atoi(mvpos.c_str()), 2000);
		getline(moveTo, mvpos);
		Move(xtplPrinter, 'z', atoi(mvpos.c_str()), 2000);
		getline(moveTo, mvpos);
		Move(xtplPrinter, 'a', atoi(mvpos.c_str()), 3000);	
		printf("Printer moved to position saved in goToPos.txt.\n");
	}
	else 
	{
		printf("Can't open file!\n");
	}

}

void posAsHome(cmdHandler*xtplPrinter)
{
	xpos = 20000;
	ypos = 20000;
	zpos = 20000;
	apos = 20000;
	SetCurrPosition(xtplPrinter, 'x', xpos);
	SetCurrPosition(xtplPrinter, 'y', ypos);
	SetCurrPosition(xtplPrinter, 'z', zpos);
	SetCurrPosition(xtplPrinter, 'a', apos);
}