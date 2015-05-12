
#include "unistd.h"
#include "signal.h"


#include "UdsComWorker.hpp"
#include <I2c.hpp>
#include "I2cDevice.hpp"
#include "RemoteAardvark.hpp"



list<I2cDevice*> I2c::deviceList;




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
	Value* params;
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
				setRequestInProcess();
				requestMethod = json->tryTogetMethod();
				params = json->tryTogetParams();
				requestId = json->getId();
				subRequestId = requestId->GetInt();
				executeFunction(*requestMethod, *params, result);
				udsWorker->transmit(response, strlen(response));
				delete *currentMsg;
				currentMsg = msgList->erase(currentMsg);
				setRequestNotInProcess();
			}
			else if(json->isNotification())
			{
				delete *currentMsg;
				currentMsg = msgList->erase(currentMsg);
			}
		}
	}
	catch(Error &e)
	{
		udsWorker->transmit(e.get(), strlen(e.get()));
		delete *currentMsg;
		currentMsg = msgList->erase(currentMsg);
		setRequestNotInProcess();
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
	udsWorker->transmit(subRequest, strlen(subRequest));
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

		//Add param powerMask for powerUp
		params.AddMember("powerMask", AA_TARGET_POWER_BOTH, dom.GetAllocator());
		aa_target_power(params);

		aa_write(params);

		aa_close(params);

		result.SetObject();
		result.AddMember("returnCode", "OK", dom.GetAllocator());

		response = json->generateResponse(*(json->getId()), result);

	}
	catch(Error &e)
	{
		printf("%s\n", e.get());
		throw;
	}
	return true;
}


void I2c::aa_open(Value &params)
{
	Value method;
	Value localParams;
	Value id;
	Value* subResultValue= NULL;

	Value* deviceValue = NULL;
	int device = 0;
	Value tempParam;
	Document* localDom = new Document();



	//Get exact method name
	method.SetString(_aa_open._name, dom.GetAllocator());
	//Get all needed params
	localParams.SetObject();
	tempParam.SetString(_aa_open.paramArray[0]._name , dom.GetAllocator());
	//map "device" -> to "Aardvark"(_aa_open.paramArray[0]._name)
	deviceValue = findObjectMember(params, "device");
	device = getPortByUniqueId(deviceValue->GetUint());

	localParams.AddMember(tempParam, device, dom.GetAllocator());
	//id of this request is the one of the incomming request +1
	id.SetInt(++subRequestId);

	subRequest = json->generateRequest(method, localParams, id);

	//send subRequest, wait for subresponse and parse subResponse to localDom (not overwriting dom of I2c)
	udsWorker->transmit(subRequest, strlen(subRequest));
	subResponse = waitForResponse();
	json->parse(subResponse, localDom);

	if(checkSubResult(localDom))
	{
		subResult = json->tryTogetResult(localDom);
		subResultValue = findObjectMember(*subResult, "Aardvark", kNumberType);

		if(subResultValue->GetInt() < 0)
		{
			delete localDom;
			throw Error(subResponse);
		}
		else
		{
			params.AddMember("Aardvark", subResultValue->GetInt(), dom.GetAllocator());
			delete localDom;
		}
	}
	else
	{
		delete localDom;
		throw Error(subResponse);
	}

}


void I2c::aa_target_power(Value &params)
{
	Value method;
	Value localParams;
	Value tempParam;
	Value id;
	Value* subResultValue= NULL;
	Document* localDom = new Document();

	localParams.SetObject();
	Value* valuePtr = findObjectMember(params, _aa_target_power.paramArray[0]._name);

	//Aardvark handle
	tempParam.SetString(_aa_target_power.paramArray[0]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, valuePtr->GetInt(), dom.GetAllocator());

	//powerMask
	valuePtr = findObjectMember(params, _aa_target_power.paramArray[1]._name);
	tempParam.SetString(_aa_target_power.paramArray[1]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, valuePtr->GetInt(), dom.GetAllocator());

	//methodname
	method.SetString(_aa_target_power._name, dom.GetAllocator());

	id.SetInt(++subRequestId);
	subRequest = json->generateRequest(method, localParams, id);

	udsWorker->transmit(subRequest, strlen(subRequest));
	subResponse = waitForResponse();

	json->parse(subResponse, localDom);
	subResult = json->tryTogetResult(localDom);

	subResultValue = findObjectMember(*subResult, "returnCode", kNumberType);

	if(subResultValue->GetInt() < 0)
	{
		delete localDom;
		throw Error("Could not open Aardvark.");
	}
	else
	{
		delete localDom;
	}

}


