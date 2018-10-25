#pragma once

#include <string>
#include <vector>
#include <fstream>

class block_par
{
private:
	// The index of the block in the chain.
	uint32_t _index;
	// A modifier used to get a suitable block.
	uint64_t _nonce;
	// Data stored in the block.
	std::string _data;
	// Hash code of this block.
	std::string _hash;
	// Time code block was created.
	long _time;

	std::string calculate_hash() const noexcept;

public:
	block_par(uint32_t index, const std::string &data);

	// Difficulty is the minimum number of zeros we require at the
	// start of the hash.
	void mine_block(uint32_t difficulty) noexcept;

	inline const std::string& get_hash() const noexcept { return _hash; }

	// Hash code of the previous block in the chain.
	std::string prev_hash;
};

class block_chain_par
{
private:
	uint32_t _difficulty;
	std::vector<block_par> _chain;

	inline const block_par& get_last_block() const noexcept { return _chain.back(); }

public:
	block_chain_par();


	void add_block(block_par &&new_block, std::ofstream* f, uint32_t diff, uint32_t chunk) noexcept;
};