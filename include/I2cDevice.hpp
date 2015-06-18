/*
 * I2cDevice.hpp
 *
 *  Created on: 10.04.2015
 *      Author: dnoack
 */

#ifndef I2C_PLUGIN_INCLUDE_I2CDEVICE_HPP_
#define I2C_PLUGIN_INCLUDE_I2CDEVICE_HPP_


class I2cDevice{

	public:
		I2cDevice(const char* name, int port, unsigned int identification)
		{
			this->name = name;
			this->identification = identification;
			this->port = port;
		};


		virtual ~I2cDevice(){};


		const char* getName(){return this->name;}
		unsigned int getIdentification(){return this->identification;}
		int getPort(){return this->port;}


	private:
		const char* name;
		unsigned int identification;
		int port;

};



#endif /* I2C_PLUGIN_INCLUDE_I2CDEVICE_HPP_ */
