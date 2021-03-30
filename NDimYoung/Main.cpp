#include "Tableaux2D.h"
#include "Tableau3D.h"
#include "TableSet.h"
#include "YoungNDim_SimpleNode.h"
#include "CommonMethods.h"

using namespace std;



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

	vector<Young2D*> young3d;
	cout << "Enter 2d layer count\n";
	int layerCnt = 0;
	cin >> layerCnt;
	for (int i = 0; i < layerCnt; i++)
	{
		young3d.push_back(new Young2D(cin));
	}
	TableSet3D tab(young3d);
	int iterations, saveEvery;
	cout << "Input total iteration count and save interval (in iteration count, ex: \"500 50\" "
		<< "will generate total 500 tables, exporting results to file each 50 iterations\n";
	cin >> iterations;
	cin >> saveEvery;
	tab.GenerateTablesAsync(iterations, "tables3d.txt", saveEvery);

	if (argc == 1)
	{
		int res = 0;
		cout << "Enter 2D tableaux generating mode or N-dim graph builder? [1 = 2d tabl, 0 = graph mode]" << endl;
		cin >> res;
		if (res)
		{
			Young2D diagram(cin);
			TableSet ts(diagram);
			cout << "Enter generation iterations" << endl;
			int genCnt = 0;
			cin >> genCnt;
			ts.GenerateTablesFast(genCnt);
			ofstream os("Tableaux.txt");
			ts.ExportTablesFast(os);
			os.close();
		}
	}
	bool infiniteGen = false, genTables = false;
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
	if(!infiniteGen)
		for (x = 0; x < argc && !genTables; x++)
			genTables = (strcmp("-tables", argv[x]) == 0);

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
	
	if (genTables)
	{
		ifstream str = ifstream(argv[x]);
		Young2D diagram(str);
		TableSet ts(diagram);
		cout << "Enter generation iterations" << endl;
		int genCnt = stoi(argv[x+1]);
		ts.GenerateTables(genCnt);
		ofstream os("Tableaux.txt");
		ts.ExportTables(os);
		os.close();
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