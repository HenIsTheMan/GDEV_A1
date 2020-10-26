#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include "System\filesystem.h"

vector<pair<string, vector<int>>> CSVReader::read_CSV_with_columnname(string filename){ //Each pair represents <column name, column values>
	vector<pair<string, vector<int>>> vRowsOfValues;
	ifstream myFile(filename);
	try{
		if(!(myFile.is_open() && myFile.good())){
			throw exception("Could not open CSV with columnName");
		}
	} catch(const exception& e){
		printf("Exception: %s\n", e.what());
		return vRowsOfValues;
	}
	string line, colName;
	int val;

	///Read column names
	getline(myFile, line);
	stringstream ss(line);
	while(getline(ss, colName, ',')){ //Extract each column name
		vRowsOfValues.push_back({colName, vector<int>{}});
	}

	while(getline(myFile, line)){
		stringstream ss(line);
		if(!(line[0] == '/' && line[1] == '/')){ //If not comment line...
			int iCol = 0; //Curr col index
			while(ss >> val){ //Extract each int
				// Add the current integer to the 'colIdx' column's values vector
				vRowsOfValues.at(iCol).second.emplace_back(val);
				if(ss.peek() == ','){ //Ignore next token if it's a comma
					ss.ignore();
				}
				++iCol;
			}
		}
	}
	myFile.close();
	return vRowsOfValues;
}

vector<vector<int>> CSVReader::read_CSV(string filename, const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS){
	bool bResult = true; //Result of CSV reading attempt
	vector<vector<int>> vRowsOfValues;
	ifstream myFile(filename);

	try{
		if(!(myFile.is_open() && myFile.good())){
			throw exception("Could not open CSV");
		}
	} catch(const exception& e){
		printf("Exception: %s\n", e.what());
		return vRowsOfValues;
	}

	string line, colname;
	int val;
	while(getline(myFile, line)){
		stringstream ss(line);
		if(!(line[0] == '/' && line[1] == '/')){ //If not comment line...
			vector<int> vColumnsOfValues;
			while(ss >> val){ //Extract each int
				vColumnsOfValues.emplace_back(val);
				if(ss.peek() == ','){ //Ignore next token if it's a comma
					ss.ignore();
				}
			}
			if(vColumnsOfValues.size() == NUM_TILES_XAXIS){
				vRowsOfValues.emplace_back(vColumnsOfValues);
			} else{
				vColumnsOfValues.clear();
				bResult = false;
			}
		}
	}
	myFile.close();
	
	if(vRowsOfValues.size() != NUM_TILES_YAXIS){ //Check if no. of rows is correct
		bResult = false;
	}
	if(bResult == false){ //Clear all vecs if an error occured during CSV reading process
		for(int i = 0; i < (int)vRowsOfValues.size(); ++i){
			vRowsOfValues[i].clear();
		}
		vRowsOfValues.clear();
	}
	return vRowsOfValues;
}