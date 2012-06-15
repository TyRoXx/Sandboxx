#include "linked_list.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>


struct linked_list_entry
{
	linked_list_entry *previous, *next;
	char value[0];
};

static linked_list_entry *allocate_entry(const void *value, size_t size)
{
	linked_list_entry *entry = malloc(sizeof(*entry) + size);
	if (entry)
	{
		memcpy(entry->value, value, size);
	}
	return entry;
}

static void deallocate_entry(linked_list_entry *entry)
{
	free(entry);
}


void linked_list_create(linked_list *list, size_t value_size)
{
	list->first = list->last = 0;
	list->value_size = value_size;
}

void linked_list_destroy(linked_list *list)
{
	while (!linked_list_empty(list))
	{
		linked_list_pop_back(list);
	}
}

int linked_list_push_front(linked_list *list, const void *value)
{
	linked_list_entry *entry = allocate_entry(value, list->value_size);
	if (!entry)
	{
		return 0;
	}
	entry->previous = 0;
	entry->next = list->first;
	list->first = entry;
	return 1;
}

int linked_list_push_back(linked_list *list, const void *value)
{
	linked_list_entry *entry = allocate_entry(value, list->value_size);
	if (!entry)
	{
		return 0;
	}
	entry->next = 0;
	entry->previous = list->last;
	if (list->last)
	{
		list->last->next = entry;
	}
	list->last = entry;
	return 1;
}

const void *linked_list_front(const linked_list *list)
{
	return list->first->value;
}

const void *linked_list_back(const linked_list *list)
{
	return list->last->value;
}

void linked_list_pop_front(linked_list *list)
{
	linked_list_entry *new_first;

	assert(!linked_list_empty(list));

	new_first = list->first->next;
	deallocate_entry(list->first);
	list->first = new_first;
	if (!new_first)
	{
		list->last = 0;
	}
	new_first->previous = 0;
}

void linked_list_pop_back(linked_list *list)
{
	linked_list_entry *new_last;

	assert(!linked_list_empty(list));

	new_last = list->last->previous;
	deallocate_entry(list->last);
	list->last = new_last;
	if (!new_last)
	{
		list->first = 0;
	}
	new_last->next = 0;
}

int linked_list_empty(const linked_list *list)
{
	return list->first == 0;
}

void linked_list_clear(linked_list *list)
{
	size_t value_size = list->value_size;
	linked_list_destroy(list);
	linked_list_create(list, value_size);
}
