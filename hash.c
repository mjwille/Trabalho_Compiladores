#include <stdio.h>
#include <string.h>
#include "hash.h"

HASH_NODE *HASH_TABLE[HASH_SIZE];

void hashInit()
{
	int i;
	for(i=0; i<HASH_SIZE; i++)
	{
		HASH_TABLE[i] = NULL;
	}
}


int hashAddress(char *text)
{
	int i, addr = 1;
	for(i=0; i<strlen(text); i++)
	{
		// Hash Function (entre 1 e 997)
		addr = (addr * text[i]) % HASH_SIZE + 1;
	}
	return addr-1;   // Corrige para ficar entre 0 e 996
}


HASH_NODE* hashFind(char *text)
{
	return NULL;   //TODO: próximas etapas do trabalho, hashInsert nos devidos locais do scanner, better comments
}


HASH_NODE* hashInsert(char *text)
{
	int addr = hashAddress(text);
	HASH_NODE *node = (HASH_NODE*) calloc(1, sizeof(HASH_NODE));

	node->type = 1;
	node->text = (char*) calloc(strlen(text)+1, sizeof(char));
	strcpy(node->text, text);
	node->next = HASH_TABLE[addr];

	HASH_TABLE[addr] = node;

	return node;
}


void hashPrint()
{
	int i;
	HASH_NODE *node;
	for(i=0; i<HASH_SIZE; i++)
		for(node=HASH_TABLE[i]; node; node=node->next)
			printf("HASH_TABLE[%d] has => type (%d), text (%s)\n", i, node->type, node->text);
}