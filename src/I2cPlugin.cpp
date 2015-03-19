/*
 * I2cPlugin.cpp
 *
 *  Created on: 17.03.2015
 *      Author: dnoack
 */


#include <I2cPlugin.hpp>
#include "UdsServer.hpp"
#include "I2c.hpp"



list<string*>* I2cPlugin::funcList;


I2cPlugin::I2cPlugin()
{

	regClientReady = false;
	comServerReady = false;
	pluginActive = true;

	//get List of key, which are supported by the driver
	I2c* tempDriver = new I2c();
	funcList = tempDriver->getAllFunctionNames();
	delete tempDriver;


	regClient = new UdsRegClient(PLUGIN_NAME, PLUGIN_NUMBER, REG_PATH, sizeof(REG_PATH), COM_PATH);
	comServer = new UdsServer(COM_PATH, sizeof(COM_PATH));

}



I2cPlugin::~I2cPlugin()
{
	delete comServer;
	delete regClient;
}




void I2cPlugin::startCommunication()
{
	pluginActive = regClient->connectToRSD();


	while(pluginActive)
	{
		sleep(3);
		comServer->checkForDeletableWorker();
	}

}


int main(int argc, const char** argv)
{

	I2cPlugin* plugin = new I2cPlugin();
	plugin->startCommunication();
	delete plugin;
	return 0;
}


