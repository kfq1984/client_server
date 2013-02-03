#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

const char *filename = "test.txt";

int SendFileData(int socket)
{
	int fd;
	int sendlength = 0;
	int i;
	int filechunklength = 0;
	char buf[MAX_LINE];
	char compressedbuf[MAX_LINE];
	int compresslength;
	int result;

	// Open the file
	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}	
	
	bzero(buf, MAX_LINE);
	filechunklength = read(fd, buf, MAX_LINE);

	// Send the file, one chunk at a time
	while(filechunklength)
    {
        //bzero(buf, MAX_LINE);
		//filechunklength = read(fd, buf, MAX_LINE);
		printf("%d\n", filechunklength);
        if(FALSE == filechunklength)
        {
            ErrorReport(FILE_READ_ERR);
			return FALSE;
        }
		compresslength = filechunklength;
		
		result = compress(compressedbuf, &compresslength, (const Bytef*)buf, filechunklength);
		printf("%d\n", result);
		printf("%d\n", compresslength);
		#if 0
		for(i = 0; i < filechunklength; i++)
		{
			printf("%x", buf[i]);
			printf("%x\n", compressedbuf[i]);
		}
		#endif
	
        //Send the chunck
        for(i = 0; i < compresslength; i += sendlength)
        {
            sendlength = send(socket, compressedbuf + i, compresslength - i, 0);
	        if(FALSE == sendlength)
	        {
	            ErrorReport(FILE_SEND_ERR);
				return FALSE;
	        }
        }
		bzero(buf, MAX_LINE);
	    filechunklength = read(fd, buf, MAX_LINE);
    }//while(filechunklength);                
    close(fd);
           
   return TRUE;
        
}

int MD5Check(int client_fd, int md5_num)
{
	char buf[MAX_LINE];
	return 0;
}

int FileCompress()
{
	
	return 0;
}
