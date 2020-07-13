#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "InfInt.h"


using namespace std;
#define TODO_METHOD { cout<<"Method unimplemented."<<endl;}

//char constants
const char MAIN_DELIM = ' ';
//string constants
const string COLUMN_DELIM = " ";
const string LAYER2D_DELIM = ",";
const string LAYER3D_DELIM = ";";
const string LEX_DELIM = "|";
const string CONNECTED_DELIM = " ";
//lex names
const string lexDimension = "dim";
const string lexLevel = "level";
const string lexId = "id";
const string lexConnected = "connected";

/*----------------------------------------------------------------
|	this struct is used instead of vector<bool/int>, is going to save 
|	~1,5 * 8 * 32 - 1 = ~383 bits = ~48 bytes per 2D layer (32 maxsize calculations)
|*/
#define BITLINESIZE 32
typedef struct _bitStr
{
	unsigned int line : BITLINESIZE;

	int GetBit(int index)
	{
		return (line >> index)&1;
	}

	void SetBit(int index, int bit)
	{
		line ^= (-bit ^ line) & (1ULL << index);
	}
}BitLine;

#pragma pack(1)
typedef struct _bitBool
{
	unsigned int val : 1;
	int GetBit(int index)
	{
		return val;
	}

	void SetBit(int bit)
	{
		val = bit;
	}
} BitBool;

//type, storing column sizes
typedef unsigned char mainType;

//main interface
class IYoung {
public:
	//TOTAL: 11 VIRTUALS HAVE TO BE OVERRIDED
	//destructor
	//LEGACY: <Clear> replaced with destructor!
	virtual ~IYoung() {}
	//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube
	virtual int AddCube(mainType* coordinates, bool checkOnly, bool TopDimension) { TODO_METHOD return 0; }
	//get layer by index
	virtual IYoung* GetLayer(int index) { TODO_METHOD return NULL; }
	virtual int GetColumn(mainType* coordinates) { TODO_METHOD return 0; }
	virtual IYoung* Clone() { TODO_METHOD return NULL; }
	virtual bool operator ==(const IYoung& other) { TODO_METHOD return true; }
	virtual void Print() TODO_METHOD
	virtual void SetBitBool(mainType* coordinates, bool bit) TODO_METHOD
	virtual bool GetBitBool(mainType* coordinates) { TODO_METHOD return false; }
	//create all possible next-gen (on graph) diagrams. Recursive.
	//method descends to all 2D layers, creates all the combinations of new ones,
	//then ascends back and composes all the variations in one collection
	virtual vector<IYoung*> Propagate() { TODO_METHOD return vector<IYoung*>(); }
	virtual vector<mainType*> GetPotentialCoords(int MainDimension) { TODO_METHOD return vector<mainType*>(); }
	//save more space
	unsigned char dimensions;
};

class Young2D : public IYoung {
	//TOTAL: 11 VIRTUALS OVERRIDED
public:
	vector<mainType> columns = { 0 };
	BitLine *oCorners;

	Young2D(mainType startCubesInZeroPoint) {
		dimensions = 2;
		columns[0] = startCubesInZeroPoint;
		oCorners = new BitLine();
		oCorners->SetBit(0, 1);
		if (startCubesInZeroPoint > 0)
			oCorners->SetBit(1, 1); //bit will be ovewritten if previous layer is one-column
	}
	Young2D() {}

	~Young2D()
		override
	{
		columns.clear();
		delete oCorners;
	}

	//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube.
	int AddCube(mainType* coordinates, bool checkOnly = false, bool TopDimension = false)
		override
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

	int GetColumn(mainType* x)
		override
	{
		if ((int)columns.size() > x[0])
			return columns[x[0]];
		else
			return 0;
	}

	void SetBitBool(mainType* coordinates, bool bit)
		override
	{
		oCorners->SetBit(coordinates[0], bit);
	}
	
	bool GetBitBool(mainType* coordinates)
		override
	{
		return oCorners->GetBit(coordinates[0]);
	}

	IYoung* GetLayer(int index)
		override
	{
		printf("ERROR: 2D DIAGRAM HAS NO LAYERS!\n");
		return NULL;
	}

	void Print()
		override
	{
		for (int i = 0; i < (int)columns.size(); i++)
			cout << (int)columns[i] << MAIN_DELIM;
	}

	string ToString()
	{
		string result = "";
		int i;
		for (i = 0; i < (int)columns.size() - 1; i++)
			result += to_string(columns[i]) + COLUMN_DELIM;
		result += to_string(columns[i]);
		return result;
	}

	Young2D* Clone()
		override
	{
		Young2D* newObj = new Young2D();
		newObj->oCorners = new BitLine();
		*(newObj->oCorners) = *oCorners;
		newObj->dimensions = 2;
		newObj->columns = columns;
		return newObj;
	}

