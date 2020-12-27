#include "YoungNDim.h"
#include "Tableaux2D.h"

using namespace std;

typedef int tabType;

//clones base form
Tableaux2D::Tableaux2D(Young2D &base)
{
	diag = base.Clone();
	for (int i = 0; i < base.columns.size(); i++)
	{
		table.push_back(vector<tabType>(base.columns[i]));
		diagN += base.columns[i];
	}
}

Tableaux2D::~Tableaux2D()
{
	table.clear();
	delete diag;
}

mainType* Tableaux2D::GetRandomPointInDiag()
{
	mainType* coord = new mainType[2];
	int rndVal = rand() % diagN;
	for (int i = 0; i < diag->columns.size(); i++)
		if (rndVal >= diag->columns[i])
			rndVal -= diag->columns[i];
		else
		{
			coord[0] = i;
			coord[1] = rndVal;
			return coord;
		}
	return coord;
}

//hookCornerCoord - max left-bottom hook cell (corner). isTerminal returns true, if the returned cell is terminal
mainType* Tableaux2D::GetRandomPointOnThisHook(mainType* hookCornerCoord, bool* isTerminal, bool destroySourceCoordArray)
{
	mainType* result = new mainType[2];
	int hookLen = (diag->columns[hookCornerCoord[0]] - hookCornerCoord[1]); //vertical hook part
	int horiz;
	for (horiz = hookCornerCoord[0]; horiz < diag->columns.size() && diag->columns[horiz]>hookCornerCoord[1]; horiz++);
	horiz -= hookCornerCoord[0] + 1;
	hookLen += horiz; //horizontal hook part
	if (hookLen == 1)
	{
		*isTerminal = true;
		result[0] = hookCornerCoord[0];
		result[1] = hookCornerCoord[1];
		if (destroySourceCoordArray)
			delete[] hookCornerCoord;
		return result;
	}
	int rndVal = rand() % (hookLen - 1); //minus corner point

	if (rndVal >= horiz)
	{	//random point placed on vertical hook part
		result[0] = hookCornerCoord[0];
		result[1] = hookCornerCoord[1] + (rndVal - horiz) + 1; //add 1 so we never get the corner point
	}
	else
	{	//random point placed on horizontal hook part
		result[0] = hookCornerCoord[0] + rndVal + 1; //add 1 so we never get the corner point
		result[1] = hookCornerCoord[1];
	}
	*isTerminal = (diag->columns[result[0]] - 1 == result[1] && (result[0] == diag->columns.size() - 1 || diag->columns[result[0] + 1] - 1 < result[1]));
	if (destroySourceCoordArray)
		delete[] hookCornerCoord;
	return result;
}

void Tableaux2D::CreateColorFile(mainType* hookCorner, int frame, bool ignoreHookCorner)
{
	ofstream os(string("frame-") + to_string(frame) + string(".txt"), ofstream::out);
	for (int x = 0; x < table.size(); x++)
		for (int y = 0; y < table[x].size(); y++)
		{
			os << x << " " << y;
			if (!ignoreHookCorner&& hookCorner[0] == x && hookCorner[1] == y) //hook corner
				os << " 0 " << 1;
			else if (table[x][y] == 0) //if non-numbered
			{
				if (((hookCorner[0] == x && y > hookCorner[1]) || (hookCorner[1] == y && x > hookCorner[0]))) //hook
					os << " 0 " << 2;
				else	//normal not-numbered cell
					os << " 0 " << 0;
			}
			else	//numbered (excluded) cell
				os << " " << table[x][y] << " " << 3;
			os << '\n';
		}
	os.close();
}

bool Tableaux2D::CoordsEqual(mainType* c1, mainType* c2, int len)
{
	while (len-- && *(c1) == *(c2))
	{
		c1++; c2++;
	}
	return len == -1;
}

void Tableaux2D::ProcessOneHookwalk(bool createOutputFiles, int *fileFrame)
{
	mainType* start = GetRandomPointInDiag(), *coord = NULL;
	ofstream os;
	if (createOutputFiles)
		CreateColorFile(start, (*fileFrame)++, false);
	bool isTerminal = false;
	while (!isTerminal)
	{
		if (coord == NULL)
			coord = GetRandomPointOnThisHook(start, &isTerminal, false);
		else {
			coord = GetRandomPointOnThisHook(coord, &isTerminal, true);
			start = NULL;
		}
		if (createOutputFiles && (start == NULL || !CoordsEqual(coord, start, 2)))
			CreateColorFile(coord, (*fileFrame)++, false);
	}
	table[coord[0]][coord[1]] = diagN;
	diag->columns[coord[0]]--;
	diagN--;
	if (createOutputFiles)
		CreateColorFile(coord, (*fileFrame)++, true);
}

void Tableaux2D::GenerateRandomTable(bool createColorFiles, bool reserveDiagram)
{
	Young2D *diagCopy = 0;
	if (reserveDiagram)
		diagCopy = diag->Clone();
	int n = 0;
	while (diagN)
		ProcessOneHookwalk(createColorFiles, &n);
	if (reserveDiagram)
		diag = diagCopy;
}

bool Tableaux2D::TablesEqual(vector<vector<tabType>> table1, vector<vector<tabType>> table2)
{
	if (table1.size() == table2.size())
		for (int i = 0; i < table1.size(); i++)
			if (table1[i].size() == table2[i].size())
			{
				for (int j = 0; j < table1[i].size(); j++)
					if (table1[i][j] != table2[i][j])
						return false;
			}
			else
				return false;
	return true;
}

vector<vector<tabType>> Tableaux2D::ExtractTable()
{
	vector<vector<tabType>> result(table.size());
	for (int i = 0; i < table.size(); i++)
		result[i] = table[i];
	table.clear();
	diagN = 0;
	for (int i = 0; i < diag->columns.size(); i++) {
		table.push_back(vector<tabType>(diag->columns[i]));
		diagN += diag->columns[i];
	}
	return result; 
}
