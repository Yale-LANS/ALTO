#pragma once

#include <string>
#include "info_resource_base.h"

typedef enum e_ALTOErrorCode
{
	E_SYNTAX = 0, 
	E_JSON_FIELD_MISSING,
	E_JSON_VALUE_TYPE,
	E_INVALID_COST_MODE,
	E_INVALID_COST_TYPE,
	E_INVALID_PROPERTY_TYPE,
	E_SERVICE_UNAVAILABLE,
	E_TEMPORARY_REDIRECT,
	E_OK
} ALTOErrorCode;

class ErrorResourceEntity : public InfoResBase
{
private:
	static const char*	ALTOCodes[8];
public:
	ErrorResourceEntity(ALTOErrorCode code);
};
