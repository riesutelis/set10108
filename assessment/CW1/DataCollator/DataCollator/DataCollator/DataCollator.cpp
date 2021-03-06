#include "pch.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

using namespace std;

int main()
{
	ofstream n("output.csv", ifstream::out);
	n.close();
	int num_of_files = 0;
	vector<double> avgs;
	ofstream o("output.csv", ifstream::app);

	//for (auto &p : fs::directory_iterator("Debug"))
	for (auto &p : fs::directory_iterator("Release"))
	{
		fs::path sp = p;
		string f_name = sp.string().substr(8, sp.string().size() - 12);
		//string f_name = sp.string().substr(6, 19); // debug
		if (f_name.substr(0, 5).compare("data ") != 0)
			continue;
		num_of_files++;

		ifstream f(p, ifstream::in);
		string word;
		if (!f.eof())
			getline(f, word);

		double acc = 0;
		int limit = 0;
		while (!f.eof())
		{
			getline(f, word, ',');
			if (word == " \n")
				continue;
			acc += stod(word);
			limit++;
			if (limit == 1000)
				break;
		}
		acc /= 1000.0f;
		avgs.push_back(acc);

		o << f_name << " ,";

	}
	o << endl;
	for (int i = 0; i < avgs.size(); i++)
		o << avgs[i] << " ,";
	o.close();

	//getchar();
}
