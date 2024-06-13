/* 
 * tcpserver.c - A multithreaded TCP echo server 
 * usage: tcpserver <port>
 * 
 * Testing : 
 * nc localhost <port> [<conn>] < input.txt
 */

#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>
#include <csignal>
#include <functional>

#include "data_store.hpp"
#include "server.hpp"

std::function<void()> signalHandler;

void signalHandlerWrapper(int signum) {
	if (signalHandler)
		signalHandler();
}

DataStore *DataStore::self = nullptr;

int main(int argc, char ** argv) {
	int port_no; // port to listen on 

	// check command line arguments
	if (argc < 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}

	// Checking validity of port no.
	if ((port_no = atoi(argv[1])) < 1024) {
		fprintf(stderr, "Can't assign port no. %s\n", argv[1]);
		fprintf(stderr, "Port no. should be integer greater than 1023\n", argv[1]);
		exit(1);
	}
	
	std::cout << "Server running on port " << port_no << '\n';
	
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