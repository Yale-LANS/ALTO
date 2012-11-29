#pragma once

#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

#include <sstream>
#include <string>
#include <vector>

using namespace json;

class InfoResBase;

class InfoResArray : public Array
{
friend class InfoResBase;
public:
	InfoResArray();
	InfoResArray(std::vector<std::string>& str_list);
	void insert(InfoResBase& irb);
	void insert(std::string str);
	void clear() { Clear(); }
};

