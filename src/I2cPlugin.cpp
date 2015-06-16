#include <I2cPlugin.hpp>
#include "I2c.hpp"



I2cPlugin::I2cPlugin(PluginInfo* pluginInfo) : PluginInterface(pluginInfo)
{
	I2c* tempDriver = new I2c();
	list<string*>* functionList = tempDriver->getAllFunctionNames();
	delete tempDriver;

	StartAcceptThread();
	if(wait_for_accepter_up() != 0)
		throw Error("Creation of Listener/worker threads failed.");

	pluginActive = true;

	regClient = new RegClient(pluginInfo, functionList, REG_PATH);
}


I2cPlugin::~I2cPlugin()
{
	delete regClient;
}


void I2cPlugin::thread_accept()
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
			comPoint = new ComPointB(new_socket, i2c, pluginNumber);
			comPoint->configureLogInfo(&infoIn, &infoOut, &info);
			//dyn_print("Uds---> sNew UdsWorker with socket: %d \n", new_socket);
			pushComPointList(comPoint);
		}
	}
}


#ifndef TESTMODE
int main(int argc, const char** argv)
{
	PluginInfo* pluginInfo = new PluginInfo(PLUGIN_NAME, PLUGIN_NUMBER, COM_PATH);
	I2cPlugin* plugin = new I2cPlugin(pluginInfo);

	plugin->start();

	delete plugin;
	delete pluginInfo;
	return 0;
}
#endif

