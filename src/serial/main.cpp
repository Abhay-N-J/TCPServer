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

#include "data_store.hpp"
#include "server.hpp"



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
		fprintf(stderr, "Port no. should  e integer greater than 1023\n", argv[1]);
		exit(1);
	}
	
	std::cout << "Server running on port " << port_no << '\n';
	
	// Attaching cleanup function to exit
	std::signal(SIGINT, cleanup);
	std::signal(SIGTERM, cleanup);
	
	int server_fd = init_server(port_no);
	main_loop(server_fd);
	cleanup(0);	
}