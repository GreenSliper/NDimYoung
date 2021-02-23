#pragma once
#include "YoungNDim.h"

using namespace std;

typedef int tabType;
class Tableau2D
{
private:
	vector<mainType> reserveColumns;
	tabType diagNReserve = 0;
public:
	vector<vector<tabType>> table;
	vector<tabType> fastTable;
	Young2D* diag;
	tabType diagN = 0;

	//initializer
	Tableau2D(Young2D &base);
	~Tableau2D();
	mainType* GetRandomPointInDiag();
	//hookCornerCoord - max left-bottom hook cell (corner). isTerminal returns true, if the returned cell is terminal
	mainType* GetRandomPointOnThisHook(mainType* hookCornerCoord, bool* isTerminal, bool destroySourceCoordArray = false);
	mainType* GetRandomPointOnThisHookFast(mainType* hookCornerCoord, bool* isTerminal);
	void CreateColorFile(mainType* hookCorner, int frame, bool ignoreHookCorner);
	static bool TablesEqual(vector<vector<tabType>> table1, vector<vector<tabType>> table2);
	bool CoordsEqual(mainType* c1, mainType* c2, int len);
	void ProcessOneHookwalk(bool createOutputFiles = false, int* fileFrame = NULL);
	void ProcessOneHookwalkFast();
	void GenerateRandomTable(bool createColorFiles = false, bool reserveDiagram = true);
	vector<tabType> GenerateRandomTableFast(bool reserveState);
	vector<vector<tabType>> ExtractTable();
};