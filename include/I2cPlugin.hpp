#ifndef INCLUDE_I2CPLUGIN_HPP_
#define INCLUDE_I2CPLUGIN_HPP_


#include "PluginInterface.hpp"

/*! Path to unix domain socket for registering to RSD.*/
#define REG_PATH "/tmp/RsdRegister.uds"
/*! Path to unix domain socket for receiving json rpcrequests from RSD.*/
#define COM_PATH "/tmp/i2cdip.uds"
/*! Unique name of this plugin.*/
#define PLUGIN_NAME "i2c"
/*! Unique plugin number of this plugin.*/
#define PLUGIN_NUMBER 2


/**
 * \class I2cPlugin
 * \brief Additional plugin for controlling I²C hardware devices by using other plugins.
 * The I2c-Plugin is not directly connected to a Hardware driver or API like AardvarkPlugin.
 * It is a more abstract but also more intelligent plugin, which can use other plugins to process its
 * own requests. Currently it uses the AardvarkPlugin to read / write through the I²C-Interface of an Aardvark.
 * The goal of this plugin is to make some common scenarios much more easier. For example an user want to write
 * something over I²C of an Aardvark, he got to open, configure, write and close the device and for every step he
 * has to send a separate json rpc request. I²C-Plugin unites this 4 requests to one and executes the 4 requests as
 * indepented sub-requests from I²C-Plugin to AardvarkPlugin.
 */
class I2cPlugin :public PluginInterface{

	public:

		/** Base-constructor.
		 * \param pluginInfo Containing necessary information for configuring the plugin.
		 */
		I2cPlugin(PluginInfo* pluginInfo);


		/** Base-destructor.*/
		~I2cPlugin();


		/**
		 * Implementation of the pure virtual function of PluginInterface.
		 * Listens to the configurired Unix domain socket and accepts incoming connection.
		 * A incoming connection will create a new instance of I2c + ComPointB.
		 * I2c implements the processInterface and will be connected to the created ComPoint.
		 * The comPoint will be configured (logging, etc.) and added to the list of comPoints.
		 * \note Because PluginInterfaceB inherits from AcceptThread, this function will run in a separate thread.
		 */
		void thread_accept();
};

#endif /* INCLUDE_I2CPLUGIN_HPP_ */
