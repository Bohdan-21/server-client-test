CC = gcc
CFLAGS = -Wall -g 
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SERVER_DIR = server
CLIENT_DIR = client
MODULES_DIR = modules
SOCKET_FILE = socket_file

TEST_DIR = test

SRCFILES = $(wildcard $(SRC_DIR)/$(MODULES_DIR)/*.c)
OBJMODULES = $(patsubst $(SRC_DIR)/$(MODULES_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCFILES));


$(OBJ_DIR)/%.o: $(SRC_DIR)/$(MODULES_DIR)/%.c $(SRC_DIR)/$(MODULES_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(SERVER_DIR):
	mkdir -p $(BIN_DIR)/$(SERVER_DIR)

$(BIN_DIR)/$(CLIENT_DIR):
	mkdir -p $(BIN_DIR)/$(CLIENT_DIR)

$(BIN_DIR)/$(SERVER_DIR)/daemon_server: $(SRC_DIR)/$(SERVER_DIR)/daemon_server.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/$(CLIENT_DIR)/client: $(SRC_DIR)/$(CLIENT_DIR)/client.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_custom_io: $(TEST_DIR)/test_custom_io.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/test_buffer: $(TEST_DIR)/buffer_test.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@


daemon_server: mk_server_dir $(BIN_DIR)/$(SERVER_DIR)/daemon_server

client: mk_client_dir $(BIN_DIR)/$(CLIENT_DIR)/client


test_custom_io: $(BIN_DIR)/test_custom_io

test_buffer: $(BIN_DIR)/test_buffer



mk_server_dir: $(BIN_DIR)/$(SERVER_DIR)

mk_client_dir: $(BIN_DIR)/$(CLIENT_DIR)



clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(SOCKET_FILE) ./server.log
