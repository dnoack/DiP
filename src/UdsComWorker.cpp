/*
 * UdsComWorker.cpp
 *
 *  Created on: 09.02.2015
 *      Author: dnoack
 */

#include <sys/select.h>
#include "errno.h"

#include "UdsComWorker.hpp"
#include "UdsServer.hpp"
#include "Plugin_Error.h"
#include "I2c.hpp"




UdsComWorker::UdsComWorker(int socket)
{

	this->request = 0;
	this->response = 0;
	this->requestInProgress = false;
	this->currentSocket = socket;
	this->i2c = new I2c(this);

	StartWorkerThread();
}



UdsComWorker::~UdsComWorker()
{
	delete i2c;

	pthread_cancel(getListener());
	pthread_cancel(getWorker());

	WaitForListenerThreadToExit();
	WaitForWorkerThreadToExit();

	deleteReceiveQueue();

}


int UdsComWorker::uds_send(string* data)
{
	//printf("Sending: %s\n", data->c_str());
	return send(currentSocket, data->c_str(), data->size(), 0);
}


int UdsComWorker::uds_send(const char* data)
{
	return send(currentSocket, data, strlen(data), 0);
}




void UdsComWorker::thread_work()
{

	worker_thread_active = true;


	StartListenerThread();
	configSignals();

	while(worker_thread_active)
	{
		request = NULL;
		//wait for signals from listenerthread

		sigwait(&sigmask, &currentSig);
		switch(currentSig)
		{
			case SIGUSR1:
				while(getReceiveQueueSize() > 0)
				{
					requestInProgress = true;
					request = receiveQueue.back();
					printf("Received: %s\n", request->c_str());

					i2c->processMsg(request);

					popReceiveQueue();
					requestInProgress = false;
				}
				break;


			default:
				printf("UdsComWorker: unkown signal \n");
				break;
		}
	}
	close(currentSocket);
}



void UdsComWorker::thread_listen()
{

	listen_thread_active = true;
	int retval;
	fd_set rfds;
	pthread_t worker_thread = getWorker();
	configSignals();

	FD_ZERO(&rfds);
	FD_SET(currentSocket, &rfds);

	while(listen_thread_active)
	{
		memset(receiveBuffer, '\0', BUFFER_SIZE);
		ready = true;

		retval = pselect(currentSocket+1, &rfds, NULL, NULL, NULL, &origmask);

		if(retval < 0)
		{
			deletable = true;
			listen_thread_active = false;
		}
		else if(FD_ISSET(currentSocket, &rfds))
		{
			recvSize = recv( currentSocket , receiveBuffer, BUFFER_SIZE, MSG_DONTWAIT);

			if(recvSize > 0)
			{
				if(!requestInProgress)
				{
					//add received data in buffer to queue
					pushReceiveQueue(new string(receiveBuffer, recvSize));
					pthread_kill(worker_thread, SIGUSR1);
				}
				else
				{
					push_backReceiveQueue(new string(receiveBuffer, recvSize));
					pthread_kill(worker_thread, SIGUSR2);
					//create worker busy flag, if worker is NOT busy send SIGUSR1 else send SIGUSR2
					//sigusr1 will be for completely new request, sigusr2 will be for requests which are part of
					// a bigger request
				}
			}
			//RSD invoked shutdown
			else
			{
				deletable = true;
				listen_thread_active = false;
			}

		}
	}
}



