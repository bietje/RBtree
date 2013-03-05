/*
 *  RBTree - Tree file
 *  Copyright (C) 2012   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tree.h"

static struct rbtree *__tree_insert(struct rbtree_root *root, struct rbtree *node);
static void tree_validate_insertion(struct rbtree_root *root, struct rbtree *node);
static void tree_rotate_swap_parent(struct rbtree_root *root, struct rbtree *parent, 
									struct rbtree *current);
static void tree_dump_node(struct rbtree *tree, FILE *stream);
static struct rbtree *__tree_search(struct rbtree *tree, int key);
static struct rbtree *tree_find_successor(struct rbtree *tree);
static struct rbtree *tree_find_predecessor(struct rbtree *tree);
static int __tree_delete_node(struct rbtree_root *tree, struct rbtree *node);
static struct rbtree *tree_find_replacement(struct rbtree *tree);
static void tree_sub_deletion(struct rbtree_root *root, struct rbtree *current);

int tree_insert(struct rbtree_root *root, struct rbtree *node)
{
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->color = RED;
	tree_validate_insertion(root, __tree_insert(root, node));
	
	return 0;
}

struct rbtree *tree_search(struct rbtree_root *root, int key)
{
	return __tree_search(root->tree, key);
}

#if HAVE_RECURSION
static struct rbtree *__tree_search(struct rbtree *tree, int key)
{
	if(tree == NULL) {
		return NULL;
	}
	
	if(tree->key == key) {
		return tree;
	}
	
	if(key < tree->key) {
		return __tree_search(tree->left, key);
	} else {
		return __tree_search(tree->right, key);
	}
}
#else

static struct rbtree *__tree_search(struct rbtree *tree, int key)
{
	for(;;) {
		if(tree == NULL) {
			return NULL;
		}
		if(tree->key == key) {
			return tree;
		}
		
		if(key < tree->key) {
			tree = tree->left;
		} else {
			tree = tree->right;
		}
	}
}
#endif

static struct rbtree *__tree_insert(struct rbtree_root *root, struct rbtree *node)
{
	struct rbtree *tree = root->tree;
	
	if(tree == NULL) {
		root->tree = node;
		/* Set the color to black. The root is always black. */
		node->color = BLACK;
		return root->tree;
	}
	
	for(;;) {
		if(node->key <= tree->key) {
			if(tree->left == NULL) {
				tree->left = node;
				node->parent = tree;
				break;
			}
			tree = tree->left;
		} else {
			if(tree->right == NULL) {
				tree->right = node;
				node->parent = tree;
				break;
			}
			tree = tree->right;
		}
	}
	
	return node;
}

struct rbtree *tree_find_leftmost(struct rbtree *tree)
{
	if(!tree) {
		return NULL;
	}
	
	for(;;) {
		if(tree->left == NULL) {
			return tree;
		}
		tree = tree->left;
	}
}

struct rbtree *tree_find_rightmost(struct rbtree *tree)
{
	if(!tree) {
		return NULL;
	}
	
	for(;;) {
		if(tree->right == NULL) {
			return tree;
		}
		tree = tree->right;
	}
}

static struct rbtree *tree_find_successor(struct rbtree *tree)
{
	if(!tree) {
		return NULL;
	}
	
	if(!tree->right) {
		struct rbtree *tmp = tree, *carriage = tree_parent(tree);
		
		while (carriage != NULL && carriage->left != tmp) {
			tmp = carriage;
			carriage = carriage->parent;
		}
		return carriage;
	}
	
	return tree_find_leftmost(tree->right);
}

static struct rbtree *tree_find_predecessor(struct rbtree *tree)
{
	if(!tree) {
		return NULL;
	}
	
	if(!tree->left) {
		struct rbtree *tmp = tree, *carriage = tree_parent(tree);
		
		while (carriage != NULL && carriage->right != tmp) {
			tmp = carriage;
			carriage = carriage->parent;
		}
		return carriage;
	}
	return tree_find_rightmost(tree->left);
}

static struct rbtree *tree_rotate_left(struct rbtree_root *root, struct rbtree *tree)
{
	struct rbtree *right = tree->right, *parent = tree_parent(tree);
	
	tree->right = right->left;
	right->parent = parent;
	right->left = tree;
	tree->parent = right;
	
