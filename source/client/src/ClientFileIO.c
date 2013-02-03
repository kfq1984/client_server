#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

int ReceiveFileData(int socket)
{
	int fd;
	int i;
	int recv_length = 0;
	int write_length = 0;
	char buf[MAX_LINE];

	// Open the file
	fd = open("client.txt", O_RDWR | O_CREAT);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}

	// Receive file data, one chunck at a time
	#if 0
	do
	{
		// Get one chunk of the file from socket
		nread = recv(socket, buf, MAX_LINE, 0);
		if(nread < 0)
		{
			ErrorReport(RECEIVE_DATA_ERR);
			close(fd);
			break;
		}

		// Write data to file
		for(i = 0; i < nread; i += nwrite)
		{
			nwrite = write(fd, buf + i, nread - i);
		}
	}while(1);
	#endif
	
    do      
    {
        bzero(buf, MAX_BUFFER_SIZE);
        recv_length = recv(socket, buf, MAX_BUFFER_SIZE, 0);
        if(FALSE == recv_length)
        {
            ErrorReport(RECEIVE_DATA_ERR);
            return FALSE;
        }
        for(i = 0; i < recv_length; i += write_length)
        {
	        write_length = write(fd, buf + i, recv_length - i);
	        if (FALSE == write_length)
	        {
	            ErrorReport(FILE_WRITE_ERR);
	            return FALSE;
	        }
        }        
    }while(recv_length);
   
	return 0;
}