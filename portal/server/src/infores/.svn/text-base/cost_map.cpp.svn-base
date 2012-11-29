#include "cost_map.h"

void CostMapCapability::addCostMode(std::vector<std::string>& cm)
{
	InfoResArray array(cm);
	addArray("cost-mode", array);
}

void CostMapCapability::addCostType(std::vector<std::string>& ct)
{
	InfoResArray array(ct);
	addArray("cost-type", array);
}

void DstCosts::addDst(std::string pid_name, double cost)
{
	addNumber(pid_name.c_str(), cost);
}


void CostMapData::addDstCost(std::string pid_name, DstCosts& dcost)
{
	addObject(pid_name.c_str(), dcost);
}

void InfoResourceCostMap::addCostMode(const std::string& cmode)
{
	addString("cost-mode", cmode);
}

void InfoResourceCostMap::addCostType(const std::string& ctype)
{
	addString("cost-type", ctype);
}

void InfoResourceCostMap::addVertionTag(const std::string& vtag)
{
	addString("map-vtag", vtag);
}

void InfoResourceCostMap::addCostMapData(CostMapData& cmd)
{
	addObject("map", cmd);
}

void InfoResourceCostMap::addCost(const std::string& src, const std::string& dst, double cost)
{
	if (_cost_matrix.find(src) == _cost_matrix.end())
		_cost_matrix[src] = std::map<std::string, double>();
	_cost_matrix[src][dst] = cost;
}

void InfoResourceCostMap::commit()
{
	CostMapData cmd;
	for (CostMatrix::iterator it = _cost_matrix.begin(); it != _cost_matrix.end(); it++)
	{
		const std::string& src = it->first;
		std::map<std::string, double> dst_map = it->second;
		DstCosts dc;

		for (std::map<std::string, double>::iterator it2 = dst_map.begin(); it2 != dst_map.end(); it2++)
		{
			const std::string& dst = it2->first;
			double& cost = it2->second;
			dc.addDst(dst, cost);
		}
		cmd.addDstCost(src, dc);
	}

	addCostMapData(cmd);
}
