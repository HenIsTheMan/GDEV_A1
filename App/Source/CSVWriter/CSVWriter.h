#pragma once
#include <string>
#include <vector>
#include <utility>
using namespace std;

struct CCSVWriter final{
	CCSVWriter() = default;
	bool write_CSV_with_columnname(string filename, vector<pair<string, vector<int>>> vData); //Write to CSV file containing column names
	bool write_CSV(string filename, const int NUM_TILES_XAXIS, const int NUM_TILES_YAXIS, vector<vector<int>> vData);
};