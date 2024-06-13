/* 
 * tcpserver.c - A multithreaded TCP echo server 
 * usage: tcpserver <port>
 * 
 * Testing : 
 * nc localhost <port> [<conn>] input.txt
 */

#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <functional>

#include "server.hpp"

std::function<void()> signalHandler;

void signalHandlerWrapper(int signum) {
	if (signalHandler)
		signalHandler();
}

int main(int argc, char ** argv) {
    int port_no; // port to listen on 
    int conn = 10; // no. of connections

	// check command line arguments
	if (argc < 2) {
		fprintf(stderr, "usage: %s <port> [<conn>]\n", argv[0]);
		exit(1);
	}

	// Checking validity of port no.
	if ((port_no = atoi(argv[1])) < 1024) {
		fprintf(stderr, "Can't assign port no. %s\n", argv[1]);
		fprintf(stderr, "Port no. should be integer greater than 1023\n", argv[1]);
		exit(1);
	}

    //Checking availability of threads
    if (argc == 3 && ((conn = atoi(argv[2])) <= 0)) {
        fprintf(stderr, "Can't make %s connections\n", argv[2]);
        conn = 10;
    }
	
    std::cout << "Server running on port " << port_no << '\n';
    std::cout << "Threads available " << conn << std::endl;
	
	// Attaching cleanup function to exit
	
	int server_fd = init_server(port_no);
	
    signalHandler = [&server_fd] {
		cleanup(server_fd);
	};
	// Attaching cleanup function to exit
	std::signal(SIGINT, signalHandlerWrapper);
	std::signal(SIGTERM, signalHandlerWrapper);

	main_loop(server_fd);
	
    cleanup(server_fd);
}