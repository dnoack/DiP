/*
 * I2c.hpp
 *
 *  Created on: 17.03.2015
 *      Author: dnoack
 */


#ifndef I2C_H_
#define I2C_H_

/*! Timeout in seconds for waiting for a subresponse*/
#define SUBRESPONSE_TIMEOUT 180

#include <pthread.h>
#include <ctime>

#include "document.h"
#include "writer.h"

#include "DriverInterface.h"
#include "WorkerThreads.hpp"
#include "ProcessInterfaceB.hpp"
#include "JsonRPC.hpp"
#include "I2cDevice.hpp"
#include "RPCMsg.hpp"



using namespace rapidjson;

class I2c;


typedef bool (I2c::*i2cfptr)(Value&, Value&);


class I2c : public ProcessInterfaceB, public DriverInterface<I2c*, i2cfptr>
{
	public:


		I2c() : DriverInterface<I2c*, i2cfptr>(this)
		{
			i2cfptr fptr;

			subResponse = NULL;
			error = NULL;
			subRequest = NULL;
			requestMethod = NULL;
			subResult = NULL;
			requestId = NULL;
			subResponseId = NULL;
		    mainResponse = NULL;
			json = new JsonRPC();
			mainRequestDom = new Document();
			subResponseDom = new Document();

			//configure signal SIGUSR2 and timeout for receiving subresponses
			sigemptyset(&set);
			sigaddset(&set, SIGUSR2);
			timeout.tv_sec = SUBRESPONSE_TIMEOUT;
			timeout.tv_nsec = 0;


			fptr = &I2c::write;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.write", fptr));
			fptr = &I2c::getAardvarkDevices;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.getAardvarkDevices", fptr));
			fptr= &I2c::getI2cDevices;
			funcMap.insert(pair<const char*, i2cfptr>("i2c.getI2cDevices", fptr));
		}


		~I2c()
		{
			delete json;
			delete mainRequestDom;
			delete subResponseDom;

		};


		/**
		 * Main method for processing a incomming message.
		 * - First the message will be checked for multiple json rpc messages, the result is within msgList.
		 * - msgList can now contain correct json rpc messages and not parsable messages.
		 * - msgList will then be iterated and every and a parse function will try to parse the message.
		 * - A correct message will be forward to executeFunction()
		 * - A incorrect message will result into a thrown exception and sending a error response (json rpc) back.
		 * \param msg A string containing a Json RPC request or notification.
		 */
		void process(RPCMsg* msg);
		bool isSubResponse(RPCMsg* rpcMsg);

		bool isRequestInProcess();

	private:

		/** Stores all I2cDevices which can be get through rpc messages to the corresponding plugins.*/
		static list<I2cDevice*> deviceList;

		/** Deletes the deviceList, all Devices will be deallocated.*/
		static void deleteDeviceList();

		JsonRPC* json;
		Document* mainRequestDom;
		Document* subResponseDom;
		rapidjson::MemoryPoolAllocator<> subRequestAllocator;

		/*! Final response message.*/
		const char* mainResponse;
		/*! Request to another plugin.*/
		const char* subRequest;
		/*! Response from another plugin. */
		const char* subResponse;
		//for generating json rpc error responses
		const char* error;


		Value* requestMethod;
		/*! The json rpc id value of the current processing main request (received from RSD).*/
		Value* requestId;

		Value* subResponseId;

		Value* subResult;


		/*Sigset for configuring SIGUSR2 to signal the Reception of subresponses.*/
		sigset_t set;
		/* Contains the timeout configuration for waiting to subresponses.*/
		struct timespec timeout;


		bool getI2cDevices(Value &params, Value &result);
		bool getAardvarkDevices(Value &params, Value &result);
		bool write(Value &params, Value &result);


		void aa_open(Value &params);
		void aa_target_power(Value &params);
		void aa_write(Value &params);
		void aa_close(Value &params);

		bool checkSubResult(Document* dom);
		void waitForResponse();

		int getPortByUniqueId(unsigned int);

};

#endif /* I2C_H_ */
