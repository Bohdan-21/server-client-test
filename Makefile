CC = gcc
ASM = nasm
CFLAGS = -m32 -Wall -g -no-pie -static
ASMFLAGS = -f elf32 -g -F dwarf

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SERVER_DIR = server
CLIENT_DIR = client
MODULES_DIR = modules
SOCKET_FILE = socket_file

SRCFILES_C = $(wildcard $(SRC_DIR)/$(MODULES_DIR)/*.c)
SRCFILES_ASM = $(wildcard $(SRC_DIR)/$(MODULES_DIR)/*.asm)

OBJMODULES_C = $(patsubst $(SRC_DIR)/$(MODULES_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCFILES_C))
OBJMODULES_ASM = $(patsubst $(SRC_DIR)/$(MODULES_DIR)/%.asm, $(OBJ_DIR)/%.o, $(SRCFILES_ASM))

OBJMODULES = $(OBJMODULES_C) $(OBJMODULES_ASM)

# Компиляция .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/$(MODULES_DIR)/%.c $(SRC_DIR)/$(MODULES_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Компиляция .asm -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/$(MODULES_DIR)/%.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(BIN_DIR)/$(SERVER_DIR):
	mkdir -p $(BIN_DIR)/$(SERVER_DIR)

$(BIN_DIR)/$(CLIENT_DIR):
	mkdir -p $(BIN_DIR)/$(CLIENT_DIR)

$(BIN_DIR)/$(SERVER_DIR)/server: $(SRC_DIR)/$(SERVER_DIR)/server.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/$(CLIENT_DIR)/client: $(SRC_DIR)/$(CLIENT_DIR)/client.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

server: mk_server_dir $(BIN_DIR)/$(SERVER_DIR)/server

client: mk_client_dir $(BIN_DIR)/$(CLIENT_DIR)/client

mk_server_dir: $(BIN_DIR)/$(SERVER_DIR)

mk_client_dir: $(BIN_DIR)/$(CLIENT_DIR)

clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(SOCKET_FILE) ./server.log
