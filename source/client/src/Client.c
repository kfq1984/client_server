#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

int main(int argc,char **argv)
{
	struct sockaddr_in server_sockaddr;
	int socket_fd;
	int result;
	int file_size;
	
	if(argc != 2)
	{
		printf("Usage: ./client server_ip_addr\n");
		exit(1);
	}
	char *server_IP = argv[1];

	// Create socket for client
	if((socket_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		ErrorReport(GET_SOCKET_ERR);
		exit(1);
	}
	
	bzero(&server_sockaddr, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, server_IP, (void *)&server_sockaddr.sin_addr);
	server_sockaddr.sin_port = htons(SERVER_PORT);

	// connect server
	printf("Client connecting....\n");
	if(FALSE == connect(socket_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)))
	{
		ErrorReport(CLIENT_CONNECT_ERR);
		exit(1);
	}

	file_size = SendLocalFileStatus(socket_fd);
	if(FALSE == file_size)
	{
		exit(1);
	}

	if(file_size == 0)
	{
		// receive data
		printf("Begin receiving data....\n");
		result = ReceiveFileData(socket_fd);
		if(FALSE == result)
		{
		    exit(1);
		}
	}
	else
	{
		if(FALSE == HandleDiffData(socket_fd, file_size))
		{
			exit(1);
		}
	}

	// close socket
	if(FALSE == close(socket_fd))
	{
		ErrorReport(CLOSE_SOCKET_ERR);
		exit(1);
	}
	return 0;
}