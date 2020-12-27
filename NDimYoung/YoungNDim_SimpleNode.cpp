#include "YoungNDim_SimpleNode.h"

using namespace std;

YoungNDim_SimpleNode::YoungNDim_SimpleNode(IYoung* _node, int _dim)
{
	dim = new InfInt();
	(*dim) = _dim;
	node = _node;
}

YoungNDim_SimpleNode::YoungNDim_SimpleNode(IYoung* _node, InfInt* _dim)
{
	dim = new InfInt();
	(*dim) = (*_dim);
	node = _node;
}
YoungNDim_SimpleNode::~YoungNDim_SimpleNode()
{
	delete node;
	delete dim;
}

//create child diagrams vector
vector<YoungNDim_SimpleNode*> YoungNDim_SimpleNode::Propagate()
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

bool YoungNDim_SimpleNode::operator==(const YoungNDim_SimpleNode& other)
{
	return ((*node) == (*other.node));
}

bool YoungNDim_SimpleNode::Equals(const YoungNDim_SimpleNode* other)
{
	return ((*node) == (*(other->node)));
}

//deletes other and adds its dim to this instance
void YoungNDim_SimpleNode::Absorb(YoungNDim_SimpleNode* other)
{
	(*dim) += (*(other->dim));
	delete other;
}