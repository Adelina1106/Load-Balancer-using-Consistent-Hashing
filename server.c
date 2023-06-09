#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "Hashtable.h"

server_memory* init_server_memory() {
	server_memory *server = malloc(sizeof(server_memory *));
	server->hashtable = ht_create(
								2000,
								hash_function_string,
								compare_function_strings);
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->hashtable, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->hashtable, key);
}

char* server_retrieve(server_memory* server, char* key) {
	char *value;

	value = ht_get(server->hashtable, key);

	return value;
}

void free_server_memory(server_memory* server) {
	ht_free(server->hashtable);
	free(server);
}