void I2c::aa_write(Value &params)
{
	Value method;
	Value localParams;
	Value tempParam;
	Value id;
	Value array;
	Value* valuePtr = NULL;
	Value* subResultValue= NULL;
	Document* localDom = new Document();

	localParams.SetObject();
	//get Aardvark handle
	valuePtr = findObjectMember(params, _aa_i2c_write.paramArray[0]._name);
	tempParam.SetString(_aa_i2c_write.paramArray[0]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, valuePtr->GetInt(), dom.GetAllocator());

	//get slave addr
	valuePtr = findObjectMember(params, _aa_i2c_write.paramArray[1]._name);
	tempParam.SetString(_aa_i2c_write.paramArray[1]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, valuePtr->GetInt(), dom.GetAllocator());

	//get flags ?
	tempParam.SetString(_aa_i2c_write.paramArray[2]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, 0, dom.GetAllocator());

	//get data
	valuePtr = findObjectMember(params, _aa_i2c_write.paramArray[3]._name);
	tempParam.SetString(_aa_i2c_write.paramArray[3]._name, dom.GetAllocator());
	localParams.AddMember(tempParam, *valuePtr, dom.GetAllocator());

	method.SetString(_aa_i2c_write._name, dom.GetAllocator());
	id.SetInt(++subRequestId);
	subRequest = json->generateRequest(method, localParams, id);

	udsWorker->transmit(subRequest, strlen(subRequest));
	subResponse = waitForResponse();


	json->parse(subResponse, localDom);
	subResult = json->tryTogetResult(localDom);
	subResultValue = findObjectMember(*subResult, "returnCode", kNumberType);

	if(subResultValue->GetInt() < 0)
	{
		delete localDom;
		throw Error("Could not open Aardvark.");
	}
	else
	{
		delete localDom;
	}
}



void I2c::aa_close(Value &params)
{
	Value method;
	Value localParams;
	Value id;
	Value* subResultValue= NULL;

	Value* deviceValue = NULL;
	Value tempParam;
	Document* localDom = new Document();


	//Get exact method name
	method.SetString(_aa_close._name, dom.GetAllocator());
	//Get all needed params
	localParams.SetObject();
	tempParam.SetString(_aa_close.paramArray[0]._name , dom.GetAllocator());

	deviceValue = findObjectMember(params, _aa_close.paramArray[0]._name);
	localParams.AddMember(tempParam, *deviceValue, dom.GetAllocator());
	//id of this request is the one of the incomming request +1

	id.SetInt(++subRequestId);

	subRequest = json->generateRequest(method, localParams, id);

	//send subRequest, wait for subresponse and parse subResponse to localDom (not overwriting dom of I2c)
	udsWorker->transmit(subRequest, strlen(subRequest));
	subResponse = waitForResponse();
	json->parse(subResponse, localDom);

	subResult = json->tryTogetResult(localDom);

	subResultValue = findObjectMember(*subResult, "returnCode", kNumberType);


	if(subResultValue->GetInt() < 0)
	{
		delete localDom;
		throw Error("Could not close Aardvark.");
	}
	else
	{
		params.AddMember("returnCode", subResultValue->GetInt(), dom.GetAllocator());
		delete localDom;
	}


}


int I2c::getPortByUniqueId(unsigned int uniqueId)
{
	list<I2cDevice*>::iterator device = deviceList.begin();
	unsigned int result = -1;

	while( device != deviceList.end())
	{
		result = (*device)->getIdentification();
		if(uniqueId == result)
		{
			return (*device)->getPort();
		}
		++device;
	}

	return result;
}


bool I2c::checkSubResult(Document* dom)
{
	bool result = false;

	if(json->isError(dom))
		result = false;
	else
		result = true;

	return result;

}


string* I2c::waitForResponse()
{
	int retCode = 0;
	retCode = sigtimedwait(&set, NULL, &timeout);
	if(retCode < 0)
		throw Error("Timeout waiting for subResponse.\n");

	subResponse = udsWorker->getNextMsg();
	printf("SubResponse: %s\n", subResponse->c_str());
	return subResponse;
}


bool I2c::isRequestInProcess()
{
	bool result = false;
	pthread_mutex_lock(&rIPMutex);
	result = requestInProcess;
	pthread_mutex_unlock(&rIPMutex);
	return result;
}


void I2c::setRequestInProcess()
{
	pthread_mutex_lock(&rIPMutex);
	requestInProcess = true;
	pthread_mutex_unlock(&rIPMutex);
}


void I2c::setRequestNotInProcess()
{
	pthread_mutex_lock(&rIPMutex);
	requestInProcess = false;
	pthread_mutex_unlock(&rIPMutex);
}
