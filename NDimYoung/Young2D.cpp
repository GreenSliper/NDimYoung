#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "InfInt.h"
#include <time.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "YoungNDim.h"
#include "CommonMethods.h"

using namespace std;

Young2D::Young2D(istream &is) : Young2D(0)
{
	string dg;
	cout << "Enter column heights, split by [SPACE]:" << endl;
	if (cin.rdbuf()->in_avail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	getline(is, dg);
	vector<string> dgNms = split(dg, " ");
	columns = vector<mainType>(dgNms.size());
	for (int i = 0; i < dgNms.size(); i++)
		columns[i] = stoi(dgNms[i]);
}

Young2D::Young2D(string str) : Young2D(0)
{
	vector<string> dgNms = split(str, "-");
	columns = vector<mainType>(dgNms.size());
	for (int i = 0; i < dgNms.size(); i++)
		columns[i] = stoi(dgNms[i]);
}

Young2D::Young2D(mainType startCubesInZeroPoint) {
	dimensions = 2;
	columns[0] = startCubesInZeroPoint;
	oCorners = new BitLine(2);
	oCorners->SetBit(0, 1);
	if (startCubesInZeroPoint > 0)
		oCorners->SetBit(1, 1); //bit will be ovewritten if previous layer is one-column
}
Young2D::Young2D() {}
Young2D::~Young2D()
{
	columns.clear();
	delete oCorners;
}

//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube.
int Young2D::AddCube(mainType* coordinates, bool checkOnly, bool TopDimension)
{
	int result = 0;
	//error case
	if (coordinates[0] < 0 || coordinates[0] > columns.size())
		return result;
	//appending
	if (columns.size() == coordinates[0]) //you can append if previous not zero
	{
		if (columns[coordinates[0] - 1] > 0) {
			if (!checkOnly)
				columns.push_back(1);	//new column
			result = 1;
		}
	}	//default case (condition not needed actually)
	else //if ((int)columns.size() > coordinates[0])
	{
		if (coordinates[0] == 0 || columns[coordinates[0] - 1] > columns[coordinates[0]]) {
			if (!checkOnly)
				columns[coordinates[0]]++;
			result = 1;
		}
	}
	//change bitline 
	if (TopDimension && !checkOnly && result)
	{
		SetBitBool(coordinates, AddCube(coordinates, true));
		coordinates[0]++;
		SetBitBool(coordinates, AddCube(coordinates, true));
		coordinates[0]--;
	}

	return result;
}

IYoung* Young2D::GetLayer(int index)
{
	printf("ERROR: 2D DIAGRAM HAS NO LAYERS!\n");
	return NULL;
}
int Young2D::GetColumn(mainType* x)
{
	if ((int)columns.size() > x[0])
		return columns[x[0]];
	else
		return 0;
}
void Young2D::SetBitBool(mainType* coordinates, bool bit)
{
	oCorners->SetBit(coordinates[0], bit);
}
bool Young2D::GetBitBool(mainType* coordinates)
{
	return oCorners->GetBit(coordinates[0]);
}

void Young2D::Print(ostream& os)
{
	for (int i = 0; i < (int)columns.size() - 1; i++)
		os << (int)columns[i] << MAIN_DELIM;
	os << (int)columns[columns.size() - 1];
}

Young2D* Young2D::Clone()
{
	Young2D* newObj = new Young2D();
	newObj->oCorners = new BitLine(oCorners->size + 1);
	newObj->oCorners->CopyLineFrom(oCorners);
	newObj->dimensions = 2;
	newObj->columns = columns;
	return newObj;
}

bool Young2D::operator==(const Young2D& other)
{
	bool result = columns.size() == other.columns.size();
	for (int x = 0; result && x < columns.size(); x++)
		result = columns[x] == other.columns[x];
	return result;
}
bool Young2D::operator==(const IYoung& other)
{
	if (other.dimensions == 2)
	{
		const Young2D* othr2d = dynamic_cast<const Young2D*>(&other);
		bool result = columns.size() == othr2d->columns.size();
		for (int x = 0; result && x < columns.size(); x++)
			result = columns[x] == othr2d->columns[x];
		return result;
	}
	return false;
}

//spawn children
vector<IYoung*> Young2D::Propagate()
{
	vector<IYoung*> children = vector<IYoung*>();
	for (mainType i = 0; i <= columns.size(); i++)
		if (oCorners->GetBit(i))
		{
			Young2D* child = Clone();
			child->AddCube(&i, false, true);	//unacceptable for higher dimensions, bc will cause bitline errors
			children.push_back(child);
		}
	//Maybe add later
	//children.shrink_to_fit();
	return children;
}
vector<mainType*> Young2D::GetPotentialCoords(int MainDimension)
{
	vector<mainType*> coords = vector<mainType*>();
	for (mainType i = 0; i <= columns.size(); i++)
		if (oCorners->GetBit(i))
		{
			mainType* coord = new mainType[MainDimension]();
			coord[0] = i;
			coords.push_back(coord);
		}
	return coords;
}