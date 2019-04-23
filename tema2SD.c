#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "tema2_SD.h"

// #define SIZE 200

// void drawTrieHelper(Tree trie, FILE* stream) {
// 	Tree tmp;
// 	if (trie == NULL) {
// 		return;
// 	}
// 	// if (trie->end) {
// 		// fprintf(stream, "    %ld[label=\"%d\", fillcolor=red]\n", (intptr_t) trie, trie->value);
// 	// } else {
// 		fprintf(stream, "    %ld[label=\"%d\", fillcolor=blue]\n", (intptr_t) trie, trie->value);
// 	// }
// 	tmp = trie->child;

// 	while (tmp != NULL) {
// 		fprintf(stream, "    %ld -> %ld \n", (intptr_t) trie, (intptr_t) tmp);
// 		drawTrieHelper(tmp, stream);
// 		tmp = tmp->sibling;
// 	}
// }

// void drawTrie(Tree trie, char *fileName) {
// 	FILE* stream = fopen("test.dot", "w");
// 	char buffer[SIZE];
// 	fprintf(stream, "digraph TRIE {\n");
// 	fprintf(stream, "    node [fontname=\"Arial\", shape=circle, style=filled, fillcolor=yellow];\n");
// 	if (!trie)
// 		fprintf(stream, "\n");
// 	else if (!trie->child)
// 		fprintf(stream, "    %ld [label=\"%d\"];\n", (intptr_t) trie, trie->value);
// 	else
// 		drawTrieHelper(trie, stream);
// 	fprintf(stream, "}\n");
// 	fclose(stream);
// 	sprintf(buffer, "dot test.dot | neato -n -Tpng -o %s", fileName);
// 	system(buffer);
// }


Tree initTree(int value, int parentValue, int nrAddress, char** address) 
{
	Tree tree;
	tree = (Tree) malloc(sizeof(struct tree));
	// id-ul nodului
	tree->value = value; 
	
	// id-ul parintelui
	tree->parentValue = parentValue;
	
	// legaturi spre parinte, copil si frati
	tree->parent = NULL;
	tree->child = NULL;
	tree->sibling = NULL;

	// numarul de adrese rezolvabile
	tree->nrAddress = nrAddress;

	// adresele rezolvabile
	tree->address = address;
	return tree;
}


// parcurgerea arborelui pe nivel si gasirea unei valori date
Tree findNode(Tree tree, int value)
{
	if (tree == NULL)
		return NULL;
	if (tree->value == value)
		return tree;
	
	if (tree->child != NULL && tree->sibling != NULL)
	{
		Tree tmp = NULL;
		tmp = findNode(tree->child, value);
		if (tmp == NULL)
			return findNode(tree->sibling, value);
	}

	if (tree->child != NULL)
		return findNode(tree->child, value);

	if (tree->sibling != NULL)
		return findNode(tree->sibling, value);
	return NULL;
}

Tree insertChildTree(Tree tree, int value, int parentValue, 
			int nrAddress, char** address) 
{
	Tree tmp;
	if (tree == NULL)
	{
		tree = initTree(value, parentValue, nrAddress, address);
		return tree;
	}

	else
	{
		Tree aux = NULL, par = NULL;
		aux = initTree(value, parentValue, nrAddress, address);
		par = findNode(tree, parentValue);
		if (par->child == NULL)
		{
			par->child = aux;
			aux->parent = par;
		}
		else
		{
			tmp = par->child;
			while (tmp->sibling != NULL)
			{
				tmp = tmp->sibling;
			}
			tmp->sibling = aux;
			aux->parent = par;
		}
	}
	return tree;
}

void traverseTree(Tree tree, Tree root)
 {
 	int i, j, ok;
 	while (tree != NULL)
 	{
 		for (i = 0; i < tree->nrAddress; i++)
 		{
 			// inca nu am gasit adresa i din tree printre adresele lui root
 			ok = 0; 
 			for (j = 0; j < root->nrAddress && ok == 0; j++)
 				if (strcmp(root->address[j], tree->address[i]) == 0)
 					// am gasit adresa i printre adresele lui root
 					ok = 1;
 			// adaug adresa i la adresele lui root
 			if (!ok) 
 			{
 		     	root->nrAddress++;
 			 	root->address = (char**)realloc(root->address, 
 			 			root->nrAddress * sizeof(char*));
 				root->address[root->nrAddress - 1] = strdup(tree->address[i]);
 			 }

 		}
 		if (tree->child != NULL)
 			traverseTree(tree->child, root);
 		tree = tree->sibling;
 	}
 }

