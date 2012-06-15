#ifndef LINKED_LIST_H
#define LINKED_LIST_H


#include <stddef.h>


typedef struct linked_list_entry linked_list_entry;

typedef struct linked_list
{
	size_t value_size;
	linked_list_entry *first, *last;
}
linked_list;


void linked_list_create(linked_list *list, size_t value_size);
void linked_list_destroy(linked_list *list);
int linked_list_push_front(linked_list *list, const void *value);
int linked_list_push_back(linked_list *list, const void *value);
const void *linked_list_front(const linked_list *list);
const void *linked_list_back(const linked_list *list);
void linked_list_pop_front(linked_list *list);
void linked_list_pop_back(linked_list *list);
int linked_list_empty(const linked_list *list);
void linked_list_clear(linked_list *list);


#endif
