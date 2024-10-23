#pragma once

#include "../common.h"

// FNV1a c++11 constexpr compile time hash functions, 32 and 64 bit
// str should be a null terminated string literal, value should be left out
// e.g fnv1a_hash_32("example")
// code license: public domain or equivalent
// post: https://notes.underscorediscovery.com/constexpr-fnv1a/
// gist: https://gist.github.com/ruby0x1/81308642d0325fd386237cfa3b44785c

constexpr u32 val_32_const = 0x811c9dc5u;
constexpr u32 prime_32_const = 0x1000193u;
constexpr u64 val_64_const = 0xcbf29ce484222325ull;
constexpr u64 prime_64_const = 0x100000001b3ull;

constexpr u32 fnv1a_hash_32(const char* str, u32 value = val_32_const) noexcept {
	for (; *str != '\0'; ++str) {
		value = (value ^ static_cast<u32>(static_cast<u8>(*str))) * prime_32_const;
	}
	return value;
}

constexpr u64 fnv1a_hash_64(const char* str, u64 value = val_64_const) noexcept {
	for (; *str != '\0'; ++str) {
		value = (value ^ static_cast<u64>(static_cast<u8>(*str))) * prime_64_const;
	}
	return value;
}
