/*
 * I2cDevice.hpp
 *
 *  Created on: 10.04.2015
 *      Author: dnoack
 */

#ifndef INCLUDE_I2CDEVICE_HPP_
#define INCLUDE_I2CDEVICE_HPP_


class I2cDevice{

	I2cDevice(const char* name, int identification, int port)
	{
		this->name = name;
		this->identification = identification;
		this->port = port;
	};


	virtual ~I2cDevice()
	{
		delete name;
	};


	const char* getName(){return this->name;}
	int getIdentification(){return this->identification;}
	int getPort(){return this->port;}


	private:
		const char* name;
		int identification;
		int port;

};



#endif /* INCLUDE_I2CDEVICE_HPP_ */
