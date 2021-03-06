// BubbleSort.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

using namespace std;

// Generates a vector of random values
vector<unsigned int> generate_values(size_t size)
{
	// Create random engine
	random_device r;
	default_random_engine e(r());
	// Generate random numbers
	vector<unsigned int> data;
	for (size_t i = 0; i < size; ++i)
		data.push_back(e());
	return data;
}

void bubble_sort(vector<unsigned int> &v)
{
	for (int i = 0; i < v.size() - 1; i++)
		for (int j = v.size() - 1; j > i; j--)
			if (v[j] < v[j - 1])
			{
				unsigned int x = v[j];
				v[j] = v[j - 1];
				v[j - 1] = x;
			}
}


int main(int argc, char **argv)
{
	// Create results file
	ofstream results("bubble.csv", ofstream::out);
	// Gather results for 2^8 to 2^16 results
	for (size_t size = 8; size <= 16; ++size)
	{
		// Output data size
		results << pow(2, size) << ", ";
		// Gather 100 results
		for (size_t i = 0; i < 100; ++i)
		{
			// Generate vector of random values
			cout << "Generating " << i << " for " << pow(2, size) << " values" << endl;
			auto data = generate_values(static_cast<size_t>(pow(2, size)));
			// Sort the vector
			cout << "Sorting" << endl;
			auto start = chrono::system_clock::now();
			bubble_sort(data);
			auto end = chrono::system_clock::now();
			auto total = chrono::duration_cast<chrono::milliseconds>(end - start).count();
			// Output time
			results << total << ",";
		}
		results << endl;
	}
	results.close();

	return 0;
}