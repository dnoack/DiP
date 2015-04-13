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
#include "I2cDevice.hpp"
#include "RemoteAardvark.hpp"

list<string*>* I2c::funcList;
list<I2cDevice*> I2c::deviceList;


void I2c::deleteFuncList()
{
	list<string*>::iterator funcItr = funcList->begin();
	while(funcItr != funcList->end())
	{
		delete *funcItr;
		funcItr = funcList->erase(funcItr);
	}
}


void I2c::deleteMsgList()
{
	list<string*>::iterator msgIt = msgList->begin();
	while(msgIt != msgList->end())
	{
		delete *msgIt;
		msgIt = msgList->erase(msgIt);
	}
}


void I2c::processMsg(string* msg)
{
	Value result;
	Value params;
	list<string*>::iterator currentMsg;

	try
	{
		msgList = json->splitMsg(msg);
		currentMsg = msgList->begin();

		while(currentMsg != msgList->end())
		{
			json->parse(*currentMsg);
			if(json->isRequest())
			{
				requestMethod = json->tryTogetMethod();
				requestId = json->getId();
				executeFunction(*requestMethod, params, result);
				udsWorker->uds_send(response);
				delete *currentMsg;
				currentMsg = msgList->erase(currentMsg);
			}
			else if(json->isNotification())
			{
				delete *currentMsg;
				currentMsg = msgList->erase(currentMsg);
			}
		}
	}
	catch(PluginError &e)
	{
		udsWorker->uds_send(e.get());
		delete *currentMsg;
		currentMsg = msgList->erase(currentMsg);
	}
	deleteMsgList();
}



bool I2c::getI2cDevices(Value &params, Value &result)
{

	getAardvarkDevices(params, result);




	//.. call further methods for getting other devices

	//generate jsonrpc rsponse like {..... "result" : {Aardvark : [id1, id2, idx] , OtherDevice : [id1, id2, idx]}}

	return true;
}



bool I2c::getAardvarkDevices(Value &params, Value &result)
{
	//generate json rpc for aa_find_devices ext
	Value method;
	Value currentParam;

	int num_devices = 0;

	Value* i2cDeviceValue = NULL;
	Value* i2cUniqueIdValue = NULL;
	Document* dom = json->getRequestDOM();


	method.SetString(_aa_find_devices_ext._name, dom->GetAllocator());
	params.SetObject();

	currentParam.SetString(_aa_find_devices_ext.paramArray[0]._name, dom->GetAllocator());
	params.AddMember( currentParam, 256, dom->GetAllocator());

	//subrequest
	subRequest = json->generateRequest(method, params, *requestId);
	udsWorker->uds_send(subRequest);
	subResponse = waitForResponse();
	//###########


	json->parse(subResponse);
	requestId = json->getId();
	subResult = json->tryTogetResult();
	if(subResult->IsObject())
	{
		i2cDeviceValue = &(*subResult)["devices"];
		i2cUniqueIdValue = &(*subResult)["unique_ids"];
		num_devices = i2cDeviceValue->Size();
		currentParam.SetArray();

		for(int i = 0; i < num_devices; i++)
		{
			deviceList.push_back(new I2cDevice("Aardvark", (*i2cDeviceValue)[i].GetInt(), (*i2cUniqueIdValue)[i].GetUint()));
			currentParam.PushBack((*i2cUniqueIdValue)[i].GetUint(), dom->GetAllocator());
		}
	}

	result.SetObject();
	result.AddMember("Aardvark", currentParam, dom->GetAllocator());


	response = json->generateResponse(*requestId, result);

	return true;
}



bool I2c::write(Value &params, Value &result)
{

	try
	{
		aa_open(params);
		udsWorker->uds_send(subRequest);
		subResponse = waitForResponse();
		//TODO: check responsecode

		//aa_target_power(params);
		subRequest = "{\"jsonrpc\": \"2.0\", \"params\": {\"Aardvark\": 1 , \"powerMask\" :  3 }, \"method\": \"Aardvark.aa_target_power\", \"id\": 3}";
		udsWorker->uds_send(subRequest);
		subResponse = waitForResponse();
		//check responsecode


		subRequest ="{\"jsonrpc\": \"2.0\", \"params\": { \"handle\": 1 , \"slave_addr\": 56, \"flags\": 0, \"num_bytes\": 2, \"data_out\": [3,0]}, \"method\": \"Aardvark.aa_i2c_write\", \"id\": 4}";
		udsWorker->uds_send(subRequest);
		subResponse = waitForResponse();
		//check responsecode


		aa_write();
		udsWorker->uds_send(subRequest);
		subResponse = waitForResponse();
		//check responsecode


		subRequest = "{\"jsonrpc\": \"2.0\", \"params\": { \"handle\": 1 } , \"method\": \"Aardvark.aa_close\", \"id\": 6}";
		udsWorker->uds_send(subRequest);
		subResponse = waitForResponse();
		//check responsecode


		//return bytes written to client
		response = "{\"jsonrpc\": \"2.0\", \"result\": \"OK\", \"id\": 7}";
	}
	catch(PluginError &e)
	{
		response = "{\"jsonrpc\": \"2.0\", \"result\": \"NOT OK\", \"id\": 7}";

	}

	return true;
}


Value* I2c::aa_open(Value &params)
{
	Value method;
	Value localParams;
	Value id;

	Value* deviceValue = NULL;
	int device = -1;
	Value tempParam;


	//Get exact method name
	method.SetString(_aa_open._name, dom.GetAllocator());
	//Get all needed params
	localParams.SetObject();
	tempParam.SetString(_aa_open.paramArray[0]._name , dom.GetAllocator());
	//map "device" -> to "Aardvark"(_aa_open.paramArray[0]._name)
	deviceValue = json->tryTogetParam("device");
	device = getPortByUniqueId(deviceValue->GetUint());

	localParams.AddMember(tempParam, device, dom.GetAllocator());
	//id of this request is the one of the incomming request +1
	id.SetInt(requestId->GetInt()+1);

	subRequest = json->generateRequest(method, localParams, id);

	return deviceValue;
}


Value* I2c::aa_target_power(Value &params)
{
	Value method;
	Value localParams;
	Value tempParam;
	Value id;

	localParams.SetObject();
	tempParam.SetString(_aa_target_power.paramArray[0]._name, dom.GetAllocator());
	//get aardvark handle !

	localParams.AddMember(tempParam, tempParam, dom.GetAllocator());

	tempParam.SetString(_aa_target_power.paramArray[1]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, tempParam, dom.GetAllocator());

	method.SetString(_aa_target_power._name, dom.GetAllocator());

	id.SetInt(requestId->GetInt()+1);
	json->generateRequest(method, localParams, id);

}


Value* I2c::aa_write()
{
	Value method;
	Value localParams;
	Value id;

	Value* data = NULL;
	Value array;


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
	id.SetInt(5);

	subRequest = json->generateRequest(method, localParams, id);

	return data;

}

int I2c::getPortByUniqueId(unsigned int uniqueId)
{
	list<I2cDevice*>::iterator device = deviceList.begin();
	int result = -1;

	while( device != deviceList.end())
	{
		result = (*device)->getIdentification();
		if(uniqueId == result)
		{
			return (*device)->getPort();
		}
	}

	return -1;
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
