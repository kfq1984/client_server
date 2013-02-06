/*
 *This c file defines error report function in server application.
 */
 

#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"


/*-----------------------------------------------------
 *function name: ErrorReport
 *input:             error code
 *output:           void
 *This function reports error in server application.
 *-----------------------------------------------------
 */
 
void ErrorReport(int errcode)
{
	switch(errcode)
	{
		// Get socket error
		case GET_SOCKET_ERR:
		{
			perror("fail to create socket");
			break;
		}

		// Bind socket error
		case SOCKET_BIND_ERR:
		{
			perror("fail to bind");
			break;
		}

		// Listen socket error
		case SERVER_LISTEN_ERR:
		{
			perror("fail to listen");
			break;
		}

		// Receive data error
		case RECV_DATA_ERR:
		{
			perror("fail to receive data");
			break;
		}
		
		// Send data error
		case SEND_DATA_ERR:
		{
			perror("fail to send data");
			break;
		}
		
		// Close socket error
		case CLOSE_SOCKET_ERR:
		{
			perror("fail to close socket");
			break;
		}

		// Create fork error
		case FORK_ERR:
		{
			perror("fail to fork");
			break;
		}

		// Open file error
		case FILE_OPEN_ERR:
		{
			perror("fail to open file");
			break;
		}

		// Read file error
		case FILE_READ_ERR:
		{
			perror("fail to read file content");
			break;
		}

		// Send file error
		case FILE_SEND_ERR:
		{
			perror("fail to send file");
			break;
		}

		// Write file error
		case FILE_WRITE_ERR:
		{
			perror("fail to write file");
			break;
		}

		//Default, for some abnormal instance
		default:
		{
			perror("can not find error code");
			break;
		}
	}

}