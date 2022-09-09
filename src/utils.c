/*******************************************************************************************
* FILE: utils.c
* AUTHORS: Cesar Ramirez & Pedro Urbina
* DESCRITPTION: Function wrapping in order to avoid constant error checking.
*******************************************************************************************/

/* All defines, data structure definition and constant definition is stored in utils.h */
#include "../includes/utils.h"


/*******************************************************************************************
* FUNCTION: int Close(int fd)
* DESCRITPTION: Close the requested file descriptor, showing error message in case of error.
* ARGS_IN: int fd - file descriptor to close
* ARGS_OUT: -1 in case of error, 0 otherwise
*******************************************************************************************/
int Close(int fd) {
		if (close(fd) == -1) {
				perror("close error");
				return -1;
		}
		return 0;
}

/*******************************************************************************************
* FUNCTION: void Pthread_create(pthread_t *tid, const pthread_attr_t *attr,
              void * (*func)(void *), void *arg)
* DESCRITPTION: Creates a thread or exits in case of failure.
* ARGS_IN: pthread_t *tid, void * (*func)(void *), void *arg - arguments for the
* 				 pthread_create function.
* ARGS_OUT: exit in case of failure
*******************************************************************************************/
void Pthread_create(pthread_t *tid, void * (*func)(void *), void *arg) {
		int n;
		if ((n = pthread_create(tid, NULL, func, arg)) == 0) return;
		errno = n;
		perror("pthread_create error");
		exit(EXIT_FAILURE);
}

/*******************************************************************************************
* FUNCTION: Pthread_cancel(pthread_t tid)
* DESCRITPTION: Cancels a thread and prints error in case of error.
* ARGS_IN: pthread_t tid - id of the thread to cancel
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_cancel(pthread_t tid) {
		if (pthread_cancel(tid) != 0) {
				fprintf(stderr, "Error when canceling thread.\n");
		}
}

/*******************************************************************************************
* FUNCTION: void Pthread_mutex_lock(pthread_mutex_t *mptr)
* DESCRITPTION: Locks the mutex and prints error in case of error.
* ARGS_IN: pthread_mutex_t *mptr - mutex to lock
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_mutex_lock(pthread_mutex_t *mptr) {
		int n;
		if ((n = pthread_mutex_lock(mptr)) == 0) return;
		errno = n;
		perror("pthread_mutex_lock error");
		exit(EXIT_FAILURE);
}

/*******************************************************************************************
* FUNCTION: void Pthread_detach(pthread_t thread)
* DESCRITPTION: detatches a thread and prints error in case of error.
* ARGS_IN: pthread_t thread - thread to detach
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_detach(pthread_t thread) {
		int n;
		if((n = pthread_detach(thread)) != 0) {
				fprintf(stderr, "pthread_detach errror, error code: %i\n", n);
				exit(EXIT_FAILURE);
		}
}

/*******************************************************************************************
* FUNCTION: void Pthread_mutex_unlock(pthread_mutex_t *mptr)
* DESCRITPTION: Unlocks the mutex and prints error in case of error.
* ARGS_IN: pthread_mutex_t *mptr - mutex to unlock
* ARGS_OUT: None.
*******************************************************************************************/
void Pthread_mutex_unlock(pthread_mutex_t *mptr) {
		int n;
		if ( (n = pthread_mutex_unlock(mptr)) == 0)
				return;
		errno = n;
		perror("pthread_mutex_unlock error");
		exit(EXIT_FAILURE);
}

/*******************************************************************************************
* FUNCTION: int Socket(int domain, int type, int protocol)
* DESCRITPTION: Calls socket function and prints error in case of error.
* ARGS_IN: same as socket function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Socket(int domain, int type, int protocol) {
		int listenfd;
		if ((listenfd = socket(domain, type, protocol)) < 0) {
				perror("socket error");
				exit(EXIT_FAILURE);
		}
		return listenfd;
}

/*******************************************************************************************
* FUNCTION: int Setsockopt(int socket, int level, int option_name, const void *option_value,
*							socklen_t option_len)
* DESCRITPTION: Calls setsockopt function and prints error in case of error.
* ARGS_IN: same as setsockopt function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len) {
		int listenfd;
		if ((listenfd = setsockopt(socket, level, option_name, option_value, option_len)) < 0) {
				perror("setsockopt fail");
				exit(EXIT_FAILURE);
		}
		return listenfd;
}

/*******************************************************************************************
* FUNCTION: void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
* DESCRITPTION: Calls bind function and prints error in case of error.
* ARGS_IN: same as bind function.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
		if (bind(sockfd, addr, addrlen) < 0) {
				perror("bind error");
				exit(EXIT_FAILURE);
		}
}

/*******************************************************************************************
* FUNCTION: void Listen(int sockfd, int backlog)
* DESCRITPTION: Calls listen function and prints error in case of error.
* ARGS_IN: same as listen function.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Listen(int sockfd, int backlog) {
		if (listen(sockfd, backlog) < 0) {
				perror("listen error");
				exit(EXIT_FAILURE);
		}
}

/*******************************************************************************************
* FUNCTION: int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
* DESCRITPTION: Calls accept function and prints error in case of error.
* ARGS_IN: same as accept function.
* ARGS_OUT: exits in case of error and returns the file descriptor otherwise.
*******************************************************************************************/
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
		int desc;
		if ((desc = accept(sockfd, addr, addrlen)) < 0) {
				perror("accept error");
		}
		return desc;
}

/*******************************************************************************************
* FUNCTION: void Signal(int signo, struct sigaction act)
* DESCRITPTION: Calls sigaction function and prints error in case of error.
* ARGS_IN: same as accept function but oact is ignored.
* ARGS_OUT: exits in case of error.
*******************************************************************************************/
void Signal(int signo, struct sigaction act) {
		struct sigaction oact;

		if (sigaction(signo, &act, &oact) < 0) {
				perror("sigaction error");
				exit(EXIT_FAILURE);
		}
}

/*******************************************************************************************
* FUNCTION: void Sigsuspend(int signal)
* DESCRITPTION: Calls sigsuspend function blocking every signal except the one passed as
*								paramenter and prints error in case of error.
* ARGS_IN: int signal - the only signal not to be ignored while the sleep.
* ARGS_OUT: None.
*******************************************************************************************/
void Sigsuspend(int signal) {
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, signal);
		sigsuspend(&mask);
		if(errno != EINTR) {
				perror("sigsuspend error");
		}
}
