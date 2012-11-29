#include "endpoints.h"

void EndPointsProperty::setPID(const std::string& ip, const std::string& pid)
{
	InfoResBase obj;
	obj.addString("pid", pid);
	addObject(ip.c_str(), obj);
}

void EndPointsProperty::setProp(const std::string& ip, const std::string& prop, const std::string& type)
{
	if (type == "pid")
		setPID(ip, prop);
}

void InfoResourceEndPointsProperty::setVtag(const std::string& vtag)
{
	addString("map-vtag", vtag);
}

void InfoResourceEndPointsProperty::setMap(EndPointsProperty& epp)
{
	addObject("map", epp);
}
