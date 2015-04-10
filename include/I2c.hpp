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

typedef bool (I2c::*afptr)(Value&, Value&);


class I2c : public DriverInterface<I2c*, afptr>
{
	public:


		I2c(UdsComWorker* udsWorker) :DriverInterface<I2c*, afptr>(this)
		{
			afptr temp;
			this->response = NULL;
			this-> udsWorker = udsWorker;
			this->subResponse = NULL;
			this->subRequest = NULL;
			sigemptyset(&set);
			sigaddset(&set, SIGUSR2);
			timeout.tv_sec = 5;
			timeout.tv_nsec = 0;

			msgList = NULL;
			json = new JsonRPC();
			temp = &I2c::write;
			funcMap.insert(pair<const char*, afptr>("i2c.write", temp));

			temp= &I2c::getI2cDevices;
			funcMap.insert(pair<const char*, afptr>("i2c.getI2cDevices", temp));

			funcList = getAllFunctionNames();
		}


		~I2c()
		{
			delete json;
		};


		static list<string*>* getFuncList(){return funcList;}

		string* processMsg(string* msg);

	private:

		static list<string*>* funcList;
		static list<I2cDevice*> deviceList;

		JsonRPC* json;
		string* response;
		string* subResponse;
		string* subRequest;
		Value lastMethod;
		UdsComWorker* udsWorker;
		list<string*>* msgList;
		sigset_t set;
		struct timespec timeout;

		bool getI2cDevices(Value &params, Value &result);
		void getAardvarkDevices();

		bool write(Value &params, Value &result);
		Value* aa_open();
		Value* aa_write();
		string* waitForResponse();
};

#endif /* I2C_H_ */
