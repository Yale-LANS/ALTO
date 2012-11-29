#pragma once

#include <vector>
#include <string>
#include <map>

#include "info_resource_base.h"
#include "info_resource_array.h"

#define COST_MODE_NUM 		"numerical"
#define COST_MODE_ORD 		"ordinal"
#define COST_TYPE_RTC 		"routingcost"
#define COST_TYPE_HPC 		"hopcount"

#define MEDIA_TYPE_NETMAP	"application/alto-networkmap+json"
#define MEDIA_TYPE_NETMAP_FT	"application/alto-networkmapfilter+json"

#define MEDIA_TYPE_COSTMAP	"application/alto-costmap+json" 
#define MEDIA_TYPE_COSTMAP_FT	"application/alto-costmapfilter+json"

#define MEDIA_TYPE_EPPROP	"application/alto-endpointprop+json"
#define MEDIA_TYPE_EPCOST	"application/alto-endpointcost+json"
#define MEDIA_TYPE_EPCOST_PM	"application/alto-endpointcostparams+json"
#define MEDIA_TYPE_EPPROP_PM	"application/alto-endpointpropparams+json"

#define MEDIA_TYPE_SEVINFO	"application/alto-serverinfo+json"
#define MEDIA_TYPE_IRD		"application/alto-directory+json"

#define MEDIA_TYPE_ERROR_JSON	"application/alto-error+json"

class Capability : public InfoResBase
{
private:
	void setCapability(std::string name, std::vector<std::string>& cap);
public:
	void setCostConstraint(bool cc);

	void setCostModes(std::vector<std::string>& cm);
	void insertCostMode(std::string cm);

	void setCostTypes(std::vector<std::string>& ct);
	void insertCostType(std::string ct);

	void setPropTypes(std::vector<std::string>& pt);
	void insertPropType(std::string pt);
};

class ResourceEntry : public InfoResBase
{
private:
	static const char* UriPredix;
public:
	void setUri(std::string params);
	void setMediaTypes(std::vector<std::string>& mts);
	void insertMediaType(std::string mts);
	void setAccepts(std::vector<std::string>& act);
	void insertAccept(std::string act);
	void setCapability(Capability& cmc);
};

class InfoResourceDirectory : public InfoResBase
{
private:
	std::vector<ResourceEntry>	res_entrys_;
	std::string			rsp_cache_;
	void setResources(std::vector<ResourceEntry>& res);
public:
	ResourceEntry* ResEtryFactory();
	void RefreshJson();
	const std::string& getRspString() const;
};
