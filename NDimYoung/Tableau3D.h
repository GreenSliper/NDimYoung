#pragma once
#include "YoungNDim.h"
#include "Tableaux2D.h"
#include <map>
#include <thread>
#include <future>

using namespace std;

const auto processor_count = thread::hardware_concurrency();

typedef int tabType;

struct tabTypePair
{
	tabType v1, v2;
	tabTypePair(tabType val1, tabType val2) :v1(val1), v2(val2) {}
public:
	bool operator ==(const tabTypePair& other) const
	{
		return v1 == other.v1 && v2 == other.v2;
	}
	bool operator <(const tabTypePair& other) const
	{
		return v1*10000 + v2 < other.v1 * 10000 + other.v2;
	}

	friend ostream& operator<<(ostream& os, const tabTypePair& dt);
};

ostream& operator<<(ostream& os, const tabTypePair& dt)
{
	os << '[' << dt.v1 << ';' << dt.v2 << ']';
	return os;
}

class Tableau3D
{
private:
	tabType diagNReserve = 0;
public:
	vector<Young2D*> diag;
	tabType diagN = 0;
	vector<vector<vector<tabType>>> table;
	vector<vector<mainType>> reserveColumns;
	vector<tabTypePair> fastTable;

	//initializer
	Tableau3D(vector<Young2D*> base);
	~Tableau3D();
	mainType* GetRandomPointInDiag();
	mainType* GetRandomPointOnThisHook(mainType* hookCornerCoord, bool* isTerminal);
	void ProcessOneHookwalk();
	vector<tabTypePair> GenerateRandomTable(bool reserveState);
	//vector<vector<tabType>> ExtractTable();
};

Tableau3D::Tableau3D(vector<Young2D*> base)
{
	for (int i = 0; i < base.size(); i++)
	{
		diag.push_back(base[i]->Clone());
		table.push_back(vector<vector<tabType>>());
		for (int j = 0; j < base[i]->columns.size(); j++)
		{
			table[i].push_back(vector<tabType>(base[i]->columns[j]));
			diagN += base[i]->columns[j];
		}
		reserveColumns.push_back(base[i]->columns);
	}
	diagNReserve = diagN;
}

Tableau3D::~Tableau3D()
{
	for (int j = 0; j < table.size(); j++)
	{
		table[j].clear();
	}
	table.clear();
	for(int i =0; i < diag.size(); i++)
		delete diag[i];
}

mainType* Tableau3D::GetRandomPointInDiag()
{
	mainType* coord = new mainType[3];
	int rndVal = rand() % diagN;
	for(int j = 0; j < diag.size(); j++)
		for (int i = 0; i < diag[j]->columns.size(); i++)
			if (rndVal >= diag[j]->columns[i])
				rndVal -= diag[j]->columns[i];
			else
			{
				coord[0] = j;
				coord[1] = i;
				coord[2] = rndVal;
				return coord;
			}
	return coord;
}

mainType* Tableau3D::GetRandomPointOnThisHook(mainType* hookCornerCoord, bool* isTerminal)
{
	int zHook = 0;
			//next layer exists
	while (hookCornerCoord[0] + zHook < diag.size() && 
		//next 2d layer				column count	>	columnID
		diag[hookCornerCoord[0] + zHook]->columns.size() > hookCornerCoord[1] &&
		//next 2d layer					column[column ID]			>=	target column height
		diag[hookCornerCoord[0] + zHook]->columns[hookCornerCoord[1]] > hookCornerCoord[2])
		zHook++;

	int yHook = 0;
			//current 2d layer		column count	> columnID + yHook
	while (diag[hookCornerCoord[0]]->columns.size() > hookCornerCoord[1] + yHook &&
		//current 2d layer		next column height					> target column height
		diag[hookCornerCoord[0]]->columns[hookCornerCoord[1] + yHook] > hookCornerCoord[2])
		yHook++;
				//current 2d layer			current column			- target column height
	int xHook = diag[hookCornerCoord[0]]->columns[hookCornerCoord[1]] - hookCornerCoord[2];

	xHook--; yHook--; zHook--;

	if(zHook == 0)
		if (yHook == 0) //move x only
		{
			*isTerminal = true;
			hookCornerCoord[2] += xHook;
		}
		else if (xHook == 0) //move y only
		{
			*isTerminal = true;
			hookCornerCoord[1] += yHook;
		}
		else //move x & y
		{
			int move = 1 + rand() % (xHook + yHook);
			if (move > xHook)	//move y
				hookCornerCoord[1] += move - xHook;
			else				//move x
				hookCornerCoord[2] += move;
		}
	else
		if (yHook == 0) 
		{
			if (xHook == 0) //move z
			{
				*isTerminal = true;
				hookCornerCoord[0] += zHook;
			}
			else //move x & z
			{
				int move = 1 + rand() % (xHook + zHook);
				if (move > xHook)	//move z
					hookCornerCoord[0] += move - xHook;
				else				//move x
					hookCornerCoord[2] += move;
			}
		}
		else if (xHook == 0) //move y & z
		{
			int move = 1 + rand() % (yHook + zHook);
			if (move > yHook)	//move z
				hookCornerCoord[0] += move - yHook;
			else				//move y
				hookCornerCoord[1] += move;
		}
		else //move x & y & z
		{
			int move = 1 + rand() % (xHook + yHook + zHook);
			if (move > xHook)	//move y & z
			{
				move -= xHook;
				if (move > yHook) //move z
					hookCornerCoord[0] += move - yHook;
				else //move y
					hookCornerCoord[1] += move;
			}
			else //move x
				hookCornerCoord[2] += move;
		}
	return hookCornerCoord;
}

