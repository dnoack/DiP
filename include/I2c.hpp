/*
 * I2c.hpp
 *
 *  Created on: 17.03.2015
 *      Author: dnoack
 */


#ifndef I2C_H_
#define I2C_H_

#include "document.h"
#include "writer.h"

#include "DriverInterface.h"
#include "JsonRPC.hpp"
#include "I2cDevice.hpp"


using namespace rapidjson;

class I2c;
class UdsComWorker;

typedef bool (I2c::*i2cfptr)(Value&, Value&);


class I2c : public DriverInterface<I2c*, i2cfptr>
{
	public:


		I2c(UdsComWorker* udsWorker) :DriverInterface<I2c*, i2cfptr>(this)
		{
			i2cfptr temp;
			this->response = NULL;
			this-> udsWorker = udsWorker;
			this->subResponse = NULL;
			this->subRequest = NULL;
			this->requestMethod = NULL;
			this->subResult = NULL;
			this->requestId = NULL;

			sigemptyset(&set);
			sigaddset(&set, SIGUSR2);
			timeout.tv_sec = 5;
			timeout.tv_nsec = 0;

			msgList = NULL;
			json = new JsonRPC();
			temp = &I2c::write;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.write", temp));

			temp = &I2c::getAardvarkDevices;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.getAardvarkDevices", temp));

			temp= &I2c::getI2cDevices;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.getI2cDevices", temp));

			funcList = getAllFunctionNames();
		}


		~I2c()
		{
			delete json;
		};


		static list<string*>* getFuncList(){return funcList;}
		static void deleteFuncList();

		void processMsg(string* msg);

	private:

		static list<string*>* funcList;
		static list<I2cDevice*> deviceList;


		static void deleteDeviceList();

		/*! Final response message.*/
		const char* response;
		/*! Request to another plugin.*/
		const char* subRequest;
		/*! Response from another plugin. */
		string* subResponse;

		JsonRPC* json;


		Value* requestMethod;
		Value* requestId;
		Value* subResult;

		UdsComWorker* udsWorker;
		list<string*>* msgList;
		sigset_t set;
		struct timespec timeout;
		Document dom;

		bool getI2cDevices(Value &params, Value &result);
		bool getAardvarkDevices(Value &params, Value &result);

		bool write(Value &params, Value &result);
		Value* aa_open(Value &params);
		Value* aa_target_power(Value &params);
		Value* aa_write();
		string* waitForResponse();

		void deleteMsgList();
		int getPortByUniqueId(unsigned int);


};

#endif /* I2C_H_ */
