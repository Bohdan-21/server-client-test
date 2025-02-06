CC = gcc
CFLAGS = -Wall -g 
OBJMODULES = list.o buffer.o

clean:
	rm -f *.o buffer_test daemon_server

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

buffer_test: buffer_test.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

daemon_server: daemon_server.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@