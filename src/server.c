/*******************************************************************************************
* FILE: server.c
* AUTHORS: Cesar Ramirez & Pedro Urbina
* DESCRITPTION: Server configuration, initialization and thread management.
*******************************************************************************************/

/* All defines, data structure definition and constant definition is stored in utils.h */
#include "../includes/utils.h"
#include "../includes/http.h"
#include "../srclib/picohttpparser.h"

/* GLOBAL VARIABLES */
/* Server configuration */
ServerConfiguration server_config;

/* Socket descriptor of the connection */
int sockfd;

/* Thread management */
pthread_mutex_t mutex; /* mutex to manage concurrent access to the critical zone */
Thread *threadPool; /* Thread pool array */


/*******************************************************************************************
* FUNCTION: ServerConfiguration get_server_configuration()
* DESCRITPTION: Function to get the information in the server.conf file to the structure
*								contaning the needed fields for the server configuration before
*								initialization. Libconfuse library is used.
* ARGS_IN: None
* ARGS_OUT: The structure containing the server's configuration.
*******************************************************************************************/
ServerConfiguration get_server_configuration() {
	/* use libconfuse to store in the global variable the relevant information about server's
	configuration */
	cfg_opt_t options[] = {
		CFG_SIMPLE_STR("server_root", &server_config.server_root),                                                                                                                                                                                                                                                 // global variable
		CFG_SIMPLE_INT("max_clients", &server_config.max_clients),
		CFG_SIMPLE_INT("listen_port", &server_config.listen_port),
		CFG_SIMPLE_STR("server_signature", &server_config.server_signature),                                                                                                                                                                                                                                                 // global variable
		CFG_END()
	};
	cfg_t* cfg;
	if ((cfg  = cfg_init(options, 0)) == NULL) {
		fprintf(stderr, "ERROR: error when using cfg_init.");
		exit (EXIT_FAILURE);
	}

	if (cfg_parse(cfg, "server.conf") != CFG_SUCCESS) {
		fprintf(stderr, "ERROR: error when using cfg_parse.");
		cfg_free(cfg);
		exit(EXIT_FAILURE);
	}
	cfg_free(cfg);

	return server_config;
}

/*******************************************************************************************
* FUNCTION: int initiate_server()
* DESCRITPTION: Initializes the socket, makes it reusable, binds it and starts to listen.
* ARGS_IN: None
* ARGS_OUT: The executions ends in case of error, otherwise the file descriptor of the
*						socket is returned.
*******************************************************************************************/
int initiate_server() {
		int fd, flag = 1;
		struct sockaddr_in serv_addr;

		/* clean from previous executions and fill the structure with the adequate fields */
		memset(&serv_addr, '0', sizeof(serv_addr));
		serv_addr.sin_family = FAM;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(server_config.listen_port);

		/* open the socket, bind and listen */
		fd = Socket(FAM, SOCK, 0);
		/* so that the port can be reused inmediately */
		Setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
		Bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		Listen(fd, server_config.max_clients);

		return fd;
	}

/*******************************************************************************************
* FUNCTION: int accept_connection(int fd)
* DESCRITPTION: Accepts a connection using the socket descriptor passed as parameter.
* ARGS_IN: int fd - socket descriptor from where to accept the connection.
* ARGS_OUT: Returns the file descriptor of the connection or -1 in case of error.
*******************************************************************************************/
int accept_connection(int fd) {
		struct sockaddr conection;
		socklen_t len = sizeof(conection);
		return Accept(fd, &conection, &len);
}

