#ifndef AARDVARK_PLUGIN_INCLUDE_UDSSERVER_HPP_
#define AARDVARK_PLUGIN_INCLUDE_UDSSERVER_HPP_


#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <list>
#include <pthread.h>
#include "signal.h"

#include "AcceptThread.hpp"
#include "ComPointB.hpp"
#include "I2c.hpp"
#include "JsonRPC.hpp"

#define MAX_CLIENTS 20



class ComServer : public AcceptThread{

	public:

		ComServer(const char* udsFile, int nameSize, int pluginNumber);
		~ComServer();


		void checkForDeletableWorker();

	private:


		 int connection_socket;
		 int pluginNumber;

		 /*!LogInformation for underlying ComPoints.*/
		LogInformation infoIn;
		LogInformation infoOut;
		LogInformation info;

		list<ComPointB*> comPointList;
		pthread_mutex_t wLmutex;

		struct sockaddr_un address;
		socklen_t addrlen;

		void pushComPointList(ComPointB* newWorker);
		void deleteComPointList();


		virtual void thread_accept();

		int optionflag;
};

#endif /* AARDVARK_PLUGIN_INCLUDE_UDSSERVER_HPP_ */
