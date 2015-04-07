/*
 * UdsWorker.hpp
 *
 *  Created on: 09.02.2015
 *      Author: dnoack
 */

#ifndef INCLUDE_UDSCOMWORKER_HPP_
#define INCLUDE_UDSCOMWORKER_HPP_

#define BUFFER_SIZE 1024


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


class UdsComWorker : public WorkerInterface, public WorkerThreads{

	public:

		UdsComWorker(int socket);
		~UdsComWorker();

		int uds_send(string* data);

		//should only be called within thread_Work thread !
		string* getNextMsg()
		{
			string* retValue = receiveQueue.back();
			popReceiveQueueWithoutDelete();
			return retValue;
		}

	private:

		virtual void thread_listen(pthread_t partent_th, int socket, char* workerBuffer);

		virtual void thread_work(int socket);


		bool requestInProgress;

		//variables for listener
		bool listen_thread_active;
		char receiveBuffer[BUFFER_SIZE];
		int recvSize;

		//variables for worker
		bool worker_thread_active;
		string* request;
		string* response;


		//not shared, more common
		pthread_t lthread;
		int currentSocket;
		I2c* i2c;

};


#endif /* INCLUDE_UDSWORKER_HPP_ */
