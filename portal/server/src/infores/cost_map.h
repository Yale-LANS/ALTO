#pragma once

#include <map>
#include <vector>
#include <string>

#include "info_resource_base.h"
#include "info_resource_array.h"

class CostMapCapability : public InfoResBase
{
public:
	void addCostMode(std::vector<std::string>& cm);
	void addCostType(std::vector<std::string>& ct);
};

class DstCosts : public InfoResBase
{
public:
	void addDst(std::string pid_name, double cost);
};

class CostMapData : public InfoResBase
{
public:
	void addDstCost(std::string pid_name, DstCosts& dcost);
};

typedef std::map<std::string, std::map<std::string, double> > CostMatrix;
class InfoResourceCostMap : public InfoResBase
{
private:
	CostMatrix _cost_matrix;
public:
	void addCostMode(const std::string& cmode);
	void addCostType(const std::string& ctype);
	void addVertionTag(const std::string& vtag);
	void addCostMapData(CostMapData& cmd);

	void addCost(const std::string& src, const std::string& dst, double cost);

	void commit();
};
