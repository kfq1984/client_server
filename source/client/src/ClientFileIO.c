#include "Client.h"
#include "ClientFunc.h"
#include "ClientMacro.h"

const char *file_name = "client.txt";
const char *no_file = "New File";
const char *response_ok = "OK";

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
	fd = open(file_name, O_RDWR | O_CREAT);
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
        bzero(buf, MAX_LINE);
        recv_length = recv(socket, buf, MAX_LINE, 0);
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

	file_size = get_file_size(file_name);
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

		int md5_num = file_size / MAX_LINE + 1;
		int md5_data_length = md5_num * 16;
		unsigned char *md5_data_buf = malloc(md5_data_length);
		bzero(md5_data_buf, md5_data_length);

		printf("md5_num = %d\n", md5_num);
		printf("md5_data_length = %d\n", md5_data_length);
		
		// Send the md5 number
		printf("Send md5_num to server.....\n");
		sprintf(data_buf, "%d", md5_num);
		send_length = send(socket, data_buf, 1, 0);
        if (FALSE == send_length)
        {
            ErrorReport(CLIENT_SEND_DATA_ERR);
            return FALSE;
        }
        bzero(data_buf, MAX_LINE);

        // Recv server's response
        read_length = recv(socket, data_buf, MAX_LINE, 0);
        if(FALSE == read_length)
        {
        	ErrorReport(RECEIVE_DATA_ERR);
        	return FALSE;
        }
        if(strcmp(data_buf, response_ok) != 0)
        {
        	ErrorReport(SERVER_RESPONSE_ERR);
        	return FALSE;
        }

        printf("Receive server response for md5 number --- Success\n");

		// Open the file
		fd = open(file_name, O_RDONLY);
		if(fd < 0)
		{
			ErrorReport(FILE_OPEN_ERR);
			return FALSE;
		}

		printf("Calculate MD5 data....");
		// Caculate MD5 data for each chunck 
	    for(i = 0; i <= file_size / MAX_LINE && read_length; i++)
	    {
	    	bzero(data_buf, MAX_LINE);
	        read_length = read(fd, data_buf, MAX_LINE);
	        if(FALSE == read_length)
	        {
	            ErrorReport(FILE_READ_ERR);
	            return FALSE;
	        }
	        MD5((unsigned char*)data_buf, read_length, md5_data_buf + i);
	    }

	    printf("Send MD5 data....");
	    // Send MD5 data to server
	    for(i = 0; i < md5_data_length; i += send_length)
        {
        	printf("Send MD5 data... Time %d\n", i);
	        send_length = send(socket, md5_data_buf + i, md5_data_length - i, 0);
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

//
// Use diff bitmap to updata local file.
//
int HandleDiffData(int socket, int file_size)
{
	int fd;
	char data_buf[MAX_LINE];
	
	int md5_num = file_size / MAX_LINE + 1;
	char *diff_bitmap = malloc(md5_num/8 + 1);

	// Receive the file diff bitmap
    if(FALSE == recv(socket, diff_bitmap, sizeof(diff_bitmap), 0))
    {
    	ErrorReport(RECEIVE_DATA_ERR);
    	return FALSE;
    }

    // Check if there is difference
    bzero(data_buf, MAX_LINE);
    if( memcmp(diff_bitmap, data_buf, sizeof(diff_bitmap)) == 0)
	{
		// No difference 
		printf("Local file already up-to-date\n");
		return TRUE;
	}

    // Send response to sever
    if(FALSE == send(socket, response_ok , strlen(response_ok) + 1, 0))
	{
	    ErrorReport(CLIENT_SEND_DATA_ERR);
		return FALSE;
	}

	// Open file
	fd = open(file_name, O_RDWR);
	if(fd < 0)
	{
		ErrorReport(FILE_OPEN_ERR);
		return FALSE;
	}
	// Receive file data, one chunck at a time. Then update the local file according to diff bitmap
	int i, j;
	char diff;
	for(i = 0; i < sizeof(diff_bitmap); i++)
	{
		int recv_length = 0;
		int write_length = 0;
		diff = diff_bitmap[i];
		for(j = 0; j < 8; j++)
		{
			if((diff>>(7-j)) & 0x1 )
			{
				int pos = i * 8 + j;
				// recv one chunck data from server
				bzero(data_buf, MAX_LINE);
		        recv_length = recv(socket, data_buf, MAX_LINE, 0);
		        if(FALSE == recv_length)
		        {
		            ErrorReport(RECEIVE_DATA_ERR);
		            return FALSE;
		        }
		        // Set file position
		        if( FALSE == lseek(fd, pos, 0))
		        {
		        	ErrorReport(FILE_LSEEK_ERR);
		        	return FALSE;
		        }
		        // write data to file
		        for(i = 0; i < recv_length; i += write_length)
		        {
			        write_length = write(fd, data_buf + i, recv_length - i);
			        if (FALSE == write_length)
			        {
			            ErrorReport(FILE_WRITE_ERR);
			            return FALSE;
			        }
		        }  
			}
		}
	}


    free(diff_bitmap);
    close(fd);

    return TRUE;
}
