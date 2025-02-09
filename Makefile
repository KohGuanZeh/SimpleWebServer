# Compiler
CC = gcc

# Compiler Flags
LWS2 = -lws2_32
CFLAGS = -Wall -Iinclude

# Directories
SRC_DIR = src
TEST_DIR = test
INC_DIR = include


webserver: $(SRC_DIR)/webserver.o $(SRC_DIR)/strutils.o
	$(CC) $(SRC_DIR)/webserver.o $(SRC_DIR)/strutils.o -o webserver $(LWS2) $(CFLAGS)

webserver.o: $(SRC_DIR)/webserver.c
	$(CC) -c $(SRC_DIR)/webserver.c -o $(SRC_DIR)/webserver.o $(CFLAGS)

strutils.o: $(SRC_DIR)/strutils.c
	$(CC) -c $(SRC_DIR)/strutils.c -o $(SRC_DIR)/strutils.o $(CFLAGS)

test_strutils: $(TEST_DIR)/test_strutils.c $(SRC_DIR)/strutils.c $(INC_DIR)/strutils.h
	$(CC) $(TEST_DIR)/test_strutils.c $(SRC_DIR)/strutils.c -o test_strutils $(CFLAGS)

clean:
	-rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o *.exe