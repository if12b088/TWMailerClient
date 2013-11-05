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
#define BUF 1024
#define PORT 6000

void sendProtocol(int create_socket, std::string msg) {

	char buffer[BUF];
	int size;
	bzero(buffer, BUF);
	//buffer[0] = '\0';

	size = send(create_socket, msg.c_str(), msg.length(), 0);

#ifdef _DEBUG
	//std::cout << "Send-size: " << size << std::endl;
#endif

	if (size == -1) {
		perror("Send error");
		return;
	}
	return;
}

std::string receiveProtocol(int create_socket) {
	char buffer[BUF];
	int size;
	bzero(buffer, BUF);

	size = recv(create_socket, buffer, BUF - 1, 0);
	if (size > 0) {
		buffer[size] = '\0';

	}
	std::string response = std::string(buffer);

	return response;
}

bool sendFileToServer(int create_socket, std::ifstream* file,
		long long fileSize) {

	file->seekg(0, std::ios::beg);
	int toRead = 0;
	while (fileSize > 0) {

		if (fileSize > BUF) {
			toRead = BUF;
		} else {
			toRead = fileSize;
		}
		char *p = new char[toRead];

		bzero(p, toRead);
		file->read(p, toRead);

		printf("%ld\n", file->gcount());

		int n = send(create_socket, p, toRead, 0);

		if (n < 0) {
			std::cerr << "ERROR writing to socket" << std::endl;
			return false;
		} else {
			printf("---------%d\n", n);
		}
		fileSize -= n;

		delete p;
	}
	return true;
}

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

	//Username nach aussen uebergeben
	*user = username;

#ifdef _DEBUG
	std::cout << msg << std::endl;
#endif

	response = "";
	sendProtocol(create_socket, msg);
	response = receiveProtocol(create_socket);

	return response;
}

