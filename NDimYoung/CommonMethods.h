#pragma once

#include "Tableaux2D.h"
#include "TableSet.h"
#include "YoungNDim_SimpleNode.h"

void ExportGraphLevel(vector<YoungNDim_SimpleNode*> level, ostream& stream);
void ExportGraph(vector<vector<YoungNDim_SimpleNode*>> graph, ostream& stream);
vector<vector<YoungNDim_SimpleNode*>> CreateGraph(YoungNDim_SimpleNode* rootDiagram, int levelCount, string exportOnFlyFileName = "\0");
vector<string> split(const string& str, const string& delim);