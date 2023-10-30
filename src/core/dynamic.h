#pragma once
// my own vector library

#ifndef UINT_MAX
#define UINT_MAX 4294967295U
#endif

typedef struct DA DA;

DA *create_DA(unsigned int itemsize);

void pushback_DA(DA *da, void *item);

void delete_DA(DA *da);

void *get_data_DA(DA *da);

unsigned int get_size_DA(DA *da);

unsigned int get_item_size_DA(DA *da);

void remove_DA(DA *da, unsigned int index);

void clear_DA(DA *da);

unsigned int get_index_DA(DA *da, void *item);