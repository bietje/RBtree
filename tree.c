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
				break;
			}
			tree = tree->left;
		} else {
			if(tree->right == NULL) {
				tree->right = node;
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
	rbtree_color_t tmp = tree->color;
	tree->color = left->color;
	left->color = tmp;
	
	return left;
}

static void tree_validate_insertion(struct rbtree_root *root, struct rbtree *node)
{
	struct rbtree *parent, *gparent;
	
	if(node->parent == NULL) {
		return;
	}
	
	while(node != root->tree && node->parent->color == RED) {
		parent = tree_parent(node);
		gparent = tree_grandparent(node);
		if(parent == gparent->left) {
			struct rbtree *right = gparent->right;
			if(right->color == RED) {
				parent->color = BLACK;
				right->color = BLACK;
				gparent->color = RED;
				node = gparent;
			} else {
				/* right is black */
				if(node == parent->right) {
					node = parent;
					tree_rotate_left(root, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				tree_rotate_right(root, node->parent->parent);
			}
		} else {
			/* repeat with left and right exchanged */
			struct rbtree *left = gparent->left;
			if(left->color == RED) {
				parent->color = BLACK;
				left->color = BLACK;
				gparent->color = RED;
				node = gparent;
			} else {
				/* right is black */
				if(node == parent->right) {
					node = parent;
					tree_rotate_left(root, node);
				}
				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				tree_rotate_right(root, node->parent->parent);
			}
		}
	}
	root->tree->color = BLACK;
}

#ifdef HAVE_DBG
void tree_dump(struct rbtree *tree, FILE *stream)
{
	struct rbtree *node = tree->left;
	fprintf(stream, "Starting tree dump!\n");
	fprintf(stream, "[[root: %u] [left:", tree->key);
	
	for(;;) {
		if(!node) {
			break;
		} else {
			fprintf(stream, " %u", node->key);
			node = node->left;
		}
	}
	
	fprintf(stream, "] [right:");
	node = tree->right;
	for(;;) {
		if(!node) {
			break;
		} else {
			fprintf(stream, " %u", node->key);
			node = node->right;
		}
	}
	
	fprintf(stream, "]\nDone dumping tree!");
	fflush(stream);
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
