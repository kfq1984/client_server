#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

const char *filename = "test.txt";

int SendFileData(int socket)
{
	int fd;
	int write_length = 0;
	int i;
	int file_block_length = 0;
	char buf[MAX_LINE];

	// Open the file
	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}	

	// Send the file, one chunk at a time
	#if 0
	do
	{
		// Get one chunk of the file from disk
		nread = read(fd, buf, MAX_LINE);
		if(nread == 0)
		{
			// All Done, close the file and the socket
			close(fd);
			close(socket);
			break;
		}

		// Send the chunck
		for(i = 0; i < nread; i+= nwrite)
		{
			// write might not take it all in one call, so we have to try until it's all written
			nwrite = write(socket, buf + i, nread - i);
			if(nwrite < 0)
			{
				ErrorReport(WRITE_SOCKET_ERR);
				exit(1);
			}
		}
	} while(1);
	#endif
	
	do
    {
        bzero(buf, MAX_BUFFER_SIZE);
		file_block_length = read(fd, buf, MAX_BUFFER_SIZE);
        if(FALSE == file_block_length)
        {
            ErrorReport(FILE_READ_ERR);
			return FALSE;
        }
        //Send the chunck
        for(i = 0; i < file_block_length; i += write_length)
        {
            write_length = send(socket, buf + i, file_block_length - i, 0);
	        if(FALSE == write_length)
	        {
	            ErrorReport(FILE_SEND_ERR);
				return FALSE;
	        }
        }
    }while(file_block_length);                
    close(fd);
           
   return TRUE;
        
}


