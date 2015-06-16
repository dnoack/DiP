#ifndef INCLUDE_I2CPLUGIN_HPP_
#define INCLUDE_I2CPLUGIN_HPP_

#include "PluginInterface.hpp"

#define REG_PATH "/tmp/RsdRegister.uds"
#define COM_PATH "/tmp/i2cdip.uds"
#define PLUGIN_NAME "i2c"
#define PLUGIN_NUMBER 2



class I2cPlugin :public PluginInterface{

	public:
		I2cPlugin(PluginInfo* pluginInfo);
		~I2cPlugin();

		void thread_accept();
};

#endif /* INCLUDE_I2CPLUGIN_HPP_ */
