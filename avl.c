#include "avl.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum Position { LEFT, RIGHT } Position;

int max(int x, int y) {
	return (x < y) ? y : x;
}

int abs(int x) {
	return (x < 0) ? -x : x;
}

/************* Utilities Declarations ****************/
void paramCalculator(AVLNodePtr tnode);
int node_size_Calculator(AVLNodePtr tnode);

AVLNodePtr balance(AVLNodePtr tnode);
AVLNodePtr rootBalance(AVLNodePtr tnode, int heavySide, int lightSide);
AVLNodePtr rotate(AVLNodePtr tnode, int heavySide, int lightSide);
AVLNodePtr findMin(AVLNodePtr tnode);
int next_missing_func(AVLNodePtr tnode, int min);

/************* Operations Implementations ****************/
AVLNodePtr avl_search( AVLNodePtr tnode, int k ){ /* Time Complexity: O(log n) */
	if (!tnode) /* if the tree is empty OR the node was not found */
		return NULL;

	if (k < tnode->key) /* if the node key is higher than the key we search -> go left */
		return avl_search(tnode->child[LEFT], k);
	else if (k > tnode->key) /* Otherwise,  if the node key is lower than the key we search -> go right */
		return avl_search(tnode->child[RIGHT], k);
	return tnode; /* if it's equal -> return it */
}

AVLNodePtr avl_insert( AVLNodePtr tnode, int k ){ /* Time Complexity: O(log n) */
	if (!tnode) /* If the tree is empty or we got to a leaf -> create new node */
		return new_avl_node(k);
	if (k < tnode->key) /* Search the new node position (like search function) */
		tnode->child[LEFT] = avl_insert(tnode->child[LEFT], k);
	else if (k > tnode->key)
		tnode->child[RIGHT] = avl_insert(tnode->child[RIGHT], k);
	else /* If the key already exists */
		return tnode;

	/* On the way back to the root -> update node's details and balance */
	paramCalculator(tnode);
	return balance(tnode);
}

AVLNodePtr avl_delete( AVLNodePtr tnode, int k ){ /* Time Complexity: O(log n) */
	if (!tnode) /* If the tree is empty or the key doesn't exist */
		return NULL;

	if (k < tnode->key) /* Search the node position (like search function) */
		tnode->child[LEFT] = avl_delete(tnode->child[LEFT], k);
	else if (k > tnode->key)
		tnode->child[RIGHT] = avl_delete(tnode->child[RIGHT], k);

	else { /* After we found the position */
		AVLNodePtr nodeToDelete = tnode, child;
		if (tnode->child[LEFT] != NULL && tnode->child[RIGHT] != NULL) { /* If the node has two children */
			nodeToDelete = findMin(nodeToDelete->child[RIGHT]); /* Find the lowest node in the right node's tree */
			tnode->key = nodeToDelete->key; /* Switch their values */
			tnode->child[RIGHT] = avl_delete(tnode->child[RIGHT], nodeToDelete->key); /* Delete the node we found */
			paramCalculator(tnode);
			return balance(tnode);
		}
		/* Otherwise, the node is a leaf or has only one child */
		child = (nodeToDelete->child[LEFT]) ? nodeToDelete->child[LEFT] : nodeToDelete->child[RIGHT];
		nodeToDelete = NULL;
		free(nodeToDelete); /* Release the memory of the node we deleted */
		paramCalculator(child);
		return balance(child);
	}

	/* On the way back to the root -> update node's details and balance */
	paramCalculator(tnode);
	return balance(tnode);
}

AVLNodePtr new_avl_node( int k ){ /* Time Complexity: O(1) */
	AVLNodePtr newNode = (AVLNodePtr)malloc(sizeof(AVLNode)); /* Allocate new memory to the node */
	if (!newNode)
		return NULL;
	/* Initialize the new node and return it */
	newNode->child[LEFT] = newNode->child[RIGHT] = NULL;
	newNode->key = k;
	newNode->height = 0;
	newNode->size = 1;
	return newNode;
}

void delete_avl_tree( AVLNodePtr root ){ /* Time Complexity: O(n) */
	/* Free all the tree by post-order */
	if (!root)
		return;
	delete_avl_tree(root->child[LEFT]);
	delete_avl_tree(root->child[RIGHT]);
	free(root);
}

/************* Queries Implementations ****************/
int next_missing( AVLNodePtr tnode ){ /* Time Complexity: O(log n) */
	if (!tnode) /* Check if the tree is empty */
		return 0;
	return next_missing_func(tnode, findMin(tnode)->key); /* call a recursive function with the root and the tree's min key */
}

int next_missing_func(AVLNodePtr tnode, int min) { /* Time Complexity: O(log n) */
	int lChildsize = (tnode->child[LEFT]) ? tnode->child[LEFT]->size : 0; /* Integer for holding left node's tree size */

	/* In each level of the tree we compare the node's key with the min value in its left sub tree
	   If the key is equal to the min + left side size -> then the node is fine
	   If it's not equal -> then the problem is in the left sub tree and we continue the recursion there */

	if (!tnode->child[LEFT] && !tnode->child[RIGHT]) { /* if the node is a leaf */
		if (tnode->key == min + lChildsize)
			return tnode->key + 1;
		else
			return min;
	}
	else if (tnode->key == min + lChildsize) { /* If the value is correct we check if there is right child */
		if (!tnode->child[RIGHT]) /* If not -> we got to the end and the missing value is the key + 1 */
			return tnode->key + 1;
		else
			return next_missing_func(tnode->child[RIGHT], min + lChildsize + 1); /* Otherwise, we continue the recursion */
	}
	else { /* If the value is not equal then the problem is in the left sub tree */
		if (!tnode->child[LEFT])
			return min;
		else
			return next_missing_func(tnode->child[LEFT], min);
	}
}

