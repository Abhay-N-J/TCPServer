/* 
 * tcpserver.c - A multithreaded TCP echo server 
 * usage: tcpserver <port>
 * 
 * Testing : 
 * nc localhost <port> [<conn>] input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <pthread.h>


int main(int argc, char ** argv) {
  int portno; /* port to listen on */
  int conn = 10; // no. of connections

  /* 
  * check command line arguments 
  */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port> [<conn>]\n", argv[0]);
    exit(1);
  }

  // DONE: Server port number taken as command line argument
  portno = atoi(argv[1]);

  if (argc == 3 && ((conn = atoi(argv[2])) <= 0)) {
		fprintf(stderr, "Can't create %s connections\n", argv[2]);
		fprintf(stderr, "Going forward with %d connections\n", conn);
	}
  std::cout << "Threads available " << conn << std::endl;

}