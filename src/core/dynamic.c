#include "dynamic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct DA
{
	unsigned int memory_size;
	unsigned int size;
	unsigned int itemsize;
	unsigned char high_memory;
	void *items;
};

unsigned int recycle_max_size = 0;
DA *HIGH_MEMORY_RECYCLE_BUFFER = 0;

void init_recycle_DA(unsigned int max_size)
{
	if (max_size > 0)
	{
		delete_recycle_DA();
		HIGH_MEMORY_RECYCLE_BUFFER = create_DA_HIGH_MEMORY(sizeof(DA *));
		recycle_max_size = max_size;
	}
}

void delete_recycle_DA(void)
{
	if (HIGH_MEMORY_RECYCLE_BUFFER != 0)
	{
		DA **das = get_data_DA(HIGH_MEMORY_RECYCLE_BUFFER);
		for (unsigned int i = 0; i < HIGH_MEMORY_RECYCLE_BUFFER->size; i++)
		{
			free(das[i]->items);
			free(das[i]);
		}
		free(HIGH_MEMORY_RECYCLE_BUFFER->items);
		free(HIGH_MEMORY_RECYCLE_BUFFER);
		HIGH_MEMORY_RECYCLE_BUFFER = 0;
	}
}

void clear_recycle_DA(void)
{
	if (HIGH_MEMORY_RECYCLE_BUFFER != 0)
	{
		DA **das = get_data_DA(HIGH_MEMORY_RECYCLE_BUFFER);
		for (unsigned int i = 0; i < HIGH_MEMORY_RECYCLE_BUFFER->size; i++)
		{
			free(das[i]->items);
			free(das[i]);
		}
		clear_DA(HIGH_MEMORY_RECYCLE_BUFFER);
	}
}

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

DA *create_DA_HIGH_MEMORY(unsigned int itemsize)
{
	DA *da = 0;
	unsigned char found = 0;
	if (HIGH_MEMORY_RECYCLE_BUFFER != 0 && HIGH_MEMORY_RECYCLE_BUFFER->size > 0)
	{
		DA **das = get_data_DA(HIGH_MEMORY_RECYCLE_BUFFER);
		unsigned int i = 0;
		for (i = 0; i < HIGH_MEMORY_RECYCLE_BUFFER->size; i++)
		{
			if (itemsize == das[i]->itemsize)
			{
				da = das[i];
				found = 1;
				break;
			}
		}
		if (found == 1)
		{
			remove_DA(HIGH_MEMORY_RECYCLE_BUFFER, i);
			da->size = 0;
		}
	}
	else if (found == 0)
	{
		da = calloc(1, sizeof(DA));
		if (da == 0)
		{
			return 0;
		}
		da->itemsize = itemsize;
		da->high_memory = 1;
	}
	return da;
}

void pushback_DA(DA *da, void *item)
{
	void *tmp = da->items;
	if (da->high_memory == 0 || da->memory_size == 0)
	{
		tmp = realloc(da->items, (da->size + 1) * da->itemsize);
		if (tmp != 0)
		{
			da->memory_size++;
		}
	}
	else if (da->size + 1 > da->memory_size)
	{
		tmp = realloc(da->items, (da->memory_size * 2) * da->itemsize);
		if (tmp != 0)
		{
			da->memory_size *= 2;
		}
	}
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
	void *tmp = da->items;
	if (da->high_memory == 0 || da->memory_size == 0)
	{
		tmp = realloc(da->items, (da->size + count) * da->itemsize);
		if (tmp != 0)
		{
			da->memory_size += count;
			da->items = tmp;
		}
	}
	else
	{
		while (da->size + count > da->memory_size)
		{
			tmp = realloc(da->items, (da->memory_size * 2) * da->itemsize);
			if (tmp != 0)
			{
				da->memory_size *= 2;
				da->items = tmp;
			}
		}
	}
	if (tmp == 0)
	{
		return;
	}
	memcpy((char *)(da->items) + da->size * da->itemsize, items, da->itemsize * count);
	da->size += count;
}

void delete_DA(DA *da)
{
	if (da && da->high_memory == 1 && HIGH_MEMORY_RECYCLE_BUFFER != 0 && HIGH_MEMORY_RECYCLE_BUFFER->size < recycle_max_size)
	{
		pushback_DA(HIGH_MEMORY_RECYCLE_BUFFER, &da);
	}
	else if (da)
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
	char *newitems = 0;
	if (da->high_memory == 0)
	{
		newitems = calloc(da->size - 1, da->itemsize);
	}
	else
	{
		newitems = da->items;
	}
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
	da->size--;
	if (da->high_memory == 0)
	{
		free(da->items);
		da->items = newitems;
		da->memory_size = da->size;
	}
}

void remove_many_DA(DA *da, unsigned int start_index, unsigned int end_index)
{
	if (start_index >= da->size || end_index >= da->size || start_index > end_index)
	{
		return;
	}
	char *newitems = 0;
	if (da->high_memory == 0)
	{
		newitems = calloc(da->size - (end_index - start_index + 1), da->itemsize);
	}
	else
	{
		newitems = da->items;
	}
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
	da->size -= (end_index - start_index + 1);
	if (da->high_memory == 0)
	{
		free(da->items);
		da->items = newitems;
		da->memory_size = da->size;
	}
}

void clear_DA(DA *da)
{
	if (da && da->high_memory == 0)
	{
		free(da->items);
		da->items = 0;
		da->size = 0;
		da->memory_size = 0;
	}
	else if (da)
	{
		da->size = 0;
	}
}

unsigned int get_index_DA(DA *da, void *item)
{
	if (da->size == 0)
	{
		return UINT_MAX;
	}
	for (unsigned int i = da->size - 1; i >= 0; i--)
	{
		if (memcmp((char *)(da->items) + i * da->itemsize, item, da->itemsize) == 0)
		{
			return i;
		}
		if (i == 0)
		{
			break;
		}
	}
	return UINT_MAX;
}

void trim_DA(DA *da)
{
	if (da->memory_size > da->size)
	{
		void *x = realloc(da->items, da->itemsize * da->size);
		if (x)
		{
			da->items = x;
			da->memory_size = da->size;
		}
	}
}