Tree deleteChildTree(Tree tree, int idServer, int nrUsers, int* server)
{
	Tree nodeDelete = NULL, tmp = NULL;
	int j;
	nodeDelete = findNode(tree, idServer);
	if(nodeDelete != NULL)
	{
		for (j = 0; j < nrUsers; j++)
			if (server[j] == idServer)
		 		server[j] = nodeDelete->parent->value;

		tmp = nodeDelete->child;
		while (tmp != NULL)
		{
			tmp->parent = nodeDelete->parent;
			tmp = tmp->sibling;
		}

		// daca nodul de sters e copil
		if (nodeDelete == nodeDelete->parent->child)
		{
			tmp = nodeDelete->parent->child;
			while (tmp->sibling != NULL)
				tmp = tmp->sibling;

			tmp->sibling = nodeDelete->child;
			nodeDelete->parent->child = nodeDelete->sibling; 
		}

		// daca nodul de sters e sibling
		else
		{
			tmp = nodeDelete->parent->child;
			while(tmp->sibling->value != idServer) tmp = tmp->sibling;
			Tree tmp2 = nodeDelete->parent->child;
			while (tmp2->sibling != NULL)
				tmp2 = tmp2->sibling;
			tmp2->sibling = nodeDelete->child;
			tmp->sibling = nodeDelete->sibling;
		}

		// dezaloc memoria alocata pentru fiecare adresa
		for (j = 0; j < nodeDelete->nrAddress; j++)
			free(nodeDelete->address[j]);

		// dezaloc memoria vectorului de adrese
		free(nodeDelete->address);
				
		// sterg nodul din arbore 
		free(nodeDelete);
	}
	return tree;
}

Tree freeTree(Tree tree, int nrNodes, int nrUsers, int *server)
{
	int i;
	Tree tmp;
	for (i = 1; i < nrNodes; i++)
	{
		tmp = findNode(tree, i);
		if (tmp != NULL)
			tree = deleteChildTree(tree, i, nrUsers, server);
	}

	// raman cu un singur nod: nodul radacina
    for (i = 0; i < tree->nrAddress; i++)
	 		free(tree->address[i]);
	free(tree->address);
	free(tree);
	tree = NULL;
	return tree;
}

void task1_printNodes(Tree tree, int nrNodes)
{
	Tree node, tmp;
	FILE* g;
	g = fopen("tree.out", "w");
	int i;
	for (i = 0; i < nrNodes; i++)
	{
		node = findNode(tree, i);
		fprintf(g, "%d", node->value);
		tmp = node->child;
		while (tmp != NULL)
		{
			fprintf(g, " %d", tmp->value);
			tmp = tmp->sibling;
		}

		if (i != nrNodes - 1)
			fprintf(g, "\n");
	}
	fclose(g);
}

void task2(Tree tree, int nrNodes)
{
	Tree node, root;
	FILE *g;
	int i, j; 

	for (i = 0; i < nrNodes; i++)
	{
		node = findNode(tree, i);
		root = node;
		if (node->child != NULL)
			traverseTree(node->child, root);

	}
	g = fopen("hierarchy.out", "w");
	for (i = 0; i < nrNodes; i++)
	{
		node = findNode(tree, i);
		fprintf(g, "%d", node->value);
		for (j = 0; j < node->nrAddress; j++)
			fprintf(g, " %s", node->address[j]);
		if (i != nrNodes - 1)
			fprintf(g, "\n");
	}
	fclose(g);
}

void task34(Tree tree, int nrNodes)
{
	FILE *g, *file;
	int nrQueries, i, j, user, ok, ok2, idServer;
	int nrUsers, *server;
	char command, addr[15];
	Tree node, tmp;
	
	// memorarea utilzatorilor
	g = fopen("users.in", "r");
	fscanf(g, "%d", &nrUsers);
	server = (int*) malloc((nrUsers + 1) * sizeof(int));
	for (i = 0; i < nrUsers; i++)
	{
		fscanf(g, "%d", &user);
		fscanf(g, "%d", &server[user]);
	}
	fclose(g);

	g = fopen("queries.in", "r");
	file = fopen("queries.out", "w");
	fscanf(g, "%d", &nrQueries);
	for (i = 0; i < nrQueries; i++)
	{
		fgetc(g);
		fscanf(g, "%c", &command);
		if (command == 'q')
		{
			fscanf(g, "%d", &user);
			fscanf(g, "%s", addr);
		}
		else
			fscanf(g, "%d", &idServer);

		if (command == 'f')
			tree = deleteChildTree(tree, idServer, nrUsers, server);

		if (command == 'q')
		{
			node = findNode(tree, server[user]);
			// pp ca adresa addr nu se gaseste in memoria nodului node
			ok = 0;
			if(!node) 
				continue;
			for (j = 0; j < node->nrAddress && ok == 0; j++)
				if (strcmp(node->address[j], addr) == 0)
				{
					fprintf(file, "%d", server[user]);
					ok = 1;
				}

			// addr nu se gaseste in memoria nodului node
			if (ok == 0)
			{
				fprintf(file, "%d", server[user]);
				
				// pp ca inca nu am gasit un parinte care contine addr 
				ok2 = 0; 
				tmp = node->parent;

				// caut din parinte in parinte primul nod care contine addr
				while (ok2 == 0)
				{
					fprintf(file, " %d", tmp->value);
					for (j = 0; j < tmp->nrAddress && ok2 == 0; j++)
						if (strcmp(tmp->address[j], addr) == 0)
							ok2 = 1;


					tmp->nrAddress++;
 				 	tmp->address = (char**)realloc(tmp->address, 
 				 				tmp->nrAddress * sizeof(char*));
 				 	tmp->address[tmp->nrAddress - 1] = strdup(addr);
					tmp = tmp->parent;
				}
				// adaug addr in memoria serverului la care e legat user
				node->nrAddress++;
 				node->address = (char**)realloc(node->address, 
 						node->nrAddress * sizeof(char*));
 				node->address[node->nrAddress - 1] = strdup(addr);
			}
			fprintf(file, "\n");
		}
	}
	fclose(file);
	fclose(g);

	tree = freeTree(tree, nrNodes, nrUsers, server);
	free(server);

}

