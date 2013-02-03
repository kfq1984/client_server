#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

int main(int argc,char **argv)
{
	struct sockaddr_in server_sockaddr;
	int socket_fd;
	
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

	char *str = "Test String";
	write(socket_fd, str , strlen(str) + 1);

	// receive data
	printf("Begin receiving data....\n");
	ReceiveFileData(socket_fd);

	// close socket
	if(close(socket_fd) == -1)
	{
		ErrorReport(CLOSE_SOCKET_ERR);
		exit(1);
	}
	return 0;
}