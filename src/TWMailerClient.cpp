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
#include <iostream>
#define BUF 1024
#define PORT 6000


std::string send(){

	printf("send-befehl:\n");

	std::string msg;
	char from[10];
	char to[10];
	char subject[82];
	char text[82];

	std::cout << "From: ";
	fgets(from, sizeof(from), stdin);
	std::cout << "To: ";
	fgets(to, sizeof(to), stdin);
	std::cout << "Subject: ";
	fgets(subject, sizeof(subject), stdin);
	std::cout << "Text: ";
	//size??
	fgets(text, sizeof(text), stdin);

	msg.append("SEND\n");
	msg.append(from);
	msg.append(to);
	msg.append(subject);
	msg.append(text);

	return msg;
}
std::string list(){
	printf("list-befehl:\n");

	std::string msg;
		char user[9];

		std::cout << "Username: ";
		fgets(user, 9, stdin);

		msg.append("LIST\n");
		msg.append(user);

		return msg;
}
std::string read(){
	printf("read-befehl\n");

	std::string msg;
	char user[9];
	char nr[20];

	std::cout << "Username: ";
	fgets(user, 9, stdin);
	std::cout << "Message-Number: ";
	// size??
	fgets(nr, 20, stdin);

	msg.append("READ\n");
	msg.append(user);
	msg.append(nr);

	return msg;
}
std::string del(){
	printf("del-befehl\n");

	std::string msg;
	char user[9];
	char nr[20];

	std::cout << "Username: ";
	fgets(user, 9, stdin);
	std::cout << "Message-Number: ";
	// size??
	fgets(nr, 20, stdin);

	msg.append("DEL\n");
	msg.append(user);
	msg.append(nr);

	return msg;
}

int main(int argc, char **argv) {
	int create_socket;
	char buffer[BUF];
	char * msgBuffer;
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
	address.sin_port = htons (PORT);
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

	do {
		printf("Command: ");
		fgets(buffer, BUF, stdin);
		//printf("%d", strcmp(buffer, "SEND\n"));

		if(strcmp(buffer, "SEND\n") == 0){
			std::string msg = send();
			//std::cout << msg << std::endl;

			msgBuffer = new char[msg.size() + 1];
			std::copy(msg.begin(), msg.end(), msgBuffer);
			//printf("%s", msgBuffer);
			msgBuffer[msg.size()] = '\0';
		}

		if(strcmp(buffer, "LIST\n") == 0){
			std::string msg = list();
			std::cout << msg << std::endl;

			msgBuffer = new char[msg.size() + 1];
			std::copy(msg.begin(), msg.end(), msgBuffer);
			msgBuffer[msg.size()] = '\0';
		}

		if(strcmp(buffer, "READ\n") == 0){
			std::string msg = read();
			std::cout << msg << std::endl;

			msgBuffer = new char[msg.size() + 1];
			std::copy(msg.begin(), msg.end(), msgBuffer);
			msgBuffer[msg.size()] = '\0';
		}

		if(strcmp(buffer, "DEL\n") == 0){
			std::string msg = del();
			std::cout << msg << std::endl;

			msgBuffer = new char[msg.size() + 1];
			std::copy(msg.begin(), msg.end(), msgBuffer);
			msgBuffer[msg.size()] = '\0';
		}
		if (send(create_socket, msgBuffer, strlen(msgBuffer), 0) == -1){
			perror("Send error");
			return EXIT_FAILURE;
		}

		size = recv(create_socket, buffer, BUF - 1, 0);
		if(size > 0){
			buffer[size] = '\0';
			printf("%s", buffer);
		}

	} while (strcmp(buffer, "QUIT\n") != 0);
	close(create_socket);
	return EXIT_SUCCESS;
}
