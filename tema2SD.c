#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tema2_SD.h"

Tree initTree(int value, int parentValue, int nrAddress, char** address) {
	Tree tree;
	tree = (Tree) malloc(sizeof(struct tree));
	// id-ul nodului
	tree->value = value; 
	
	// id-ul parintelui
	tree->parentValue = parentValue;
	
	// copiii 
	tree->parent = NULL;
	tree->child = NULL;
	tree->sibling = NULL;

	// numarul de adrese rezolvabile
	tree->nrAddress = nrAddress;

	// adresele rezolvabile
	tree->address = address;
	return tree;
}

int noOfKidsTree(Tree tree) 
{
	Tree tmp;
	int kids = 0;
	tmp = tree->child;
	while (tmp != NULL)
	{
		kids++;
		tmp = tmp->sibling;
	}
	return kids;
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


// gaseste copilul unui nod care are o anumita valoare
Tree getChildTree(Tree tree, int value) 
{

	Tree tmp;
	tmp = tree->child;

	while (tmp != NULL)
	{
		if (tmp->value == value)
			return tmp;
		tmp = tmp->sibling;
	}

	return NULL;
}


Tree deleteChildTree(Tree tree, int value) 
{
	Tree tmp, aux;
	tmp = getChildTree(tree, value);
	if (tmp != NULL)
	{
		if (tmp == tree->child)
		{
			tree->child = tmp->sibling;
			return tree;
		}

		aux = tree->child;
		while (aux->sibling != tmp)
			aux = aux->sibling;
		aux->sibling = tmp->sibling;
		return tree;
	}
	return tree;
}

 void traverseTree(Tree tree)
 {
 	while (tree != NULL)
 	{
 		printf("%d ", tree->value);
 		if (tree->child != NULL)
 			traverseTree(tree->child);
 		tree = tree->sibling;
 	}
 }
// }
// List initList(char* value) {
// 	List new_list;
// 	new_list = (List) malloc(sizeof(struct list));
// 	if (new_list == NULL)
// 	{
// 		printf("Alocarea dinamica a esuat");
// 		exit(-1);
// 	}
// 	new_list->value = strdup(value);
// 	new_list->next = NULL;
// 	return new_list;
// }

// List addLastList(List l, char* value) {
// 	List new_element = initList(value);
// 	List temp;
// 	if (l == NULL)
// 	{
// 		l = new_element;
// 		return l;
// 	}
// 	temp = l;
// 	while (temp->next != NULL)
// 		temp = temp->next;
// 	temp->next = new_element;
// 	return l;
// }

// List addItemList(List l, char* address, char* value) {
// 	List temp;

// 	temp = l;
// 	while (temp != NULL && strcmp(address, temp->value) != 0)
// 		temp = temp->next;

// 	List new_element = initList(value);
// 	new_element->next = temp->next;
// 	temp->next = new_element;

// 	return l;
// }

// List addFirstList(List l, char* value) {
// 	List new_element = initList(value);
// 	new_element->next = l;
// 	new_element->value = value;
// 	return new_element;
	
// }


void task1(Tree tree, int nrNodes)
{
	Tree node, tmp;
	FILE* g;
	g = fopen("tree.out", "w");
	int i;
	for (i = 0; i < nrNodes; i++)
	{
		node = findNode(tree, i);
		// afisez valoarea nodului i
		fprintf(g, "%d", node->value);
		//printf("%d ", node->value);
		tmp = node->child;
		while (tmp != NULL)
		{
			fprintf(g, " %d", tmp->value);
		//	printf("%d ", tmp->value);
			tmp = tmp->sibling;
		}

		if (i != nrNodes - 1)
			fprintf(g, "\n");
		//printf("\n");
	}
	fclose(g);
}

// void task2(Tree tree, int nrNodes)
// {
// 	Tree node, tmp;
// 	FILE *g;
// 	int i, j; 
// 	g = fopen("hierarchy.out", "w");
// 	for (i = 0; i < nrNodes; i++)
// 	{
// 		node = findNode(tree, i);
// 		fprintf(g, "%d", node->value);

// 		for (j = 0; j < node->nrAddress; j++)
// 			fprintf(g, " %s", node->address[j]);

// 		if (i != nrNodes - 1)
// 			fprintf(g, "\n");
// 	}
// }
int main()
{
	// citirea din fisier a datelor
	FILE* f;
	Tree tree = NULL, aux, par;
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
		for (i = 0 ; i < nrNodes; i++)
		{
			fscanf(f, "%d", &value);
			fscanf(f, "%d", &parentValue);
			fscanf(f, "%d", &nrAddress);
			address = (char**) malloc(nrAddress * sizeof(char*));
			for (j = 0; j < nrAddress; j++)
			{
				fscanf(f, "%s", buffer);
				address[j] = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
				strcpy(address[j], buffer); 
			}

			// nodul radacina e prima valoare din arbore
			 if (parentValue == -1)
			 	tree = initTree(value, parentValue, nrAddress, address);
	

			// pentru orice nod care nu e nod radacina, ii caut parintele
			// in arbore; daca il gasesc, leg nodul la parinte; daca nu-l
			// gasesc, bag nodul intr-o lista de asteptare
			
			if (value != 0)
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

			// dezaloc memoria pt vectorul de adrese corespunzator nodului i
			 // for (j = 0; j < nrAddress; j++)
			 // 	free(address[i]);
			 // free(address);
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
	task1(tree, nrNodes);
	fclose(f);

	// cerinta 2 - afisez id-urilor nodurilor + adresele rezolvabile asignate
	// f = fopen("hierarchy.in", "r");
	// task2(tree, nrNodes);
	// fclose(f);

///	traverseTree(tree);
	// dezaloc memoria
	//free(waitNodes);
	// inchid fisierul
	return 0;
}
