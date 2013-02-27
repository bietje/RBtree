/*
 *  RBTree - Main file
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

static struct rbtree_root root;

int main(int argc, char **argv)
{
	root.size = 0;
	root.tree = NULL;
	
	/* add nodes */
#if 1
	tree_add_node(&root, 20);;
	tree_add_node(&root, 10);
	tree_add_node(&root, 30);
	tree_add_node(&root, 40);
	tree_add_node(&root, 27);
	tree_add_node(&root, 25);
	tree_add_node(&root, 28);
	tree_add_node(&root, 26);
#else
	int i = 1;
	for(; i <= 12; i++) {
		tree_add_node(&root, i);
	}
// 	tree_add_node(&root, 2);
// 	tree_add_node(&root, 1);
#endif
	/* dump tree */
	tree_dump(root.tree, stdout);
// 	tree_cleanup(root.tree);
	return 0;
}