#pragma once
// my own vector library

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

typedef struct DA DA;

void init_recycle_DA(unsigned int max_size);

void delete_recycle_DA(void);

void clear_recycle_DA(void);

DA *create_DA(unsigned int itemsize);

DA *create_DA_HIGH_MEMORY(unsigned int itemsize);

void pushback_DA(DA *da, void *item);

void pushback_many_DA(DA *da, void *items, unsigned int count);

void delete_DA(DA *da);

void *get_data_DA(DA *da);

unsigned int get_size_DA(DA *da);

unsigned int get_item_size_DA(DA *da);

void remove_DA(DA *da, unsigned int index);

void remove_many_DA(DA *da, unsigned int start_index, unsigned int end_index);

void clear_DA(DA *da);

unsigned int get_index_DA(DA *da, void *item);

void trim_DA(DA *da);