#ifndef I2CPLUGIN_HPP_
#define I2CPLUGIN_HPP_


#define REG_PATH "/tmp/RsdRegister.uds"
#define COM_PATH "/tmp/i2cdip.uds"
#define PLUGIN_NAME "i2c"
#define PLUGIN_NUMBER 2
#define WAIT_TIME 3 //wait time for loop


#include "UdsServer.hpp"
#include "UdsRegClient.hpp"


class I2cPlugin {

	public:
		I2cPlugin();
		~I2cPlugin();


		static list<string*>* getFuncList(){return funcList;}

		void start();

	private:

		static list<string*>* funcList;

		void deleteFuncList();

		UdsServer* comServer;
		UdsRegClient* regClient;
		bool pluginActive;
		sigset_t sigmask;
		sigset_t origmask;
};

#endif /* I2CPLUGIN_HPP_ */
