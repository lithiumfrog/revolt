#pragma once

#include "../common.h"

#include "../containers/darray.h"

struct dictionary_linear_pair_st {
    u64 key;
    void* value;
};

struct dictionary_linear_st {
    dictionary_linear_pair_st* pair_darray;
};

dictionary_linear_st* dictionary_linear_create(memory_pool_st* pool, memory_tag_e tag);

void dictionary_linear_push(dictionary_linear_st* dictionary, u64 key, void* value);
void* dictionary_linear_find_from_beginning(dictionary_linear_st* dictionary, u64 key);
void* dictionary_linear_find_from_end(dictionary_linear_st* dictionary, u64 key);
