/*
 * UdsWorker.hpp
 *
 *  Created on: 09.02.2015
 *      Author: dnoack
 */

#ifndef INCLUDE_UDSCOMWORKER_HPP_
#define INCLUDE_UDSCOMWORKER_HPP_


//unix domain socket definition
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>


#include "JsonRPC.hpp"
#include "WorkerInterface.hpp"
#include "WorkerThreads.hpp"
#include "I2c.hpp"


class UdsServer;


class UdsComWorker : public WorkerInterface<string>, public WorkerThreads{

	public:

		UdsComWorker(int socket);
		~UdsComWorker();

		int uds_send(string* data);
		int uds_send(const char* data);

		//should only be called within thread_Work thread !
		string* getNextMsg()
		{
			string* retValue = receiveQueue.back();
			popReceiveQueueWithoutDelete();
			return retValue;
		}

	private:

		virtual void thread_listen();

		virtual void thread_work();


		bool requestInProgress;
		string* request;
		string* response;
		int currentSocket;
		I2c* i2c;

};


#endif /* INCLUDE_UDSWORKER_HPP_ */
