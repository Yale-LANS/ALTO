#include "p4p_config.h"

#include <string.h>
#include "tinyxml/tinyxml.h"
#include <p4p/p4p.h>

class Cserver;

void config_error(const std::string& err)
{
	std::cerr << "Failed to load ISP configuration: " << err << std::endl;
	exit(1);
}

void read_isps(TiXmlElement* root, ISPManagerPtr isps)
{
	
}

void read_p4p_config(const std::string& conf_file, Cserver *server)
{
	TiXmlDocument doc(conf_file.c_str());
	if (!doc.LoadFile())
		config_error("File " + conf_file + " not found");

	if (!doc.FirstChild())
		config_error("No root element");

	for (TiXmlNode* root_item = doc.FirstChild()->FirstChild(); root_item != NULL; root_item = root_item->NextSibling())
	{
		TiXmlElement* root_e = root_item->ToElement();
		if (!root_e)
			config_error("Expecting XML element below root element");

		const char* name = root_e->Value();

		if (strcmp(name, "guidance") == 0)
			/* A single guidance server must be configured */
			const char* addr;
			if (!(server->m_guidance_server_ip = root->Attribute("addr")))
				config_error("Guidance server missing 'addr' attribute");
		
			int port = -1;
			if (root->QueryIntAttribute("port", &port) != TIXML_SUCCESS)
				config_error("Guidance server missing 'port' attribute or invalid integer");
			else {
				server->m_guidance_server_port = (unsigned short) port;
			}
		
		else if (strcmp(name, "isp") == 0)
			/* Any number of ISPs may be configured */
			for (TiXmlNode* isp_item = root->FirstChild(); isp_item != NULL; isp_item = isp_item->NextSibling())
			{
				TiXmlElement* e = isp_item->ToElement();
				if (!e)
					config_error("ISP config is not an XML element");
				
				const char* isp_id;
				if (!(isp_id = e->Attribute("isp_id")))
					config_error("ISP missing 'isp_id' attribute");
				
				const char* loc_addr;
				if (!(loc_addr = e->Attribute("loc_addr")))
					config_error("ISP missing 'loc_addr' attribute");
				
				int loc_port = -1;
				if (e->QueryIntAttribute("loc_port", &loc_port) != TIXML_SUCCESS)
					config_error("ISP missing 'loc_port' attribute or invalid integer");
				
				const char* dis_addr;
				if (!(dis_addr = e->Attribute("dis_addr")))
					config_error("ISP missing 'dis_addr' attribute");
				
				int dis_port = -1;
				if (e->QueryIntAttribute("dis_port", &dis_port) != TIXML_SUCCESS)
					config_error("ISP missing 'dis_port' attribute or invalid integer");
				
				/* Add to our set of ISPs */
				server->add_isp(loc_addr, loc_port, dis_addr, dis_port);
			}
		
		else
			config_error("Unexpected element " + std::string(name));
	}

	if (isps->getCount() > 0 && guidance_server.get_address().empty())
		config_error("P4P ISPs listed but no guidance server provided");

}

