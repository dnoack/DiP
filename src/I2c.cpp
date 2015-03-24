/*
 * I2c.cpp
 *
 *  Created on: 	17.03.2015
 *  Last edited:	20.03.2015
 *  Author: 		dnoack
 */

#include "UdsComWorker.hpp"
#include <I2c.hpp>
#include "unistd.h"
#include "signal.h"

list<string*>* I2c::funcList;


string* I2c::processMsg(string* msg)
{
	Value result;
	Value params;
	Document* dom;

	try
	{
		dom = json->parse(msg);
		if(json->isRequest())
		{
			state = 0;
			lastMethod = (*dom)["method"];
			executeFunction(lastMethod, params, result);
		}
		/*
		else
		{
			if(lastMethod != NULL)
				executeFunction(lastMethod, params, result);
		}*/


	}
	catch(PluginError &e)
	{
		throw;
	}


	return response;
}


bool I2c::write(Value &params, Value &result)
{
	try
	{
		aa_open();
		udsWorker->uds_send(subRequest);
		delete subRequest;
		subResponse = waitForResponse();
		//do something with subResponse


		subRequest = new string("{\"jsonrpc\": \"2.0\", \"params\": {\"handle\": 1 , \"powerMask\" :  3 }, \"method\": \"Aardvark.aa_target_power\", \"id\": 3}");
		udsWorker->uds_send(subRequest);
		delete subRequest;
		subResponse = waitForResponse();


		subRequest = new string("{\"jsonrpc\": \"2.0\", \"params\": { \"handle\": 1 , \"slave_addr\": 56, \"flags\": 0, \"num_bytes\": 2, \"data_out\": [3,0]}, \"method\": \"Aardvark.aa_i2c_write\", \"id\": 4}");
		udsWorker->uds_send(subRequest);
		delete subRequest;
		subResponse = waitForResponse();


		aa_write();
		udsWorker->uds_send(subRequest);
		delete subRequest;
		subResponse = waitForResponse();


		subRequest = new string("{\"jsonrpc\": \"2.0\", \"params\": { \"handle\": 1 } , \"method\": \"Aardvark.aa_close\", \"id\": 7}");
		udsWorker->uds_send(subRequest);
		delete subRequest;
		subResponse = waitForResponse();

		response = new string("{\"jsonrpc\": \"2.0\", \"result\": \"OK\", \"id\": 7}");
	}
	catch(PluginError &e)
	{
		response = new string("{\"jsonrpc\": \"2.0\", \"result\": \"NOT OK\", \"id\": 7}");

	}




	return true;
}


Value* I2c::aa_open()
{
	Value* device = NULL;
	Value id;
	Value method;
	Value localParams;
	Document dom;
	char* localRequest = NULL;

	device = json->tryTogetParam("device");


	method.SetString("Aardvark.aa_open", dom.GetAllocator());
	localParams.SetObject();
	localParams.AddMember("port", *device, dom.GetAllocator());
	id.SetInt(1);

	localRequest = json->generateRequest(method, localParams, id);

	//send through tunnel server
	subRequest = new string(localRequest);

	return device;
}


Value* I2c::aa_write()
{
	Value* data = NULL;
	Value id;
	Value method;
	Value localParams;
	Value array;
	Document dom;
	char* localRequest = NULL;


	method.SetString("Aardvark.aa_i2c_write", dom.GetAllocator());
	localParams.SetObject();
	localParams.AddMember("handle", 1, dom.GetAllocator());

	array.SetArray();
	array.PushBack(1, dom.GetAllocator());
	array.PushBack(191, dom.GetAllocator());

	localParams.AddMember("data_out", array, dom.GetAllocator());
	localParams.AddMember("slave_addr", 56, dom.GetAllocator());
	localParams.AddMember("num_bytes", 2, dom.GetAllocator());
	localParams.AddMember("flags", 0, dom.GetAllocator());
	id.SetInt(4);

	localRequest = json->generateRequest(method, localParams, id);

	//send through tunnel server
	subRequest = new string(localRequest);

	return data;

}


string* I2c::waitForResponse()
{
	int retCode = 0;
	retCode = sigtimedwait(&set, NULL, &timeout);
	if(retCode < 0)
		throw PluginError("Timeout waiting for subResponse.\n");

	subResponse = udsWorker->getNextMsg();
	printf("SubResponse: %s\n", subResponse->c_str());
	return subResponse;
}
