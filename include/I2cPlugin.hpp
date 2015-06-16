#ifndef INCLUDE_I2CPLUGIN_HPP_
#define INCLUDE_I2CPLUGIN_HPP_


#define REG_PATH "/tmp/RsdRegister.uds"
#define COM_PATH "/tmp/i2cdip.uds"
#define PLUGIN_NAME "i2c"
#define PLUGIN_NUMBER 2
#define WAIT_TIME 3 //wait time for loop


#include <ComServer.hpp>
#include "RegClient.hpp"

class RegClient;

class I2cPlugin {

	public:
		I2cPlugin();
		~I2cPlugin();


		void start();

	private:

		ComServer* comServer;
		RegClient* regClient;
		bool pluginActive;
		sigset_t sigmask;
		sigset_t origmask;
};

#endif /* INCLUDE_I2CPLUGIN_HPP_ */