	if(tree->right) {
		tree->right->parent = tree;
	}
	
	if(parent) {
		if(tree == parent->left) {
			parent->left = right;
			
		} else {
			parent->right = right;
		}
	} else {
		root->tree = right;
	}
	
	
	
	return right;
}

static struct rbtree *tree_rotate_right(struct rbtree_root *root, struct rbtree *tree)
{
	struct rbtree *left = tree->left, *parent = tree_parent(tree);
	
	tree->left = left->right;
	left->parent = parent;
	tree->parent = left;
	left->right = tree;
	
	if(tree->left) {
		tree->left->parent = tree;
	}
	
	if(parent) {
		if(tree == parent->right) {
			parent->right = left;
		} else {
			parent->left = left;
		}
	} else {
		root->tree = left;
	}
	
	return left;
}

static void tree_validate_insertion(struct rbtree_root *root, struct rbtree *current)
{
	struct rbtree *x;
	
	if(current == root->tree) {
		return;
	}
	
	current = tree_parent(current);
	while(current != root->tree && current->color == RED) {
		if((x = tree_node_has_sibling(current)) != NULL) {
			/* parent has a sibling, pull black down from the GP */
			if(x->color == RED) {
				x->color = BLACK;
				current->color = BLACK;
				tree_parent(current)->color = RED;
				current = tree_grandparent(current);
				if(!current) {
					break;
				}
				continue;
			} else {
				if(tree_parent_on_left(current) && current->left->color == RED) {
					/* 
					* if current is the right child of its parent, rotate right. If current is on the left,
					* rotate left.
					*/
					tree_rotate_right(root, current);
					current = tree_parent(current);
				} else if(!tree_parent_on_left(current) && !current->left->color == RED) {
					tree_rotate_left(root, current);
					current = tree_parent(current);
				}
				
				tree_rotate_swap_parent(root, tree_parent(current), current);
			}
		} else {
			tree_rotate_swap_parent(root, tree_parent(current), current);
		}
	}
	root->tree->color = BLACK;
}

static void tree_rotate_swap_parent(struct rbtree_root *root, struct rbtree *parent, 
									struct rbtree *current)
{
	rbtree_color_t tmp = parent->color;
	/* rotate in the direction that sets current as parent of pre-rotation parent */
	if(parent->right == current) {
		/* rotate to the left */
		tree_rotate_left(root, parent);
	} else {
		tree_rotate_right(root, parent);
	}
	
	parent->color = current->color;
	current->color = tmp;
}

int tree_delete_node(struct rbtree_root *root, struct rbtree *node)
{
	int rc = __tree_delete_node(root, node);
	if(root->tree) {
		root->tree->color = BLACK;
	}
	return rc;
}

typedef enum
{
	TREE_DELETION_TERMINATE = 0,
	TREE_DELETION_CASE0, //!< Current is a red leaf.
	TREE_DELETION_CASE1, //!< Current is black with one red child.
	TREE_DELETION_CASE2, //!< Current is black and has no children.
} tree_deletion_case_t;

typedef enum
{
	TREE_SUB_DELETION_TERMINATE = 0,
	TREE_SUB_DELETION_CASE0, //!< Currents sibling is red.
	TREE_SUB_DELETION_CASE1, //!< Currents sibling is black with two black children.
	
	/**
	 * \brief Currents sibling is black with at least one red child.
	 * 
	 * Case two is executed if the far newphew is black.
	 */
	TREE_SUB_DELETION_CASE2,
	/**
	 * \brief Currents sibling is black with at least one red child.
	 * 
	 * This step is reached through step 2 or directly (i.e. the far nephew is red).
	 */
	TREE_SUB_DELETION_CASE3,
	/**
	 * \brief Currents sibling is black with at least one red child.
	 * 
	 * This step is reached through step 3 and this is the final rotation.
	 */
	TREE_SUB_DELETION_CASE4,
	TREE_SUB_DELETION_CASE5,
	TREE_SUB_DELETION_CASE6,
} tree_deletion_subcase_t;

