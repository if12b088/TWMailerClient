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
#define BUF 1024
#define PORT 6000

std::string sendToServer(int create_socket, std::string msg) {

	char buffer[BUF];
	int size;
	buffer[0] = '\0';

	size = send(create_socket, msg.c_str(), msg.length(), 0);

#ifdef _DEBUG
	//std::cout << "Send-size: " << size << std::endl;
#endif

	if (size == -1) {
		perror("Send error");
		return "EXIT_FAILURE";
	}

	size = recv(create_socket, buffer, BUF - 1, 0);
	if (size > 0) {
		buffer[size] = '\0';

	}

#ifdef _DEBUG
	//std::cout << "Recv-size: " << size << std::endl;
#endif

	std::string answer = std::string(buffer);
	return answer;
}

//bool sendFileToServer(int create_socket, std::ifstream file) {
//
//	file.seekg(0, std::ios::beg);
//	while (file.tellg() != -1) {
//		char *p = new char[BUF];
//
//		bzero(p, BUF);
//		file.read(p, BUF);
//
//		printf("%ld\n", file.gcount());
//
//		int n = send(create_socket, p, BUF, 0);
//		if (n < 0) {
//			std::cerr << "ERROR writing to socket" << std::endl;
//			return false;
//		} else {
//			printf("---------%d\n", n);
//		}
//
//		delete p;
//	}
//	return true;
//}

std::string login(int create_socket, std::string *user) {

	std::string msg;
	std::string response;

	std::string username;
	std::string password;

	do {
		username = "";
		std::cout << "Username (max 8 characters): ";
		getline(std::cin, username);

		if (username.length() > 8) {
			std::cout << "max 8 characters, please enter an other username!\n";
		}
	} while (username.length() > 8);
	password = getpass("Password: ");
//	std::cout << "Password: ";
	//getline(std::cin, password);

	msg.append("LOGIN\n");
	msg.append(username);
	msg.append("\n");
	msg.append(password);
	msg.append("\n");

	*user = username;
	response = "";
	response = sendToServer(create_socket, msg);

	return response;
}

std::string send(int create_socket, std::string username) {

	std::string msg;
	std::string response;

//	std::string from;
	std::string to;
	std::string toTemp;
	std::string subject;
	std::string text;
	std::string textTemp;
	std::string fileName;
	int fileSize;
//	std::list<std::string> fileNameList;
	std::ifstream file;

	//FROM
//	do {
//		from = "";
//		std::cout << "From (max 8 characters): ";
//		getline(std::cin, from);
//
//		if (from.length() > 8) {
//			std::cout << "max 8 characters, please enter an other sender!\n";
//		}
//	} while (from.length() > 8);

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

	//Muti ATT
//	do {
//		std::cout << "Attachment: ";
//		fileName = "";
//		getline(std::cin, fileName);
//		if (fileName != "") {
//			fileNameList.push_back(fileName);
//		}
//	} while (fileName != "");

	std::cout << "Attachment: ";
	fileName = "";
	getline(std::cin, fileName);

	if (fileName == "") {
		fileSize = 0;
	} else {
		file.open(fileName.c_str(), std::ios::binary);
		//std::ifstream file;
		//file.open(fileName, std::ios::binary);

		if (file.is_open()) {
			file.seekg(0, std::ios::end);
			fileSize = file.tellg();
			file.seekg(0, std::ios::beg);
			std::cout << "FileSize: " << fileSize << std::endl;

//			struct stat results;
//			int fileSizeStat = 0;
//
//			stat(fileName.c_str(), &results);
//			fileSizeStat = results.st_size;
//
//			std::cout << "FileSize form stat: " << fileSizeStat << std::endl;

		} else {
			std::cerr << "File not open" << std::endl;
		}

	}

	msg.append("SEND\n");
	//msg.append(from);
	msg.append(username);
	msg.append("\n");
	msg.append(to);
	msg.append("\n");
	msg.append(subject);
	msg.append("\n");
	msg.append(text);
	msg.append(std::to_string(fileSize));
	msg.append("\n");
	if (fileName != "") {
		msg.append(fileName);
		msg.append("\n");
	}

	// Send Protokoll
	response = sendToServer(create_socket, msg);

	// Send Attachments
	//sendFileToServer(create_socket, file);

	file.seekg(0, std::ios::beg);
	while (file.tellg() != -1) {
#ifdef _DEBUG
		std::cout << "begin sending file" << std:: endl;
#endif
		char *p = new char[BUF];

		bzero(p, BUF);
		file.read(p, BUF);

		printf("%ld\n", file.gcount());

		int n = send(create_socket, p, BUF, 0);

		if (n < 0) {
			std::cerr << "ERROR writing to socket" << std::endl;
			//return false;
		} else {
			printf("---------%d\n", n);
		}

		delete p;
	}
	file.close();

#ifdef _DEBUG
	//std::cout << msg << std::endl;
#endif
	return msg;
}
std::string list(int create_socket, std::string username) {

	std::string msg;
	std::string response;

	//std::string user;

	//USERNAME
//	do {
//		user = "";
//		std::cout << "Username (max 8 characters): ";
//		getline(std::cin, user);
//
//		if (user.length() > 8) {
//			std::cout << "max 8 characters, please enter an other username!\n";
//		}
//	} while (user.length() > 8);

	msg.append("LIST\n");
	//msg.append(user);
	msg.append(username);
	msg.append("\n");

	response = sendToServer(create_socket, msg);

	return msg;
}
std::string read(int create_socket, std::string username) {

	std::string msg;
	std::string response;

	//std::string user;
	std::string nr;

	//USERNAME
//	do {
//		user = "";
//		std::cout << "Username (max 8 characters): ";
//		getline(std::cin, user);
//
//		if (user.length() > 8) {
//			std::cout << "max 8 characters, please enter an other username!\n";
//		}
//	} while (user.length() > 8);

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("READ\n");
	//msg.append(user);
	msg.append(username);
	msg.append("\n");
	msg.append(nr);
	msg.append("\n");

	response = sendToServer(create_socket, msg);

	return msg;
}
std::string del(int create_socket, std::string username) {

	std::string msg;
	std::string response;

	//std::string user;
	std::string nr;

	//USERNAME
//	do {
//		user = "";
//		std::cout << "Username (max 8 characters): ";
//		getline(std::cin, user);
//
//		if (user.length() > 8) {
//			std::cout << "max 8 characters, please enter an other username!\n";
//		}
//	} while (user.length() > 8);

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("DEL\n");
	//msg.append(user);
	msg.append(username);
	msg.append("\n");
	msg.append(nr);
	msg.append("\n");

	response = sendToServer(create_socket, msg);

	return msg;
}

