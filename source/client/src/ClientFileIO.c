/*
 *This c file defines file processing functions in client application.
 */


#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

// Global variable
//const char *clientfilename = "client.txt";
const char *clientfilename = "client.txt";
const char *compressedfilename = "client.gz";
const char *bitmap = "bitmap";
const char *no_file = "local file not exist!";
int bitmaplength;


/*-----------------------------------------------------
 *function name: GetFileSize
 *input:             file name
 *output:           file size
 *This function get the file size
 *-----------------------------------------------------
 */

int GetFileSize(const char *file_name) 
{
       struct stat buf;
       if (stat(file_name, &buf) != 0 ) 
       		return 0;
       return( buf.st_size );
}


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
	}
	close(fd);
	gzclose(fc);
	return TRUE;

}

/*-----------------------------------------------------
 *function name: SendLocalFileStatus
 *input:             socket descriptor
 *output:           file size
 *This function send local file status: not exist or very small / md5 
 *data of every chunck of file
 *-----------------------------------------------------
 */

int SendLocalFileStatus(int socket)
{
	int file_size;

	// Get client file size.
	file_size = GetFileSize(clientfilename);
	printf("File size = %u\n", file_size);

    if( file_size <= MAX_LINE)
	{
		// No such file, or file size is smaller than one chunck, then send "local file not exist!"
		if(FALSE == send(socket, no_file , strlen(no_file) + 1, 0))
		{
			// Send error
		    ErrorReport(SEND_DATA_ERR);
			return FALSE;
		}
	}
	else
	{		
		// Incremental updata, send MD5 value of every chunks to server 
		int i;
		int fd;
		char data_buf[MAX_LINE];
		int readlength;
		int sendlength;

		// Send MD5 length
		int md5datalength = ((file_size / MAX_LINE) + 1) * 16;
		printf("Begin to send MD5 data length, md5datalength = %d\n", md5datalength);
		sprintf(data_buf, "%d", md5datalength);
		if(FALSE == send(socket, data_buf , strlen(data_buf) + 1, 0))
		{
		    ErrorReport(SEND_DATA_ERR);
			return FALSE;
		}

		// Receive bitmaplength from server
		readlength = recv(socket, data_buf, MAX_LINE, 0);
	 	if(FALSE == readlength)
		{
			// Failed to receive
			ErrorReport(RECEIVE_DATA_ERR);
			return FALSE;
		}		
		bitmaplength = (int)data_buf[0];	
		printf("Received bitmaplength = %d\n", bitmaplength);
		unsigned char *md5_data_buf = malloc(md5datalength);
		bzero(md5_data_buf, md5datalength);
		
		// Open the file
		fd = open(clientfilename, O_RDONLY);
		if(fd < 0)
		{
			ErrorReport(FILE_OPEN_ERR);
			close(fd);
	        free(md5_data_buf);
			return FALSE;
		}

		// Caculate MD5 data for each chunck 
	    for(i = 0; i < md5datalength; i += 16)
	    {
	    	bzero(data_buf, MAX_BUFFER_SIZE);
	        readlength = read(fd, data_buf, MAX_BUFFER_SIZE);
	        if(FALSE == readlength)
	        {
	            ErrorReport(FILE_READ_ERR);
				close(fd);
	    		free(md5_data_buf);
	            return FALSE;
	        }
	        MD5((unsigned char*)data_buf, readlength, md5_data_buf + i);
	    }

	    // Send MD5 data to server
	    printf("Begin to send MD5 data, md5datalength = %d\n", md5datalength);
        sendlength = send(socket, md5_data_buf, md5datalength, 0);
        if (FALSE == sendlength)
        {
        	close(fd);
    		free(md5_data_buf);
            ErrorReport(FILE_WRITE_ERR);
            return FALSE;
        }

		close(fd);
		free(md5_data_buf);
	  
	}
	return file_size;
}
 
 /*-----------------------------------------------------
  *function name:  ReceiveBitmapData
  *input:			socket descriptor
  *output:		status, 0-success, -1-fail
  *This function receive bitmap information from server
  *-----------------------------------------------------
  */

