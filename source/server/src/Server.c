#include "Server.h"
#include "ServerFunc.h"
#include "ServerMacro.h"

int main(int argc,char **argv)
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    int result;
	int md5_num;

	//  Create and name a socket for the server.
	

    if(FALSE == (server_sockfd = socket(AF_INET,SOCK_STREAM,0)))
    {
        // Create socket failed, report error
        ErrorReport(GET_SOCKET_ERR);
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    server_len = sizeof(server_addr);

	// Socket bind

    if(FALSE == (bind(server_sockfd,(struct sockaddr *)&server_addr ,sizeof(server_addr)))) 
    {
        // Socket bind failed, report error
        ErrorReport(SOCKET_BIND_ERR);
        exit(1);
    }

	// Create a connection queue and initialize readfds to handle input from server_sockfd.

    if(FALSE == listen(server_sockfd, MAX_CONNECT_NUM))
    {
    	// Listen failed, report error
    	ErrorReport(SERVER_LISTEN_ERR);
		exit(1);
    }

	//  Now wait for clients and requests.
    while(1) 
	{
        char buf[MAX_LINE];
		int pid;
        int ret;

        printf("server waiting......\n");
        
        client_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        
        printf("accepting client on fd %d\n", client_sockfd);
        // Check if need to send file.
		ret = recv(client_sockfd, buf, MAX_LINE, 0);

		if(FALSE == ret)
        {
            ErrorReport(READ_SOCKET_ERR);
            exit(1);
        }
		printf("serving client on fd %d\n", client_sockfd);

		// create child process to send file
		pid = fork();
		if(pid < 0)
		{
			ErrorReport(FORK_ERR);
			exit(1);
		}
		else if(pid == 0)
		{
		#if 0
			close(server_sockfd);
	        if(strcmp(buf,"New file") == TRUE)
			{
				// This is the client process: sending file
				
				result = SendFileData(client_sockfd);
				if(FALSE == result)
				{
				    ErrorReport(FILE_SEND_ERR);
					exit(1);
				}
				printf("File Transfer Finished\n");
				exit(0);
			}
			else
			{
			    md5_num = buf[0];
				if(FALSE == send(client_sockfd, TRUE, 1))
		        {
		            ErrorReport(FILE_SEND_ERR);
					return FALSE;
		        };
				ret = Md5Check(client_sockfd, md5_num);
				if(FALSE == ret)
		        {
		            ErrorReport(READ_SOCKET_ERR);
		            exit(1);
		        }
				
			}
		#endif
			// This is the client process: sending file
			
			result = SendFileData(client_sockfd);
			if(FALSE == result)
			{
			    ErrorReport(FILE_SEND_ERR);
				exit(1);
			}
			printf("File Transfer Finished\n");
			exit(0);
		}
		else
		{
			close(client_sockfd);
		}

    }
}
