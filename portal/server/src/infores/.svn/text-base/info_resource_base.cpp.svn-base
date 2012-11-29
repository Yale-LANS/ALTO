#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

#include <sstream>
#include <string>

#include "info_resource_base.h"
#include "info_resource_array.h"

const std::string InfoResBase::toJson()
{
	std::stringstream stream;
	Writer::Write((Object)(*this), stream);
	return stream.str();
}

void InfoResBase::addString(const std::string& name, const std::string& str)
{
	(*this)[name.c_str()] = String(str);
}

void InfoResBase::addNumber(const std::string& name, double num)
{
	// precision : 0.1
	int n = (int)(num * 10.0 + 0.5);
	(*this)[name.c_str()] = Number(n / 10.0);
}

void InfoResBase::addBoolean(const std::string& name, bool bit)
{
	(*this)[name.c_str()] = Boolean(bit);
}

void InfoResBase::addObject(const std::string& name, InfoResBase& obj)
{
	(*this)[name.c_str()] = (Object&)obj;
}

void InfoResBase::addArray(const std::string& name, InfoResArray& ary)
{
	(*this)[name.c_str()] = (Array&)ary;
}

