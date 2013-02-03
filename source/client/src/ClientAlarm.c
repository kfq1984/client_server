
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
			perror("fail to write file");
			break;
		}

		case FILE_READ_ERR:
		{
			perror("fail to read file");
			break;
		}

		case FILE_LSEEK_ERR:
		{
			perror("fail to lseek file");
			break;
		}
		
		case SERVER_RESPONSE_ERR:
		{
			perror("Server fail to response to client");
			break;
		}
		
		default:
		{
			perror("can not find error code");
			break;
		}
	}

}