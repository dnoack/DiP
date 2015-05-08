#ifndef I2C_PLUGIN_INCLUDE_UDSREGCLIENT_HPP_
#define I2C_PLUGIN_INCLUDE_UDSREGCLIENT_HPP_

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

#include <UdsRegWorker.hpp>
#include "UdsRegClient.hpp"
#include "JsonRPC.hpp"
#include "Error.hpp"

using namespace std;


class UdsRegClient{


	public:
		UdsRegClient(const char* pluginName, int pluginNumber,const char* regPath, int size, const char* comPath);
		~UdsRegClient();


		void connectToRSD();
		void registerToRSD();
		void unregisterFromRSD();

	private:

		static struct sockaddr_un address;
		static socklen_t addrlen;

		UdsRegWorker* regWorker;
		bool ready;
		int optionflag;
		int currentSocket;

		const char* pluginName;
		int pluginNumber;
		const char* pluginPath;
};

#endif /* I2C_PLUGIN_INCLUDE_UDSREGCLIENT_HPP_ */
