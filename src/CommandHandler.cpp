/*
 * CommandHandler.cpp
 *
 *  Created on: 06.11.2013
 *      Author: matthias
 */

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

CommandHandler::CommandHandler(int socket) {
	this->socket = socket;

}

CommandHandler::~CommandHandler() {
}

void CommandHandler::sendProtocol(std::string msg) {

	char buffer[BUF];
	int size;
	bzero(buffer, BUF);
	//buffer[0] = '\0';

	size = send(socket, msg.c_str(), msg.length(), 0);

	if (size == -1) {
		perror("Send error");
		return;
	}
	return;
}

std::string CommandHandler::receiveProtocol() {
	char buffer[BUF];
	int size;
	bzero(buffer, BUF);

	size = recv(socket, buffer, BUF - 1, 0);
	if (size > 0) {
		buffer[size] = '\0';

	}
	std::string response = std::string(buffer);

	return response;
}

bool CommandHandler::sendFileToServer(std::ifstream* file, long long fileSize) {

	file->seekg(0, std::ios::beg);
	int toRead = 0;
	int sendSize;
	char blockOK[2];
	while (fileSize > 0) {

		if (fileSize > BUF) {
			toRead = BUF;
		} else {
			toRead = fileSize;
		}
		char *p = new char[toRead];

		bzero(p, toRead);
		file->read(p, toRead);

		do {
			sendSize = send(socket, p, toRead, 0);
			recv(socket, blockOK, 2, 0);
			if (strcmp(blockOK, "Y") != 0) {
				printf("blockOK: %s\n", blockOK);
			}
		} while (strcmp(blockOK, "Y") != 0);

		if (sendSize < 0) {
			std::cerr << "ERROR writing to socket" << std::endl;
			return false;
		}

		fileSize -= sendSize;

		delete p;
	}
	return true;
}

std::string CommandHandler::login() {

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

	msg.append("LOGIN\n");
	msg.append(username);
	msg.append("\n");
	msg.append(password);
	msg.append("\n");

	response = "";
	sendProtocol(msg);
	response = receiveProtocol();

	return response;

}

