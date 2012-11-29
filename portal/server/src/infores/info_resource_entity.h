#pragma once

#include "info_resource_base.h"
#include "info_resource_metadata.h"

#include <string>

class InfoResourceEntity : public InfoResBase
{
private:
	static std::string JsonStr;
public:
	static std::string& MakeJsonStr(InfoResBase& data)
	{
		InfoResourceEntity ire;
		InfoResourceMetaData meta;
		ire.setMeta(meta);
		ire.setData(data);
		JsonStr = ire.toJson();
		return JsonStr;
	}

	void setMeta(InfoResourceMetaData& meta);
	void setData(InfoResBase& data);
};
