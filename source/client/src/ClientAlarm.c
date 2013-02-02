
#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"


void errorreport(int errcode)
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
		
		default:
		{
			perror("can not find error code");
			break;
		}
	}

}