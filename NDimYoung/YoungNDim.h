#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "InfInt.h"
#include "BitLine.h"

#pragma once
using namespace std;
//char constants
const char MAIN_DELIM = ' ';
//type, storing column sizes
typedef unsigned char mainType;
#define TODO_METHOD { cout<<"Method unimplemented."<<endl;}
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
	virtual void Print(ostream& os) TODO_METHOD
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

	Young2D(mainType startCubesInZeroPoint);
	Young2D();

	~Young2D() override;

	//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube.
	int AddCube(mainType* coordinates, bool checkOnly = false, bool TopDimension = false) override;

	int GetColumn(mainType* x) override;
	void SetBitBool(mainType* coordinates, bool bit) override;
	bool GetBitBool(mainType* coordinates) override;
	IYoung* GetLayer(int index) override;

	void Print(ostream& os) override;

	Young2D* Clone() override;

	bool operator==(const Young2D& other);
	bool operator==(const IYoung& other) override;

	//spawn children
	vector<IYoung*> Propagate() override;

	vector<mainType*> GetPotentialCoords(int MainDimension) override;
};

class YoungNDim : public IYoung {
public:
	//TOTAL: 11 OVERRIDES
	vector<IYoung*> layers;
	BitBool* canAddLayer;
	//recursive constructor
	YoungNDim(mainType startCubesInZeroPoint, unsigned char _dimensions);
	YoungNDim(vector<IYoung*> _layers, unsigned char _dimensions);
	YoungNDim();

	//destructor
	~YoungNDim() override;

	//check if all the numbers in the sequence are equal to 0 (eg. 0 0 0 0)
	static bool IsVectorZero(mainType* vect, int length);

	//returns 1 if added 0 if not. If you use checkOnly it will only return the possibility of adding a cube
	//also refreshes the bitmask (BitLine) of the 2D layers 
	int AddCube(mainType* coordinates, bool checkOnly = false, bool TopDimension = false) override;

	IYoung* GetLayer(int index) override;

	//Recursive get. Returns the height of the column at given coordinates
	int GetColumn(mainType* coordinates) override;

	//for oCorners
	void SetBitBool(mainType* coordinates, bool bit) override;
	bool GetBitBool(mainType* coordinates) override;

	void Print(ostream& os) override;

	//recursive cloning
	YoungNDim* Clone() override;
	//clone this whole object and replace one layer with the given one
	//the given replaceLayer will be used, so copy it before passing
	YoungNDim* Clone(IYoung* replaceLayer, int index);

	//recursive comparasing
	bool operator==(const YoungNDim& other);
	//casting to upper one
	bool operator ==(const IYoung& other) override;

	//create all possible next-gen (on graph) diagrams. 
	vector<IYoung*> Propagate() override;

	//get all current oCorners
	vector<mainType*> GetPotentialCoords(int MainDimension) override;
};

