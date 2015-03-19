/*
 * UDS.h
 *
 *  Created on: 04.02.2015
 *      Author: dnoack
 */

#ifndef INCLUDE_UDSSERVER_HPP_
#define INCLUDE_UDSSERVER_HPP_

//unix domain socket definition
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <list>
#include <pthread.h>

#include "signal.h"
#include "JsonRPC.hpp"


class UdsComWorker;



class UdsServer{

	public:

		UdsServer(const char* udsFile, int nameSize);

		~UdsServer();

		void start();

		int call();

		void startCom();

		bool isReady(){return ready;}


		static void pushWorkerList(UdsComWorker* newWorker);
		void checkForDeletableWorker();

	private:
		int optionflag;
		static bool ready;

		static int connection_socket;

		//list of pthread ids with all the active worker. push and pop must be protected by mutex
		static list<UdsComWorker*> workerList;
		static pthread_mutex_t wLmutex;

		static struct sockaddr_un address;
		static socklen_t addrlen;


		static void* uds_accept(void*);


};



#endif /* INCLUDE_UDSSERVER_HPP_ */