static int __tree_delete_node(struct rbtree_root *root, struct rbtree *node)
{
	struct rbtree *current = node, *parent, *replacement;
	int rc = 0;
	char replace = 0;
	tree_deletion_case_t _case;
	
	successor:
	if(!current->left && !current->right) {
		if(current->color == RED) {
			_case = TREE_DELETION_CASE0;
		} else {
			_case = TREE_DELETION_CASE2;
		}
	} else if(!(current->left && current->right) && current->color == BLACK) {
		/* current has at most one child (and current is black) */
		_case = TREE_DELETION_CASE1;
	} else if(current->left && current->right) {
		replacement = current;
		current = tree_find_replacement(replacement);
		goto successor;
	}
	
	while(_case) {
		switch(_case) {
			case TREE_DELETION_CASE1:
				parent = tree_parent(current);
				if(parent) {
					if(parent->left == current) {
						parent->left = (current->left) ? current->left : current->right;
						parent->left->parent = parent;
						parent->left->color = BLACK;
						
					} else {
						/* current is the right child of its parent */
						parent->right = (current->left) ? current->left : current->right;
						parent->right->parent = parent;
						parent->right->color = BLACK;
					}
				} else {
					/* current is root */
					root->tree = (current->left) ? current->left : current->right;
					root->tree->color = BLACK;
					root->tree->parent = NULL;
				}
				_case = TREE_DELETION_TERMINATE;
				break;
				
			case TREE_DELETION_CASE2:
				if(root->tree == current) {
					root->tree = NULL;
					_case = TREE_DELETION_TERMINATE;
					break;
				}
				/* current is NOT the root */
				tree_sub_deletion(root, current);
				break;
				
			case TREE_DELETION_CASE0:
			default:
				parent = tree_parent(current);
				if(parent) {
					if(parent->left == current) {
						parent->left = NULL;
					} else {
						parent->right = NULL;
					}
				}
				_case = TREE_DELETION_TERMINATE;
				break;
		}
	}

	if(replace) {
		current->left = replacement->left;
		current->right = replacement->right;
		current->parent = replacement->parent;
		if(replacement->parent) {
			if(replacement->parent->left == replacement) {
				current->parent->left = replacement;
			} else {
				current->parent->right = replacement;
			}
		}
	}
	
	return rc;
}

static void tree_sub_deletion(struct rbtree_root *root, struct rbtree *current)
{
	
}

static struct rbtree *tree_find_replacement(struct rbtree *tree)
{
	struct rbtree *successor = tree_find_successor(tree);
	
	if(successor->color == RED || !(successor->color == BLACK && successor->left == NULL 
		&& successor->right == NULL)) {
		return successor;
	} else {
		return tree_find_predecessor(tree);
	}
}

#ifdef HAVE_DBG
void tree_dump(struct rbtree *tree, FILE *stream)
{
	struct rbtree *node = __tree_search(tree, 10);
	node = tree_find_replacement(node);
	printf("Replacement would be: %X\n", node->key);
	tree_dump_node(tree, stream);
	fputc('\n', stream);
}

static void tree_dump_node(struct rbtree *tree, FILE *stream)
{
	if (tree == NULL)
	{
		printf("null");
		return;
	}

	printf("d:[");
	printf("%u,%s,%u", tree->key, (tree->color == RED) ? "RED" : "BLACK", 
		   (tree->parent != NULL) ? tree->parent->key : -1);
	
	printf("]");
	if (tree->left != NULL)
	{
		printf("l:[");
		tree_dump_node(tree->left, stream);
		printf("]");
	}
	if (tree->right != NULL)
	{
		printf("r:[");
		tree_dump_node(tree->right, stream);
		printf("]");
	}
}

void tree_add_node(struct rbtree_root *root, int key)
{
	struct rbtree *node = malloc(sizeof(*node));
	
	if(node == NULL) {
		return;
	}
	node->key = key;
	tree_insert(root, node);
}

void tree_cleanup(struct rbtree *root)
{
	struct rbtree *node = root->left, *tmp;
	
	for(;;) {
		if(node) {
			tmp = node->left;
			free(node);
			node = tmp;
		} else {
			break;
		}
	}
	
	/* repeat for right */
	node = root->right;
	for(;;) {
		if(node) {
			tmp = node->right;
			free(node);
			node = tmp;
		} else {
			break;
		}
	}
	
	free(root);
}
#endif
