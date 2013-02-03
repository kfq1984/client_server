/*
 *This c file defines file processing functions in server application.
 */


#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

const char *originalfilename = "test.txt";
const char *compressedfilename = "test.gz";

/*-----------------------------------------------------
 *function name: SendFileData
 *input:             socket descriptor
 *output:           status, 0-success, -1-fail
 *This function sends data to client
 *-----------------------------------------------------
 */

int SendFileData(int socket)
{
	int fd;
	int sendlength = 0;
	int i;
	int filechunklength = 0;
	char buf[MAX_LINE];


	// Open the file
	fd = open(compressedfilename, O_RDONLY);
	if(fd < 0)
	{
		// Open file fail, error report
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}	
	
	// Send the file, one chunk at a time
	bzero(buf, MAX_LINE);
	filechunklength = read(fd, buf, MAX_LINE);
	while(filechunklength)
    {
        if(FALSE == filechunklength)
        {	
        	// Read file fail, error report, close file
            ErrorReport(FILE_READ_ERR);
			close(fd);
			return FALSE;
        }

        // Use for loop to make sure the chunck has been sent completely
        for(i = 0; i < filechunklength; i += sendlength)
        {
            sendlength = send(socket, buf + i, filechunklength - i, 0);
	        if(FALSE == sendlength)
	        {
	        	// Send file error, error report, close file
	            ErrorReport(FILE_SEND_ERR);
				close(fd);
				return FALSE;
	        }
        }
		// Fresh buffer, send new chunk
		bzero(buf, MAX_LINE);
	    filechunklength = read(fd, buf, MAX_LINE);
    }              
    close(fd);
           
   return TRUE;
        
}

/*-----------------------------------------------------
 *function name: FileCompress
 *input:             void
 *output:           status, 0-success, -1-fail
 *This function compresses original file to a compressed file
 *-----------------------------------------------------
 */

int FileCompress(void)
{
	int fd;
	gzFile fc;
	char compressedbuf[MAX_LINE];
	int compresslength;
	int writelength;

	// Open original file
	fd = open(originalfilename, O_RDONLY);
	// Open and create a compressed file to save compressed content
    fc = gzopen(compressedfilename, "wb");

	// Compress file, one chunk at a time
	bzero(compressedbuf, MAX_LINE);
	compresslength = read(fd, compressedbuf, MAX_LINE);
	while(compresslength)
    {
        if(FALSE == compresslength)
        {
        	// Read original file fail, error report, close files
            ErrorReport(FILE_READ_ERR);
			close(fd);
			gzclose(fc);
			return FALSE;
        }

        writelength = gzwrite(fc, compressedbuf, compresslength);
        if(FALSE == writelength)
        {
        	// Write compressed file fail, error report, close files
            ErrorReport(FILE_WRITE_ERR);
			close(fd);
			gzclose(fc);
			return FALSE;
        }
		// Fresh buffer, compress new chunk
		bzero(compressedbuf, MAX_LINE);
	    compresslength = read(fd, compressedbuf, MAX_LINE);
    }  
    close(fd);
	gzclose(fc);
	
	return TRUE;
}
