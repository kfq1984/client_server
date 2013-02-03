
#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"


void ErrorReport(int errcode)
{
	switch(errcode)
	{
		case GET_SOCKET_ERR:
		{
			perror("fail to create socket");
			break;
		}

		case CLIENT_CONNECT_ERR:
		{
			perror("fail to connect");
			break;
		}

		case RECEIVE_DATA_ERR:
		{
			perror("fail to receive data");
			break;
		}

		case CLOSE_SOCKET_ERR:
		{
			perror("fail to close socket");
			break;
		}

		case FILE_OPEN_ERR:
		{
			perror("fail to open file");
			break;
		}

		case FILE_WRITE_ERR:
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