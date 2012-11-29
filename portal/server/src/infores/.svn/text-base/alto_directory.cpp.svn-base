#include "alto_directory.h"

void Capability::setCapability(std::string name, std::vector<std::string>& cap)
{
	InfoResArray array(cap);
	addArray(name.c_str(), array);
}

void Capability::setCostConstraint(bool cc)
{
	addBoolean("cost-constraints", Boolean(cc));
}

void Capability::setCostModes(std::vector<std::string>& cm)
{
	setCapability("cost-modes", cm);
}

void Capability::insertCostMode(std::string cm)
{
	if (Find("cost-modes") != End())
	{
		InfoResArray& array = (InfoResArray&)((Array&)(*this)["cost-modes"]);
		array.insert(cm);
	}
	else
	{
		std::vector<std::string> v_cm(1, cm);
		setCostModes(v_cm);
	}
}

void Capability::setCostTypes(std::vector<std::string>& ct)
{
	setCapability("cost-types", ct);
}

void Capability::insertCostType(std::string ct)
{
	if (Find("cost-types") != End())
	{
		InfoResArray& array = (InfoResArray&)((Array&)(*this)["cost-types"]);
		array.insert(ct);
	}
	else
	{
		std::vector<std::string> v_ct(1, ct);
		setCostTypes(v_ct);
	}
}

void Capability::setPropTypes(std::vector<std::string>& pt)
{
	setCapability("prop-types", pt);
}

void Capability::insertPropType(std::string pt)
{
	if (Find("prop-types") != End())
	{
		InfoResArray& array = (InfoResArray&)((Array&)(*this)["prop-types"]);
		array.insert(pt);
	}
	else
	{
		std::vector<std::string> v_pt(1, pt);
		setPropTypes(v_pt);
	}
}

///////////////////////////////////////////////////////////////

//const char* ResourceEntry::UriPredix = "http://p4p-1.cs.yale.edu:6671";
extern std::string ALTO_Server;

void ResourceEntry::setUri(std::string params)
{
	std::string uri(ALTO_Server.c_str());
	uri = uri + std::string(params);
	addString("uri", uri.c_str());
}

void ResourceEntry::setMediaTypes(std::vector<std::string>& mts)
{	
	InfoResArray array(mts);
	addArray("media-types", array);
}

void ResourceEntry::insertMediaType(std::string mts)
{
	if (Find("media-types") != End())
	{
		InfoResArray& array = (InfoResArray&)((Array&)(*this)["prop-types"]);
		array.insert(mts);
	}
	else {	
		std::vector<std::string> v_mts(1, mts);
		setMediaTypes(v_mts);
	}
}

void ResourceEntry::setAccepts(std::vector<std::string>& acts)
{
	InfoResArray array(acts);
	addArray("accepts", array);
}

void ResourceEntry::insertAccept(std::string acts)
{
	if (Find("accepts") != End())
	{
		InfoResArray& array = (InfoResArray&)((Array&)(*this)["prop-types"]);
		array.insert(acts);
	}
	else
	{
		std::vector<std::string> v_acts(1, acts);
		setAccepts(v_acts);
	}
}

void ResourceEntry::setCapability(Capability& cmc)
{
	addObject("capabilities", cmc);
}

///////////////////////////////////////////////////////////////

void InfoResourceDirectory::setResources(std::vector<ResourceEntry>& res)
{
	InfoResArray array;
	for (unsigned int i = 0; i < res.size(); i++)
	{
		array.insert(res[i]);
	}
	addArray("resources", array);

	rsp_cache_ = toJson();
}

void InfoResourceDirectory::RefreshJson()
{
	Clear();
	setResources(res_entrys_);
}

const std::string& InfoResourceDirectory::getRspString() const
{
	return rsp_cache_;
}

ResourceEntry* InfoResourceDirectory::ResEtryFactory()
{
	res_entrys_.push_back(ResourceEntry());
	return &res_entrys_[res_entrys_.size() -1];
}

