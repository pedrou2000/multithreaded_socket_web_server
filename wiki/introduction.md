## Introduction

Authors: Cesar Ramirez & Pedro Urbina, pair 5, group 2301.

The objective of this practice is to create a completely functional web server which manages GET, POST and OPTIONS requests and can be configured with the server.conf file.

### Project Structure

* `src` directory where source files are included. (.c)
* `srclib` directory where the picohttpparser library source code used in the http module is stored.
* `includes` directory where the header files of our source code are stored.
* `htmlfiles` directory with the html code and web page which executes scripts and generates different kinds of requests.
* `obj` temporary directory created when make executed, contains all the .o files.
* `lib` temporary directory created when make executed, contains the .a library files of picohttpparser and http.
* `wiki` directory where the wiki is stored.

The project source code is divided mainly in 3 modules:
* server (server.c): implements the handling of threads and everything related to the socket management. It is the main file
of the project, the one which will finally be executed as it includes the main function. The server configuration is also carried
out in this file.
* utils (utils.h and utils.c): contains all the necessary includes for the rest of the modules, the data structure definition
used in other modules and functions that wrap up error handling to make code more readable.
* http (http.h and http.c): contains all the code related with http: the reception of request, parsing and response sending.

### Used Libraries
- We have used the picohttpparser library in order to parse the received http requests.

- We have used the libconfuse library to read and parse the configuration file.

### Codes Implemented and Used by the Server

* 200
* 400
* 404
* 500

### Supported Resource Types

* .txt
* .html and .htm
* .gif
* .jpeg, .jpg
* .mpeg, .mpg
* .avi
* .mov
* .doc, .docx
* .pdf

Script execution:
* .py
* .php

### Installation

Some version of python and php must be installed on your system, the necessary to execute the
scripts your server is going to use.
In order to execute the server the library libconfuse must already be installed in your system.
You must also have root permissions in order to bind the port.  

### Execution

In order to execute you must only enter "make all" in the terminal so that everything is compiled
and you can run the server using "sudo ./server", as elevated privileges are needed to bind the port.

In order for the server to work with the usual html file, the media folder containing images and a
video as the ones given to us be added to htmlfiles/www/.

The port used by the server can be configured in the server.conf file, together with the maximum number
of clients (i.e. the number of concurrent threads that will be created), the server's signature and the
path to where the server files are saved. All this can be configured in the server.conf even after
compilation, but in order for changes to make effect the server must be restarted.

In order to quit the execution you must send SIGINT to the process, which can be usually done by clicking
control + c in the terminal. In order to clean the project you can enter in the terminal "make clean".
