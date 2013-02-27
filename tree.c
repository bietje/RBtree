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

static void tree_rotate_swap_parent(struct rbtree_root *root, struct rbtree *parent, struct rbtree *current)
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

#ifdef HAVE_DBG
void tree_dump(struct rbtree *tree, FILE *stream)
{
	tree_dump_node(tree, stream);
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
