/*
 * UdsRegClient.hpp
 *
 *  Created on: 25.02.2015
 *      Author: dnoack
 */

#ifndef INCLUDE_UDSREGCLIENT_HPP_
#define INCLUDE_UDSREGCLIENT_HPP_

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <list>
#include <pthread.h>
#include "signal.h"
#include <string>

#include "UdsRegClient.hpp"
#include "UdsRegWorker.hpp"
#include "JsonRPC.hpp"

using namespace std;


class UdsRegClient{


	public:
		UdsRegClient(const char* pluginName, int pluginNumber,const char* regPath, int size, const char* comPath);
		~UdsRegClient();

		bool connectToRSD();
		void unregisterFromRSD();



	private:

		static struct sockaddr_un address;
		static socklen_t addrlen;

		UdsRegWorker* regWorker;
		JsonRPC* json;
		bool ready;
		int optionflag;
		int currentSocket;
		const char* pluginName;
		int pluginNumber;
		const char* pluginPath;

		int sendData(string* data);
};


#endif /* INCLUDE_UDSREGCLIENT_HPP_ */
