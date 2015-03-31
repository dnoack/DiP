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
	memset(receiveBuffer, '\0', BUFFER_SIZE);
	this->listen_thread_active = false;
	this->worker_thread_active = false;
	this->recvSize = 0;
	this->lthread = 0;
	this->request = 0;
	this->response = 0;
	this->requestInProgress = false;
	this->currentSocket = socket;
	this->i2c = new I2c(this);

	StartWorkerThread(currentSocket);
}



UdsComWorker::~UdsComWorker()
{
	worker_thread_active = false;
	listen_thread_active = false;
	delete i2c;

	pthread_cancel(getListener());
	pthread_cancel(getWorker());

	WaitForListenerThreadToExit();
	WaitForWorkerThreadToExit();

}


int UdsComWorker::uds_send(string* data)
{
	return send(currentSocket, data->c_str(), data->size(), 0);
}




void UdsComWorker::thread_work(int socket)
{

	worker_thread_active = true;

	//start the listenerthread and remember the theadId of it
	lthread = StartListenerThread(pthread_self(), currentSocket, receiveBuffer);

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
					try
					{
						response = i2c->processMsg(request);
					}
					catch(PluginError &e)
					{
						response = new string(e.get());
					}
					catch(...)
					{
						printf("Unkown exception.\n");
					}

					popReceiveQueue();
					send(currentSocket, response->c_str(), response->size(), 0);

					delete response;
					requestInProgress = false;
				}
				break;

			case SIGUSR2:
				printf("UdsComWorker: SIGUSR2\n");
				break;
			default:
				printf("UdsComWorker: unkown signal \n");
				break;
		}

	}
	close(currentSocket);
}



void UdsComWorker::thread_listen(pthread_t parent_th, int socket, char* workerBuffer)
{

	listen_thread_active = true;
	int retval;
	fd_set rfds;

	configSignals();

	FD_ZERO(&rfds);
	FD_SET(socket, &rfds);

	while(listen_thread_active)
	{
		memset(receiveBuffer, '\0', BUFFER_SIZE);
		ready = true;

		retval = pselect(socket+1, &rfds, NULL, NULL, NULL, &origmask);

		if(retval < 0)
		{
			deletable = true;
			listen_thread_active = false;
		}
		else if(FD_ISSET(socket, &rfds))
		{
			recvSize = recv( socket , receiveBuffer, BUFFER_SIZE, MSG_DONTWAIT);

			if(recvSize > 0)
			{
				if(!requestInProgress)
				{
					//add received data in buffer to queue
					pushReceiveQueue(new string(receiveBuffer, recvSize));
					pthread_kill(parent_th, SIGUSR1);
				}
				else
				{
					pushReceiveQueue(new string(receiveBuffer, recvSize));
					pthread_kill(parent_th, SIGUSR2);
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



