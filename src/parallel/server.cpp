#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>

#include "data_store.hpp"
#include "server.hpp"

#define BUFFER_SIZE 1024

void cleanup(int signum) {
	DataStore::deleteInstance();
	std::cout << "Stopped Server\n";
	exit(0);
}


std::string read_msg(int sock, DataStore *dataObj, bool *toClose, Reader *reader) {
	std::cout << "Reading...\n";
	std::string key = reader->next();
	if (key == "END" || key == "Closed" || key == "ERR") { 
		*toClose = true;
		return key;
	}
	std::string value = dataObj->getValue(key) + "\n";
	send(sock, value.c_str(), value.length(), 0);
    return "";
}

std::string write_msg(DataStore *dataObj, bool *toClose, Reader *reader) {
	std::cout << "Writing...\n";
	std::string key = reader->next();
	if (key == "END" || key == "Closed" || key == "ERR") { 
		*toClose = true;
		return key;
	}
    std::string value = reader->next();
	if (value == "END" || value[0] != ':' || value == "Closed" || value == "ERR") {
		*toClose = true;
		return value;
	}
	value = value.substr(1, value.length() - 1);
	dataObj->setValue(key, value);
    return "";
}

void count_msg(int sock, DataStore *dataObj, bool *toClose, Reader *reader) {
	std::cout << "Counting...\n";
	std::string keys = std::to_string(dataObj->countKeys()) + "\n";
	send(sock, keys.c_str(), keys.length(), 0);
}

std::string delete_msg(int sock, DataStore *dataObj, bool *toClose, Reader *reader) {
	std::cout << "Deleting...\n";
	std::string key = reader->next();
	if (key == "END" || key == "Closed" || key == "ERR") { 
		*toClose = true;
		return key;
	}
	if (!dataObj->deleteKey(key))
		send(sock, "NULL\n", 6, 0);
    return "";
}

int init_server(int port_no) {
	int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        fprintf(stderr, "Socket initialization error\n");
		exit(1);
    }

    // Binding the socket to the address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_no);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		fprintf(stderr, "Socket initialization error in binding\n");
		exit(1);
    }
	if (listen(server_fd, 10) < 0) {
        fprintf(stderr, "Socket Initialization error in listening\n");
		exit(1);
    }

    std::cout << "Server is listening on port " << port_no << std::endl;

	return server_fd;
}

void main_loop(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    DataStore* instance = DataStore::getInstance();
    Reader* reader;
	bool toClose = true;

	while (true) {
		char buffer[BUFFER_SIZE] = {0};
		if (toClose) {
			new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
			if (new_socket < 0) {
				fprintf(stderr, "Accept failed");
				continue;
			}
			std::cout << "Connection accepted from "
                  << address.sin_addr.s_addr << ":"
                  << ntohs(address.sin_port) << std::endl;
            reader = new Reader(new_socket);
			toClose = false;
		}

        std::string input = reader->next();
        if (input == "END" || input == "ERR" || input == "Closed") {
            std::cout << "Client disconnected\n";
            toClose = true;
        }
		else if (input == "READ")
			input = read_msg(new_socket, instance, &toClose, reader);
		else if (input == "WRITE")
			input = write_msg(instance, &toClose, reader);
		else if (input == "COUNT")
			count_msg(new_socket, instance, &toClose, reader);
		else if (input == "DELETE")
			input = delete_msg(new_socket, instance, &toClose, reader);
        else {
            std::cout << "Unknown command: " << input << std::endl;
        }
		// Closing the connection
		if (toClose) {
			char msg[] = "FIN\n";
			if (input != "ERR" && input != "Closed")
				send(new_socket, &msg, std::strlen(msg), 0);
			close(new_socket);
            if (reader)
                delete reader;
            reader = nullptr;
		}
	}
	close(server_fd);
}

Reader::Reader(int sock) {
    this->sock = sock;
}

void Reader::split(const char* s, int len) {
    std::string temp;
    for (int i = 0; i < len; i++) {
        if (s[i] != '\n') temp += s[i];
        else {
            list.push(trim(temp));
            temp = "";
        }
    }
    if (!temp.empty())
        list.push(trim(temp));
}

std::string Reader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \n\r\t");
    return str.substr(first, last - first + 1);
}

std::string Reader::next() {
    if (!list.empty()) {
        std::string next_input = list.front();
        list.pop();
        return next_input;
    }
    char buffer[BUFFER_SIZE] = {0};
    int val_read = read(sock, &buffer, BUFFER_SIZE);
    if (val_read < 0) {
        fprintf(stderr, "Read error: %s\n", std::strerror(errno));
        return "ERR";
    }
    else if (val_read == 0) {
        std::cout << "Client disconnected\n";
        return "Closed";
    }
    split(buffer, val_read);
    if (!list.empty()) {
        std::string next_input = list.front();
        list.pop();
        return next_input;
    }
    return "";
}