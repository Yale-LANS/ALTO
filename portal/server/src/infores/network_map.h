#pragma once

#include "info_resource_base.h"
#include "info_resource_entity.h"
#include "address_group.h"

class NetworkMapData : public InfoResBase
{
public:
	void setPID(std::string name, EndpointAddrGroup& eag);
	void setPID(std::string name, std::vector<std::string>& ip_v4, std::vector<std::string>& ip_v6);
};

typedef std::map<std::string, std::vector<std::string> > AddrGroup; 

class InfoResourceNetworkMap : public InfoResBase
{
private:
	AddrGroup _pids_v4;
	AddrGroup _pids_v6;
	std::string _ver_tag;

	bool hasPID(std::string pid);
	bool isIPv6(std::string ip);
public:
	void setVerTag(std::string ver_tag);
	void setMap(NetworkMapData& map);

	void addIP(std::string pid, std::string ip);

	void commit();

};
