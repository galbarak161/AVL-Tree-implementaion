#ifndef AVL_H
#define AVL_H

typedef struct AVLNode * AVLNodePtr;

typedef struct AVLNode {
	AVLNodePtr child[2];
	int key, height, size;
} AVLNode;

AVLNodePtr avl_delete(AVLNodePtr tnode, int k);

AVLNodePtr avl_insert(AVLNodePtr tnode, int k);

AVLNodePtr new_avl_node(int k);

void delete_avl_tree(AVLNodePtr root);


AVLNodePtr avl_search(AVLNodePtr tnode, int k);

int next_missing(AVLNodePtr tnode);

int avl_rank(AVLNodePtr tnode, int k);

AVLNodePtr avl_select(AVLNodePtr tnode, int k);

#endif
