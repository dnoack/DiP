/*
 * I2c.hpp
 *
 *  Created on: 17.03.2015
 *      Author: dnoack
 */

#include "DriverInterface.h"
#include "JsonRPC.hpp"
#include "document.h"
#include "writer.h"

#ifndef I2C_H_
#define I2C_H_

using namespace rapidjson;

class I2c;

typedef bool (I2c::*afptr)(Value&, Value&);


class I2c : public DriverInterface<I2c*, afptr>
{
	public:
		I2c() :DriverInterface<I2c*, afptr>(this)
		{
			afptr temp;
			this->state = 0;
			this->response = NULL;
			json = new JsonRPC();
			temp = &I2c::write;
			funcMap.insert(pair<const char*, afptr>("i2c.write", temp));

			funcList = getAllFunctionNames();
		}

		~I2c()
		{
			delete json;
		};

		string* processMsg(string* msg);

		static list<string*>* getFuncList(){return funcList;}

	private:

		int state;
		JsonRPC* json;
		string* response;
		Value lastMethod;

		static list<string*>* funcList;

		bool write(Value &params, Value &result);

		Value* aa_open();
		Value* aa_write();
};

#endif /* I2C_H_ */
