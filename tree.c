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

int tree_insert(struct rbtree_root *root, struct rbtree *node)
{
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	node->color = RED;
	tree_validate_insertion(root, __tree_insert(root, node));
	
	return 0;
}

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

static struct rbtree *tree_rotate_left(struct rbtree_root *root, struct rbtree *tree)
{
	struct rbtree *right = tree->right, *parent = tree_parent(tree);
	
	tree->right = right->left;
	right->parent = parent;
	
	if(parent) {
		if(tree == parent->left) {
			parent->left = right;
			
		} else {
			parent->right = right;
		}
	} else {
		root->tree = right;
	}
	
	right->left = tree;
	tree->parent = right;
	rbtree_color_t tmp = tree->color;
	tree->color = right->color;
	right->color = tmp;
	
	return right;
}

// static void tree_rotate_left2(struct rbtree *tree, struct rbtree *node)
// {
// 	struct rbtree *right = node->right;
// 	
// 	node->right = right->left;
// 	if(right->left != NULL) {
// 		right->left->parent = node;
// 	}
// 	right->parent = node->parent;
// 	
// 	if(node->parent == NULL) {
// 		tree->root = right;
// 	} else {
// 		if(node == (node->parent)->left) {
// 			node->parent->left = right;
// 		} else {
// 			node->parent->right = right;
// 		}
// 	}
// 	
// 	right->left = node;
// 	node->parent = right;
// }

// static void tree_rotate_right2(struct rbtree *tree, struct rbtree *node)
// {
// 	
// }

static struct rbtree *tree_rotate_right(struct rbtree_root *root, struct rbtree *tree)
{
	struct rbtree *left = tree->left, *parent = tree_parent(tree);
	
	tree->left = left->right;
	
	left->parent = parent;
	
	
	if(parent) {
		if(tree == parent->right) {
			parent->right = left;
		} else {
			parent->left = left;
		}
	} else {
		root->tree = left;
	}
	
	left->right = tree;
	tree->parent = left;
// 	rbtree_color_t tmp = tree->color;
// 	tree->color = left->color;
// 	left->color = tmp;
	
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
				current = tree_parent(current);
				continue;
			} else {
				/* 
				 * if current is the right child of its parent, rotate right. If current is on the left,
				 * rotate left.
				 */
				if(current == tree_parent(current)->right) {
					current = tree_rotate_right(root, current);
				} else {
					current = tree_rotate_left(root, current);
				}
			}
		} else {
			x = tree_parent(current);
			/* rotate in the direction that sets current as parent of pre-rotation parent */
			if(x->right == current) {
				/* rotate to the left */
				tree_rotate_left(root, x);
			} else {
				tree_rotate_right(root, x);
			}
			rbtree_color_t tmp = x->color;
			x->color = current->color;
			current->color = tmp;
		}
	}
	root->tree->color = BLACK;
}

#ifdef HAVE_DBG
void tree_dump(struct rbtree *tree, FILE *stream)
{
	struct rbtree *parent;
	char *location;
	if(tree == NULL) {
		return;
	}
	
	parent = tree_parent(tree);
	if(parent) {
		if(parent->left == tree) {
			location = "Node is left of parent";
		} else {
			location = "Node is right of parent";
		}
	} else {
		location = "Node is the root";
	}

	fprintf(stream, "Node key: %u :: Color: %u :: %s\n", tree->key, tree->color, location);
	tree_dump(tree->left, stream);
	tree_dump(tree->right, stream);
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
