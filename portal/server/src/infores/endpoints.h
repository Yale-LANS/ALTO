#pragma once

#include "info_resource_base.h"
#include "cost_map.h"

class EndPointsProperty : public InfoResBase
{
private:
	void setPID(const std::string& ip, const std::string& pid);
public:
	void setProp(const std::string& ip, const std::string& prop, const std::string& type);
};

class InfoResourceEndPointsProperty : public InfoResBase
{
public:
	void setVtag(const std::string& vtag);
	void setMap(EndPointsProperty& epp);
};

typedef InfoResourceCostMap EndPointsCost;