int ReceiveBitmapData(int socket)
{
	int fb;
	char data_buf[MAX_LINE];
	int readlength, writelength, Sendlength;
	char *ack = "ACK";

	fb = open(bitmap, O_RDWR | O_CREAT);
	if(fb < 0)
	{
		// Open file fail, error report
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}

	// Receive bitmap, one chunk at a time
	bzero(data_buf, MAX_LINE);
	readlength = recv(socket, data_buf, bitmaplength, 0);
	if(FALSE == readlength)
	{
		// Receive bitmap fail, error report, close files
		ErrorReport(RECEIVE_DATA_ERR);
		close(fb);
		return FALSE;
	}
	printf("%d\n", *data_buf);
	writelength = write(fb, data_buf, readlength);
	if(FALSE == writelength)
	{
		// Write original file fail, error report, close files
		ErrorReport(FILE_WRITE_ERR);
		close(fb);
		return FALSE;
	}
	close(fb);
	
	Sendlength= send(socket, ack, strlen(ack) + 1, 0);
	if(FALSE == Sendlength)
	{
		ErrorReport(SEND_DATA_ERR);
		return FALSE;
	}

	return TRUE;
}
 
 /*-----------------------------------------------------
  *function name:  ReceiveFileUpdata
  *input:			socket descriptor
  *output:		status, 0-success, -1-fail
  *This function receive file update chunks 
  *-----------------------------------------------------
  */

int ReceiveFileUpdate(int socket)
{
	int fb,fd;
	int i,j;
	int bitindex = 7;
	int bitmapindex = 0;
	char data_buf[MAX_LINE];
	char bitmap_buf[bitmaplength];
	int readlength, writelength;
	char updatestatus;

	fb = open(bitmap, O_RDONLY);
	if(fb < 0)
	{
		// Open file fail, error report
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}
	fd = open(clientfilename, O_RDWR);
	if(fd < 0)
	{
		// Open file fail, error report
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}
	// Get bitmap information
	readlength = read(fb, bitmap_buf, bitmaplength);
	if(FALSE == readlength)
	{
		// Read bitmap fail, error report, close files
		ErrorReport(RECEIVE_DATA_ERR);
		close(fb);
		close(fd);
		return FALSE;
	}
	// Incremental update, receive updated file chunks
	bzero(data_buf, MAX_LINE);
	readlength = recv(socket, data_buf, MAX_LINE, 0);
	i = 0;
	j = 7;
	while(readlength)
	{
		if(FALSE == readlength)
		{
			// Receive data, error report, close files
			ErrorReport(RECEIVE_DATA_ERR);
			close(fb);
			close(fd);
			return FALSE;
		}
		// Get bitmap status, if 1-chunk update, 0-no update
		for(i = bitmapindex; i < bitmaplength; i++)
		{
			for(j = bitindex; j >= 0; j--)
			{
				updatestatus = (bitmap_buf[i]>>j) & 0x1;
				if(1 == updatestatus)
				{
					bitmapindex = i;
					bitindex =j;
					break;
				}
			}
			if(1 == updatestatus)
			{
				break;
			}
		}
		lseek(fd, ((bitmapindex * 8) + (7 - bitindex))* MAX_LINE, SEEK_SET);
		writelength = write(fd, data_buf, readlength);
		if(FALSE == writelength)
		{
			// Write original file fail, error report, close files
			ErrorReport(FILE_WRITE_ERR);
			close(fb);
			close(fd);
			return FALSE;
		}
		// Fresh buffer, receive new chunk
		bzero(data_buf, MAX_LINE);
		readlength = recv(socket, data_buf, MAX_LINE, 0);
	}
	close(fb);
	close(fd);
	// Update finish, remove bitmap file
	remove(bitmap);
	
	return TRUE;
}
