#include <iostream>
#include <string>
#include <fstream>
#include "block_chain.h"

using namespace std;

int main()
{
	string s = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	ofstream results("difficulty.csv", ofstream::out);
	for (int j = 0; j < 100; j++)
	{
		results << "Iteration " << j << "," << endl;

		block_chain bchain;
		for (uint32_t i = 1; i < 100u; ++i)
		{
			results << "Block " << i << "," << endl;

			cout << "Mining block " << i << "..." << endl;
			bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data") + s));
		}
	}
	results.close();
    return 0;
}