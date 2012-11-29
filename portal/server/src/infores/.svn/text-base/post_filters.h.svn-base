#pragma once

#include "json/reader.h"
#include "json/writer.h"
#include "json/elements.h"

#include <sstream>
#include <string>
#include <set>

#include "error_resource_entity.h"

using namespace json;

class PostFilter
{
private:
	bool _has_constraints;
	double _upper_bound;
	bool _upper_tight;
	double _lower_bound;
	bool _lower_tight;
public:
	PostFilter();
	static std::string StripIP(const std::string& ip_str);
	virtual ALTOErrorCode readJson(std::string& json_str) = 0;

	bool hasConstraints() { return _has_constraints; }
	double getUpperBound() { return _upper_bound; }
	bool getUpperTight() { return _upper_tight; }
	double getLowerBound() { return _lower_bound; }
	bool getLowerTight() { return _lower_tight; }

	bool inConstraints(double value);
	ALTOErrorCode readConstraints(Object& obj);
};

class JsonPIDSet : public PostFilter
{
private:
	std::set<std::string> _pid_set;

public:
	std::set<std::string>& getPIDSet() { return _pid_set; }
	virtual ALTOErrorCode readJson(std::string& json_str);
};

class JsonPIDMatrix : public PostFilter
{
private:
	std::string _cost_mode;
	std::string _cost_type;

	std::set<std::string> _srcs;
	std::set<std::string> _dsts;

public:
	virtual ALTOErrorCode readJson(std::string& json_str);
	std::set<std::string>& getPIDSrcs() { return _srcs; }
	std::set<std::string>& getPIDDsts() { return _dsts; }
	std::string& getCostMode() { return _cost_mode; }
	std::string& getCostType() { return _cost_type; }
};

class JsonEndPointsProperty : public PostFilter
{
private:
	std::set<std::string> _properties;
	std::set<std::string> _end_points;

public: 
	virtual ALTOErrorCode readJson(std::string& json_str);
	std::set<std::string>& getProperties() { return _properties; }
	std::set<std::string>& getEndPoints() { return _end_points; }
};

class JsonEndPointsCost : public PostFilter
{
private:
	std::string _cost_mode;
	std::string _cost_type;
	
	std::set<std::string> _srcs;
	std::set<std::string> _dsts;
public:
	virtual ALTOErrorCode readJson(std::string& json_str);
	std::set<std::string>& getEndPointsSrcs() { return _srcs; }
	std::set<std::string>& getEndPointsDsts() { return _dsts; }
	std::string& getCostMode() { return _cost_mode; }
	std::string& getCostType() { return _cost_type; }
};

