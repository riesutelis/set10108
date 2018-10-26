#include <iostream>
#include <string>
#include <fstream>
#include "block_chain.h"

using namespace std;

int main()
{
	string s = "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdf";

	for (int dat = 0; dat < 3; dat++)
	{
		string data = "";
		for (int i = 0; i < dat; i++)
		{
			s += s;
			data = s;
		}
		for (int diff = 1; diff <= 4; diff++)
		{
			ofstream *file = new ofstream("data " + to_string(dat) + " difficulty " + to_string(diff) + ".csv", ofstream::out);
			for (int i = 1; i <= 100; i++)
				*file << "Block " << i << ", ";
			*file << endl;
			for (int j = 1; j <= 100; j++)
			{
				block_chain bchain;
				for (uint32_t i = 1; i <= 100u; ++i)
				{
					cout << "Data size " << dat << " Difficulty " << diff << " Test " << j << endl;
					cout << "Mining block " << i << "..." << endl;
					bchain.add_block(block(i, string("Block ") + to_string(i) + string(" Data") + data), file, diff);
				}
				*file << endl;
			}
			file->close();
		}
	}
	return 0;
}