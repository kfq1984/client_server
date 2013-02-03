/*
 *This c file defines file processing functions in client application.
 */


#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"


const char *clientfilename = "client.txt";
const char *compressedfilename = "client.gz";


/*-----------------------------------------------------
 *function name: ReceiveFileData
 *input:             socket descriptor
 *output:           status, 0-success, -1-fail
 *This function receives data from server and writes data in a file
 *-----------------------------------------------------
 */

int ReceiveFileData(int socket)
{
	int fd;
	int i;
	int recvlength = 0;
	int writelength = 0;
	char buf[MAX_LINE];


	// Open the file to save data
	fd = open(compressedfilename, O_RDWR | O_CREAT);
	if(fd < 0)
	{
		// Open file fail, error report
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}

	// Receive file data, one chunck at a time
	bzero(buf, MAX_LINE);
    recvlength = recv(socket, buf, MAX_LINE, 0);
    while(recvlength)
    {

        if(FALSE == recvlength)
        {	
        	// Receive data fail, error report, close file
            ErrorReport(RECEIVE_DATA_ERR);
			close(fd);
            return FALSE;
        }
		// Use for loop to make sure all the received data have written in file
        for(i = 0; i < recvlength; i += writelength)
        {
	        writelength = write(fd, buf + i, recvlength - i);
	        if (FALSE == writelength)
	        {
	        	// Write file fail, error report, close file
	            ErrorReport(FILE_WRITE_ERR);
				close(fd);
	            return FALSE;
	        }
        } 
		// Fresh buffer, receive new chunk
		bzero(buf, MAX_LINE);
    	recvlength = recv(socket, buf, MAX_LINE, 0);
    }
	close(fd);
   
	return TRUE;
}


/*-----------------------------------------------------
 *function name: FileDecompress
 *input:             void
 *output:           status, 0-success, -1-fail
 *This function decompresses receives data to original file
 *-----------------------------------------------------
 */


int FileDecompress(void)
{
	int fd;
	gzFile fc;
	char decompressedbuf[MAX_LINE];
	int decompresslength;
	int writelength;


	// Open file to save original file content
	fd = open(clientfilename, O_RDWR | O_CREAT);
	// Open compressed file
	fc = gzopen(compressedfilename, "rb");

	// Decompress file, one chunk at a time
	bzero(decompressedbuf, MAX_LINE);
	decompresslength = gzread(fc, decompressedbuf, MAX_LINE);
	while(decompresslength)
	{
		if(FALSE == decompresslength)
		{
			// Read compressed file fail, error report, close files
			ErrorReport(FILE_READ_ERR);
			close(fd);
			gzclose(fc);
			return FALSE;
		}

		writelength = write(fd, decompressedbuf, decompresslength);
		if(FALSE == writelength)
		{
			// Write original file fail, error report, close files
			ErrorReport(FILE_WRITE_ERR);
			close(fd);
			gzclose(fc);
			return FALSE;
		}
		// Fresh buffer, decompress new chunk
		bzero(decompressedbuf, MAX_LINE);
		decompresslength = gzread(fc, decompressedbuf, MAX_LINE);
	}//while(filechunklength);	  
	close(fd);
	gzclose(fc);
	return TRUE;

}

