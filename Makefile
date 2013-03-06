#     RBTree - Makefile
#     Copyright (C) <year>  <name of author>
# 
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.

# SRCS
SRCS = tree.c main.c
TREE_SRCS = tree.c
#OBJECS
OBJS=$(SRCS:.c=.o)
TREE_OBJS=$(TREE_SRCS:.c=.o)

#PROGRAMS
CC = gcc
CC_FLAGS = -c -fPIC -Wall -O0 -DHAVE_DBG -g

#FILENAMES
LIBNAME = librbtree.so
DBG_EXEC = rbtree

#TARGETS
DBG_TARGET = $(DBG_EXEC)
LIB_TARGET = $(LIBNAME)

.PHONY: all clean

all: lib

lib: $(TREE_OBJS)
	$(CC) -shared -Wl,-soname,$(LIBNAME) -o $(LIBNAME) $(TREE_OBJS) -lc

dbg: $(OBJS)
	$(CC) -o $(DBG_TARGET) $(OBJS) -Wall -O0 -g

%o: %c
	$(CC) -o $@ $< $(CC_FLAGS)

clean:
	@rm -rfv $(OBJS) $(LIB_TARGET) $(DBG_TARGET)
