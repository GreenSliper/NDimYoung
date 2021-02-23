#pragma once
#include "Tableaux2D.h"
#include <map>

using namespace std;

class TableSet
{
	vector<int> tablesCount; 
	map<vector<vector<tabType>>, int> tableCounts;
	map<vector<tabType>, int> tableCountsFast;
	Tableau2D* tableGenerator;
	void GenerateOneTable();
	void GenerateOneTableFast();
public:
	TableSet(Young2D &form);
	~TableSet();
	void GenerateTables(int count);
	void GenerateTablesFast(int count);
	void ExportTables(ostream& os);
	void ExportTablesFast(ostream &os);
};