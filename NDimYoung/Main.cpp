#include "Tableaux2D.h"
#include "Tableau3D.h"
#include "TableSet.h"
#include "YoungNDim_SimpleNode.h"
#include "CommonMethods.h"

using namespace std;

const auto core_count = thread::hardware_concurrency();

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
	srand(time(0));

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
	else if (argc == 5)
	{
		bool tablegen3D = false;
		int i;
		for (i = 0; i < argc &&!tablegen3D; i++)
			tablegen3D = (strcmp("-tablegen3D", argv[i]) == 0);
		if (tablegen3D) {
			vector<Young2D*> young3d;
			vector<string> layers = split(string(argv[i++]), "|");
			
			for (int i = 0; i < layers.size(); i++)
				young3d.push_back(new Young2D(layers[i]));
			TableSet3D tab(young3d);
			int iterations, saveEvery;
			//cout << "Input total iteration count and save interval (in iteration count, ex: \"500 50\" "
			//	<< "will generate total 500 tables, exporting results to file each 50 iterations\n";
			iterations = atoi(argv[i++]);
			saveEvery = atoi(argv[i++]);
			tab.GenerateTablesAsync(iterations, "tables3d.txt", saveEvery);
		}
	}
	else if (argc == 6)
	{
		bool tablegen3D = false;
		int i;
		for (i = 0; i < argc && !tablegen3D; i++)
			tablegen3D = (strcmp("-tablegen3D", argv[i]) == 0);
		vector<future<void>> tasks;
		if (tablegen3D)
		{
			int startLevel, endLevel, totalIterations, exportEvery;
			startLevel = atoi(argv[i++]);
			endLevel = atoi(argv[i++]);
			totalIterations = atoi(argv[i++]);
			exportEvery = atoi(argv[i++]);
			cout << "Creating graph levels " << startLevel << " - " << endLevel << endl;
			auto graph = CreateGraph(new YoungNDim_SimpleNode(new YoungNDim(1, 3)), endLevel);
			for (int j = startLevel; j < endLevel; j++)
			{
				cout << "Processing level " << j << endl;
				sort(graph[j].begin(), graph[j].end(),
					[](YoungNDim_SimpleNode* l, YoungNDim_SimpleNode* r)
					{   return *(l->dim) < *(r->dim);  });
				
				double targetDimPercents[] = {95, 90, 80, 70, 60, -1};

				double* targPercent = targetDimPercents;
				vector<YoungNDim_SimpleNode*> inspected;
				if (graph[j].size() > 10)
				{
					bool added2d = false;
					InfInt max = (*(graph[j][graph[j].size() - 1]->dim));
					//cout <<"max: "<< *(graph[j][graph[j].size() - 1]->dim) << endl;
					inspected.push_back(graph[j][graph[j].size() - 1]);
					for (int k = graph[j].size() - 1; k > -1 && !added2d; k--)
					{
						auto diag = dynamic_cast<YoungNDim*>(graph[j][k]->node);
						vector<Young2D*> layers2d;
						for (int l = 0; l < diag->layers.size(); l++)
							layers2d.push_back(dynamic_cast<Young2D*>(diag->layers[l]));
						if (layers2d.size() == 1)
							added2d = true;
						else
						{
							bool allLayers1col = true;
							for (int z = 0; z < layers2d.size() && allLayers1col; z++)
								allLayers1col = (layers2d[z]->columns.size() == 1);
							if ((added2d = allLayers1col) == false)
							{
								bool allLayersCols1 = true;
								for (int z = 0; z < layers2d.size() && allLayersCols1; z++)
									for (int m = 0; m < layers2d[z]->columns.size() && allLayersCols1; m++)
										allLayersCols1 = (layers2d[z]->columns[m] == 1);
								added2d = allLayersCols1;
							}
						}
						if (added2d)
							inspected.push_back(graph[j][k]);
					}
					for (int k = graph[j].size() - 1; k > -1 && *targPercent >= 0; k--)
					{
						//cout << *(graph[j][k]->dim) << endl;
						if (max.toLong() * (*targPercent) * 0.01 > (graph[j][k]->dim)->toLong())
						{
							inspected.push_back(graph[j][k]);
							targPercent++;
						}
					}
				}
				else
					inspected = graph[j];
				for (int k = 0; k < inspected.size(); k++)
				{
					if (*inspected[k]->dim < 2)
						continue;
					cout << "Generating tables for diagram: " << endl;
					inspected[k]->node->Print(cout);
					cout << endl;
					cout << "dim = " << *inspected[k]->dim << "; " <<
						*(inspected[k]->dim)*100.0/ *(inspected[0]->dim) <<"\% of max"<< endl;
					auto diag = dynamic_cast<YoungNDim*>(inspected[k]->node);
					vector<Young2D*> layers2d;
					for (int l = 0; l < diag->layers.size(); l++)
						layers2d.push_back(dynamic_cast<Young2D*>(diag->layers[l]));
					stringstream ss;
					inspected[k]->node->Print(ss);
					string fileName = ss.str() + ".txt";
					tasks.push_back(async([layers2d, totalIterations, fileName, exportEvery]
						{ 
							(new TableSet3D(layers2d))->GenerateTablesAsync(totalIterations, fileName.c_str(), exportEvery);
						}
					));
					if (tasks.size() >= core_count)
					{
						int mx = tasks.size();
						for (int z = 0; z < mx; z++)
						{
							tasks[0].get();
							tasks.erase(tasks.begin());
						}
					}
				}
			}
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