std::string sendDebug(int create_socket, std::string username) {

	std::string msg;
	std::string response;

	std::string to = "if12b046";
	std::string subject = "Hallo es kommt gleich ein file";
	std::string text = "Das ist der Text zu dem File\nDer Text ist aber nicht sehr spannend\n.\n";
	std::string fileName = "file1.txt";
	int fileSize;
	std::ifstream file;

	if (fileName == "") {
		fileSize = 0;
	} else {
		file.open(fileName.c_str(), std::ios::binary);

		if (file.is_open()) {
			file.seekg(0, std::ios::end);
			fileSize = file.tellg();
			file.seekg(0, std::ios::beg);
			std::cout << "FileSize: " << fileSize << std::endl;


		} else {
			std::cerr << "File not open" << std::endl;
		}

	}

	msg.append("SEND\n");
	msg.append(username);
	msg.append("\n");
	msg.append(to);
	msg.append("\n");
	msg.append(subject);
	msg.append("\n");
	msg.append(text);
	msg.append(std::to_string(fileSize));
	msg.append("\n");
	if (fileName != "") {
		msg.append(fileName);
		msg.append("\n");
	}

	// Send Protokoll
	response = sendToServer(create_socket, msg);

	// Send Attachments
	//sendFileToServer(create_socket, file);

	file.seekg(0, std::ios::beg);
	while (file.tellg() != -1) {
#ifdef _DEBUG
		std::cout << "begin sending file" << std:: endl;
#endif
		char *p = new char[BUF];

		bzero(p, BUF);
		file.read(p, BUF);

		printf("%ld\n", file.gcount());

		int n = send(create_socket, p, BUF, 0);

		if (n < 0) {
			std::cerr << "ERROR writing to socket" << std::endl;
			//return false;
		} else {
			printf("---------%d\n", n);
		}

		delete p;
	}
	file.close();

#ifdef _DEBUG
	//std::cout << msg << std::endl;
#endif
	return msg;
}

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
	std::string command;
	struct sockaddr_in address;
	int size;
	std::string msg;
	std::string response;
	std::string username;

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

	do {
		response = "";
		response = login(create_socket, &username);

#ifdef _DEBUG
		std::cout << msg << std::endl;
#endif

		//response = sendToServer(create_socket, msg);

#ifdef _DEBUG
		//std::cout << "response: " << response << std::endl;
#endif

#ifdef _DEBUG
		//std::cout << "user: " << username << std::endl;
#endif

		if (response == "BAN\n") {
			std::cout << "To many tries, you have been baned!\n" << std::endl;
			exit(EXIT_SUCCESS);
		}

	} while (response != "OK\n");

	printCommands();
	do {
		command = "";
		std::cout << "Command: ";
		msg = "";
		getline(std::cin, command);

		if (command != "q") {
			if (command == "s") {
				msg = send(create_socket, username);
			} else if (command == "l") {
				msg = list(create_socket, username);
			} else if (command == "r") {
				msg = read(create_socket, username);
			} else if (command == "d") {
				msg = del(create_socket, username);
			} else if (command == "w") {
				msg = sendDebug(create_socket, username);
			} else {
				std::cout << "unknown command!" << std::endl;
			}

#ifdef _DEBUG
			std::cout << msg << std::endl;
#endif

		}
	} while (command != "q");
	close(create_socket);
	return EXIT_SUCCESS;
}