int main()
{
	// citirea din fisier a datelor
	FILE* f;
	Tree tree = NULL, aux = NULL;
	int nrNodes, value, parentValue, nrAddress, i, j, nrmax = 5, ok;
	char buffer[BUFMAX];

	// vector alocat dinamic in care tin nodurile pe care le citesc si care
	// inca nu au parinte in arbore
	Tree *waitNodes;
	waitNodes = (Tree*)malloc(nrmax * sizeof(Tree));
	int waitNr = 0;
	char **address;
	f = fopen("tree.in", "r");
	if (f == NULL)
		printf("Eroare! Nu s-a putut deschide fisierul!");
	else
	{
		// citesc numarul de noduri
		fscanf(f, "%d", &nrNodes);
		// citesc fiecare nod in parte, parintele lui, numarul de adrese 
		// rezolvabile si adresele propriu-zise
		for (i = 0; i < nrNodes; i++)
		{
			fscanf(f, "%d", &value);
			fscanf(f, "%d", &parentValue);
			fscanf(f, "%d", &nrAddress);
			address = (char**) malloc(nrAddress * sizeof(char*));
			for (j = 0; j < nrAddress; j++)
			{
				fscanf(f, "%s", buffer);
				address[j] = strdup(buffer);
			}

			// nodul radacina e prima valoare din arbore
			 if (parentValue == -1)
			 	tree = initTree(value, parentValue, nrAddress, address);
	

			// pentru orice nod care nu e nod radacina, ii caut parintele
			// in arbore; daca il gasesc, leg nodul la parinte; daca nu-l
			// gasesc, bag nodul intr-o lista de asteptare
			
			if (value != 0)
			{
				if (findNode(tree, parentValue) != NULL)
					tree = insertChildTree(tree, value, parentValue, nrAddress, 
																	address);

				else
				{
					aux = initTree(value, parentValue, nrAddress, address);
					if (waitNr == nrmax)
					{
						nrmax = nrmax * 2;
						waitNodes = (Tree*)realloc(waitNodes, nrmax * sizeof(Tree));
					}
					waitNodes[waitNr] = aux;
					waitNr++;
				}
			}
		 }
	}

	// presupun ca toate nodurile care nu aveau parinte in arbore in urma  
	// citirii au fost introduse in arbore in urma adaugarii altor noduri
	// OBS: setez valoarea parintelui nodului care si-a gasit loc in arbore 
	// in urma adaugarii altor noduri ca fiind -10
	
	ok = 0;
	while (ok == 0)
	{
		// pp ca toate nodurile au un loc in arbore
		ok = 1;
		for (j = 0; j < waitNr; j++)
		{
			if (waitNodes[j]->parentValue != -10)
				ok = 0;

			// daca nodul are parinte in arbore
			if (findNode(tree, waitNodes[j]->parentValue) != NULL)
			{
				// adaug nodul in arbore
				tree = insertChildTree(tree, waitNodes[j]->value, 
						waitNodes[j]->parentValue, waitNodes[j]->nrAddress, 
						waitNodes[j]->address);

				// scot nodul din lista de noduri care nu si-au gasit parinte
				waitNodes[j]->parentValue = -10;
			}
		}
	}

	// cerinta 1 - afisez id-urile nodurilor + copiii acestora
	task1_printNodes(tree, nrNodes);

	// cerinta 2 - afisez id-urilor nodurilor + adresele rezolvabile asignate
	task2(tree, nrNodes);
	fclose(f);

	// cerinta 3 - rezolv cererile de adrese de la utilizatori 
	//drawTrie(tree, "tree.png");
	task34(tree, nrNodes);

	// dezaloc memoria
	for (i = 0; i < waitNr; i++)
	  	free(waitNodes[i]);
	free(waitNodes);
	return 0;
}
