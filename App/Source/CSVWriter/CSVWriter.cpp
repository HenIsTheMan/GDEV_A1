#include "CSVWriter.h"
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "System\filesystem.h"

bool CCSVWriter::write_CSV_with_columnname(string filename, vector<pair<string, vector<int>>> vData){
	ofstream myFile(filename, ios::out | ios::trunc);
	try{
		if(!myFile.is_open() || !myFile.good()){
			throw runtime_error("");
		}
	} catch(const runtime_error&){
		return false;
	}

	///Write all data (including column names)
	for(auto iterRow = vData.begin(); iterRow != vData.end(); ++iterRow){
		for(auto iterColumn = iterRow->second.begin(); iterColumn != iterRow->second.end(); ++iterColumn){
			myFile << *iterColumn;
			if(iterColumn != iterRow->second.end()){
				myFile << ", ";
			}
			myFile << endl;
		}
	}
	myFile.close();
	return true;
}

bool CCSVWriter::write_CSV(string filename, const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS, vector<vector<int>> vData){
	ofstream myFile(filename, ios::out | ios::trunc);
	try{
		if(!myFile.is_open() || !myFile.good()){
			throw runtime_error("");
		}
	} catch(const runtime_error&){
		return false;
	}

	if(vData.size() == NUM_TILES_YAXIS){ //If no. of rows in vData matches NUM_TILES_YAXIS...
		vector<vector<int>>::iterator iterRow = vData.begin();
		if((*iterRow).size() == NUM_TILES_XAXIS){ //If no. of columns in vData matches NUM_TILES_XAXIS...
			for(iterRow = vData.begin(); iterRow != vData.end(); iterRow++){
				for(auto iterColumn = (*iterRow).begin(); iterColumn != (*iterRow).end(); ++iterColumn){
					myFile << *iterColumn;
					if(iterColumn != (*iterRow).end()){
						myFile << ", ";
					}
				}
				myFile << endl;
			}
		}
	}
	myFile.close();
	return true;
}