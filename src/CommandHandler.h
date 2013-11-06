/*
 * CommandHandler.h
 *
 *  Created on: 06.11.2013
 *      Author: matthias
 */

#ifndef COMMANDHANDLER_H_
#define COMMANDHANDLER_H_

class CommandHandler {
public:
	CommandHandler(int socket);
	virtual ~CommandHandler();
	virtual std::string login();
	virtual void sendCommand();
	virtual void list();
	virtual void read();
	virtual void del();
private:
	int socket;

	virtual void sendProtocol(std::string msg);
	virtual std::string receiveProtocol();
	virtual bool sendFileToServer(std::ifstream* file, long long fileSize);
};

#endif /* COMMANDHANDLER_H_ */
