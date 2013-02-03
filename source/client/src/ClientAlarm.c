/*
 *This c file defines error report function in client application.
 */

 
#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

/*-----------------------------------------------------
 *function name: ErrorReport
 *input:             error code
 *output:           void
 *This function reports error in client application.
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

		// Connect to server error
		case CLIENT_CONNECT_ERR:
		{
			perror("fail to connect");
			break;
		}

		// Receive data error
		case RECEIVE_DATA_ERR:
		{
			perror("fail to receive data");
			break;
		}
		
		// Close socket error
		case CLOSE_SOCKET_ERR:
		{
			perror("fail to close socket");
			break;
		}
		
		// Open file error
		case FILE_OPEN_ERR:
		{
			perror("fail to open file");
			break;
		}

		// Write file error
		case FILE_WRITE_ERR:
		{
			perror("fail to write file");
			break;
		}

		// Read file error
		case FILE_READ_ERR:
		{
			perror("fail to read file");
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