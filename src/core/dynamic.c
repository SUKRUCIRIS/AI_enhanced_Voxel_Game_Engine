#include "dynamic.h"
#include <stdlib.h>
#include <string.h>

struct DA
{
	unsigned int size;
	unsigned int itemsize;
	void *items;
};

DA *create_DA(unsigned int itemsize)
{
	DA *da = calloc(1, sizeof(DA));
	if (da == 0)
	{
		return 0;
	}
	da->itemsize = itemsize;
	return da;
}

void pushback_DA(DA *da, void *item)
{
	void *tmp = realloc(da->items, (da->size + 1) * da->itemsize);
	if (tmp != 0)
	{
		da->items = tmp;
	}
	else
	{
		return;
	}
	memcpy((char *)(da->items) + da->size * da->itemsize, item, da->itemsize);
	da->size++;
}

void pushback_many_DA(DA *da, void *items, unsigned int count)
{
	void *tmp = realloc(da->items, (da->size + count) * da->itemsize);
	if (tmp != 0)
	{
		da->items = tmp;
	}
	else
	{
		return;
	}
	memcpy((char *)(da->items) + da->size * da->itemsize, items, da->itemsize * count);
	da->size += count;
}

void delete_DA(DA *da)
{
	if (da)
	{
		free(da->items);
		free(da);
	}
}

void *get_data_DA(DA *da)
{
	return da->items;
}

unsigned int get_size_DA(DA *da)
{
	return da->size;
}

unsigned int get_item_size_DA(DA *da)
{
	return da->itemsize;
}

void remove_DA(DA *da, unsigned int index)
{
	if (index >= da->size)
	{
		return;
	}
	char *newitems = calloc(da->size - 1, da->itemsize);
	if (newitems == 0)
	{
		return;
	}
	if (index == 0)
	{
		memcpy(newitems, (char *)(da->items) + da->itemsize, da->itemsize * (da->size - 1));
	}
	else
	{
		memcpy(newitems, da->items, da->itemsize * index);
		memcpy(newitems + (da->itemsize * index), (char *)(da->items) + (da->itemsize * (index + 1)), da->itemsize * (da->size - 1 - index));
	}
	free(da->items);
	da->items = newitems;
	da->size--;
}

void remove_many_DA(DA *da, unsigned int start_index, unsigned int end_index)
{
	if (start_index >= da->size || end_index >= da->size || start_index > end_index)
	{
		return;
	}
	char *newitems = calloc(da->size - (end_index - start_index + 1), da->itemsize);
	if (newitems == 0)
	{
		return;
	}
	if (start_index == 0)
	{
		memcpy(newitems, (char *)(da->items) + da->itemsize * (end_index - start_index + 1),
			   da->itemsize * (da->size - (end_index - start_index + 1)));
	}
	else
	{
		memcpy(newitems, da->items, da->itemsize * start_index);
		memcpy(newitems + (da->itemsize * start_index), (char *)(da->items) + (da->itemsize * (end_index + 1)),
			   da->itemsize * (da->size - 1 - end_index));
	}
	free(da->items);
	da->items = newitems;
	da->size -= (end_index - start_index + 1);
}

void clear_DA(DA *da)
{
	if (da)
	{
		free(da->items);
		da->items = 0;
		da->size = 0;
	}
}

unsigned int get_index_DA(DA *da, void *item)
{
	for (unsigned int i = 0; i < da->size; i++)
	{
		if (memcmp((char *)(da->items) + i * da->itemsize, item, da->itemsize) == 0)
		{
			return i;
		}
	}
	return UINT_MAX;
}