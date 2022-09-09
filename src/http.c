/*******************************************************************************************
* FILE: http.c
* AUTHORS: Cesar Ramirez & Pedro Urbina
* DESCRITPTION: Management and handling of http requests and replies.
*******************************************************************************************/

/* All defines, data structure definition and constant definition is stored in utils.h */
#include "../includes/http.h"


/*******************************************************************************************
* FUNCTION: void print_request(Request r)
* DESCRITPTION: Prints the fields of a Request structure.
* ARGS_IN: Request r - Request structure whose fields are to be printed
* ARGS_OUT: None
*******************************************************************************************/
void print_request(Request r) {
		printf("PRINTING REQUEST: \n\n");
		printf("method name: %s\n", r.method);
		printf("path name: %s\n", r.path);
		printf("number of headers: %d\n", r.num_headers);
		printf("length of the request: %d\n", r.length);
		printf("arguments in post: %s\n", r.args);
		for(int i = 0; i < r.num_headers; i++) {
				printf("header %d:\n", i);
				printf("header name: '%s', header content: '%s'\n", r.headers[i].name, r.headers[i].value);
		}
}

/*******************************************************************************************
* FUNCTION: void get_time(char* s)
* DESCRITPTION: Writes the actual time on a string.
* ARGS_IN: char* s - string where the time is going to be written
*                    Format example: Mon, 17 May 2021 14:29:48 GMT
* ARGS_OUT: None
*******************************************************************************************/
void get_time(char* s) {
		time_t now = time(0);
		struct tm tm = *gmtime(&now);
		strftime(s, SMALL_STRING_SIZE, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

/*******************************************************************************************
* FUNCTION: int get_content_type(char * path, char * res)
* DESCRITPTION: Writes the content type on a string given the path to the file
* ARGS_IN: char* path - path of the file from where we are getting the type
*					 char * res - where the resulting type will be written
* ARGS_OUT: -1 in case of error or unknown type, 1 in case of python scrypt, 2 in case of
* 					php script, 0 otherwise
*******************************************************************************************/
int get_content_type(char * path, char * res) {
		if (strstr(path, ".txt")) {
				strcpy(res, "text/plain");
		} else if (strstr(path, ".html") || strstr(path, ".htm")) {
				strcpy(res, "text/html");
		} else if (strstr(path, ".py")) {
				strcpy(res, "text/html");
				return PYTHON_SCRIPT;
		} else if (strstr(path, ".php")) {
				strcpy(res, "text/html");
				return PHP_SCRIPT;
		} else if (strstr(path, ".gif")) {
				strcpy(res, "image/gif");
		} else if (strstr(path, ".jpeg") || strstr(path, ".jpg")) {
				strcpy(res, "image/jpeg");
		} else if (strstr(path, ".mpeg") || strstr(path, ".mpg")) {
				strcpy(res, "video/mpeg");
		} else if (strstr(path, ".avi")) {
				strcpy(res, "video/avi");
		} else if (strstr(path, ".mov")) {
				strcpy(res, "video/mov");
		} else if (strstr(path, ".doc") || strstr(path, ".docx")) {
				strcpy(res, "application/msword");
		} else if (strstr(path, ".pdf")) {
				strcpy(res, "application/pdf");
		} else {
				// unknown type
				return ERROR;
		}

		return OK;
}

/*******************************************************************************************
* FUNCTION: void get_content_lenght_and_last_modified(char* path, long * size,
*						char * last_modified)
* DESCRITPTION: Writes the content date of the last modification and the size of a given file.
* ARGS_IN: char* path - path of the file
*					 char* res - where the resulting length of the file will be written
*          char * last_modified - where the date of the last modification is stored
* ARGS_OUT: none
*******************************************************************************************/
void get_content_lenght_and_last_modified(char* path, long * size, char * last_modified) {
		struct stat st;
		stat(path, &st);
		*size =  st.st_size;
		strftime(last_modified, SMALL_STRING_SIZE, "%a, %d %b %Y %H:%M:%S %Z", gmtime(&st.st_mtime));
}

/*******************************************************************************************
* FUNCTION: void clean_and_close(int desc, Request * request)
* DESCRITPTION: Closes connection in case there was a connection close header and dellocates
* 							a Request structure.
* ARGS_IN: int desc - descriptor to close in case of connection close
*					 Request * request - request to be freed
* ARGS_OUT: none
*******************************************************************************************/
void clean_and_close(int desc, Request * request) {
		// check not NULL already
		if (request) {
				if (request->connection_close == TRUE) {
						// the client sent a connection close in their request
						//printf("Connection close received.\n");
						Close(desc);
				}
				if (request->headers) free(request->headers);
				free(request);
		}
}


/*******************************************************************************************
* FUNCTION: void send_200_ok(int desc, int version, char * content_type, long content_len,
*						char * date, char * last_modified, char * server_signature)
* DESCRITPTION: Sends a 200 OK reply to the through the specified descriptor given the
* 							arguments to be written in the headers of the response
* ARGS_IN: int desc - descriptor through where the the reply will be sent
*					 int version - http version to be written in the header
* 				 char * content_type - content type to be witten in the Content-Type header
* 				 long content_len - length of the file, to be witten in the Content-Length header
* 				 char * date - string containing the date to be used as the Date header
* 				 char * last_modified - string containing the date of the last modification,
* 																to be used as the Last-Modified header
* 				 char * server_signature - string containing the server's signature, to be
* 																	 used as the Server header
* ARGS_OUT: none
*******************************************************************************************/
void send_200_ok(int desc, int version, char * content_type, long content_len, char * date, char * last_modified, char * server_signature) {
		char buffer[LARGE_STRING_SIZE];
		int ret;

		// write the request on the buffer
		ret = sprintf(buffer, "HTTP/1.%d 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld"
		              "\r\nDate: %s\r\nServer: %s\r\nLast-Modified: %s\r\n\r\n",
		              version, content_type, content_len, date, server_signature, last_modified);
		if (ret < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				return;
		}

		// send the request through the given descriptor
		ret = send(desc, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0) {
				fprintf(stderr, "ERROR: send failed.\n");
		}
}

/*******************************************************************************************
* FUNCTION: void send_200_ok_options(int desc, int version, char * date,
* 					char * server_signature)
* DESCRITPTION: Sends a 200 OK, options reply to the through the specified descriptor given
* 							the	arguments to be written in the headers of the response. The allow header
* 							is always "Allow: GET, POST, OPTIONS".
* ARGS_IN: int desc - descriptor through where the the reply will be sent
*					 int version - http version to be written in the header
* 				 char * date - string containing the date to be used as the Date header
* 				 char * server_signature - string containing the server's signature, to be
* 																	 used as the Server header
* ARGS_OUT: none
*******************************************************************************************/
void send_200_ok_options(int desc, int version, char * date, char * server_signature) {
		char buffer[LARGE_STRING_SIZE];

		// write the request on the buffer
		int ret = sprintf(buffer, "HTTP/1.%d 200 OK\r\nContent-Length: 0\r\nDate: %s\r\nServer: %s\r\n"
		                  "Allow: GET, POST, OPTIONS\r\n", version, date, server_signature);
		if (ret < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				return;
		}

		// send the request through the given descriptor
		ret = send(desc, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0) {
				fprintf(stderr, "ERROR: send failed.\n");
		}
}

/*******************************************************************************************
* FUNCTION: void send_400_bad_request(int desc, int version, char * date,
* 					char * server_signature)
* DESCRITPTION: Sends a 400 bad request reply to the through the specified descriptor given
* 							the arguments to be written in the headers of the response
* ARGS_IN: int desc - descriptor through where the the reply will be sent
*					 int version - http version to be written in the header
* 				 char * date - string containing the date to be used as the Date header
* 				 char * server_signature - string containing the server's signature, to be
* 																	 used as the Server header
* ARGS_OUT: none
*******************************************************************************************/
void send_400_bad_request(int desc, int version, char * date, char * server_signature) {
		printf("Bad request!, error 400 sent.\n");

		char buffer[LARGE_STRING_SIZE];
		int ret;
		char error_400[MEDIUM_STRING_SIZE] = "<html><b>400 Bad Request</b></html>";

		// write the request on the buffer
		ret = sprintf(buffer, "HTTP/1.%d 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: %ld"
		              "\r\nDate: %s\r\nServer: %s\r\n\r\n%s",
		              version, strlen(error_400), date, server_signature, error_400);
		if (ret < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				return;
		}

		// send the request through the given descriptor
		send(desc, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0) {
				fprintf(stderr, "ERROR: send failed.\n");
		}
}

/*******************************************************************************************
* FUNCTION: void send_404_not_found(int desc, int version, char * date,
* 					char * server_signature)
* DESCRITPTION: Sends a 404 not found reply to the through the specified descriptor given
* 							the arguments to be written in the headers of the response
* ARGS_IN: int desc - descriptor through where the the reply will be sent
*					 int version - http version to be written in the header
* 				 char * date - string containing the date to be used as the Date header
* 				 char * server_signature - string containing the server's signature, to be
* 																	 used as the Server header
* ARGS_OUT: none
*******************************************************************************************/
void send_404_not_found(int desc, int version, char * date, char * server_signature) {
		printf("ERROR: not found!, error 404.\n");

		char buffer[LARGE_STRING_SIZE];
		int ret;
		char error_404[MEDIUM_STRING_SIZE] = "<html><b>404 Not Found</b></html>";

		// write the request on the buffer
		ret = sprintf(buffer, "HTTP/1.%d 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: %ld"
		              "\r\nDate: %s\r\nServer: %s\r\n\r\n%s",
		              version, strlen(error_404), date, server_signature, error_404);
		if (ret < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				return;
		}

		// send the request through the given descriptor
		send(desc, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0) {
				fprintf(stderr, "ERROR: send failed.\n");
		}
}

/*******************************************************************************************
* FUNCTION: void send_500_server_error(int desc, int version, char * date,
* 					char * server_signature)
* DESCRITPTION: Sends a 500 server error reply to the through the specified descriptor given
* 							the arguments to be written in the headers of the response
* ARGS_IN: int desc - descriptor through where the the reply will be sent
*					 int version - http version to be written in the header
* 				 char * date - string containing the date to be used as the Date header
* 				 char * server_signature - string containing the server's signature, to be
* 																	 used as the Server header
* ARGS_OUT: none
*******************************************************************************************/
void send_500_server_error(int desc, int version, char * date, char * server_signature) {
		printf("ERROR: Internal server error!, sent 500 message.\n");

		char buffer[LARGE_STRING_SIZE];
		int ret;
		char error_500[MEDIUM_STRING_SIZE] = "<html><b>500 internal server error</b></html>";

		// write the request on the buffer
		ret = sprintf(buffer, "HTTP/1.%d 500 Server Error\r\nContent-Type: text/html\r\nContent-Length: %ld"
		              "\r\nDate: %s\r\nServer: %s\r\n\r\n%s",
		              version, strlen(error_500), date, server_signature, error_500);
		if (ret < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				return;
		}

		// send the request through the given descriptor
		send(desc, buffer, strlen(buffer), MSG_NOSIGNAL);
		if (ret < 0) {
				fprintf(stderr, "ERROR: send failed.\n");
		}

}


/*******************************************************************************************
* FUNCTION: Request* get_and_parse_request(int desc, char * date, char * server_signature)
* DESCRITPTION: Reads a request from the descriptor, parses it and saves the information
* 							in a Request structure.
* ARGS_IN: int desc - descriptor through where request will be read and the the reply will be sent
* 				 char * date - string containing the date to be used as the Date header in case of
*												 internal server error
* 				 char * server_signature - string containing the server's signature, to be
* 																used as the Server header in case of internal server error
* ARGS_OUT: Request structure containg all the important information got from the request
*******************************************************************************************/
Request* get_and_parse_request(int desc, char * date, char * server_signature) {
		char buf[4096], *method, *path;
		int pret, minor_version;
		struct phr_header headers[100];
		size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
		ssize_t rret;

		/* parse request using picohttpparser and its example of use in github */
		while (1) {
				/* read the request */
				while ((rret = read(desc, buf + buflen, sizeof(buf) - buflen)) == -1 && errno == EINTR);

				if (rret == 0) {
						// if server has read nothing it means the client has closed the connection
						return NULL;
				} else if (rret < 0) {
						// if it has read a negative number of bytes there has been an error on the server
						fprintf(stderr, "ERROR: read from descriptor returned negative number.\n");
						send_500_server_error(desc, 1, date, server_signature);
						return NULL;
				}

				prevbuflen = buflen;
				buflen += rret;
				/* parse the request */
				num_headers = sizeof(headers) / sizeof(headers[0]);
				pret = phr_parse_request(buf, buflen, (const char**) &method, &method_len,
				                         (const char**) &path, &path_len, &minor_version,
				                         headers, &num_headers, prevbuflen);
				if (pret > 0) {
						// successfully parsed the request, exit the loop
						break;
				} else if (pret == -1) {
						// if there has been an error on phr_parse_request send a 500 message
						fprintf(stderr, "ERROR: Error on phr_parse_request.\n");
						send_500_server_error(desc, 1, date, server_signature);
						return NULL;
				}
				// else request is incomplete, continue the loop, unless the buffer is full
				if (buflen == sizeof(buf)) {
						fprintf(stderr, "ERROR: Request does not fit in the buffer.\n");
						send_500_server_error(desc, 1, date, server_signature);
						return NULL;
				}
		}

		/* allocating memory for our data structure containing the request */
		Request* request = NULL;
		request = calloc(1, sizeof(Request));
		if(request == NULL) {
				fprintf(stderr, "Error when allocating memory for request.\n");
				send_500_server_error(desc, 1, date, server_signature);
				return NULL;
		}

		/* save the obtained values in our data structure */
		if (sprintf(request->method, "%.*s", (int)method_len, method) < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				free (request);
				send_500_server_error(desc, 1, date, server_signature);
				return NULL;
		}
		if (sprintf(request->path, "%.*s", (int)path_len, path) < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				free (request);
				send_500_server_error(desc, 1, date, server_signature);
				return NULL;
		}
		request->version = minor_version;
		request->num_headers = num_headers;
		request->length = pret;
		request->has_args = FALSE;
		request->connection_close = FALSE;

		/* store the post and / or get arguments */
		request->args[0] = '\0'; // in order to use strcat directly
		if ((strcmp(request->method, "POST") == 0) || (strcmp(request->method, "GET") == 0)) {
				// if it is a get or post request...
				if (strcmp(request->method, "POST") == 0) {
						if (strlen(pret+buf)) {
								// if it is a post request and it has a body save the arguments
								strcat(request->args, pret + buf);
								request->has_args = TRUE;
						} else {
								// else post without body => send bad request
								printf("POST without arguments, sending bad request\n");
								free(request);
								send_400_bad_request(desc, request->version, date, server_signature);
								return NULL;
						}
				}
				if(strstr(request->path, "?")) {
						// if it has argument in the url, obtain then with strtok
						char * token;
						token = strtok(request->path, "?");
						token = strtok(NULL, "?");

						if (token == NULL) {
								// if nothing after the "?" the request is incorrect
								printf("Nothing after '?', sending bad request\n");
								free(request);
								send_400_bad_request(desc, request->version, date, server_signature);
								return NULL;
						} else {
								// else add the arguments in the url to the request field
								if(request->has_args) {
										// add an intermidiate space if it is a post with body and arguments
										strcat(request->args, " ");
								}
								strcat(request->args, token);
								request->has_args = TRUE;
						}
				}
		}

		/* save the headers */
		request->headers = calloc(num_headers, sizeof(Header));
		if(request->headers == NULL) {
				fprintf(stderr, "ERROR: error when allocaing memory for headers\n");
				free(request);
				send_500_server_error(desc, 1, date, server_signature);
				return NULL;
		}

		for(int i = 0; i < num_headers; i++) {
				sprintf(request->headers[i].name, "%.*s", (int)headers[i].name_len, headers[i].name);
				sprintf(request->headers[i].value, "%.*s", (int)headers[i].value_len, headers[i].value);
				request->headers[i].name_len =  (int)headers[i].name_len;
				request->headers[i].value_len =  (int)headers[i].value_len;
				if (strstr(request->headers[i].name, "Connection")) {
						if (strstr(request->headers[i].value, "Close")) {
								// if we have received a Connection: close save it
								request->connection_close = TRUE;
						}
				}
		}

		return request;
}


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
int process_http_request(int desc, char * server_root, char * server_signature) {
		char buffer[LARGE_STRING_SIZE];
		int ret;
		char date[SMALL_STRING_SIZE];
		get_time(date);

		// first of all parse the request in order to have the information correctly stored in the
		// the data structure
		Request *request = get_and_parse_request(desc, date, server_signature);
		if(request == NULL) {
				// if we have not been able to parse the request close the descriptor and inform with return
				Close(desc);
				return END_OF_CONNECTION;
		}

		/* obtain the final path concatenating the server_root and the path of the request */
		char final_file_path[MEDIUM_STRING_SIZE];
		if (sprintf(final_file_path, "%s%s", server_root, request->path) < 0) {
				fprintf(stderr, "ERROR: sprintf failed.\n");
				send_500_server_error(desc, request->version, date, server_signature);
				clean_and_close(desc, request);
				return OK;
		}

		/* obtain the content type using the function designed for that */
		char content_type[SMALL_STRING_SIZE];
		int script;
		if((script = get_content_type(request->path, content_type)) == ERROR) {
				printf("Not supported type of file.\n");
				send_400_bad_request(desc, request->version, date, server_signature);
				clean_and_close(desc, request);
				return OK;
		}

		/* NON SCRIPT GET CASE */
		if ((strcmp(request->method, "GET") == 0) && (script == NON_SCRIPT)) {
			  /* get case and not a script */
				printf("Received GET request!\n");


				if(request->has_args == TRUE) {
						/* if it has arguments then the request is incorrect, as it should be a script */
						printf("Get with parameters but not srcipt!\n");
						send_400_bad_request(desc, request->version, date, server_signature);
						clean_and_close(desc, request);
						return OK;
				}

				/* open the desired resource in order to be sent */
				int file = open(final_file_path, O_RDONLY);
				if(file == -1) {
						/* if requested file is not oppened is because it does not exist */
						fprintf(stderr, "ERROR: requested file not found, %s\n", final_file_path);
						send_404_not_found(desc, request->version, date, server_signature);
						clean_and_close(desc, request);
						return OK;
				}

				/* the length of the file and the last modified time are obtained */
				long file_len;
				char last_modified[SMALL_STRING_SIZE];
				get_content_lenght_and_last_modified(final_file_path, &file_len, last_modified);

				/* the request headers are sent with the previously obtained information */
				send_200_ok(desc, request->version, content_type, file_len, date, last_modified, server_signature);

				/* the contents of the file are sent */
				while ((ret = read(file, buffer, LARGE_STRING_SIZE)) > 0) {
						if (send(desc, buffer, ret, MSG_NOSIGNAL) == -1) {
								perror("send");
								fprintf(stderr, "ERROR: send failed.\n");
						}
				}

				/* the file descriptor is closed */
				Close(file);


		/* SCRIPT CASE: GET OR POST */
		} else if (((strcmp(request->method, "POST") == 0) || (strcmp(request->method, "GET") == 0)) &&
		           ((script == PHP_SCRIPT) || (script == PYTHON_SCRIPT))) {

				/* the file in the url is a script and the method is get or post */
				printf("Received POST request or GET with args!\n");

				if(script == PHP_SCRIPT) {
						/* if it is a php write in the buffer the command to be executed by popen */
						if (sprintf(buffer, "php %s %s", final_file_path, request->args) < 0) {
								fprintf(stderr, "ERROR: sprintf failed.\n");
								send_500_server_error(desc, request->version, date, server_signature);
								clean_and_close(desc, request);
								return OK;
						}
				} else if (script == PYTHON_SCRIPT) {
						/* if it is a python write in the buffer the command to be executed by popen */
						if (sprintf(buffer, "python %s %s", final_file_path, request->args) < 0) {
								fprintf(stderr, "ERROR: sprintf failed.\n");
								send_500_server_error(desc, request->version, date, server_signature);
								clean_and_close(desc, request);
								return OK;
						}
				}

				char script_output[LARGE_STRING_SIZE];
				/* clean the buffer from previous executions */
				memset(script_output, 0, LARGE_STRING_SIZE);

				/* execute the script with the arguments parsed from the request */
				FILE* pipe_desc = popen(buffer, "r");
				if(pipe_desc == NULL) {
						perror("popen");
						pclose(pipe_desc);
						fprintf(stderr, "ERROR: error when creating the pipe\n");
						send_500_server_error(desc, request->version, date, server_signature);
						clean_and_close(desc, request);
						return OK;
				}

				/* read the output of the script into another buffer, script_output */
				ret = fread(script_output, 1, LARGE_STRING_SIZE, pipe_desc);
				if(ret < 0 || strlen(script_output) == 0) {
						perror("fread");
						pclose(pipe_desc);
						fprintf(stderr, "ERROR: error when reading the output of the script!\n");
						send_500_server_error(desc, request->version, date, server_signature);
						clean_and_close(desc, request);
						return OK;
				}

				/* descriptor no longer needed */
				pclose(pipe_desc);

				/* Obtain the last modified of the script, the file_len won't be used */
				long file_len;
				char last_modified[SMALL_STRING_SIZE];
				get_content_lenght_and_last_modified(final_file_path, &file_len, last_modified);

				/* send the response headers to the client */
				send_200_ok(desc, request->version, content_type, strlen(script_output), date, last_modified, server_signature);

				/* send the output of the script to the client */
				if (send(desc, script_output, strlen(script_output), MSG_NOSIGNAL)  == -1) {
						perror("send");
						fprintf(stderr, "ERROR: send failed.\n");
				}



		/* OPTIONS CASE */
		} else if (strcmp(request->method, "OPTIONS") == 0) {
				printf("Received OPTIONS request!\n");
				/* if treceived an options request answer appropiately */
				send_200_ok_options(desc, request->version, date, server_signature);

		} else {
				printf("Unknown type of request!, %s\n", request->method);
				/* if the request is not a GET POST or OPTIONS then or the request is not
				well formed or the server cannot understand it, either way send a bad
				request reply */
				send_400_bad_request(desc, request->version, date, server_signature);
		}

		/* clean the Request structure and close the descriptor if connection close */
		clean_and_close(desc, request);

		return OK;
}
