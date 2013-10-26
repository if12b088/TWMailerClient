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
#define BUF 1024
#define PORT 6000

std::string send() {

	std::string msg;

	std::string from;
	std::string to;
	std::string toTemp;
	std::string subject;
	std::string text;
	std::string textTemp;
	//FROM
	do {
		from = "";
		std::cout << "From (max 8 characters): ";
		getline(std::cin, from);

		if (from.length() > 8) {
			std::cout << "max 8 characters, please enter an other sender!\n";
		}
	} while (from.length() > 8);

	//TO
	do {

		toTemp = "";
		std::cout << "To (max 8 characters, press ENTER to end): ";
		getline(std::cin, toTemp);
		if (toTemp.length() > 8) {
			std::cout << "max 8 characters, please enter an other recipient!\n";
		} else {
			if (toTemp != "") {
				to.append(toTemp);
				to.append(";");
			}

		}
	} while (toTemp != "");

	// SUBJECT
	do {
		std::cout << "Subject (max 80): ";
		getline(std::cin, subject);
		if (subject.length() > 80) {
			std::cout << "Subject is too long, max 80 characters!\n";
		}
	} while (subject.length() > 80);

	//TEXT
	std::cout << "Text (enter \". ENTER\" in a new line to end): ";
	do {
		textTemp = "";
		getline(std::cin, textTemp);
		text.append(textTemp);
		text.append("\n");
	} while (textTemp != ".");

	msg.append("SEND\n");
	msg.append(from);
	msg.append("\n");
	msg.append(to);
	msg.append("\n");
	msg.append(subject);
	msg.append("\n");
	msg.append(text);
//	msg.append("\n");

	return msg;
}
std::string list() {

	std::string msg;
	std::string user;

	//USERNAME
	do {
		user = "";
		std::cout << "Username (max 8 characters): ";
		getline(std::cin, user);

		if (user.length() > 8) {
			std::cout << "max 8 characters, please enter an other username!\n";
		}
	} while (user.length() > 8);

	msg.append("LIST\n");
	msg.append(user);
	msg.append("\n");

	return msg;
}
std::string read() {

	std::string msg;
	std::string user;
	std::string nr;

	//USERNAME
	do {
		user = "";
		std::cout << "Username (max 8 characters): ";
		getline(std::cin, user);

		if (user.length() > 8) {
			std::cout << "max 8 characters, please enter an other username!\n";
		}
	} while (user.length() > 8);

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("READ\n");
	msg.append(user);
	msg.append("\n");
	msg.append(nr);
	msg.append("\n");

	return msg;
}
std::string del() {

	std::string msg;
	std::string user;
	std::string nr;

	//USERNAME
	do {
		user = "";
		std::cout << "Username (max 8 characters): ";
		getline(std::cin, user);

		if (user.length() > 8) {
			std::cout << "max 8 characters, please enter an other username!\n";
		}
	} while (user.length() > 8);

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("DEL\n");
	msg.append(user);
	msg.append("\n");
	msg.append(nr);
	msg.append("\n");

	return msg;
}

void printCommands() {
	std::cout << "##########################################################\n";
	std::cout << "####                      COMMANDS                    ####\n";
	std::cout << "#### s (SEND: send an email)                          ####\n";
	std::cout << "#### l (LIST: list all email from a user)            #####\n";
	std::cout << "#### r (READ: read a specific email)                 #####\n";
	std::cout << "#### d (DEL: delete a specific email                 #####\n";
	std::cout << "#### q (QUIT: end program)                           #####\n";
	std::cout << "##########################################################\n";
}

int main(int argc, char **argv) {
	int create_socket;
	char buffer[BUF];
	std::string command;
	struct sockaddr_in address;
	int size;
	std::stringstream msg;

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

	if (connect(create_socket, (struct sockaddr *) &address, sizeof(address))
			== 0) {
		printf("Connection with server (%s) established\n",
				inet_ntoa(address.sin_addr));
		size = recv(create_socket, buffer, BUF - 1, 0);
		if (size > 0) {
			buffer[size] = '\0';
			printf("%s", buffer);
		}
	} else {
		perror("Connect error - no server available");
		return EXIT_FAILURE;
	}
	printCommands();
	do {
		command = "";
		std::cout << "Command: ";
		getline(std::cin, command);

		if (command != "q") {
			if (command == "s") {
				msg << send();
			}

			if (command == "l") {
				msg << list();
			}

			if (command == "r") {
				msg << read();
			}

			if (command == "d") {
				std::string msg = del();
			}

#ifdef _DEBUG
			std::cout << msg.str();
#endif

			if (send(create_socket, msg.str().c_str(), msg.str().size(), 0)
					== -1) {
				perror("Send error");
				return EXIT_FAILURE;
			}

			size = recv(create_socket, buffer, BUF - 1, 0);
			if (size > 0) {
				buffer[size] = '\0';
				printf("%s", buffer);
			}
		}
	} while (command != "q");
	close(create_socket);
	return EXIT_SUCCESS;
}
