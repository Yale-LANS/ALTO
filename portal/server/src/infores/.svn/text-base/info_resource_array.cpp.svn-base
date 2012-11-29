#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

#include <sstream>
#include <string>

#include "info_resource_array.h"
#include "info_resource_base.h"

InfoResArray::InfoResArray()
{
	;
}

InfoResArray::InfoResArray(std::vector<std::string>& str_list)
{
	for (unsigned int i = 0; i < str_list.size(); i++)
	{
		insert(str_list[i].c_str());
	}
}

void InfoResArray::insert(InfoResBase& irb)
{
	Insert((Object&)irb);
}

void InfoResArray::insert(std::string str)
{
	Insert(UnknownElement(String(str)));
}

