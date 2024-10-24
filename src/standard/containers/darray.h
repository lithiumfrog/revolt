#pragma once

#include "../common.h"

#include "platform/platform.h"


#include <iostream>


// TODO:
// darrayx_create<(row count) 2, (key) u64, (value) struct>
// darrayx_create<(row count) 3, (key) u64, (value) struct, (value) struct>

// // Variadic template that accepts a variable number of arguments
// template<int N, typename... Args>
// struct mytemplate;

// // General template for all values of N
// template<typename T1, typename... Args>
// struct mytemplate<2, T1, Args...> {
//     mytemplate(T1 value1, Args... values) {
//         std::cout << "Template with N=2, values: " << value1;
//         (..., (std::cout << ", " << values));  // Fold expression to print all arguments
//         std::cout << std::endl;
//     }
// };

// // Specialization for N = 3
// template<typename T1, typename... Args>
// struct mytemplate<3, T1, Args...> {
//     mytemplate(T1 value1, Args... values) {
//         std::cout << "Template with N=3, values: " << value1;
//         (..., (std::cout << ", " << values));  // Fold expression to print all arguments
//         std::cout << std::endl;
//     }
// };

// int main() {
//     mytemplate<2, int, double> obj1(1, 2.5);  // Template with 2 arguments
//     mytemplate<3, int, double, std::string> obj2(1, 2.5, "hello");  // Template with 3 arguments

//     return 0;
// }



typedef struct darray_header_st {
    u64 length;
    u64 capacity;
    u64 stride;
    memory_pool_st* pool;
    memory_tag_e tag;
} darray_header_st;

#define darray_header_size_bytes ((sizeof(darray_header_st) + MIN_ALIGNMENT - 1) & ~(MIN_ALIGNMENT - 1))


// create
void* _darray_create(u64 capacity, u64 stride, memory_pool_st* pool, memory_tag_e tag, const char* file, int line);
template<typename T>
T* darray_create(u64 capacity, memory_pool_st* pool, memory_tag_e tag) {
    void* array = _darray_create(capacity, sizeof(T), pool, tag, __FILE__, __LINE__);
    return static_cast<T*>(array);
}

// destroy
void _darray_destroy(void* array, const char* file, int line);
template<typename T>
void darray_destroy(T*& array) {
    _darray_destroy(array, __FILE__, __LINE__);
    array = nullptr;
}

// grow
void* _darray_grow(void* array, const char* file, int line);

// resize (increase capacity)
void* _darray_resize(void* array, u64 new_capacity, const char* file, int line);
template<typename T>
void darray_resize(T*& array, u64 new_capacity) {
    array = static_cast<T*>(_darray_resize(array, new_capacity, __FILE__, __LINE__));
}

// get capacity
u64 darray_capacity_get(void* array);

// set length manually (e.g., for writing data into array directly)
void darray_length_set(void* array, u64 new_length);

// get length
u64 darray_length_get(void* array);

// push
void* _darray_push(void* array, const void* value_ptr, const char* file, int line);
template<typename T>
void darray_push(T*& array, const T& value) {
    T typed_value = value;
    array = static_cast<T*>(_darray_push(array, &typed_value, __FILE__, __LINE__));
}

// pop
void _darray_pop(void* array, void* dest);
template<typename T>
T darray_pop(T*& array) {
    T value;
    _darray_pop(array, &value);
    return value;
}

// pop at
void _darray_pop_at(void* array, u64 index, void* dest);
template<typename T>
T darray_pop_at(T*& array, u64 index) {
    T value;
    _darray_pop_at(array, index, &value);
    return value;
}

// erase any matching
void _darray_erase_any_matching(void* array, const void* value);
template<typename T>
void darray_erase_any_matching(T*& array, const T& value) {
    _darray_erase_any_matching(array, &value);
}

// find one and move to end
void _darray_find_one_and_move_to_end(void* array, const void* value);
template<typename T>
void darray_find_and_move_to_end(T*& array, const T& value) {
    _darray_find_one_and_move_to_end(array, &value);
}

// zero
void darray_zero(void* array);
