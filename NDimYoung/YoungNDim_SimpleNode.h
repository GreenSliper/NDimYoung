#include "YoungNDim.h"
#include "InfInt.h"
#include "BitLine.h"
#pragma once
//Simple class for top-level diagram. Stores dimension.
class YoungNDim_SimpleNode
{
public:
	//pointer to main object
	IYoung* node;
	//dimensions (ways from the beginning of the graph to this diagram
	InfInt* dim;//TODO create own realization

	YoungNDim_SimpleNode(IYoung* _node, int _dim = 1);
	YoungNDim_SimpleNode(IYoung* _node, InfInt* _dim);
	~YoungNDim_SimpleNode();

	//create child diagrams vector
	vector<YoungNDim_SimpleNode*> Propagate();
	//deletes other and adds its dim to this instance
	void Absorb(YoungNDim_SimpleNode* other);

	bool operator==(const YoungNDim_SimpleNode& other);
	bool Equals(const YoungNDim_SimpleNode* other);
};