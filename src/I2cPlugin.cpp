#include <I2cPlugin.hpp>
#include "UdsServer.hpp"
#include "I2c.hpp"


list<string*>* I2cPlugin::funcList;


I2cPlugin::I2cPlugin()
{
	pluginActive = true;
	//get List of key, which are supported by the driver
	I2c* tempDriver = new I2c(NULL);
	funcList = tempDriver->getAllFunctionNames();
	delete tempDriver;

	regClient = new UdsRegClient(PLUGIN_NAME, PLUGIN_NUMBER, REG_PATH, sizeof(REG_PATH), COM_PATH);
	comServer = new UdsServer(COM_PATH, sizeof(COM_PATH));
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
		regClient->registerToRSD();

		pluginActive = true;
		while(pluginActive)
		{
			sleep(WAIT_TIME);
			comServer->checkForDeletableWorker();
		}
	}
	catch(PluginError &e)
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
}


int main(int argc, const char** argv)
{
	I2cPlugin* plugin = new I2cPlugin();
	plugin->start();
	delete plugin;
	return 0;
}

