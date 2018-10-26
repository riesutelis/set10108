#include <iostream>
#include <string>
#include <fstream>
#include "block_chain.h"
#include "block_chain_par_while.h"

using namespace std;

int main()
{
	string s = "qwertyuiop[]asdfghjklzxcvbnm";

	for (int chunk = 1; chunk <= 32; chunk *= 4)
	{
		for (int dat = 0; dat <= 3; dat++)
		{
			string data = "";
			for (int i = 0; i < dat; i++)
				data += s;
			for (int diff = 1; diff <= 3; diff++)
			{
				ofstream *file = new ofstream("data " + to_string(dat) + " difficulty " + to_string(diff) + " chunk size " + to_string(chunk) + ".csv", ofstream::out);
				for (int i = 1; i <= 100; i++)
					*file << "Block " << i << ", ";
				*file << endl;
				for (int j = 1; j <= 100; j++)
				{
					block_chain_par bchain;
					for (uint32_t i = 1; i <= 100u; ++i)
					{
						cout << "Chunk size " << chunk << " Data size " << dat << " Difficulty " << diff << " Test " << j << endl;
						cout << "Mining block " << i << "..." << endl;
						bchain.add_block(block_par(i, string("Block ") + to_string(i) + string(" Data") + data), file, diff, chunk);
					}
					*file << endl;
				}
				file->close();
			}
		}
	}
	return 0;
}