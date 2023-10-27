#include "dynamic.h"
#include <stdlib.h>
#include <string.h>

struct DA
{
	void *items;
	unsigned int size;
	unsigned int itemsize;
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

void clear_DA(DA *da)
{
	if (da)
	{
		free(da->items);
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