#ifndef I2C_H_
#define I2C_H_

/*! Timeout in seconds for waiting for a subresponse*/
#define SUBRESPONSE_TIMEOUT 180

#include <pthread.h>
#include <ctime>

#include "document.h"
#include "writer.h"

#include "ComPointB.hpp"
#include <RPCInterface.hpp>
#include "WorkerThreads.hpp"
#include "ProcessInterfaceB.hpp"
#include "JsonRPC.hpp"
#include "I2cDevice.hpp"
#include "OutgoingMsg.hpp"
#include "IncomingMsg.hpp"


using namespace rapidjson;

class I2c;

/** Signature of a functionpointer to a memberfunction of I2c.*/
typedef bool (I2c::*i2cfptr)(Value&, Value&);


/**
 * \class I2c
 * I2c is a device independent plugin. Device independent means that it has no underlying
 * driver or driver api assigned to hardware. It uses other plugins, through RSD, to control
 * hardware. It is possible to make the RPC functions of I2c more complex/intelligent and through that
 * the origin client requests more easy. For the functionality of sending and receiving sub-requests
 * to and from other plugins it uses ProcessInterfacB and ComPointB, which are slightly different from
 * ProcessInterface and ComPoint. After sending a sub-request, the corresponding function will block and
 * wait for a sub-response. ComPointB can receive further messages during this block and let them analyze through
 * ProcessInterfaceb. If the incoming message is the message the functions waits for, the function stops blocking
 * and continues to work.
 */
class I2c : public ProcessInterfaceB, public RPCInterface<I2c*, i2cfptr>
{
	public:

		/**Base-constructor.*/
		I2c();


		/**Base-destructor.*/
		~I2c();


		/**
		 * Analyzes the incoming message and executes a requested function of I2c.
		 * Only json rpc requests or notification can be processed by I2c.
		 * Response or anything else will be discarded. Notifications are only used for binding
		 * a I2c instance to a ConnectionContext.
		 * \param input The incoming message we want to process.
		 * \return Outgoing message containing a json rpc response or error response.
		 */
		OutgoingMsg* process(IncomingMsg* input);


		/**
		 * Checks if a message is a json rpc response and if the json rpc id is the same
		 * as the one from the last main-request.
		 * \param rpcMsg The message that should be analyzed.
		 * \return True if the message is the corresponding subResponse to a main-request, false otherwise.
		 */
		bool isSubResponse(RPCMsg* rpcMsg);


	private:

		/** Stores all I2cDevices which can be get through rpc messages to the corresponding plugins.*/
		list<I2cDevice*> deviceList;
		/** Json RPC parser.*/
		JsonRPC* json;
		/** DOM for the main-request.*/
		Document* mainRequestDom;
		/** DOM for the sub-response.*/
		Document* subResponseDom;
		/** */
		rapidjson::MemoryPoolAllocator<> subRequestAllocator;

		/*! Final response message.*/
		const char* mainResponse;
		/*! Request to another plugin.*/
		const char* subRequest;
		/*! Response from another plugin. */
		const char* subResponse;
		//for generating json rpc error responses
		const char* error;
		/*! The json rpc id value of the current processing main request (received from RSD).*/
		Value* requestId;
		/*! Containing the value "result" of the las sub-response.*/
		Value* subResult;


		/*Sigset for configuring SIGUSR2 to signal the Reception of subresponses.*/
		sigset_t set;
		/* Contains the timeout configuration for waiting to subresponses.*/
		struct timespec timeout;


		/** Deletes the deviceList, all Devices will be deallocated.*/
		void deleteDeviceList();

		/**
		 * Calls all function to gather information about all devices with I²C interfaces.
		 * \params Can be an empty rapidjson::Value.
		 * \return Will contain a named array for every different I²C hardware.
		 *  The array-name will be the name of the hardware and will contain the unique identifiers.
		 */
		bool getI2cDevices(Value &params, Value &result);


		/**
		 * Gets all Aardvark devices and saves them to a list.
		 * For getting the information about the aardvark devices, a sub-request to the Aardvark-Plugin will be send
		 * and a sub-response will be received.
		 * \return Will contain a array named "Aardvark" and all serial numbers of the different Aardvark devices which are available.
		 */
		bool getAardvarkDevices(Value &params, Value &result);


		/**
		 * Sends aa_open, aa_target_power, aa_i2c_write and aa_close as json rpc requests to the Aardvark-Plugin.
		 * Every request is send as sub-request, while waiting for a sub-response the function will block. Every
		 * sub-request will have the same json rpc id as the main request. Because of that, a sub-response can assigned
		 * to a sub-request and/or the main-request. If everything works fine, the function will send a json rpc response for
		 * the main-request. If something goes wrong a json rpc error response will be send immediately and aa_write will be aborted.
		 *
		 */
		bool write(Value &params, Value &result);



		bool read(Value &params, Value &result);


		/**
		 * Sends a json rpc request (sub-request) to the Aardvark-plugin and waits for the
		 * corresponding sub-response. On success the function will add the received result
		 * to the value params, so that further functions can use it.
		 * \throws Error If a json rpc error response was received as sub-response.
		 */
		void aa_open(Value &params);


		/**
		 * Sends a json rpc request (sub-request) to the Aardvark-plugin and waits for the corresponding
		 * sub-response.
		 * \throws Error If the received json rpc response contains a negative return value.
		 */
		void aa_target_power(Value &params);



		void aa_write(Value &params);



		void aa_read(Value &params, Value &result);


		/**
		 * Sends a json rpc request (sub-request) to the Aardvark-plugin and waits for the corresponding
		 * sub-response.
		 * \throws Error If the received json rpc response contains a negative return value.
		 */
		void aa_close(Value &params);


		/**
		 * Checks if the sub response is a result or error.
		 * \param dom DOM containing json rpc response/error.
		 * \return False if the DOM contains a json rpc response erorr, true otherwise.
		 */
		bool checkSubResult(Document* dom);


		/**
		 * Waits a specific time for a incoming sub-response by waiting for the signal SIGUSR2.
		 * If the signal was received, the function will just exit.
		 * \throws Error If the signal SIGUSR2 was not received within the specified time.
		 * \note Timeout is set through SUBRESPONSE_TIMEOUT define.
		 */
		void waitForResponse();


		/**
		 * Searches for a device in the deviceList by its uniqueId and return the corresponding port.
		 * \param uniqueId The unique id of the device (most likely the serial number).
		 * \param The corresponding port.
		 */
		int getPortByUniqueId(unsigned int uniqueId);

};

#endif /* I2C_H_ */
