#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

int main(int argc,char **argv)
{
	struct sockaddr_in server_sockaddr;
	int s_fd;
	
	char buf[MAX_LINE];
	int n;
	
	if(argc != 2)
	{
		printf("Usage: ./client server_ip_addr\n");
		exit(1);
	}
	char *server_IP = argv[1];

	// Create socket for client
	if((s_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		errorreport(GET_SOCKET_ERR);
		exit(1);
	}
	
	bzero(&server_sockaddr, sizeof(server_sockaddr));
	server_sockaddr.sin_family = AF_INET;
	inet_pton(AF_INET, server_IP, (void *)&server_sockaddr.sin_addr);
	server_sockaddr.sin_port = htons(SERVER_PORT);

	// connect server
	printf("Client connecting....\n");
	if(FALSE == connect(s_fd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)))
	{
		errorreport(CLIENT_CONNECT_ERR);
		exit(1);
	}

	// send data
	char *str = "test string";
	send(s_fd, str , strlen(str) + 1, 0);

	// receive data
	printf("Begin receiving data....\n");
	
	if( FALSE == recv(s_fd , buf, MAX_LINE, 0) )
	{
		errorreport(RECEIVE_DATA_ERR);
		exit(1);
	}
	printf("the length of str = %s\n" , buf);

	// close socket
	if(close(s_fd) == -1)
	{
		errorreport(CLOSE_SOCKET_ERR);
		exit(1);
	}
	return 0;
}