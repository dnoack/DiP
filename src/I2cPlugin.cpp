#include <ComServer.hpp>
#include <I2cPlugin.hpp>
#include "I2c.hpp"



I2cPlugin::I2cPlugin()
{
	pluginActive = true;
	sigemptyset(&origmask);
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	sigaddset(&sigmask, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &sigmask, &origmask);

	I2c* tempDriver = new I2c();
	list<string*>* functionList = tempDriver->getAllFunctionNames();
	delete tempDriver;

	regClient = new RegClient(new Plugin(PLUGIN_NAME, PLUGIN_NUMBER, COM_PATH), functionList, REG_PATH);
	comServer = new ComServer(COM_PATH, sizeof(COM_PATH), PLUGIN_NUMBER);
}


I2cPlugin::~I2cPlugin()
{
	delete comServer;
	delete regClient;
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

#ifndef TESTMODE
int main(int argc, const char** argv)
{
	I2cPlugin* plugin = new I2cPlugin();
	plugin->start();
	delete plugin;
	return 0;
}
#endif