/*******************************************************************************************
* FUNCTION: void* thread_main(void *arg)
* DESCRITPTION: Function executed by each thread. Each thread waits in the mutex till
* 							another one unlocks the mutex because it has accepted a connection.
* ARGS_IN: void * arg - an int pointer to the thread_num of the current thread
* ARGS_OUT: None
*******************************************************************************************/
void* thread_main(void *arg) {
		int connfd;

		/* Threads are detacched because main thread cannot join them,
		   as they are caught up in the mutex and the accept */
		Pthread_detach(pthread_self());

		/* each thread accepts connections forever */
		for (;;) {
				/* the socket must be protected with a mutex because it is a global variable */
				Pthread_mutex_lock(&mutex);
				connfd = accept_connection(sockfd);
				Pthread_mutex_unlock(&mutex);

				/* thread_count attribute of the thread counts the number of connections stablished by the client
				by the current thread */
				threadPool[(intptr_t) arg].thread_count++;

				if(connfd >= 0) {
						/* connetion is persistent so while the process_http_request does not send an END_OF_CONNECTION, keep answering
						all the requests carried out by the client */
						while(process_http_request(connfd, server_config.server_root, server_config.server_signature) != END_OF_CONNECTION);

				} else {
						fprintf(stderr, "ERROR: invalid connection.\n");
				}
		}
	}

/*******************************************************************************************
* FUNCTION: void threads_init(long nthreads, Thread ** poolp)
* DESCRITPTION: This function initilizes nthreads threads and saves its relevant information
*								into the Thread arrray. The global mutex to access the critical zone is also
* 							initialized. Each thread will execute the thread_main function.
* ARGS_IN: long nthreads - number of threads in the pool
*					 Thread ** poolp - pointer to the pool array to be initialized, and allocated
* ARGS_OUT: None
*******************************************************************************************/
void threads_init(long nthreads, Thread ** poolp) {
		/* mutex initialized for access to the critical zone (global variables) */
		pthread_mutex_init(&mutex,NULL);

		/* memory is allocated in order to store the information abou the threads */
		if((*poolp = calloc(nthreads, sizeof(Thread))) == NULL) {
				fprintf(stderr, "Error when alocating memory for threads.\n");
				exit(EXIT_FAILURE);
		}

		/* start all the threads, saving their information into the array */
		for(int i = 0; i < nthreads; i++) {
				/* each created thread  will execute the thread_main function */
				Pthread_create(&((*poolp)[i].thread_tid), thread_main, (void *)(intptr_t)i);
				/* the thread_num field is an identifier only valid inside our program */
				(*poolp)[i].thread_num = i;
		}
	}

/*******************************************************************************************
* FUNCTION: void sig_int(int signo)
* DESCRITPTION: Empty handler for the SIGINT signal.
* ARGS_IN: None
* ARGS_OUT: None
*******************************************************************************************/
void sig_int(int signo) {
		/* Do nothing */
}






/*******************************************************************************************
* FUNCTION: int main(int argc, char **argv)
* DESCRITPTION: Main function of the whole project. Uses the rest of the functions to
*						make the whole server work: initialization, configuration, thread management...
* ARGS_IN: int argc - number of input arguments
*					 char **argv - input arguments
* ARGS_OUT: returns EXIT_SUCCESS in case everything goes as expected, EXIT_FAILURE otherwise.
*******************************************************************************************/
int main(int argc, char **argv) {
		/* get the server configuration from the server.config file into the global variable
		containing the different fields: nº of clients, port, nº of threads & server signature */
		server_config = get_server_configuration();

		/* here the socket is oppened, binded and starts to listen */
		sockfd = initiate_server();
		printf("Listening connections. SIGINT to close server.\n");

		/* block SIGINT for child threads */
		sigset_t set;
		sigemptyset(&set);
		sigaddset(&set, SIGINT);
		pthread_sigmask(SIG_BLOCK, &set, NULL);

		/* thread pool is created and started, together with a mutex to access the critical zone */
		threads_init(server_config.max_clients, &threadPool);

		/* everything done by threads, wait SIGINT signal to close server */
		struct sigaction act;
		act.sa_handler = sig_int;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;

		Signal(SIGINT, act);
		Sigsuspend(SIGINT);

		/* print the number of connections each thread has received, before leaving */
		printf("\n");
		for (int i = 0; i < server_config.max_clients; i++) {
				printf("thread %d, %ld connections\n", i, threadPool[i].thread_count);
		}

		/* clean before leaving */
		if (threadPool) free(threadPool);
		if (server_config.server_root) free(server_config.server_root);
		if (server_config.server_signature) free(server_config.server_signature);

		exit(EXIT_SUCCESS);
}
