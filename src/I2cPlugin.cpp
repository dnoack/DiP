#include <ComServer.hpp>
#include <I2cPlugin.hpp>
#include "I2c.hpp"


list<string*>* I2cPlugin::funcList;


I2cPlugin::I2cPlugin()
{
	pluginActive = true;
	sigemptyset(&origmask);
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	sigaddset(&sigmask, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &sigmask, &origmask);
	//get List of key, which are supported by the driver
	I2c* tempDriver = new I2c();
	funcList = tempDriver->getAllFunctionNames();
	delete tempDriver;

	regClient = new RegClient(PLUGIN_NAME, PLUGIN_NUMBER, REG_PATH, COM_PATH);
	comServer = new ComServer(COM_PATH, sizeof(COM_PATH), PLUGIN_NUMBER);
}


I2cPlugin::~I2cPlugin()
{
	delete comServer;
	delete regClient;
	deleteFuncList();
}


void I2cPlugin::start()
{
	try
	{
		regClient->connectToRSD();
		regClient->sendAnnounceMsg();

		pluginActive = true;
		while(pluginActive)
		{
			sleep(WAIT_TIME);
			comServer->checkForDeletableWorker();
			if(regClient->isDeletable())
				pluginActive = false;
		}
	}
	catch(Error &e)
	{
		printf("%s \n", e.get());
	}
}

void I2cPlugin::deleteFuncList()
{
	list<string*>::iterator i = funcList->begin();
	while(i != funcList->end())
	{
		delete *i;
		i = funcList->erase(i);
	}
	delete funcList;
}

#ifndef TESTMODE
int main(int argc, const char** argv)
{
	I2cPlugin* plugin = new I2cPlugin();
	plugin->start();
	delete plugin;
	return 0;
}
#endif

