# Compiler
CC = gcc -std=c99 -g

# Compiler Flags
LWS2 = -lws2_32
CFLAGS = -Wall -Iinclude

# Directories
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = test
INC_DIR = include


webserver: $(OBJ_DIR)/main.o $(OBJ_DIR)/server.o $(OBJ_DIR)/client_handler.o $(OBJ_DIR)/http_parser.o $(OBJ_DIR)/strutils.o
	$(CC) $(OBJ_DIR)/main.o $(OBJ_DIR)/server.o $(OBJ_DIR)/client_handler.o $(OBJ_DIR)/http_parser.o $(OBJ_DIR)/strutils.o -o webserver $(LWS2) $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

main.o: $(SRC_DIR)/main.c $(INC_DIR)/server.h
	$(CC) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o $(CFLAGS)

server.o: $(SRC_DIR)/server.c $(INC_DIR)/client_handler.h
	$(CC) -c $(SRC_DIR)/server.c -o $(OBJ_DIR)/server.o $(CFLAGS)

client_handler.o: $(SRC_DIR)/client_handler.c $(INC_DIR)/client_handler.h $(INC_DIR)/http_parser.h $(INC_DIR)/strutils.h
	$(CC) -c $(SRC_DIR)/client_handler.c -o $(OBJ_DIR)/client_handler.o $(CFLAGS)

http_parser.o: $(SRC_DIR)/http_parser.c $(INC_DIR)/http_parser.h $(INC_DIR)/strutils.h
	$(CC) -c $(SRC_DIR)/http_parser.c -o $(OBJ_DIR)/http_parser.o $(CFLAGS)

fileutils.o: $(SRC_DIR)/fileutils.c $(INC_DIR)/fileutils.h $(INC_DIR)/strutils.h
	$(CC) -c $(SRC_DIR)/fileutils.c -o $(OBJ_DIR)/fileutils.o $(CFLAGS)

strutils.o: $(SRC_DIR)/strutils.c $(INC_DIR)/strutils.h
	$(CC) -c $(SRC_DIR)/strutils.c -o $(OBJ_DIR)/strutils.o $(CFLAGS)

test_strutils: $(TEST_DIR)/test_strutils.c $(SRC_DIR)/strutils.c $(INC_DIR)/strutils.h
	$(CC) $(TEST_DIR)/test_strutils.c $(SRC_DIR)/strutils.c -o test_strutils $(CFLAGS)

clean:
	-rm -f $(OBJ_DIR)/*.o $(TEST_DIR)/*.o *.exe