std::string send(int create_socket) {

	std::string msg;
	std::string response;

	std::string to;
	std::string toTemp;
	std::string subject;
	std::string text;
	std::string textTemp;
	std::string fileName;
	long long fileSize;
//	std::list<std::string> fileNameList;
	std::ifstream file;

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
	msg.append(to);
	msg.append("\n");
	msg.append(subject);
	msg.append("\n");
	msg.append(text);
	msg.append(std::to_string(fileSize));
	msg.append("\n");
	if (fileName != "") {
		int pos = fileName.find_last_of("/");
		msg.append(fileName.substr(pos + 1, fileName.length()));
		msg.append("\n");
	}

	// Send Protocol
	sendProtocol(create_socket, msg);

	// Send Attachments
	if (fileSize != 0) {
		sendFileToServer(create_socket, &file, fileSize);
		file.close();
	}

	//Receive Protocol
	response = receiveProtocol(create_socket);

	if (response == "OK\n") {
		std::cout << "Message has been sent successfully" << std::endl;
	} else {
		std::cout << "There was an error sending the message" << std::endl;
	}

	return response;
}
std::string list(int create_socket) {

	std::string msg;
	std::string response;

	msg.append("LIST\n");

	// Send Protocol
	sendProtocol(create_socket, msg);

	//Receive Protocol
	char countListChar[BUF];
	Helper::readline(create_socket, countListChar, BUF - 1);
	int countList = atoi(countListChar);
	response.append(countListChar);
	//Get all list elements
	while (countList != 0) {
		char responseChar[BUF];
		Helper::readline(create_socket, responseChar, BUF - 1);
		response.append(std::string(responseChar));
		countList--;
	}

	//AUSGABE
	std::cout << response << std::endl;

	return response;
}
std::string read(int create_socket) {

	std::string msg;
	std::string response;

	std::string nr;

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("READ\n");
	msg.append(nr);
	msg.append("\n");

	// Send Protocol
	sendProtocol(create_socket, msg);

	//Receive Protocol
	//msgNr
	char msgNrChar[BUF];
	Helper::readline(create_socket, msgNrChar, BUF - 1);
	response.append("Msg-Nr  : ");
	response.append(msgNrChar);
	//from
	char fromChar[BUF];
	Helper::readline(create_socket, fromChar, BUF - 1);
	response.append("From    : ");
	response.append(fromChar);
	//from
	char toChar[BUF];
	Helper::readline(create_socket, toChar, BUF - 1);
	response.append("To      : ");
	response.append(toChar);

	//ATT and/or Text

	long long fileSize = 0;
	std::string fileName;
	char textTempChar[BUF];
	std::string textTempStr;
	std::string text;
	char lastChar = 0;
	int sizeText = 1;

	do {
		lastChar = textTempStr[sizeText - 1];

		textTempStr = "";
		textTempChar[0] = '\0';
		sizeText = Helper::readline(create_socket, textTempChar,
		BUF - 1);
		//Attachment
		if (strcmp(textTempChar, "ATT\n") == 0) {
			//FileName
			char fileNameChar[BUF];
			Helper::readline(create_socket, fileNameChar, BUF - 1);
			response.append("FileName: ");
			response.append(fileNameChar);
			fileName = Helper::removeNewline(std::string(fileNameChar));
			//Size
			char fileSizeChar[BUF];
			Helper::readline(create_socket, fileSizeChar, BUF - 1);
			response.append("FileSize: ");
			response.append(fileSizeChar);
			fileSize = atoi(fileSizeChar);
		}

		textTempStr = std::string(textTempChar);

		if (lastChar != '\n' || textTempStr.compare(".\n") != 0) {
			text.append(textTempStr);
		}
	} while (lastChar != '\n' || textTempStr.compare(".\n") != 0);

	//ReadFile

	if (fileSize != 0) {
		char* file = new char[fileSize];
		bzero(file, fileSize);
		int toRead;
		char* pos = file;

		while (fileSize > 0) {

			if (fileSize < BUF) {
				toRead = fileSize;
			} else {
				toRead = BUF;
			}
			char readBuffer[toRead];
			bzero(readBuffer, toRead);

			recv(create_socket, readBuffer, toRead, 0);

			memcpy(pos, readBuffer, toRead);
			//std::cout << readBuffer << std::endl;
			pos += toRead;
			fileSize -= toRead;
		}
		//write file
		std::stringstream attachmentPath;
		attachmentPath << fileName;
		std::ofstream outfile(attachmentPath.str(), std::ofstream::binary);
		outfile.write(file,fileSize);
		outfile.close();
	}
	//response = receiveProtocol(create_socket);

	//AUSGABE
	std::cout << response << std::endl;

	return response;
}
std::string del(int create_socket) {

	std::string msg;
	std::string response;

	std::string nr;

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);

	msg.append("DEL\n");
	msg.append(nr);
	msg.append("\n");

	// Send Protocol
	sendProtocol(create_socket, msg);
	//Receive Protocol
	response = receiveProtocol(create_socket);

	//AUSGABE
	std::cout << response << std::endl;

	return response;
}
//TODO delete
std::string sendDebug(int create_socket) {

	std::string msg;
	std::string response;

	std::string to = "if12b046";
	std::string subject = "Hallo es kommt gleich ein file";
	std::string text =
			"Das ist der Text zu dem File\nDer Text ist aber nicht sehr spannend\n.\n";
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

	// Send Protocol
	sendProtocol(create_socket, msg);
	//Receive Protocol
	response = receiveProtocol(create_socket);

	// Send Attachments
	if (fileSize != 0) {
		sendFileToServer(create_socket, &file, fileSize);
		file.close();
	}
	return response;
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
		// LOGIN
		response = "";
		response = login(create_socket, &username);

		if (response == "BAN\n") {
			std::cout << "To many tries, you have been baned!\n" << std::endl;
			exit(EXIT_SUCCESS);
		}

	} while (response != "OK\n");

	printCommands();

	//start Command block
	do {
		command = "";
		std::cout << "Command: ";
		msg = "";

		//read Command
		getline(std::cin, command);

		if (command != "q") {
			if (command == "s") {
				send(create_socket);
			} else if (command == "l") {
				list(create_socket);
			} else if (command == "r") {
				read(create_socket);
			} else if (command == "d") {
				del(create_socket);
			} else if (command == "w") {
				sendDebug(create_socket);
			} else {
				std::cout << "unknown command!" << std::endl;
			}

#ifdef _DEBUG
			//std::cout << msg << std::endl;
#endif

		}
	} while (command != "q");
	close(create_socket);
	return EXIT_SUCCESS;
}
