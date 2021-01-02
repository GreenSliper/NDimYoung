#include "Tableaux2D.h"
#include "TableSet.h"
#include "YoungNDim_SimpleNode.h"

void ExportGraphLevel(vector<YoungNDim_SimpleNode*> level, ostream& stream)
{
	for (int i = 0; i < level.size(); i++)
	{
		level[i]->node->Print(stream);
		stream << '|' << (*level[i]->dim) << '\n';
	}
}

void ExportGraph(vector<vector<YoungNDim_SimpleNode*>> graph, ostream& stream)
{
	for (int i = 0; i < graph.size(); i++)
	{
		stream << "Level: " << i + 1 << "; diagrams count: " << graph[i].size() << '\n';
		ExportGraphLevel(graph[i], stream);
	}
}

//create graph with diagrams & their dimensions only 
vector<vector<YoungNDim_SimpleNode*>> CreateGraph(YoungNDim_SimpleNode* rootDiagram, int levelCount, string exportOnFlyFileName = "\0")
{
	//result
	vector<vector<YoungNDim_SimpleNode*>> levels(levelCount);
	//add root diagram
	levels[0] = { rootDiagram };
	if (exportOnFlyFileName[0] != '\0') {
		ofstream os(exportOnFlyFileName, ios_base::app);
		os << "Level: 1; diagrams count: 1 \n";
		ExportGraphLevel(levels[0], os);
		os.close();
	}
	int streamNum = 4;
	//iterate & create new levels based on previous ones
	for (int i = 1; i < levelCount; i++)
	{
		//foreach diagram in previous level
		for (int j = 0; j < levels[i - 1].size(); j++) {
			//create children pool of selected
			vector<YoungNDim_SimpleNode*> children = levels[i - 1][j]->Propagate();
			//we can delete previously computed diagrams to save memory
			if (exportOnFlyFileName[0] != '\0')
				delete levels[i - 1][j];
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
		levels[i - 1].clear();
		//this level won't be expanded
		levels[i].shrink_to_fit();
		//export on fly
		if (exportOnFlyFileName[0] != '\0') {
			ofstream os(exportOnFlyFileName, ios_base::app);
			os << "\n" << "Level: " << i + 1 << "; diagrams count: " << levels[i].size() << '\n';
			ExportGraphLevel(levels[i], os);
			os.close();
		}
		cout << "levels computed: " << i + 1 << '\n';
		//all ancestors checked, go to next generation
	}
	return levels;
}

vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos)
			pos = str.length();
		string token = str.substr(prev, pos - prev);
		//if (!token.empty()) //allow empty entries
		tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}