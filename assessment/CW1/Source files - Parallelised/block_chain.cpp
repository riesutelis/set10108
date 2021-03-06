#include "block_chain.h"
#include "sha256_par_for.h"

#include <iostream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

ofstream *result;

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

	auto start = steady_clock::now();

	while (_hash.substr(0, difficulty) != str)
	{
		++_nonce;
		_hash = calculate_hash();
	}

	auto end = steady_clock::now();
	duration<double> diff = end - start;
	*result << diff.count() << ", ";

	cout << "Block mined: " << _hash << " in " << diff.count() << " seconds" << endl;
}

std::string block::calculate_hash() const noexcept
{
	string s;
	s = std::to_string(_index) + std::to_string(_time) + _data + std::to_string(_nonce) + prev_hash;
	return sha256_par(s);
}

block_chain::block_chain()
{
	_chain.emplace_back(block(0, "Genesis Block"));
	_difficulty = 1;
}

void block_chain::add_block(block &&new_block, std::ofstream* f, uint32_t diff) noexcept
{
	result = f;
	new_block.prev_hash = get_last_block().get_hash();
	new_block.mine_block(diff);
	_chain.push_back(new_block);
}