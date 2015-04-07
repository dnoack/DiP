/*
 * I2cPlugin.hpp
 *
 *  Created on: 17.03.2015
 *  Author: dnoack
 */

#ifndef I2CPLUGIN_HPP_
#define I2CPLUGIN_HPP_


#define REG_PATH "/tmp/RsdRegister.uds"
#define COM_PATH "/tmp/ganzneu.uds"
#define PLUGIN_NAME "i2c"
#define PLUGIN_NUMBER 2
#define WAIT_TIME 3 //wait time for loop


#include "UdsServer.hpp"
#include "UdsRegClient.hpp"


class I2cPlugin {

	public:
		I2cPlugin();
		~I2cPlugin();


		void startCommunication();
		void registerToRSD();

		static list<string*>* getFuncList(){return funcList;}


	private:

		static list<string*>* funcList;

		UdsServer* comServer;
		UdsRegClient* regClient;
		bool pluginActive;
};

#endif /* I2CPLUGIN_HPP_ */