	bool operator==(const Young2D& other)
	{
		bool result = columns.size() == other.columns.size();
		for (int x = 0; result && x < columns.size(); x++)
			result = columns[x] == other.columns[x];
		return result;
	}

	bool operator==(const IYoung& other)
		override
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
	vector<IYoung*> Propagate()
		override
	{
		vector<IYoung*> children = vector<IYoung*>();
		for(mainType i = 0; i <= columns.size(); i++)
			if (oCorners->GetBit(i))
			{
				Young2D* child = Clone(); 
				child->AddCube(&i);	//unacceptable for higher dimensions, bc will cause bitline errors
				children.push_back(child);
			}
		//Maybe add later
		//children.shrink_to_fit();
		return children;
	}

	vector<mainType*> GetPotentialCoords(int MainDimension) 
		override 
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

};


class YoungNDim : public IYoung {
public:
	//TOTAL: 9 OVERRIDES
	vector<IYoung*> layers;
	BitBool* canAddLayer;
	//recursive constructor
	YoungNDim(mainType startCubesInZeroPoint, unsigned char _dimensions)
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
			cout<<"LOGIC ERR: You tried to create N-dim Young diagram with less than 3 dimensions. See Young2D.\n";
	}

	YoungNDim(vector<IYoung*> _layers, unsigned char _dimensions)
	{
		layers = _layers;
		dimensions = _dimensions;
	}

	YoungNDim() {}
	
	//destructor
	~YoungNDim()
		override
	{
		for (int i = 0; i < (int)layers.size(); i++)
			delete layers[i];
		delete canAddLayer;
		layers.clear();
	}

	//check if all the numbers in the sequence are equal to 0 (eg. 0 0 0 0)
	static bool IsVectorZero(mainType* vect, int length)
	{
		bool result = true;
		while (length-- > 0 && result)
			result = vect[length] == 0;
		return result;
	}

	//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube
	//also refreshes the bitmask (BitLine) of the 2D layers 
	int AddCube(mainType* coordinates, bool checkOnly = false, bool TopDimension = false)
		override
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

	IYoung* GetLayer(int index)
		override
	{
		if (index < (int)layers.size())
			return layers[index];
		return NULL;
	}

	//Recursive get. Returns the height of the column at given coordinates
	int GetColumn(mainType* coordinates)
		override
	{
		IYoung* lyr = GetLayer(coordinates[dimensions - 2]);
		if (lyr!=NULL)
			return lyr->GetColumn(coordinates);
		else
			return 0;
	}

	//for oCorners refreshment
	void SetBitBool(mainType* coordinates, bool bit)
		override
	{
		if (coordinates[dimensions - 2] < layers.size())
			layers[coordinates[dimensions - 2]]->SetBitBool(coordinates, bit);
		else if(coordinates[dimensions - 2] == layers.size())
			canAddLayer->SetBit(bit);
	}

	bool GetBitBool(mainType* coordinates)
		override
	{
		if (coordinates[dimensions - 2] == layers.size())
			return canAddLayer->val;
		return layers[coordinates[dimensions-2]]->GetBitBool(coordinates);
	}

	void Print()
		override
	{
		for (int i = 0; i < (int)layers.size(); i++)
		{
			layers[i]->Print();
			cout << MAIN_DELIM;
		}
	}

	/*
	TODO: REPLACE WITH CHAR*
	string ToString()
	{
		string result = "";
		int i;
		for (i = 0; i < (int)layers.size() - 1; i++)
			result += layers[i].ToString() + LAYER2D_DELIM;
		result += layers[i].ToString();
		return result;
	}*/

	//recursive cloning
	YoungNDim* Clone()
		override
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
	//the given instance will be used, so copy it before passing
	YoungNDim* Clone(IYoung* replaceLayer, int index)
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
	bool operator==(const YoungNDim& other)
	{
		bool result = layers.size() == other.layers.size();
		for (int x = 0; result && x < layers.size(); x++)
			result = (*layers[x] == *other.layers[x]);
		return result;
	}

	//casting to upper one
	bool operator ==(const IYoung& other)
		override
	{
		if (other.dimensions == dimensions)
			return *this == *dynamic_cast<const YoungNDim*>(&other); //bizzare, need to check
		return false;
	}

	//create all possible next-gen (on graph) diagrams. 
	vector<IYoung*> Propagate()
		override
	{
		vector<mainType*> potentialCoords = GetPotentialCoords(dimensions);
		vector<IYoung*> children = vector<IYoung*>(); //so 2d are available
		children.reserve(potentialCoords.size());
		for (int i = 0; i < potentialCoords.size(); i++)
		{
			YoungNDim* copy = Clone();
			if (copy->AddCube(potentialCoords[i], false, true))
				children.push_back(copy);
			else {
				//just in case, this part never executes
				cout << "ERR:" << endl << "_______________________" << endl << "DIAG:";
				copy->Print();
				cout << endl;
				for (int x = 0; x < dimensions - 1; x++)
					printf("%d ", potentialCoords[i][x]);
				cout << endl << "_______________________"<<endl;
				delete copy;
			}
			delete[] potentialCoords[i];
		}
		potentialCoords.clear();
		return children;
	}

	//get all current oCorners
	vector<mainType*> GetPotentialCoords(int MainDimension)
		override
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

};

