#pragma once

#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

#include <sstream>
#include <string>

using namespace json;

class InfoResArray;

class InfoResBase : public Object
{
friend class InfoResArray;
public:
	void addString(const std::string& name, const std::string& str);
	void addNumber(const std::string& name, double num);
	void addBoolean(const std::string& name, bool bit);
	void addObject(const std::string& name, InfoResBase& obj);
	void addArray(const std::string& name, InfoResArray& ary);

	const std::string toJson();
	void clear() { Clear(); }
	void commit() { ; }
};