void Tableau3D::ProcessOneHookwalk()
{
	mainType* coord = GetRandomPointInDiag();
	bool isTerminal = false;
	while (!isTerminal)
		coord = GetRandomPointOnThisHook(coord, &isTerminal);
	fastTable.push_back(tabTypePair(coord[0], coord[1]));
	diag[coord[0]]->columns[coord[1]]--;
	diagN--;
	delete coord;
}

vector<tabTypePair> Tableau3D::GenerateRandomTable(bool reserveState)
{
	fastTable.clear();
	while (diagN)
		ProcessOneHookwalk();
	if (reserveState) {
		diagN = diagNReserve;
		for(int i = 0; i < diag.size();i++)
			diag[i]->columns = reserveColumns[i];
	}
	return fastTable;
}

class TableSet3D
{
	Tableau3D* tableGenerator;
	void GenerateOneTable(map<vector<tabTypePair>, int> &tableMap);
public:
	TableSet3D(vector<Young2D*> form);
	~TableSet3D();
	map<vector<tabTypePair>, int> GenerateTables(int count);
	/// <summary>
	/// Multithread version
	/// </summary>
	/// <param name="exportEvery">Export data via ExportTables every N generations</param>
	void GenerateTablesAsync(int count, const char* file, int exportEvery = 100000);
	void ExportTables(map<vector<tabTypePair>, int> tableMap, ostream& os);
};

TableSet3D::TableSet3D(vector<Young2D*> form)
{
	vector<Young2D*> formCopy;
	for (int i = 0; i < form.size(); i++)
		formCopy.push_back(form[i]->Clone());
	tableGenerator = new Tableau3D(formCopy);
}

TableSet3D::~TableSet3D()
{
	delete tableGenerator;
}

void TableSet3D::GenerateOneTable(map<vector<tabTypePair>, int> &tableMap)
{
	vector<tabTypePair> tab = tableGenerator->GenerateRandomTable(true);
	tableMap[tab]++;
}

map<vector<tabTypePair>, int> TableSet3D::GenerateTables(int count)
{
	map<vector<tabTypePair>, int> tableMap;
	for (int i = 0; i < count; i++)
	{
		GenerateOneTable(tableMap);
	}
	return tableMap;
}

void TableSet3D::GenerateTablesAsync(int count, const char* file, int exportEvery)
{
	map<vector<tabTypePair>, int> tableMap;

	while (count > 0)
	{
		int cnt = min(exportEvery, count);

		vector<future<map<vector<tabTypePair>, int>>> calcs;
		for (int i = 0; i < processor_count; i++)
		{
			TableSet3D* set = new TableSet3D(tableGenerator->diag);
			calcs.push_back(async([set, cnt]
				{
					auto res = set->GenerateTables(cnt / processor_count);
					delete set;
					return res;
				}));
		}
		for (int i = 0; i < processor_count; i++)
		{
			auto mp = calcs[i].get();
			for (auto elem : mp)
				tableMap[elem.first] += elem.second;
		}
		ofstream of(file);
		ExportTables(tableMap, of);
		of.close();
		count -= cnt;
	}
}

void TableSet3D::ExportTables(map<vector<tabTypePair>, int> tableMap, ostream& os)
{
	int i = 0;
	for (auto elem : tableMap)
	{
		os << i++;
		for (int j = 0; j < elem.first.size(); j++)
			os << elem.first[j];
		os << " " << elem.second << endl;
	}
}