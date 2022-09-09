/*******************************************************************************************
* FILE: utils.h
* AUTHORS: Cesar Ramirez & Pedro Urbina
* DESCRITPTION: Function wrapping, includes and data structures defined here.
*******************************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <confuse.h>

#include "../srclib/picohttpparser.h"

#define NON_SCRIPT 0
#define PYTHON_SCRIPT 1
#define PHP_SCRIPT 2

#define END_OF_CONNECTION -1
#define ERROR -1
#define OK 0
#define TRUE 1
#define FALSE 0

/* different sizes for buffers */
#define TINY_STRING_SIZE 31
#define SMALL_STRING_SIZE 255
#define MEDIUM_STRING_SIZE 2047
#define LARGE_STRING_SIZE 1024*8-1

#define FAM AF_INET
#define SOCK SOCK_STREAM

/* structure that stores all the information about the server configuration */
typedef struct {
		char* server_root;
		char* server_signature;
		long max_clients;
		long listen_port;
} ServerConfiguration;

/* structure that stores all the relevant information of a thread */
typedef struct {
		/* thread global identifier */
		pthread_t thread_tid;
		/* identifier only valid inside the same process */
		int thread_num;
		/* number of connections to the thread */
		long thread_count;
} Thread;

/* structure that stores all the relevant information of an http header */
typedef struct {
		/* name of the header (before the ':') */
		char name[MEDIUM_STRING_SIZE];
		/* value of the header (after the ':') */
		char value[MEDIUM_STRING_SIZE];
		int name_len;
		int value_len;
} Header;

/* structure that stores all the relevant information of an http request */
typedef struct {
		/* method type, f.e. GET, POST... */
		char method[TINY_STRING_SIZE];
		/* path to the requested file or script */
		char path[SMALL_STRING_SIZE];
		/* integer representing the http version of the request */
		int version;
		/* array of headers */
		Header* headers;
		int num_headers;
		/* length of the file to transmit */
		int length;
		/* buffer which stores all the arguments to be passed to the script, either
		in the body of the post or in the url after the '?' */
		char args[MEDIUM_STRING_SIZE];
		/* boolean representing if the request has arguments to be passed to a script */
		int has_args;
		/* boolean representing if the request contained a Connection: close header */
		int connection_close;
} Request;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/*******************************************************************************************
* FUNCTION: int Close(int fd)
* DESCRITPTION: Close the requested file descriptor, showing error message in case of error.
* ARGS_IN: int fd - file descriptor to close
* ARGS_OUT: -1 in case of error, 0 otherwise
*******************************************************************************************/
int Close(int fd);


/*******************************************************************************************
* FUNCTION: Pthread_cancel(pthread_t tid)
* DESCRITPTION: Cancels a thread and prints error in case of error.
* ARGS_IN: pthread_t tid - id of the thread to cancel
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_cancel(pthread_t tid);

/*******************************************************************************************
* FUNCTION: void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
              void * (*func)(void *), void *arg)
* DESCRITPTION: Creates a thread or exits in case of failure.
* ARGS_IN: pthread_t *tid, void * (*func)(void *), void *arg - arguments for the
* 				 pthread_create function.
* ARGS_OUT: exit in case of failure
*******************************************************************************************/
void Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg);

/*******************************************************************************************
* FUNCTION: void Pthread_mutex_lock(pthread_mutex_t *mptr)
* DESCRITPTION: Locks the mutex and prints error in case of error.
* ARGS_IN: pthread_mutex_t *mptr - mutex to lock
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_mutex_lock(pthread_mutex_t *mptr);

/*******************************************************************************************
* FUNCTION: void Pthread_mutex_unlock(pthread_mutex_t *mptr)
* DESCRITPTION: Unlocks the mutex and prints error in case of error.
* ARGS_IN: pthread_mutex_t *mptr - mutex to unlock
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_mutex_unlock(pthread_mutex_t *mptr);

/*******************************************************************************************
* FUNCTION: void Pthread_detach(pthread_t thread)
* DESCRITPTION: detatches a thread and prints error in case of error.
* ARGS_IN: pthread_t thread - thread to detach
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_detach(pthread_t thread);


/*******************************************************************************************
* FUNCTION: int Socket(int domain, int type, int protocol)
* DESCRITPTION: Calls socket function and prints error in case of error.
* ARGS_IN: same as socket function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Socket(int domain, int type, int protocol);

/*******************************************************************************************
* FUNCTION: int Setsockopt(int socket, int level, int option_name, const void *option_value,
*							socklen_t option_len)
* DESCRITPTION: Calls setsockopt function and prints error in case of error.
* ARGS_IN: same as setsockopt function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);

/*******************************************************************************************
* FUNCTION: void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
* DESCRITPTION: Calls bind function and prints error in case of error.
* ARGS_IN: same as bind function.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*******************************************************************************************
* FUNCTION: void Listen(int sockfd, int backlog)
* DESCRITPTION: Calls listen function and prints error in case of error.
* ARGS_IN: same as listen function.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Listen(int sockfd, int backlog);

/*******************************************************************************************
* FUNCTION: int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
* DESCRITPTION: Calls accept function and prints error in case of error.
* ARGS_IN: same as accept function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);


/*******************************************************************************************
* FUNCTION: void Signal(int signo, struct sigaction act)
* DESCRITPTION: Calls sigaction function and prints error in case of error.
* ARGS_IN: same as accept function but oact is ignored.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Signal(int signo, struct sigaction act);

/*******************************************************************************************
* FUNCTION: void Sigsuspend(int signal)
* DESCRITPTION: Calls sigsuspend function blocking every signal except the one passed as
*								paramenter and prints error in case of error.
* ARGS_IN: int signal - the only signal not to be ignored while the sleep.
* ARGS_OUT: None.
*******************************************************************************************/
void Sigsuspend(int signal);

#endif
