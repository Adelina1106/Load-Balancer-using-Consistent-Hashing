#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CircularDoublyLinkedList.h"
#include "utils.h"


doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list = malloc(sizeof(struct doubly_linked_list_t));
    DIE(list == NULL, "Could not allocate list.\n");

    list->head = NULL;
    list->size = 0;
    list->data_size = data_size;

    return list;
}


dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    dll_node_t *wanted_node;
    unsigned int i;

    if (n == 0) {
        wanted_node = list->head;
    } else if (n == dll_get_size(list) - 1) {
        wanted_node = list->head->prev;
    } else {
        wanted_node = list->head;
        for (i = 0; i < n; i++) {
         wanted_node = wanted_node->next;
        }
    }

    return wanted_node;
}


void
dll_add_nth_node(doubly_linked_list_t* list, unsigned int n, const void* data)
{
    struct dll_node_t* new_node, *curr;

    new_node = (struct dll_node_t*)malloc(sizeof(struct dll_node_t));
    DIE(new_node == NULL, "Could not allocate node.\n");

    new_node->data = malloc(list->data_size*sizeof(void*));
    DIE(new_node->data == NULL, "Could not allocate node data.\n");

    memcpy(new_node->data, data, list->data_size);

    if (dll_get_size(list) == 0) {
        list->head = new_node;
        new_node->next = new_node;
        new_node->prev = new_node;
        list->size++;
    } else if (dll_get_size(list) == 1 && n == 0) {
        new_node->next = list->head;
        new_node->prev = list->head->prev;
        list->head->prev = new_node;
        list->head->next = new_node;
        list->head = new_node;
        list->size++;
    } else if (dll_get_size(list) > 1 && n == 0) {
        new_node->next = list->head;
        new_node->prev = list->head->prev;
        list->head->prev->next = new_node;
        list->head->prev = new_node;
        list->head = new_node;
        list->size++;
    } else if (dll_get_size(list) != 0 && n != 0 && n < list->size) {
        curr = dll_get_nth_node(list, n);
        new_node->next = curr;

        new_node->prev = curr->prev;
        curr->prev->next = new_node;
        curr->prev = new_node;
        list->size++;
    } else if (n >= dll_get_size(list)) {
        list->head->prev->next = new_node;
        new_node->next = list->head;
        new_node->prev = list->head->prev;
        list->head->prev = new_node;
        list->size++;
    }
}


dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, unsigned int n)
{
    struct dll_node_t *curr, *tail;

    tail = list->head->prev;

    if (dll_get_size(list) == 1) {
        curr = list->head;
        list->head = NULL;
        list->size--;
    } else if (n == 0 && dll_get_size(list) > 1) {
        curr = list->head;
        tail->next = list->head->next;
        list->head->next->prev = tail;
        list->head = list->head->next;
        list->size--;
    } else if (n < dll_get_size(list) && dll_get_size(list) != 1) {
        curr = dll_get_nth_node(list, n);
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        list->size--;
    } else {
        curr = dll_get_nth_node(list, list->size - 1);
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        list->size--;
    }


    return curr;
}


unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return(list->size);
}


void
dll_free(doubly_linked_list_t** pp_list)
{
    struct dll_node_t *node;
    node = (*pp_list)->head;

    if (dll_get_size(*pp_list) != 1 && dll_get_size(*pp_list) != 0) {
       while (node->next != (*pp_list)->head) {
           node = node->next;
           free(node->prev->data);
           free(node->prev);
       }
       free(node->data);
       free(node);
    } else if (dll_get_size(*pp_list) == 1){
        free(node->data);
        free(node);
    }

    free(*pp_list);
}


void
dll_print_int_list(doubly_linked_list_t* list)
{
    struct dll_node_t *curr;

    if (dll_get_size(list) != 0) {
        curr = list->head;
        do {
            printf("%d ", *((int*)curr->data));
            curr = curr->next;
        } while (curr != list->head);
    } else if (dll_get_size(list) == 0) {
        printf("Lista goala.\n");
    }

    printf("\n");
}


void
dll_print_string_list(doubly_linked_list_t* list)
{
    struct dll_node_t *curr;

    if (dll_get_size(list) != 0) {
        curr = list->head;
        do {
            printf("%s ", (char*)curr->data);
            curr = curr->next;
        } while (curr != list->head);
    } else if (dll_get_size(list) == 0) {
        printf("Lista goala.\n");
    }

    printf("\n");
}


void
dll_print_ints_left_circular(dll_node_t* start)
{
    struct dll_node_t *curr;

    curr = start;

     do {
            printf("%d ", *((int*)curr->data));
            curr = curr->prev;
        } while (curr != start);

    printf("\n");
}


void
dll_print_ints_right_circular(dll_node_t* start)
{
    struct dll_node_t *curr;

    curr = start;

     do {
            printf("%d ", *((int*)curr->data));
            curr = curr->next;
        } while (curr != start);

    printf("\n");
}
