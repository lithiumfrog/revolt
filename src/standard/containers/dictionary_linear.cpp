#include "dictionary_linear.h"

#include "platform/platform.h"
#include "platform/log.h"

dictionary_linear_st* dictionary_linear_create(memory_pool_st* pool, memory_tag_e tag)
{
    dictionary_linear_st* dictionary = (dictionary_linear_st*)pool_allocate(sizeof(dictionary_linear_st), pool, tag, true);
    dictionary->pair_darray = darray_create<dictionary_linear_pair_st>(1, pool, tag);

    return dictionary;
}


void dictionary_linear_push(dictionary_linear_st* dictionary, u64 key, void* value)
{
    dictionary_linear_pair_st pair;
    pair.key = key;
    pair.value = value;
    darray_push(dictionary->pair_darray, pair);
}

void* dictionary_linear_find_from_beginning(dictionary_linear_st* dictionary, u64 key)
{
    u64 length = darray_length_get(dictionary->pair_darray);
    darray_header_st* header = (darray_header_st*)((u8*)(dictionary->pair_darray) - darray_header_size_bytes);
    for (u64 i = 0; i < length; i++)
    {
        dictionary_linear_pair_st* pair = (dictionary_linear_pair_st*)((u8*)(dictionary->pair_darray) + (header->stride * i));
        if (pair->key == key)
        {
            return pair->value;
        }
    }

    return NULL;
}

void* dictionary_linear_find_from_end(dictionary_linear_st* dictionary, u64 key)
{
    LOG_FATAL("TODO: make dictionary_linear_find_from_end, currently finding from beginning");
    return dictionary_linear_find_from_beginning(dictionary, key);
}
