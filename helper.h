#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFMAX 10000

typedef struct tree 
{
	// id-ul nodului
	int value;
	
	// id-ul parintelui
	int parentValue;
	
	// legatura catre nodul parinte - inca nu stiu daca am nevoie
	struct tree *parent;

	// copiii nodului
	struct tree *child;
	struct tree *sibling;
	
	// numarul de adrese rezolvabila
	int nrAddress;
	
	// vectorul de adrese
	char **address;
} *Tree;


Tree initTree(int value, int parentValue, int nrAddress, char** address);
int noOfKidsTree(Tree tree);
Tree insertChildTree(Tree tree, int value, int parentValue, int nrAddress, 
							char** address);
Tree deleteChildTree(Tree tree, int value);
Tree findNode(Tree tree, int value);
