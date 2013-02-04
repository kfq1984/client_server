# 
# 'make'		build executable file
#
# 'make clean' 	remove all .o and executable files
#

# define the C complier to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wall -g
LIBS = -lcrypto -lssl -lz

# define any directories containing header filers other than /usr/include
INCLUDES = -I./source/client/include -I./source/server/include

# define the C source files
CLIENT_SRCS_FOLDER = source/client/src
CLIENT_SRCS = 	$(CLIENT_SRCS_FOLDER)/Client.c \
				$(CLIENT_SRCS_FOLDER)/ClientAlarm.c \
				$(CLIENT_SRCS_FOLDER)/ClientFileIO.c

SERVER_SRCS_FOLDER = source/server/src
SERVER_SRCS = 	$(SERVER_SRCS_FOLDER)/Server.c \
				$(SERVER_SRCS_FOLDER)/ServerAlarm.c \
				$(SERVER_SRCS_FOLDER)/ServerFileIO.c

# define the C object file
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

# define the C target file
CLIENT_TARGET = ./bin/client/client
SERVER_TARGET = ./bin/server/server
	
.PHONY: depend clean

all: client server

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(CLIENT_TARGET) $(CLIENT_OBJS) 

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(SERVER_TARGET) $(SERVER_OBJS)

# this is a suffix replacement rule for building .o's from .c's
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET) $(CLIENT_OBJS) $(SERVER_OBJS)

