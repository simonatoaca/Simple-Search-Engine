build: search_engine.c search_engine.h search_utils.c search_utils.h
	gcc -Wall -Wextra -g  LinkedList.c Stack.c Hashtable.c \
	search_engine.c search_utils.c -o search
clean: 
	rm search
