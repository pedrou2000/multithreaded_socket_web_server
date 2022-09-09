/*******************************************************************************************
* FILE: http.h
* AUTHORS: Cesar Ramirez & Pedro Urbina
* DESCRITPTION: Management and handling of http requests and replies.
*******************************************************************************************/

#ifndef _HTTP_H
#define _HTTP_H

/* All defines, data structure definition and constant definition is stored in utils.h */
#include "utils.h"
#include "../srclib/picohttpparser.h"


/*******************************************************************************************
* FUNCTION: int process_http_request(int desc, char * server_root, char * server_signature)
* DESCRITPTION: Receive a request from the descriptor, parse it and answer it depending on
* 							if it is a POST, GET or OPTIONS request.
* ARGS_IN: int desc - descriptor through where request will be read and the the reply will be sent
* 				 char * server_root - string containing the path where the server's files are stored
* 				 char * server_signature - string containing the server's signature, to be
* 																used as the Server header
* ARGS_OUT: -1 in case connection has ended and 0 otherwise
*******************************************************************************************/
int process_http_request(int desc, char * server_root, char * server_signature);

#endif
