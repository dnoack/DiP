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

		}

		~I2c()
		{
			delete json;
		};

		string* processMsg(string* msg);

	private:
		bool write(Value &params, Value &result);
		int state;
		JsonRPC* json;
		string* response;
		Value lastMethod;
};

#endif /* I2C_H_ */
