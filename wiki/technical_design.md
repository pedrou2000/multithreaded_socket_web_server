## Technical Design

### Server Design

We have decided to implement the server using a thread pool. At the beginning of the implementation we
tried different server types such as creating one thread per client request, prefork with file locking
to protect accept (1000 children in pool) and one fork per client request and they gave worse results when there were 500 concurrent
clients, each one making 500 requests of 400 bytes each (4,04772, 3,266676s, 58,3742s respectively) whereas
our chosen option prethreaded with mutex locking to protect accept (1000 threads in pool) took only 3,15759s
in the same conditions. Other executions were carried out varying the parameters and in most of them the
thread pool gave us the best execution times, therefore that was the one we chose to implement.

Everything related with threads, such as the pool creation, the semaphore used to access the the accept connections being careful about the concurrent
access to the file descriptor of the socket is implemented in the server.c file. Everything related with socket management is also implemented in the
server.c file so that the threads can access the file descriptors easily, as the length of code related to this part is not very big.

### Server's configuration

The server configuration can be easily carried by changing the server.conf file. By changing the left hand side of the
'=' sign the corresponding variables in the program are changed (the server must be restarted for changes to take effect).
The meaning and use of each variable is explained subsequently:

* server_root: path to the files the server can access, it will be concatenated with the address in the url of the http request
in order to obtain the final path of the desired file.

* max_clients: the maximum number of connections the server will handle, if more connections happen the server will just discard them,
as this variable is used as the backlog parameter of the listen function, which "defines the maximum length to which the queue of pending connections for sockfd may grow". It is also the number of threads that will be created.

* listen_port: port in which our server will work.

* server_signature: server's signature included in the header of the http replies.

In order to implement this functionality we mainly used the libconfuse library in order to parse the server.conf file. To do that, we implemented
get_server_configuration function in the server.c file.

### Server's http

The server receives and replies using http. In order to carry out that functionality we have designed the http modules (http.c and http.h), whose main
function process_http_request handles all the work using auxiliary functions, therefore it is the only one included in the http.h file. It works with GET, POST and OPTIONS requests. When receiving a request with the Connection: close header the server closes that connection, otherwise it leaves it open as it is an http 1.1 server. The server can answer requests with different message codes:

* 200 OK: used on correct requests where the file has been found, the script has been executed correctly or it was an OPTIONS request and
everything worked fine. Implemented in the send_200_ok and send_200_ok_options functions.

* 400 Bad Request: sent to the client whenever the server cannot understand the request. Implemented in the send_400_bad_request function.

* 404 Not Found: sent to the client whenever the requested resource cannot be found in the specified directory.

* 500 Internal Server Error: sent to the client every there is a problem with the execution that does not have to do with a client error.

### Server's Scripts

The server can execute scripts in case of a script specified in the url and arguments in the body (POST) or url (GET or POST). To do that,
we decided to use the function popen in order to execute a python or php command. If it is not any of these two types of scripts the
script won't be executed and 400 Bad Request will be sent. The same will happen if there were no arguments in the request. The output of the
script will be sent in the body of the http response in case everything works correctly.

Two other scripts have been developed, hola.py and farenheit.py, which can be found in the "htmlfiles/www/scripts" directory. The first one receives a
name and prints hello 'name'! and the second one converts Celsius to Fahrenheit printing the result. This scripts have been added to the index.html
file so that the user of this web page can input data to the scripts and receive the results back in the server's response. In case the input data is
not correct the scrypt generates the adequate error message so that the server's reply helps understand the user what must be inputted in the adequate
field.
