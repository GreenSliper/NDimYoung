#pragma once
#include "Tableaux2D.h"
#include <map>

using namespace std;

class TableSet
{
	vector<int> tablesCount; 
	map<vector<vector<tabType>>, int> tableCounts;
	Tableaux2D* tableGenerator;
	void GenerateOneTable();
public:
	TableSet(Young2D &form);
	~TableSet();
	void GenerateTables(int count);
	void ExportTables(ostream &os);
};