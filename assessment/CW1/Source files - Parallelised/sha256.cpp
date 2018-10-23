#include "sha256.h"

#include <cstring>
#include <fstream>

using namespace std;

const uint32_t SHA256::sha256_k[64] =
        {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
         0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
         0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
         0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
         0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
         0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
         0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
         0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
         0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
         0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
         0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
         0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
         0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
         0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
         0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
         0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

static inline uint32_t SHA2_CH(uint32_t x, uint32_t y, uint32_t z) noexcept
{
    return ((x & y) ^ (~x & z));
}

static inline uint32_t SHA2_MAJ(uint32_t x, uint32_t y, uint32_t z) noexcept
{
    return ((x & y) ^ (x & z) ^ (y & z));
}

static inline uint32_t SHA256_F1(uint32_t x, uint32_t a, uint32_t b, uint32_t c) noexcept
{
    return ((x >> a) | (x << ((sizeof(x) << 3u) - a))) ^ ((x >> b) | (x << ((sizeof(x) << 3u) - b))) ^ ((x >> c) | (x << ((sizeof(x) << 3u) - c)));
}

static inline uint32_t SHA256_F2(uint32_t x, uint32_t a, uint32_t b, uint32_t c) noexcept
{
    return ((x >> a) | (x << ((sizeof(x) << 3u) - a))) ^ ((x >> b) | (x << ((sizeof(x) << 3u) - b))) ^ (x >> c);
}

static inline void SHA2_UNPACK32(uint32_t x, unsigned char *str) noexcept
{
    *((str) + 3) = static_cast<uint8_t>(x);
    *((str) + 2) = static_cast<uint8_t>(x >> 8u);
    *((str) + 1) = static_cast<uint8_t>(x >> 16u);
    *((str) + 0) = static_cast<uint8_t>(x >> 24u);
}

static inline uint32_t SHA2_PACK32(const unsigned char *str) noexcept
{
    return static_cast<uint32_t>(*(str + 3u))
           | static_cast<uint32_t>(*(str + 2) << 8u)
           | static_cast<uint32_t>(*(str + 1) << 16u)
           | static_cast<uint32_t>(*(str + 0) << 24u);
}

void SHA256::transform(const unsigned char *message, size_t block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    const unsigned char *sub_block;
    for (size_t i = 0; i < block_nb; ++i)
    {
        sub_block = message + (i << 6u);
        for (size_t j = 0; j < 16; ++j)
            w[j] = SHA2_PACK32(&sub_block[j << 2u]);
        for (size_t j = 16; j < 64; ++j)
            w[j] = SHA256_F2(w[j - 2], 17, 19, 10) + w[j - 7] + SHA256_F2(w[j - 15], 7, 18, 3) + w[j - 16];
        for (size_t j = 0; j < 8; ++j)
            wv[j] = m_h[j];
        for (size_t j = 0; j < 64; ++j)
        {
            t1 = wv[7] + SHA256_F1(wv[4], 6, 11, 25) + SHA2_CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0], 2, 13, 22) + SHA2_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }
        for (size_t j = 0; j < 8; ++j)
        {
            m_h[j] += wv[j];
        }
    }
}

void SHA256::init()
{
    m_h[0] = 0x6a09e667;
    m_h[1] = 0xbb67ae85;
    m_h[2] = 0x3c6ef372;
    m_h[3] = 0xa54ff53a;
    m_h[4] = 0x510e527f;
    m_h[5] = 0x9b05688c;
    m_h[6] = 0x1f83d9ab;
    m_h[7] = 0x5be0cd19;
    m_len = 0;
    m_tot_len = 0;
}

void SHA256::update(const unsigned char *message, size_t len)
{
    size_t block_nb;
    size_t new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;
    tmp_len = SHA224_256_BLOCK_SIZE - m_len;
    rem_len = len < tmp_len ? len : tmp_len;
    memcpy(&m_block[m_len], message, rem_len);
    if (m_len + len < SHA224_256_BLOCK_SIZE)
    {
        m_len += len;
        return;
    }
    new_len = len - rem_len;
    block_nb = new_len / SHA224_256_BLOCK_SIZE;
    shifted_message = message + rem_len;
    transform(m_block, 1);
    transform(shifted_message, block_nb);
    rem_len = new_len % SHA224_256_BLOCK_SIZE;
    memcpy(m_block, &shifted_message[block_nb << 6u], rem_len);
    m_len = rem_len;
    m_tot_len += (block_nb + 1) << 6u;
}

void SHA256::final(unsigned char *digest)
{
    size_t block_nb;
    size_t pm_len;
    size_t len_b;
    block_nb = (1u + ((SHA224_256_BLOCK_SIZE - 9u) < (m_len % SHA224_256_BLOCK_SIZE)));
    len_b = (m_tot_len + m_len) << 3u;
    pm_len = block_nb << 6u;
    memset(m_block + m_len, 0, pm_len - m_len);
    m_block[m_len] = 0x80;
    SHA2_UNPACK32(static_cast<uint32_t>(len_b), m_block + pm_len - 4u);
    transform(m_block, block_nb);
    for (size_t i = 0; i < 8; ++i)
    {
        SHA2_UNPACK32(m_h[i], &digest[i << 2u]);
    }
}

std::string sha256(const std::string &input)
{
    unsigned char digest[SHA256::DIGEST_SIZE];
    memset(digest, 0, SHA256::DIGEST_SIZE);
    SHA256 ctx = SHA256();
    ctx.init();
    ctx.update(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());
    ctx.final(digest);
    char buf[2 * SHA256::DIGEST_SIZE + 1];
    buf[2 * SHA256::DIGEST_SIZE] = 0;
    for (size_t i = 0; i < SHA256::DIGEST_SIZE; ++i)
        sprintf(buf + i * 2, "%02x", digest[i]);/////////////////////////////////// Parallel here ///////////////////////////
    return std::string(buf);
}