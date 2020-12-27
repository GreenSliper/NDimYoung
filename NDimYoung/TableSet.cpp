#include "TableSet.h"

using namespace std;

TableSet::TableSet(Young2D &form)
{
	tableGenerator = new Tableaux2D(form);
}

TableSet::~TableSet()
{
	tableCounts.clear();
	delete tableGenerator;
}

void TableSet::GenerateOneTable()
{
	tableGenerator->GenerateRandomTable(false);
	vector<vector<tabType>> table = tableGenerator->ExtractTable();
	tableCounts[table]++;
}

void TableSet::GenerateTables(int count)
{
	for (int i = 0; i < count; i++)
		GenerateOneTable();
}

void TableSet::ExportTables(ostream &os)
{
	int i = 0;
	for (auto elem : tableCounts)
	{
		os << i++ << " " << elem.second << endl;
	}
}