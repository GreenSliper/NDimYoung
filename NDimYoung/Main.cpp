#include "Tableaux2D.h"
#include "TableSet.h"
#include "YoungNDim_SimpleNode.h"

using namespace std;

#pragma region Common_Methods

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
#pragma endregion

int main(int argc, char* argv[])
{
	//MANIPULATING DIAGRAMS MANUALLY EXAMPLE:
	/*
	YoungNDim_SimpleNode* diag = new YoungNDim_SimpleNode(new YoungNDim(1, 4));
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
	srand(0);
	if (argc == 1)
	{
		int res = 0;
		cout << "Enter 2D tableaux generating mode? [1 = yes, 0 = no]" << endl;
		cin >> res;
		if (res)
		{
			string dg;
			Young2D diagram(0);
			cout << "Enter column heights, split by [SPACE]:" << endl;
			char c;
			while (cin.get(c) && c != '\n');
			getline(cin, dg);
			vector<string> dgNms = split(dg, " ");
			diagram.columns = vector<mainType>(dgNms.size());
			for (int i = 0; i < dgNms.size(); i++)
				diagram.columns[i] = stoi(dgNms[i]);
			TableSet ts(diagram);
			cout << "Enter generation iterations" << endl;
			int genCnt = 0;
			cin >> genCnt;
			ts.GenerateTables(genCnt);
			ofstream os("Tableaux.txt");
			ts.ExportTables(os);
			os.close();
		}
	}
	bool infiniteGen = false;
	int x = 0;
	int graphDim = 4, lvls;
	IYoung* startDiag;
	//create filename
	string fileName;
	time_t _tm = time(NULL);
	fileName = string(asctime(localtime(&_tm)));
	fileName.pop_back();
	fileName = string("graph ") + fileName + string(".txt");
	replace(fileName.begin(), fileName.end(), ':', '.');

	for (; x < argc && !infiniteGen; x++)
		infiniteGen = (strcmp("-all", argv[x]) == 0);
	//2nd key stands for dimension of the graph diagrams
	if (infiniteGen) {
		graphDim = atoi(argv[x]);
		fileName = to_string(graphDim) + "D " + fileName;
		ofstream os(fileName);
		os.close();
		if (graphDim == 2)
			startDiag = new Young2D(1);
		else
			startDiag = new YoungNDim(1, graphDim);
		CreateGraph(new YoungNDim_SimpleNode(startDiag), 999, fileName);
		return 0;
	}
	//ask dimensions count
	cout << "Welcome to Young N-dimensonal graph builder!" << endl;
	do {
		cout << "Enter target dimension of the graph diagrams: " << endl;
		cin >> graphDim;
		if (graphDim < 2)
			cout << "graph must have 2+ dimensions!" << endl;
	} while (graphDim < 2);
	//ask level count
	do {
		cout << "Enter the number of levels you want to be computed: " << endl;
		cin >> lvls;
		if (lvls < 2)
			cout << endl << "graph must have 2+ levels!" << endl;
	} while (lvls < 2);
	//choose an option
	int ans = 0;
	do {
		cout << "1) Export to file" << endl << "2) Print to console" << endl << "1 or 2 (type number)?" << endl;
		cin >> ans;
		if (ans < 1 || ans > 2)
			cout << endl << "Please, select option 1 or 2" << endl;
	} while (ans < 1 || ans > 2);

	if (graphDim == 2)
		startDiag = new Young2D(1);
	else
		startDiag = new YoungNDim(1, graphDim);

	switch (ans)
	{
	case 1:
		fileName = to_string(graphDim) + "D " + fileName;
		CreateGraph(new YoungNDim_SimpleNode(startDiag), lvls, fileName);
		break;
	case 2:
		//create simple graph
		vector<vector<YoungNDim_SimpleNode*>> graph = CreateGraph(new YoungNDim_SimpleNode(startDiag), lvls);
		//print graph
		ExportGraph(graph, cout);
		break;
	}
	cout << "Success!" << endl;
	system("pause");
	return 0;
}