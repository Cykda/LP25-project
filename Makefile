CC=gcc
CFLAGS=-O2 -Wall
LDFLAGS=-lcrypto
BIN_DIR=bin
INCLUDE_DIR=include
SRC_DIR=src


all: lp25-backup


$(BIN_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -I $(INCLUDE_DIR) -o $@

errors.o: $(SRC_DIR)/errors.c $(INCLUDE_DIR)/errors.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/errors.c -I $(INCLUDE_DIR) -o $(BIN_DIR)/errors.o

color.o: $(SRC_DIR)/colors.c $(INCLUDE_DIR)/color.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/colors.c -I $(INCLUDE_DIR) -o $(BIN_DIR)/color.o



file-properties.o: $(SRC_DIR)/file-properties.c $(INCLUDE_DIR)/file-properties.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file-properties.c -std=c11 -lcrypt -I $(INCLUDE_DIR) -I /usr/include/openssl -o $(BIN_DIR)/file-properties.o

lp25-backup: $(SRC_DIR)/main.c $(BIN_DIR)/files-list.o $(BIN_DIR)/sync.o $(BIN_DIR)/configuration.o $(BIN_DIR)/file-properties.o $(BIN_DIR)/processes.o $(BIN_DIR)/messages.o $(BIN_DIR)/utility.o $(BIN_DIR)/errors.o $(BIN_DIR)/colors.o
	$(CC) -I $(INCLUDE_DIR) -o $(BIN_DIR)/$@ $^


clean:
	rm -f $(BIN_DIR)/*.o $(BIN_DIR)/lp25-backup
