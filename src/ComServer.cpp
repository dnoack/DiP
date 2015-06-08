#include <ComServer.hpp>
#include "JsonRPC.hpp"



UdsServer::UdsServer( const char* udsFile, int nameSize)
{
	optionflag = 1;
	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, udsFile, nameSize);
	addrlen = sizeof(address);

	pthread_mutex_init(&wLmutex, NULL);

	unlink(udsFile);
	setsockopt(connection_socket, SOL_SOCKET, SO_REUSEADDR, &optionflag, sizeof(optionflag));
	bind(connection_socket, (struct sockaddr*)&address, addrlen);


	StartAcceptThread();

	if(wait_for_accepter_up() != 0)
		throw Error("Creation of Listener/worker threads failed.");

}


UdsServer::~UdsServer()
{
	pthread_t accepter = getAccepter();
	if(accepter != 0)
		pthread_cancel(accepter);

	WaitForAcceptThreadToExit();

	deleteComPointList();
	close(connection_socket);
	pthread_mutex_destroy(&wLmutex);
}


void UdsServer::thread_accept()
{
	int new_socket = 0;
	ComPointB* comPoint = NULL;
	I2c* i2c = NULL;
	listen(connection_socket, MAX_CLIENTS);

	//dyn_print("Accepter created\n");
	while(true)
	{
		new_socket = accept(connection_socket, (struct sockaddr*)&address, &addrlen);
		if(new_socket > 0)
		{
			i2c = new I2c();
			comPoint = new ComPointB(new_socket, i2c, 2);
			//TODO: enter plugin id instead of just 1
			//dyn_print("Uds---> sNew UdsWorker with socket: %d \n", new_socket);
			pushComPointList(comPoint);
		}
	}
}


void UdsServer::deleteComPointList()
{
	list<ComPointB*>::iterator worker= comPointList.begin();

	while(worker != comPointList.end())
	{
		delete *worker;
		worker = comPointList.erase(worker);
	}
}


void UdsServer::pushComPointList(ComPointB* comPoint)
{
	pthread_mutex_lock(&wLmutex);
	comPointList.push_back(comPoint);
	//dyn_print("Uds---> Number of UdsWorker: %d \n", comPointList.size());
	pthread_mutex_unlock(&wLmutex);
}


void UdsServer::checkForDeletableWorker()
{
	pthread_mutex_lock(&wLmutex);

	list<ComPointB*>::iterator i = comPointList.begin();
	while(i != comPointList.end())
	{
		if((*i)->isDeletable())
		{
			//dyn_print("Uds---> Deleting UdsWorker. %d still left.\n", comPointList.size()-1);
			delete *i;
			i = comPointList.erase(i);
		}
		else
			++i;
	}
	pthread_mutex_unlock(&wLmutex);
}
