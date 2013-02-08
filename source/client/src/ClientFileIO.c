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
const char *md5 = "md5";
int bitmaplength;
char filereplaceind = 0;
unsigned long long server_file_size = 0;


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
	unsigned long long client_file_size;

	// Get client file size.
	client_file_size = GetFileSize(clientfilename);
	printf("File size = %llu\n", client_file_size);

    if( client_file_size <= MAX_LINE)
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
		int md5datalength = ((client_file_size / MAX_LINE) + 1) * 16;
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
		server_file_size = atoi(data_buf);
		printf("%llu\n", server_file_size);
		bitmaplength = ((server_file_size / MAX_LINE) /8) + 1;
		if(server_file_size < client_file_size)
		{
			filereplaceind = 1;
		}
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
	return client_file_size;
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
	int fb, fd, fn;
	int i, j;
	char data_buf[MAX_LINE];
	char bitmap_buf[bitmaplength];
	int recvlength, writelength, readlength;
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
	for(i = 0; i < bitmaplength; i++)
	{
		for(j = 7; j >= 0; j--)
		{
			updatestatus = (bitmap_buf[i]>>j) & 0x1;
			if(1 == updatestatus)
			{	
				bzero(data_buf, MAX_LINE);
				recvlength = recv(socket, data_buf, MAX_LINE, 0);
				if(FALSE == recvlength)
				{
					// Receive data, error report, close files
					ErrorReport(RECEIVE_DATA_ERR);
					close(fb);
					close(fd);
					return FALSE;
				}
				lseek(fd, ((i * 8) + (7 - j))* MAX_LINE, SEEK_SET);
				writelength = write(fd, data_buf, recvlength);
				if(FALSE == writelength)
				{
					// Write original file fail, error report, close files
					ErrorReport(FILE_WRITE_ERR);
					close(fb);
					close(fd);
					return FALSE;
				}
			}
		}
	}
	// If file in client > file in server, delete rest content
	if(1 == filereplaceind)
	{
		fn = open("tmp", O_RDWR | O_CREAT);
		lseek(fd, 0, SEEK_SET);
		bzero(data_buf, MAX_LINE);
		readlength = read(fd, data_buf, MAX_LINE);
		while(readlength)
		{	
			if(FALSE == readlength)
			{
				// read file error, error report, close files
				ErrorReport(FILE_READ_ERR);
				close(fb);
				close(fd);
				close(fn);
				return FALSE;
			}
			writelength = write(fn, data_buf, (server_file_size > readlength ? readlength : server_file_size));
			if(FALSE == writelength)
			{
				// Write tmp file fail, error report, close files
				ErrorReport(FILE_WRITE_ERR);
				close(fb);
				close(fd);
				close(fn);
				return FALSE;
			}
			server_file_size -= readlength;
			if(server_file_size < 0)
			{
				break;
			}
			bzero(data_buf, MAX_LINE);
			readlength = read(fd, data_buf, MAX_LINE);
		}
		close(fd);
		close(fn);
		remove(clientfilename);
		rename("tmp", clientfilename);
	}
	close(fb);
	if(filereplaceind != 1)
	{
		close(fd);
	}
		
	
	// Update finish, remove bitmap file
	remove(bitmap);
	
	return TRUE;
}
