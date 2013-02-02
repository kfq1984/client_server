#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"


void SendFileData(int client_fd)
{
	int n;
	char *str = "Test String";
	int i = 0;
	for( i = 0; i < 100; i++ )
	{
		n = write(client_fd, str, strlen(str) + 1);
		if( n < 0 )
		{
			errorreport(WRITE_SOCKET_ERR);
			return;
		}	
	}
}


