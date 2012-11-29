#include "info_resource_entity.h"

std::string InfoResourceEntity::JsonStr = std::string();

void InfoResourceEntity::setMeta(InfoResourceMetaData& meta)
{
	addObject("meta", meta);
}

void InfoResourceEntity::setData(InfoResBase& data)
{
	addObject("data", data);
}
