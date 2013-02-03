#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

const char *no_file = "local file not exist!";

u_int32_t get_file_size(const char *file_name) 
{
       struct stat buf;
       if (stat(file_name, &buf) != 0 ) 
       		return 0;
       return( buf.st_size );
}

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

//
// Send local file status: not exist or very small / md5 data of every chunck of file
//
// @return file_size
//
int SendLocalFileStatus(int socket)
{
	u_int32_t file_size;

	file_size = get_file_size("client.txt");
	printf("File size = %u\n", file_size);

	if( file_size <= MAX_LINE)
	{
		// No such file, or file size is smaller than one chunck, then send "local file not exist!"
		if(FALSE == send(socket, no_file , strlen(no_file) + 1, 0))
		{
		    ErrorReport(CLIENT_SEND_DATA_ERR);
			return FALSE;
		}
	}
	else
	{		
		int i;
		int fd;
		char data_buf[MAX_LINE];
		int read_length = 0;
		int send_length = 0;

		int md5_data_length = (file_size / MAX_BUFFER_SIZE + 1) * 16;
		unsigned char *md5_data_buf = malloc(md5_data_length);
		bzero(md5_data_buf, md5_data_length);

		printf("md5_data_length = %d\n", md5_data_length);
		
		// Open the file
		fd = open("client.txt", O_RDONLY);
		if(fd < 0)
		{
			ErrorReport(FILE_OPEN_ERR);
			return FALSE;
		}

		// Caculate MD5 data for each chunck 
	    for(i = 0; i <= file_size / MAX_BUFFER_SIZE && read_length; i++)
	    {
	    	bzero(data_buf, MAX_BUFFER_SIZE);
	        read_length = read(fd, data_buf, MAX_BUFFER_SIZE);
	        if(FALSE == read_length)
	        {
	            ErrorReport(FILE_READ_ERR);
	            return FALSE;
	        }
	        MD5((unsigned char*)data_buf, read_length, md5_data_buf + i);
	    }

	    // Send MD5 data to server
	    // @todo send all md5 data in one chunck?????
	    for(i = 0; i < md5_data_length; i += send_length)
        {
	        send_length = send(socket, md5_data_buf , md5_data_length - i, 0);
	        if (FALSE == send_length)
	        {
	            ErrorReport(FILE_WRITE_ERR);
	            return FALSE;
	        }
        }  

	    close(fd);
	    free(md5_data_buf);
	}
	return file_size;
}
