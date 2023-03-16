#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"
#include "utils.h"

linked_list_t*
ll_create(unsigned int data_size) {
    struct linked_list_t* list = malloc(sizeof(struct linked_list_t));
    DIE(list == NULL, "Could not allocate list.\n");

    list->head = NULL;
    list->size = 0;
    list->data_size = data_size;

    return list;
}


void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    unsigned int i;
    struct ll_node_t* new_node, *curr;

    new_node = (struct ll_node_t*)malloc(sizeof(struct ll_node_t));
    DIE(new_node == NULL, "Could not allocate node.\n");

    new_node->data = malloc(list->data_size*sizeof(void*));
    DIE(new_node->data == NULL, "Could not allocate node data.\n");

    memcpy(new_node->data, new_data, list->data_size);

    if (list == NULL)
        printf("Empty list.\n");

    if (n == 0) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;
    } else if (n >= ll_get_size(list) && n != 0 && list->head != NULL) {
        curr = list->head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
        list->size++;
    } else if (n >= ll_get_size(list) && n != 0 && list->head == NULL) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;

    } else {
        curr = list->head;
        for (i = 0; i < n - 1; i++) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
        list->size++;
    }
}


ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    unsigned int i;
    struct ll_node_t *prev, *node;

    if (list == NULL || list->head == NULL)
        printf("Empty list.\n");

    if (n >= ll_get_size(list) - 1 && n != 0) {
        node = list->head->next;
	    prev = list->head;
	    while (node->next != NULL) {
		    prev = node;
		    node = node->next;
	    }
	    prev->next = NULL;
	    list->size--;
    } else if (n == 0) {
        node = list->head;
        list->head = node->next;
        node->next = NULL;
        list->size--;

    } else {
        node = list->head->next;
	    prev = list->head;
        for (i = 0; i < n - 1; i++) {
            prev = node;
            node = node->next;
        }
        prev->next = node->next;
        node->next = NULL;
    }

    return node;
}


unsigned int
ll_get_size(linked_list_t* list)
{
    return(list->size);
}


void
ll_free(linked_list_t** pp_list)
{
    struct ll_node_t *node, *next_node;
    node = (*pp_list)->head;

    while (node != NULL) {
        next_node = node->next;
        free(node->data);
        free(node);
        node = next_node;
    }

    free(*pp_list);
    *pp_list = NULL;
}


void
ll_print_int(linked_list_t* list)
{
    struct ll_node_t *curr;

    curr = list->head;

        while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
        }

    printf("\n");
}


void
ll_print_string(linked_list_t* list)
{
    struct ll_node_t *curr;

    curr = list->head;

        while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
        }

    printf("\n");
}
