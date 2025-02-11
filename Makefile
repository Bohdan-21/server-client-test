CC = gcc
CFLAGS = -Wall -g 
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SERVER_DIR = server
CLIENT_DIR = client
MODULES_DIR = modules
SOCKET_FILE = socket_file

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


daemon_server: mk_server_dir $(BIN_DIR)/$(SERVER_DIR)/daemon_server

mk_server_dir: $(BIN_DIR)/$(SERVER_DIR)


clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(SOCKET_FILE)
