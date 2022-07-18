TARGET = ssh_cracker
CC     = gcc
SRCS   = src/main.c

ssh_cracker:
	$(CC) $(SRCS) -o $(TARGET) -lssh -lpthread -Ofast -Wall

clean:
	rm -fr $(TARGET)
