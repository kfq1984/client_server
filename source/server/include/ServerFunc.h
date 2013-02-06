/*
 * This head file include all the functions in server application.
 */
 
void ErrorReport(int errcode);    				// Error report function
int SendFileData(int client_fd);					// Send file to client
int FileCompress(void);							// Compress file
int ReceiveFileStatus(int socket);				// Receive file status
int SendUpdateData(int socket);				// Send file update chunks