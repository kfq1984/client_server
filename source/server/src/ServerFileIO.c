#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

const char *filename = "test.txt";

void SendFileData(int socket)
{
	int fd;
	int nread;
	int nwrite, i;
	char buf[MAX_LINE];

	// Open the file
	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		exit(1);
	}	

	// Send the file, one chunk at a time
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
}


