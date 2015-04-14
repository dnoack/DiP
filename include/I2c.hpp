/*
 * I2c.hpp
 *
 *  Created on: 17.03.2015
 *      Author: dnoack
 */


#ifndef I2C_H_
#define I2C_H_

/*! Timeout in seconds for waiting for a subresponse*/
#define SUBRESPONSE_TIMEOUT 5

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
			i2cfptr fptr;

			this-> udsWorker = udsWorker;
			this->subResponse = NULL;
			this->subRequest = NULL;
			this->requestMethod = NULL;
			this->subResult = NULL;
			this->requestId = NULL;
			this->response = NULL;
			this->msgList = NULL;
			this->subRequestId = 0;
			this->json = new JsonRPC();

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
			funcList = getAllFunctionNames();
		}


		~I2c()
		{
			delete json;
		};


		/**
		 * FuncList will be called by UdsRegWorker during the registration process to RSD.
		 * \return Pointer to a list which contains all functionnames as string.
		 *   This names can be used to call the functions via json rpc request.
		 */
		static list<string*>* getFuncList(){return funcList;}

		/** Deletes funcList and deallocated all of its content.*/
		static void deleteFuncList();

		/**
		 * Main method for processing a incomming message.
		 * - First the message will be checked for multiple json rpc messages, the result is within msgList.
		 * - msgList can now contain correct json rpc messages and not parsable messages.
		 * - msgList will then be iterated and every and a parse function will try to parse the message.
		 * - A correct message will be forward to executeFunction()
		 * - A incorrect message will result into a thrown exception and sending a error response (json rpc) back.
		 * \param msg A string containing a Json RPC request or notification.
		 */
		void processMsg(string* msg);

	private:

		/** Stores all functionname from this class after a call of getAllFunctionNames()*/
		static list<string*>* funcList;
		/** Stores all I2cDevices which can be get through rpc messages to the corresponding plugins.*/
		static list<I2cDevice*> deviceList;

		/** Deletes the deviceList, all Devices will be deallocated.*/
		static void deleteDeviceList();



		/*! Final response message.*/
		const char* response;
		/*! Request to another plugin.*/
		const char* subRequest;
		/*! Response from another plugin. */
		string* subResponse;

		JsonRPC* json;
		Document dom;
		Value* requestMethod;

		/*! The json rpc id value of the current processing main request (received from RSD).*/
		Value* requestId;
		/*! The json rpc id value ot the last send subRequest (request to anther plugin).*/
		int subRequestId;

		Value* subResult;

		/*Corresponding Unix-Domain-Socket Communication modul.*/
		UdsComWorker* udsWorker;
		/*Contains a list of (hopefully) json rpc requests or notifications.*/
		list<string*>* msgList;


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


		string* waitForResponse();

		void deleteMsgList();

		int getPortByUniqueId(unsigned int);


};

#endif /* I2C_H_ */
