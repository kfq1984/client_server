/*
 * This head file include all the functions in client application.
 */

 
void ErrorReport(int errcode);              //Error report function
int ReceiveFileData(int socket);           // Receive data from server
int FileDecompress(void);				   // Decompress file to a original file
int ReceiveBitmapData(int socket);		   // Receive bitmap information
int SendLocalFileStatus(int socket);	   // Send file status, no file or file is too small or MD5 information
int ReceiveFileUpdate(int socket);		   // Receive file update chunks and update file