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

SRCS = tree.c main.c
OBJS=$(SRCS:.c=.o)
CC = gcc
CC_FLAGS = -c -Wall -O0 -DHAVE_DBG -g
TARGET = rbtree

.PHONY: all clean

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -Wall -O0 -g

%o: %c
	$(CC) -o $@ $< $(CC_FLAGS)

clean:
	@rm -rfv $(OBJS) $(TARGET)
