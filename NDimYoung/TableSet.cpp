#include "TableSet.h"
#include "omp.h"
using namespace std;

TableSet::TableSet(Young2D &form)
{
	tableGenerator = new Tableau2D(form);
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

void TableSet::GenerateOneTableFast()
{
	vector<tabType> tab = tableGenerator->GenerateRandomTableFast(true);
	tableCountsFast[tab]++;
}

void TableSet::GenerateTables(int count)
{
	for (int i = 0; i < count; i++)
		GenerateOneTable();
}

void TableSet::GenerateTablesFast(int count)
{
	for (int i = 0; i < count; i++)
		GenerateOneTableFast();
}

void TableSet::ExportTables(ostream &os)
{
	int i = 0;
	for (auto elem : tableCounts)
	{
		os << i++ << " " << elem.second << endl;
	}
}

void TableSet::ExportTablesFast(ostream& os)
{
	int i = 0;
	for (auto elem : tableCountsFast)
	{
		os << i++ << " " << elem.second << endl;
	}
}