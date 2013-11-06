/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>
#include <sys/stat.h>
#include "Helper.h"
#include "CommandHandler.h"
#define BUF 1024
#define PORT 6000

void printCommands() {
	std::cout << "##########################################################\n";
	std::cout << "####                      COMMANDS                    ####\n";
	std::cout << "#### s (SEND: send an email)                          ####\n";
	std::cout << "#### l (LIST: list all email from a user)             ####\n";
	std::cout << "#### r (READ: read a specific email)                  ####\n";
	std::cout << "#### d (DEL : delete a specific email                 ####\n";
	std::cout << "#### q (QUIT: end program)                            ####\n";
	std::cout << "##########################################################\n";
}

int main(int argc, char **argv) {
	int create_socket;
	char buffer[BUF];
	struct sockaddr_in address;
	int size;

	if (argc < 2) {
		printf("Usage: %s ServerAdresse\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket error");
		return EXIT_FAILURE;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_aton(argv[1], &address.sin_addr);

	if (connect(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
		printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));
		size = recv(create_socket, buffer, BUF - 1, 0);
		if (size > 0) {
			buffer[size] = '\0';
			printf("%s", buffer);
			if (strcmp(buffer, "BANNED\n") == 0) {
				std::cout << "Your IP is Blocked\n" << std::endl;
				exit(EXIT_SUCCESS);
			}
		}
	} else {
		perror("Connect error - no server available");
		return EXIT_FAILURE;
	}

	CommandHandler* handler = new CommandHandler(create_socket);

	std::string response;
	do {
// LOGIN
		response = "";
		response = handler->login();

		if (response == "BAN\n") {
			std::cout << "To many tries, you have been baned!\n" << std::endl;
			exit(EXIT_SUCCESS);
		}
	} while (response != "OK\n");

	printCommands();

	std::string command;

	//start Command block
	do {
		command = "";
		std::cout << "Command: ";

		//read Command
		getline(std::cin, command);

		if (command != "q") {
			if (command == "s") {
				handler->sendCommand();
			} else if (command == "l") {
				handler->list();
			} else if (command == "r") {
				handler->read();
			} else if (command == "d") {
				handler->del();
			} else {
				std::cout << "unknown command!" << std::endl;
			}
		}
	} while (command != "q");
	close(create_socket);
	return EXIT_SUCCESS;
}
