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
#include <omp.h>
#include "YoungNDim.h"

using namespace std;

#pragma region YoungNDim
YoungNDim::YoungNDim(mainType startCubesInZeroPoint, unsigned char _dimensions)
{
	dimensions = _dimensions;
	layers = vector<IYoung*>();
	canAddLayer = new BitBool();
	canAddLayer->val = 1;
	if (_dimensions > 3)
	{
		YoungNDim* layer = new YoungNDim(startCubesInZeroPoint, _dimensions - 1);
		layers.push_back(layer);
	}
	else if (_dimensions == 3)
	{
		Young2D* layer = new Young2D(startCubesInZeroPoint);
		layers.push_back(layer);
	}
	else
		cout << "LOGIC ERR: You tried to create N-dim Young diagram with less than 3 dimensions. See Young2D.\n";
}

YoungNDim::YoungNDim(vector<IYoung*> _layers, unsigned char _dimensions)
{
	layers = _layers;
	dimensions = _dimensions;
}

YoungNDim::YoungNDim() {}

//destructor
YoungNDim::~YoungNDim()
{
	for (int i = 0; i < (int)layers.size(); i++)
		delete layers[i];
	delete canAddLayer;
	layers.clear();
}

//check if all the numbers in the sequence are equal to 0 (eg. 0 0 0 0)
bool YoungNDim::IsVectorZero(mainType* vect, int length)
{
	bool result = true;
	while (length-- > 0 && result)
		result = vect[length] == 0;
	return result;
}

//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube
//also refreshes the bitmask (BitLine) of the 2D layers 
int YoungNDim::AddCube(mainType* coordinates, bool checkOnly, bool TopDimension)
{
	int result = 0;
	if (coordinates[dimensions - 2] < 0 || coordinates[dimensions - 2] > layers.size())	//out of bounds
		return 0;
	// if new layer needed
	if (coordinates[dimensions - 2] == layers.size())
	{
		if (IsVectorZero(coordinates, dimensions - 3))
		{
			if (!checkOnly)	//if we are actually adding (not just checking)
			{
				if (dimensions == 3)	//2d layer needed
					layers.push_back(new Young2D(1));
				else
					layers.push_back(new YoungNDim(1, dimensions - 1));	//creating normal layer
			}
			result = 1;	//ok, cube can be added
		}
	}
	//modifying existing layer, recursive model (exiting on 2D layer)
	else
	{
		//1st layer can always be modified, then checks normal column conditions
		if (coordinates[dimensions - 2] == 0 || layers[coordinates[dimensions - 2] - 1]->GetColumn(coordinates) > GetColumn(coordinates))
			result = layers[coordinates[dimensions - 2]]->AddCube(coordinates, checkOnly, false);
	}
	//refresh bitmask (not recursive)
	if (TopDimension && result && !checkOnly)
	{
		SetBitBool(coordinates, AddCube(coordinates, true, false));
		for (int i = 0; i < dimensions - 1; i++)
		{
			coordinates[i]++;
			SetBitBool(coordinates, AddCube(coordinates, true, false));
			coordinates[i]--;
		}
	}
	return result;
}

IYoung* YoungNDim::GetLayer(int index)
{
	if (index < (int)layers.size())
		return layers[index];
	return NULL;
}

//Recursive get. Returns the height of the column at given coordinates
int YoungNDim::GetColumn(mainType* coordinates)
{
	IYoung* lyr = GetLayer(coordinates[dimensions - 2]);
	if (lyr != NULL)
		return lyr->GetColumn(coordinates);
	else
		return 0;
}

//for oCorners refreshment
void YoungNDim::SetBitBool(mainType* coordinates, bool bit)
{
	if (coordinates[dimensions - 2] < layers.size())
		layers[coordinates[dimensions - 2]]->SetBitBool(coordinates, bit);
	else if (coordinates[dimensions - 2] == layers.size())
		canAddLayer->SetBit(bit);
}

bool YoungNDim::GetBitBool(mainType* coordinates)
{
	if (coordinates[dimensions - 2] == layers.size())
		return canAddLayer->val;
	return layers[coordinates[dimensions - 2]]->GetBitBool(coordinates);
}

void YoungNDim::Print(ostream& os)
{
	for (int i = 0; i < (int)layers.size() - 1; i++)
	{
		layers[i]->Print(os);
		for (int j = 1; j < dimensions; j++)
			os << MAIN_DELIM;
	}
	layers[layers.size() - 1]->Print(os);
}

//recursive cloning
YoungNDim* YoungNDim::Clone()
{
	YoungNDim* newObj = new YoungNDim();
	newObj->canAddLayer = new BitBool();
	newObj->canAddLayer->val = canAddLayer->val;
	newObj->layers = layers;
	newObj->dimensions = dimensions;
	for (int x = 0; x < (int)layers.size(); x++)
		newObj->layers[x] = layers[x]->Clone();
	return newObj;
}

//clone this whole object and replace one layer with the given one
//the given replaceLayer will be used, so copy it before passing
YoungNDim* YoungNDim::Clone(IYoung* replaceLayer, int index)
{
	YoungNDim* newObj = new YoungNDim();
	newObj->canAddLayer = new BitBool();
	newObj->canAddLayer->val = canAddLayer->val;
	newObj->layers = layers;
	newObj->dimensions = dimensions;
	for (int x = 0; x < (int)layers.size(); x++)
		if (x != index)
			newObj->layers[x] = layers[x]->Clone();
		else
			newObj->layers[x] = replaceLayer;
	return newObj;
}

//recursive comparasing
bool YoungNDim::operator==(const YoungNDim& other)
{
	bool result = layers.size() == other.layers.size();
	for (int x = 0; result && x < layers.size(); x++)
		result = (*layers[x] == *other.layers[x]);
	return result;
}

//casting to upper one
bool YoungNDim::operator ==(const IYoung& other)
{
	if (other.dimensions == dimensions)
		return *this == *dynamic_cast<const YoungNDim*>(&other); //bizzare, need to check
	return false;
}

//create all possible next-gen (on graph) diagrams. 
vector<IYoung*> YoungNDim::Propagate()
{
	vector<mainType*> potentialCoords = GetPotentialCoords(dimensions);
	vector<IYoung*> children = vector<IYoung*>(); //so 2d are available
	children.reserve(potentialCoords.size());
	for (int i = 0; i < potentialCoords.size(); i++)
	{
		YoungNDim* copy = Clone();
		if (copy->AddCube(potentialCoords[i], false, true))
			children.push_back(copy);
		/*else {
			//just in case, this part never executes
			cout << "ERR:" << endl << "_______________________" << endl << "DIAG:";
			copy->Print(cout);
			cout << endl;
			for (int x = 0; x < dimensions - 1; x++)
				printf("%d ", potentialCoords[i][x]);
			cout << endl << "_______________________"<<endl;
			delete copy;
		}*/
		delete[] potentialCoords[i];
	}
	potentialCoords.clear();
	return children;
}

//get all current oCorners
vector<mainType*> YoungNDim::GetPotentialCoords(int MainDimension)
{
	vector<mainType*> results = vector<mainType*>();
	for (int i = 0; i < layers.size(); i++)
	{
		vector<mainType*> children = layers[i]->GetPotentialCoords(MainDimension);
		for (int j = 0; j < children.size(); j++)
			children[j][dimensions - 2] = i;
		results.insert(results.end(), children.begin(), children.end());
	}
	if (canAddLayer->val) {
		mainType* coords = new mainType[MainDimension]();
		coords[dimensions - 2] = layers.size();
		results.push_back(coords);
	}
	return results;
}
#pragma endregion