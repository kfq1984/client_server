/*
 *This head file defines all the marco in server application
 */

 
/*General macro define*/ 
#define MAX_LINE            4096
#define SERVER_PORT         9237
#define TRUE                0
#define FALSE               -1
#define MAX_CONNECT_NUM     10

/*define error code*/
#define GET_SOCKET_ERR      1001
#define SOCKET_BIND_ERR     1002
#define SERVER_LISTEN_ERR   1003
#define RECV_DATA_ERR		1004
#define SEND_DATA_ERR		1005
#define CLOSE_SOCKET_ERR    1006
#define FORK_ERR			1007
#define FILE_OPEN_ERR		1008
#define FILE_READ_ERR		1009
#define FILE_SEND_ERR       1010
#define FILE_WRITE_ERR		1011