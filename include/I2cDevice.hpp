#ifndef INCLUDE_I2CDEVICE_HPP_
#define INCLUDE_I2CDEVICE_HPP_


/**
 * \class I2cDevice
 * \brief Represents  an I²C device.
 * I2cDevice represents an I²C device regardless what hardware the device has.
 * Currently we just know Totalphase Aardvark as I²C device, adding other hardware devices
 * can result in adding additional variables to this class.
 */
class I2cDevice{

	public:

		/**
		 * Base-constructor.
		 * \param name Name of the device hardware.
		 * \param port Accesspoint to open a AArdvark device.
		 * \param identification Unique identification of a device, like serial number.
		 */
		I2cDevice(const char* name, int port, unsigned int identification)
		{
			this->name = name;
			this->identification = identification;
			this->port = port;
		};

		/**Base-destructor.*/
		virtual ~I2cDevice(){};


		/**\return Name of the hardware used for this device.*/
		const char* getName(){return this->name;}


		/** \return Unique identification number of this device.*/
		unsigned int getIdentification(){return this->identification;}


		/** \return Port of for this device.*/
		int getPort(){return this->port;}


	private:
		/* Name of the device hardware.*/
		const char* name;
		/* Unique identification of a device, like serial number.*/
		unsigned int identification;
		/* Accesspoint to open a AArdvark device.*/
		int port;

};



#endif /*INCLUDE_I2CDEVICE_HPP_ */
