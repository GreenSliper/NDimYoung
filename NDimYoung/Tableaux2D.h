#pragma once
#include "YoungNDim.h"

using namespace std;

typedef int tabType;
class Tableaux2D
{
public:
	vector<vector<tabType>> table;
	Young2D* diag;
	tabType diagN = 0;

	//initializer
	Tableaux2D(Young2D &base);
	~Tableaux2D();
	mainType* GetRandomPointInDiag();
	//hookCornerCoord - max left-bottom hook cell (corner). isTerminal returns true, if the returned cell is terminal
	mainType* GetRandomPointOnThisHook(mainType* hookCornerCoord, bool* isTerminal, bool destroySourceCoordArray = false);
	void CreateColorFile(mainType* hookCorner, int frame, bool ignoreHookCorner);
	static bool TablesEqual(vector<vector<tabType>> table1, vector<vector<tabType>> table2);
	bool CoordsEqual(mainType* c1, mainType* c2, int len);
	void ProcessOneHookwalk(bool createOutputFiles = false, int *fileFrame = NULL);
	void GenerateRandomTable(bool createColorFiles = false, bool reserveDiagram = true);
	vector<vector<tabType>> ExtractTable();
};