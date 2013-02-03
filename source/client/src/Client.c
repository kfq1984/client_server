/*
 *This c file is main function of client.
 */


#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"


int main(int argc,char **argv)
{
	struct sockaddr_in server_sockaddr;
	int socket_fd;
	int result;
	char *str = "Test String";

	// Get server address
	if(argc != 2)
	{
		printf("Usage: ./client server_ip_addr\n");
		exit(1);
	}
	char *server_IP = argv[1];

	// Create socket for client
	if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		// Get socket error, error report
		ErrorReport(GET_SOCKET_ERR);
		exit(1);
	}

	// Fill socket information
	bzero(&server_sockaddr, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, server_IP, (void *)&server_sockaddr.sin_addr);
	server_sockaddr.sin_port = htons(SERVER_PORT);

	// Connect server
	printf("Client connecting....\n");
	if(FALSE == connect(socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)))
	{
		// Connect server fail, error report
		ErrorReport(CLIENT_CONNECT_ERR);
		exit(1);
	}

	// Send a test information to sever
	if(FALSE == send(socket_fd, str , strlen(str) + 1, 0))
	{
		// Send information fail, error report
	    ErrorReport(SEND_DATA_ERR);
		exit(1);
	}

	// Receive data from server
	printf("Begin receiving data....\n");
	result = ReceiveFileData(socket_fd);
	if(FALSE == result)
	{
		// Receive data fail, error report happened in ReceiveFileData function, just exit here
	    exit(1);
	}
	printf("Data received\n");

	// Decompress file, get the original file copy
	result = FileDecompress();
	if(FALSE == result)
	{
		// Decompress file fail, error report happened in FileDecompress function, just exit here
	    exit(1);
	}
    
	// Close socket
	if(FALSE == close(socket_fd))
	{
		// Close socket err, error report
		ErrorReport(CLOSE_SOCKET_ERR);
		exit(1);
	}
	return 0;
}