void CommandHandler::sendCommand() {

	std::string msg;
	std::string response;

	std::string to;
	std::string toTemp;
	std::string subject;
	std::string text;
	std::string textTemp;
	std::string fileName;
	long long fileSize;
	std::ifstream file;

	//TO
	do {

		toTemp = "";
		std::cout << "To (max 8 characters, press ENTER to end): ";
		getline(std::cin, toTemp);
		if (toTemp.length() > 8) {
			std::cout << "max 8 characters, please enter an other recipient!\n";
		} else {
			if (toTemp.length() > 0) {
				to.append(toTemp);
				to.append(";");
			} else if (toTemp.length() == 0 && to.length() == 0) {
				std::cout << "no recipient choosen!" << std::endl;
				return;
			}

		}
	} while (toTemp != "");

	// SUBJECT
	do {
		std::cout << "Subject (max 80): ";
		getline(std::cin, subject);
		if (subject.length() > 80) {
			std::cout << "Subject is too long, max 80 characters!\n";
		} else if (subject.length() == 0) {
			subject = "[NO SUBJECT]";
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
		} else {
			std::cerr << "File not open" << std::endl;
			return;
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
	sendProtocol(msg);

	// Send Attachments
	if (fileSize != 0) {
		sendFileToServer(&file, fileSize);
		file.close();
	}

	//Receive Protocol
	response = receiveProtocol();

	if (response == "OK\n") {
		std::cout << "Message has been sent successfully" << std::endl;
	} else {
		std::cout << "There was an error sending the message" << std::endl;
	}
}
void CommandHandler::list() {

	std::string msg;
	std::string response;

	msg.append("LIST\n");

	// Send Protocol
	sendProtocol(msg);

	//Receive Protocol
	char countListChar[BUF];
	Helper::readline(socket, countListChar, BUF - 1);
	int countList = atoi(countListChar);
	response.append(countListChar);
	//Get all list elements
	while (countList != 0) {
		char responseChar[BUF];
		Helper::readline(socket, responseChar, BUF - 1);
		response.append(std::string(responseChar));
		countList--;
	}

	//AUSGABE
	std::cout << response << std::endl;
}

void CommandHandler::read() {

	std::string msg;
	std::string response;

	std::string nr;

	//MESSAGE-NUMBER
	std::cout << "Message-Number: ";
	getline(std::cin, nr);
	if (nr.length() == 0) {
		std::cout << "no Message-Numer given!" << std::endl;
		return;
	}

	msg.append("READ\n");
	msg.append(nr);
	msg.append("\n");

	// Send Protocol
	sendProtocol(msg);

	//Receive Protocol

	//msgNr
	char msgNrChar[BUF];
	Helper::readline(socket, msgNrChar, BUF - 1);
	long long msgNr = atol(msgNrChar);

	if (msgNr == 0) {
		char errorMsg[BUF];
		Helper::readline(socket, errorMsg, BUF - 1);
		std::cout << errorMsg << std::endl;
		return;
	}

	response.append("Msg-Nr  : ");
	response.append(msgNrChar);

	//from
	char fromChar[BUF];
	Helper::readline(socket, fromChar, BUF - 1);
	response.append("From    : ");
	response.append(fromChar);

	//to
	char toChar[BUF];
	Helper::readline(socket, toChar, BUF - 1);
	response.append("To      : ");
	response.append(toChar);

	//subject
	char subjectChar[BUF];
	Helper::readline(socket, subjectChar, BUF - 1);
	response.append("Subject : ");
	response.append(subjectChar);

	//fileSize
	long long fileSize = 0;
	std::string fileName;

	char fileSizeChar[BUF];
	Helper::readline(socket, fileSizeChar, BUF - 1);
	response.append("FileSize: ");
	response.append(fileSizeChar);
	fileSize = atoll(fileSizeChar);
	if (fileSize > 0) {
		//FileName
		char fileNameChar[BUF];
		Helper::readline(socket, fileNameChar, BUF - 1);
		response.append("FileName: ");
		response.append(fileNameChar);
		fileName = Helper::removeNewline(std::string(fileNameChar));
	}

	//Text
	char textTempChar[BUF];
	std::string textTempStr;
	std::string text;
	char lastChar = 0;
	int sizeText = 1;

	do {
		lastChar = textTempStr[sizeText - 1];

		textTempStr = "";
		textTempChar[0] = '\0';
		sizeText = Helper::readline(socket, textTempChar, BUF - 1);

		textTempStr = std::string(textTempChar);

		if (lastChar != '\n' || textTempStr.compare(".\n") != 0) {
			text.append(textTempStr);
		}

	} while (lastChar != '\n' || textTempStr.compare(".\n") != 0);

	response.append("Text    : ");
	response.append(text);

	//ReadFile

	if (fileSize != 0) {
		char* file = new char[fileSize];
		bzero(file, fileSize);
		int toRead;
		char* pos = file;
		int readSize;
		long long tempSize = fileSize;
		std::string blockOK;

		while (tempSize > 0) {

			if (tempSize < BUF) {
				toRead = tempSize;
			} else {
				toRead = BUF;
			}
			char readBuffer[toRead];
			bzero(readBuffer, toRead);

			do {
				readSize = recv(socket, readBuffer, toRead, 0);

				if (readSize == toRead) {
					blockOK = "Y";
				} else {
					blockOK = "N";
				}
				send(socket, blockOK.c_str(), 2, 0);
			} while (readSize != toRead);

			memcpy(pos, readBuffer, readSize);
			pos += readSize;
			tempSize -= readSize;
		}

		//write file
		std::stringstream attachmentPath;
		attachmentPath << fileName;
		std::ofstream outfile(attachmentPath.str(), std::ofstream::binary);
		outfile.write(file, fileSize);
		outfile.close();
		delete[] file;
	}

	//AUSGABE
	std::cout << response << std::endl;

}

void CommandHandler::del() {

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
	sendProtocol(msg);
	//Receive Protocol
	response = receiveProtocol();

	//AUSGABE
	std::cout << response << std::endl;
}
