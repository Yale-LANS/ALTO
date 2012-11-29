#include "error_resource_entity.h"

const char* ErrorResourceEntity::ALTOCodes[8] = {
				"E_SYNTAX", 
				"E_JSON_FIELD_MISSING",
				"E_JSON_VALUE_TYPE",
				"E_INVALID_COST_MODE",
				"E_INVALID_COST_TYPE",
				"E_INVALID_PROPERTY_TYPE",
				"E_SERVICE_UNAVAILABLE",
				"E_TEMPORARY_REDIRECT"
				};

ErrorResourceEntity::ErrorResourceEntity(ALTOErrorCode code)
{
	if (code >= E_SYNTAX && code <= E_TEMPORARY_REDIRECT)
		addString("code", ALTOCodes[code]);
}