//class for top-level diagram. Stores dimension.
class YoungNDim_SimpleNode
{
public:
	//pointer to main object
	IYoung* node;
	//dimensions (ways from the beginning of the graph to this diagram
	InfInt* dim;//TODO create own realization

	YoungNDim_SimpleNode(IYoung* _node, int _dim = 1)
	{
		dim = new InfInt();
		(*dim) = _dim;
		node = _node;
	}

	YoungNDim_SimpleNode(IYoung* _node, InfInt* _dim)
	{
		dim = new InfInt();
		(*dim) = (*_dim);
		node = _node;
	}

	~YoungNDim_SimpleNode()
	{
		delete node;
		delete dim;
	}

	//create child diagrams vector
	vector<YoungNDim_SimpleNode*> Propagate()
	{
		vector<IYoung*> children = node->Propagate();
		vector<YoungNDim_SimpleNode*> results(children.size());
		for (int i = 0; i < children.size(); i++)
		{
			results[i] = new YoungNDim_SimpleNode(children[i], dim);	//children receive same dim
		}
		children.clear();
		return results;
	}

	bool operator==(const YoungNDim_SimpleNode& other)
	{
		return ((*node) == (*other.node));
	}

	bool Equals(const YoungNDim_SimpleNode* other)
	{
		return ((*node) == (*(other->node)));
	}

	//deletes other and adds its dim to this instance
	void Absorb(YoungNDim_SimpleNode* other)
	{
		(*dim) += (*(other->dim));
		delete other;
	}
};

//create graph with diagrams & their dimensions only 
vector<vector<YoungNDim_SimpleNode*>> CreateGraph(YoungNDim_SimpleNode* rootDiagram, int levelCount)
{
	//result
	vector<vector<YoungNDim_SimpleNode*>> levels(levelCount);
	//add root diagram
	levels[0] = { rootDiagram };
	//iterate & create new levels based on previous ones
	for (int i = 1; i < levelCount; i++)
	{
		//foreach diagram in previous level
		for (int j = 0; j < levels[i - 1].size(); j++) {
			//create children pool of selected
			vector<YoungNDim_SimpleNode*> children = levels[i - 1][j]->Propagate();
			//check new children (if they are already in the main pool)
			for (int k = 0; k < levels[i].size(); k++)
				for (int l = 0; l < children.size(); l++)
					if (levels[i][k]->Equals(children[l]))
					{
						levels[i][k]->Absorb(children[l]);	//big pool absorbs a piece of smaller one
						children.erase(children.begin() + l);	//remove absorbed diagram from check-pool
						l--;	//step back (we destroyed last index)
					}
			//add new, unique children
			levels[i].insert(levels[i].end(), children.begin(), children.end());
			//go to next ancestor
		}
		//all ancestors checked, go to next generation
	}
	return levels;
}

int main(int argc, char* argv[])
{
	/*YoungNDim_SimpleNode* diag = new YoungNDim_SimpleNode(new YoungNDim(1, 4));
	mainType* coords = new mainType[3];
	coords[0] = coords[1] = coords[2] = 0;
	//diag->node->AddCube(coords, false, true);
	coords[2] = 1;
	diag->node->AddCube(coords, false, true);
	//diag->node->AddCube(coords, false, true);
	diag->node->Print();
	printf("\n");
	vector<YoungNDim_SimpleNode*> children = diag->Propagate();
	for (int lvl = 0; lvl < children.size(); lvl++)
	{
		children[lvl]->node->Print();
		printf("\n");
	}*/
	/*for (int x = 0; x < graph[2].size(); x++)
	{
		printf("PARENT:\n");
		graph[2][x]->node->Print();
		printf("\n");
		printf("CHILDREN:\n");
		vector<YoungNDimHead*> children1 = graph[2][x]->Propagate();
		for (int lvl = 0; lvl < children1.size(); lvl++)
		{
			children1[lvl]->node->Print();
			printf("\n");
		}
		printf("\n\n\n\n");
	}*/
	vector<vector<YoungNDim_SimpleNode*>> graph = CreateGraph(new YoungNDim_SimpleNode(new YoungNDim(1, 4)), 9);
	for (int lvl = 0; lvl < graph.size(); lvl++)
	{
		printf("LEVEL %d\n", lvl + 1);
		printf("Count: %d\n", graph[lvl].size());
		for (int index = 0; index < graph[lvl].size(); index++)
		{
			graph[lvl][index]->node->Print();
			printf(", dim = %d", graph[lvl][index]->dim->toInt());
			printf("\n");
		}
		printf("\n");
	}
	getchar();
}