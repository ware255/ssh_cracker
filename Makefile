TARGET = ssh_cracker
CC     = gcc
SRCS   = src/main.c

ssh_cracker:
	$(CC) $(SRCS) -o $(TARGET) -lssh -Wall

clean:
	rm -fr $(TARGET)
