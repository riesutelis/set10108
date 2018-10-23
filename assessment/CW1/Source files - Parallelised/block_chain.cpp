#include "block_chain.h"
#include "sha256.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <omp.h>
#include <limits.h>

using namespace std;
using namespace std::chrono;

ofstream *result;

const uint32_t thread_count = omp_get_num_threads();
uint32_t chunk_size;

// Note that _time would normally be set to the time of the block's creation.
// This is part of the audit a block chain.  To enable consistent results
// from parallelisation we will just use the index value, so time increments
// by one each time: 1, 2, 3, etc.
block::block(uint32_t index, const string &data)
	: _index(index), _data(data), _nonce(0), _time(static_cast<long>(index))
{
}

void block::mine_block(uint32_t difficulty) noexcept
{
	string str(difficulty, '0');

	//auto start = system_clock::now();
	auto start = steady_clock::now();

	//while (_hash.substr(0, difficulty) != str)
	//{
	//	++_nonce;
	//	_hash = calculate_hash();
	//}
	// Variables required for paralelisation
	int done = INT32_MAX; // Shared condition for ending parallel loop
	std::string thread_hash; // A private hash contaner for each thread
	int lowest_i = 5; // Shared lowestt index used to get the first good hash
	bool good = false; // Private bool to tell a thread if a good hash is found
	uint32_t max_chunk_i = thread_count * chunk_size; // Max index value for a single block of threads

// While loop turned into a parallel for, with a shared bool as end condition
#pragma omp parallel for num_threads(thread_count) default(none) private(_nonce, thread_hash, good, chunk_size) shared(lowest_i, done) schedule(static, chunk_size)
//	for (int i = 1; !done; i++)
	for (int i = 1; i < done; i++)
	{
		_nonce = i;
		// Private thread_hash makes sure calculate_hash is used concurenly
		thread_hash = calculate_hash();
		if (thread_hash.substr(0, difficulty) == str)
		{
			good = true;
		}
// The next bit of code needs to be done once per block (all threads have to complete the previous bit before evaluating the outcome)
#pragma omp barrier
#pragma omp critical
		// If THIS THREAD has a good hash
		if (good)
			// If the 
			if (i % max_chunk_i < lowest_i)
			{
				lowest_i = i % max_chunk_i;
				done = 0;
				_hash = thread_hash;
			}
	}

	//auto end = system_clock::now();
	auto end = steady_clock::now();
	duration<double> diff = end - start;
	*result << diff.count() << ", ";

	cout << "Block mined: " << _hash << " in " << diff.count() << " seconds" << endl;
}

std::string block::calculate_hash() const noexcept
{
	/*stringstream ss;
	ss << _index << _time << _data << _nonce << prev_hash;
	return sha256(ss.str());*/
	string s;
	s = std::to_string(_index) + std::to_string(_time) + _data + std::to_string(_nonce) + prev_hash;
	return sha256(s);
}

block_chain::block_chain()
{
	_chain.emplace_back(block(0, "Genesis Block"));
	_difficulty = 1;
}

void block_chain::add_block(block &&new_block, std::ofstream* f, uint32_t diff, uint32_t chunk) noexcept
{
	chunk_size = chunk;
	result = f;
	new_block.prev_hash = get_last_block().get_hash();
	//new_block.mine_block(_difficulty);
	new_block.mine_block(diff);
	_chain.push_back(new_block);
}