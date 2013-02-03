/*
 * This head file include all the functions in client application.
 */

 
void ErrorReport(int errcode);              //Error report function
int ReceiveFileData(int socket);           // Receive data from server
int FileDecompress(void);				   // Decompress file to a original file