int avl_rank( AVLNodePtr tnode, int k ){ /* Time Complexity: O(log n) */
	if (!tnode) /* If the tree is empty -> return 0*/
		return 0;

	if(!tnode->child[LEFT] && !tnode->child[RIGHT]) /* If the node is a leaf we compare the key to k */
		return (k >= tnode->key);

	if (k < tnode->key) /* If the key is higher, we continue left */
		return avl_rank(tnode->child[LEFT], k);

	else { /* Otherwise, we add to rank the size of the left sub tree (if exits) and continue right */
		int rank = (tnode->child[LEFT]) ? tnode->child[LEFT]->size : 0;
		return rank + 1 + avl_rank(tnode->child[RIGHT], k);
	}
}

AVLNodePtr avl_select(AVLNodePtr tnode, int k) { /* Time Complexity: O(log n) */
	/* Stops if the tree is empty OR k is equal/smallest from 1 or k is larger then n  or we got to the node we are looking for. */
	if (!tnode || (!tnode->child[LEFT] && k <= 1) || (!tnode->child[RIGHT] && k >= tnode->size) || (node_size_Calculator(tnode->child[LEFT]) + 1) == k)
		return tnode;

	/* If the root is smallest then the node whose key rank is k we will search it in the right subtree. */
	else if ((node_size_Calculator(tnode->child[LEFT]) + 1) < k)
		return avl_select(tnode->child[RIGHT], k - (node_size_Calculator(tnode->child[LEFT]) + 1));

	/* If the root is bigger then the node whose key rank is k we will search it in the left subtree. */
	else
		return avl_select(tnode->child[LEFT], k);
}

/* Utilities */
int node_size_Calculator(AVLNodePtr tnode) { /* Time Complexity: O(1) */
	if (!tnode)
		return 0;
	return tnode->size;
}

void paramCalculator(AVLNodePtr tnode) { /* Time Complexity: O(1) */
	int lHeight, rHeight, lSize, rSize;
	if (!tnode)
		return;

    lHeight = -1, rHeight = -1, lSize = 0, rSize = 0;
	if (tnode->child[LEFT]) { /* Get left child parameters */
		lHeight = tnode->child[LEFT]->height;
		lSize = tnode->child[LEFT]->size;
	}
	if (tnode->child[RIGHT]) { /* Get right child parameters */
		rHeight = tnode->child[RIGHT]->height;
		rSize = tnode->child[RIGHT]->size;
	}

	/* Calculate the node parameters according to children parameters */
	tnode->height = max(lHeight, rHeight) + 1;
	tnode->size = lSize + rSize + 1;
}

AVLNodePtr balance(AVLNodePtr tnode) { /* O(1) */
	int heightLeft, heightRight;
	if (!tnode)
		return tnode;
	/* Get the children's heights */
	heightLeft = (tnode->child[LEFT]) ? tnode->child[LEFT]->height : -1;
	heightRight = (tnode->child[RIGHT]) ? tnode->child[RIGHT]->height : -1;

	/* Check if we need to do rotate */
	if (abs(heightLeft - heightRight) <= 1)
		return tnode;

	/* If we do -> check which child is heavy and rotate it */
	else if (heightLeft > heightRight) /* root is "left-heavy" */
		return rootBalance(tnode, LEFT, RIGHT);

	else /* root is "right-heavy" */
		return rootBalance(tnode, RIGHT, LEFT);
}

AVLNodePtr findMin(AVLNodePtr tnode){ /* Time Complexity: O(log n) */
	/* move all the way down to the leftest child */
	if (tnode == NULL || tnode->child[LEFT] == NULL)
		return tnode;
	return findMin(tnode->child[LEFT]);
}

AVLNodePtr rotate(AVLNodePtr tnode, int heavySide, int lightSide) { /* Time Complexity: O(1) */
	/* Switch pointers according to rotation algorithm */
	AVLNodePtr child = tnode->child[heavySide];

	tnode->child[heavySide] = child->child[lightSide];
	child->child[lightSide] = tnode;

	/* Update parameters and return the new parent (child) */
	paramCalculator(tnode);
	paramCalculator(child);
	return child;
}

AVLNodePtr rootBalance(AVLNodePtr tnode, int heavySide, int lightSide) { /* Time Complexity: O(1) */
	AVLNodePtr temp = tnode->child[heavySide];
	/* Get the children's heights */
	int heightLeft = (temp->child[heavySide]) ? temp->child[heavySide]->height : -1;
	int heightRight = (temp->child[lightSide]) ? temp->child[lightSide]->height : -1;

	/* Check if the situation is LR or RL rotate */
	if (heightLeft < heightRight)
		tnode->child[heavySide] = rotate(temp, lightSide, heavySide);
	return rotate(tnode, heavySide, lightSide);
}

/* Print AVL */
void padding(char ch, int n) {
	int i;
	for (i = 0; i < n; i++)
		putchar(ch);
}
void print_avl_tree(AVLNodePtr root, int level) {
	if (root == NULL)
	{
		return;
	}
	print_avl_tree(root->child[RIGHT], level + 1);
	padding('\t', level);
	printf("%d(%d/%d)", root->key, root->height, root->size);
	if (root->child[LEFT] != NULL && root->child[RIGHT] != NULL)
		printf("<\n");
	else if (root->child[LEFT] != NULL)
		printf("\\\n");
	else if (root->child[RIGHT] != NULL)
		printf("\/\n");
	else
		printf("\n");
	print_avl_tree(root->child[LEFT], level + 1);
}


int main() {
	AVLNodePtr root = NULL;
	printf("AVL: Hello World!\n");
	return 0;
}
