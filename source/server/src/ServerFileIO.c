#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

const char *filename = "test.txt";

void SendFileData(int client_fd)
{
	int fd;
	int nread;
	int nwrite, i;
	char buf[MAX_LINE];

	// Open the file
	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		errorreport(FILE_OPEN_ERR);
		exit(1);
	}	

	// Send the file, one chunk at a time
	do
	{
		// Get one chunk of the file from disk
		nread = read( fd );
	}
}


