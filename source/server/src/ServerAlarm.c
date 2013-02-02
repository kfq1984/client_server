#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

void ErrorReport(int errcode)
{
	switch(errcode)
	{
		case GET_SOCKET_ERR:
		{
			perror("fail to create socket");
			break;
		}

		case SOCKET_BIND_ERR:
		{
			perror("fail to bind");
			break;
		}

		case SERVER_LISTEN_ERR:
		{
			perror("fail to listen");
			break;
		}

		case SOCKET_SELECT_ERR:
		{
			perror("fail to select");
			break;
		}

		case WRITE_SOCKET_ERR:
		{
			perror("fail to write");
			break;
		}

		case READ_SOCKET_ERR:
		{
			perror("fail to read");
			break;
		}

		case FORK_ERR:
		{
			perror("fail to fork");
			break;
		}

		case FILE_OPEN_ERR:
		{
			perror("fail to open file");
			break;
		}
		
		default:
		{
			perror("can not find error code");
			break;
		}
	}

}