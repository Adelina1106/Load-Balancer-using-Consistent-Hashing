#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "Hashtable.h"
#include "CircularDoublyLinkedList.h"
#include "LinkedList.h"
#include "utils.h"

struct load_balancer {
    server_memory **servers;  // array-ul de servere
    int servers_number;  // nr de servere
    doubly_linked_list_t *hashring;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// functie ce initializeaza load_balancer
load_balancer* init_load_balancer() {
    load_balancer *main_server = malloc(sizeof(load_balancer));
    DIE(main_server == NULL, "Could not allocate.\n");
    main_server->hashring = dll_create(sizeof(struct info_h));
    main_server->servers = calloc(99999, sizeof(server_memory *));
    DIE(main_server->servers == NULL, "Could not allocate.\n");

    main_server->servers_number = 0;

    return main_server;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    unsigned int hash_key = hash_function_key(key), size;
    dll_node_t *curr = main->hashring->head;
    int server, id;

    char *product = loader_retrieve(main, key, &server);

    *server_id = -1;

    // cazul in care produsul nu se afla deja in sistem
    if (product == NULL) {
        do {
            if (hash_key <= ((info_h*)curr->data)->tag) {
                *server_id = ((info_h*)curr->data)->id_server;
                id = ((info_h*)curr->data)->id_server;
                server_store(main->servers[id], key, value);
                size = ((info_h*)curr->data)->keys_size;
                strcpy(((struct info_h*)curr->data)->keys[size] , key);
                ((info_h*)curr->data)->keys_size++;
                break;
            }
            curr = curr->next;
        } while (curr != main->hashring->head);

        // daca hash-ul cheii este mai mare decat hash-ul tuturor replicilor
        // se retine produsul in serverul continut de prima
        // replica de pe hashring
        if (*server_id == -1) {
            curr = main->hashring->head;
            *server_id = ((info_h*)curr->data)->id_server;
            id = ((info_h*)curr->data)->id_server;
            server_store(main->servers[id], key, value);
            size = ((info_h*)curr->data)->keys_size;
            strcpy(((info_h*)curr->data)->keys[size] , key);
            ((info_h*)curr->data)->keys_size++;
        }

    } else {  // cazul cand produsul se afla deja in sistem
              // se returneaza serverul pe care se afla
        do {
           for (unsigned int i = 0; i < ((info_h*)curr->data)->keys_size; i++) {
                if (strcmp(key , ((info_h*)curr->data)->keys[i]) == 0) {
                    *server_id = ((info_h*)curr->data)->id_server;
                    break;
                }
            }
            curr = curr->next;
        } while (curr != main->hashring->head);
    }
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
    char *value;
    unsigned int id;
    dll_node_t *curr = main->hashring->head;

    // se cauta key in fiecare server continut de replicile
    // de pe hashring si se returneaza serverul ce o contine
    do {
        id = ((info_h*)curr->data)->id_server;
        value = server_retrieve(main->servers[id], key);
        if (value != NULL) {
            *server_id = ((info_h*)curr->data)->id_server;
            return value;
        }

        curr = curr->next;
    } while (curr != main->hashring->head);

	return value;
}

// functie ce sorteaza hashring-ul dupa etichetele replicilor
void sort_hashring(doubly_linked_list_t *list) {
    dll_node_t *curr = list->head, *index = NULL;
    int aux, id;
    unsigned int i, aux2;
    char aux1[500][100], temp1[500][100], temp2[500][100];
    for (int i = 0; i < 500; i++) {
        strcpy(aux1[i], "NULL");
        strcpy(temp1[i], "NULL");
        strcpy(temp2[i], "NULL");
    }

    do {
        index = curr->next;
        while (index != list->head) {
            if (((info_h*)curr->data)->tag > ((info_h*)index->data)->tag) {
                // se interschimba etichetele
                aux2 = ((info_h*)curr->data)->tag;
                ((info_h*)curr->data)->tag = ((info_h*)index->data)->tag;
                ((info_h*)index->data)->tag = aux2;

                // se interschimba id-urile serverelor
                aux = ((info_h*)curr->data)->id_server;
                id = ((info_h*)index->data)->id_server;
                ((info_h*)curr->data)->id_server = id;
                ((info_h*)index->data)->id_server = aux;

                // se interschimba array-urile cu cheile
                // obiectelor salvate de fiecare replica
                unsigned int curr_size = ((info_h*)curr->data)->keys_size;
                unsigned int index_size = ((info_h*)index->data)->keys_size;

                if (curr_size == index_size) {
                    for (i = 0; i < curr_size; i++) {
                        strcpy(temp1[i], ((info_h*)curr->data)->keys[i]);
                        strcpy(temp2[i], ((info_h*)index->data)->keys[i]);

                        strcpy(aux1[i] , temp1[i]);
                        strcpy(((info_h*)curr->data)->keys[i] , temp2[i]);
                        strcpy(((info_h*)index->data)->keys[i] , aux1[i]);
                    }
                } else if (curr_size > index_size) {
                    for (i = 0; i < curr_size; i++) {
                        strcpy(temp1[i], ((info_h*)curr->data)->keys[i]);
                        strcpy(temp2[i], ((info_h*)index->data)->keys[i]);

                        strcpy(aux1[i] , temp1[i]);
                        strcpy(((info_h*)curr->data)->keys[i] , temp2[i]);
                        strcpy(((info_h*)index->data)->keys[i] , aux1[i]);
                    }

                    ((info_h*)curr->data)->keys_size = index_size;
                    ((info_h*)index->data)->keys_size = curr_size;

                } else if (curr_size < index_size) {
                     for (i = 0; i < index_size; i++) {
                        strcpy(temp1[i], ((info_h*)curr->data)->keys[i]);
                        strcpy(temp2[i], ((info_h*)index->data)->keys[i]);

                        strcpy(aux1[i] , temp1[i]);
                        strcpy(((info_h*)curr->data)->keys[i] , temp2[i]);
                        strcpy(((info_h*)index->data)->keys[i] , aux1[i]);
                    }

                    ((info_h*)curr->data)->keys_size = index_size;
                    ((info_h*)index->data)->keys_size = curr_size;
                }
            }
            index = index->next;
        }
        curr = curr->next;
    } while (curr->next != list->head);
}


void loader_add_server(load_balancer* main, int server_id) {
    unsigned int tag0, tag1, tag2, hash_tag0, hash_tag1;
    unsigned int hash_tag2, id, size, tag, tag_n, hash_next;
    int nr = main->servers_number;
    dll_node_t *curr, *next;
    struct info_h info;
    char *val;
    char aux[500][100];
    for (int i = 0; i < 500; i++) {
        strcpy(aux[i], "NULL");
    }

    // se initializeaza noul server
    main->servers[server_id] = init_server_memory();

    main->servers_number++;

    // se calculeaza etichetele replicilor hashring-ului
    tag0 = server_id;
    tag1 = 100000 + server_id;
    tag2 = 200000 + server_id;

    hash_tag0 = hash_function_servers(&tag0);
    hash_tag1 = hash_function_servers(&tag1);
    hash_tag2 = hash_function_servers(&tag2);

    // se initilizeaza fiecare element din array-ul de keys cu "NULL"
    for (int i = 0; i < 500; i++) {
        strcpy(info.keys[i], "NULL");
    }

    // se adauga in hashring cele 3 replici ale unui server
    info.keys_size = 0;
    info.id_server = server_id;
    info.tag = hash_tag0;
    dll_add_nth_node(main->hashring, main->hashring->size, &info);

    info.tag = hash_tag1;
    dll_add_nth_node(main->hashring, main->hashring->size, &info);

    info.tag = hash_tag2;
    dll_add_nth_node(main->hashring, main->hashring->size, &info);

    // se sorteaza hashring-ul dupa etichete
    sort_hashring(main->hashring);

    curr = main->hashring->head;
    next = curr->next;

    // se rebalanseaza obiectele in functie de noul server
    do {
        if (((info_h*)curr->data)->id_server == server_id && nr > 1) {
            while (((info_h*)next->data)->id_server == server_id) {
                next = next->next;
            }
            id = ((info_h *)next->data)->id_server;

            unsigned int key_ob_size = ((info_h*)next->data)->keys_size;

            for (unsigned int i = 0; i < key_ob_size; i++) {
                // se verifica daca replica serverului urmator
                // celui nou adaugat retine obiecte
                if (strcmp(((info_h*)next->data)->keys[i] , "NULL") != 0) {
                    strcpy(aux[i], ((info_h*)next->data)->keys[i]);
                    hash_next = hash_function_key(aux[i]);

                    if (next != main->hashring->head) {
                        tag = ((info_h*)curr->data)->tag;
                        tag_n = ((info_h*)next->data)->tag;

                        // conditia pentru a muta obiectul pe serverul nou
                        if (tag > hash_next && tag < tag_n) {
                            val = server_retrieve(main->servers[id], aux[i]);
                            // se retine obiectul pe serverul nou
                            server_store(main->servers[server_id], aux[i], val);
                            size = ((info_h*)curr->data)->keys_size;
                            strcpy(((info_h*)curr->data)->keys[size], aux[i]);
                            ((info_h*)curr->data)->keys_size++;
                            // se sterge obiectul de pe serverul vechi
                            server_remove(main->servers[id], aux[i]);
                            strcpy(((info_h*)next->data)->keys[i], "NULL");
                        }
                        if (((info_h*)next->data)->tag < hash_next) {
                            val = server_retrieve(main->servers[id], aux[i]);
                            server_store(main->servers[server_id], aux[i], val);
                            size = ((info_h*)curr->data)->keys_size;
                            strcpy(((info_h*)curr->data)->keys[size], aux[i]);
                            ((info_h*)curr->data)->keys_size++;
                            server_remove(main->servers[id], aux[i]);
                            strcpy(((info_h*)next->data)->keys[i] , "NULL");
                        }
                    } else {
                        // cazul cand o replica a serverului nou este
                        // pe ultima pozitie in hashring
                        tag = ((info_h*)curr->data)->tag;
                        tag_n = ((info_h*)next->data)->tag;

                        // conditia pentru a muta obiectul pe serverul nou
                        if (tag > hash_next && hash_next > tag_n) {
                            val = server_retrieve(main->servers[id], aux[i]);
                            server_store(main->servers[server_id], aux[i], val);
                            size = ((info_h*)curr->data)->keys_size;
                            strcpy(((info_h*)curr->data)->keys[size], aux[i]);
                            ((info_h*)curr->data)->keys_size++;
                            server_remove(main->servers[id], aux[i]);
                            strcpy(((info_h*)next->data)->keys[i] , "NULL");
                        }
                    }
                }
            }
        }
        curr = curr->next;
        next = curr->next;
    } while (curr != main->hashring->head);
}


void loader_remove_server(load_balancer* main, int server_id) {
    unsigned int i, cnt = -1, size_next, h_size = main->hashring->size;
    char *value;
    char aux[500][100];
    for (int i = 0; i < 500; i++) {
        strcpy(aux[i], "NULL");
    }
    dll_node_t *curr, *next, *removed;
    curr = main->hashring->head;
    next = curr->next;

    for (i = 0; i < h_size; i++) {
        cnt++;
        if (((info_h*)curr->data)->id_server == server_id) {
              while (((info_h*)next->data)->id_server == server_id) {
                next = next->next;
            }
            int id_next = ((info_h*)next->data)->id_server;
            unsigned int size = ((info_h*)curr->data)->keys_size;

            for (unsigned int i = 0; i < size; i++) {
                // daca replica serverului retine obiecte, ele vor fi
                // mutate pe serverul urmator
                if (strcmp(((info_h*)curr->data)->keys[i] , "NULL") != 0) {
                    strcpy(aux[i], ((info_h*)curr->data)->keys[i]);
                    value = server_retrieve(main->servers[server_id], aux[i]);
                    server_store(main->servers[id_next], aux[i], value);
                    size_next = ((info_h*)next->data)->keys_size;
                    strcpy(((info_h*)next->data)->keys[size_next], aux[i]);
                    ((info_h*)next->data)->keys_size++;
                    server_remove(main->servers[server_id], aux[i]);
                }
            }
            curr = curr->next;
            next = curr->next;

            // se sterg replicile serverului din hashring
            removed = dll_remove_nth_node(main->hashring, cnt);

            cnt--;

            free(removed->data);
            free(removed);

            } else {
                curr = curr->next;
                next = curr->next;
            }
    }
}

// se elibereaza memoria utilizata
void free_load_balancer(load_balancer* main) {
    unsigned int i;

    for (i = 0; i < 99999; i++) {
        if (main->servers[i] != NULL) {
            free_server_memory(main->servers[i]);
        } else {
            free(main->servers[i]);
        }
    }

    free(main->servers);
    dll_free(&main->hashring);
    free(main);
}
