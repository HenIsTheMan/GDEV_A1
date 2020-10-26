#pragma once
#include <string>
#include <vector>
#include <utility>
using namespace std;

struct CSVReader final{
	CSVReader() = default;
	vector<pair<string, vector<int>>> read_CSV_with_columnname(string filename); //Read from CSV file which contains column names
	vector<vector<int>> read_CSV(string filename, const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS); //Read from CSV file
};