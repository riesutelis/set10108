#include "block_chain_par_while.h"
#include "sha256.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <omp.h>
#include <limits.h>

using namespace std;
using namespace std::chrono;

ofstream *result_par;

const uint32_t thread_count = omp_get_max_threads();
uint32_t chunk_size;

// Note that _time would normally be set to the time of the block's creation.
// This is part of the audit a block chain.  To enable consistent results
// from parallelisation we will just use the index value, so time increments
// by one each time: 1, 2, 3, etc.
block_par::block_par(uint32_t index, const string &data)
	: _index(index), _data(data), _nonce(0), _time(static_cast<long>(index))
{
}

void block_par::mine_block(uint32_t difficulty) noexcept
{
	string str(difficulty, '0');

	auto start = steady_clock::now();

	// Variables required for paralelisation
	bool done = false; // Shared condition for ending parallel loop
	std::string thread_hash; // A private hash contaner for each thread
	int lowest_i = INT32_MAX; // Shared lowestt index used to get the first good hash
	bool good = false; // Private bool to tell a thread if a good hash is found

	int i = 1;
	while (!done)
#pragma omp parallel num_threads(thread_count) default(none) shared(lowest_i, done) private(good, thread_hash, chunk_size)
	{
		int nonce = omp_get_thread_num() * chunk_size + i;
		for (int j = 0; j < chunk_size; j++)
		{
			thread_hash = calculate_hash(nonce);
			if (thread_hash.substr(0, difficulty) == str)
			{
				good = true;
				break;
			}
			else
				nonce++;
		}
#pragma omp barrier
#pragma omp critical
		{
			if (good)
				if (nonce < lowest_i)
				{
					lowest_i = nonce;
					done = true;
					_hash = thread_hash;
				}
			i += chunk_size;
		}
	}

	auto end = steady_clock::now();
	duration<double> diff = end - start;
	*result_par << diff.count() << ", ";

	cout << "Block mined: " << _hash << " in " << diff.count() << " seconds" << endl;
}

std::string block_par::calculate_hash(uint64_t nonce) const noexcept
{
	string s;
	s = std::to_string(_index) + std::to_string(_time) + _data + std::to_string(nonce) + prev_hash;
	return sha256(s);
}

block_chain_par::block_chain_par()
{
	_chain.emplace_back(block_par(0, "Genesis Block"));
	_difficulty = 1;
}

void block_chain_par::add_block(block_par &&new_block, std::ofstream* f, uint32_t diff, uint32_t chunk) noexcept
{
	chunk_size = chunk;
	result_par = f;
	new_block.prev_hash = get_last_block().get_hash();
	new_block.mine_block(diff);
	_chain.push_back(new_block);
}