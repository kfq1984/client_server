#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

void ReceiveFileData(int socket)
{
	int fd;
	int nread;
	int nwrite, i;
	char buf[MAX_LINE];

	// Open the file
	fd = open("client.txt", O_RDWR | O_CREAT);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		exit(1);
	}

	// Receive file data, one chunck at a time
	do
	{
		// Get one chunk of the file from socket
		nread = read(socket, buf, MAX_LINE);
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
}