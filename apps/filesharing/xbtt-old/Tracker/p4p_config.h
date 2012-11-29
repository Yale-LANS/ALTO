#ifndef P4P_CONFIG_H
#define P4P_CONFIG_H

#include <string>

/* Read P4P configuration from configuration file: ISP Portals and Guidance server */
void read_p4p_config(const std::string& conf_file, Cserver *server);

#endif // P4P_CONFIG_H
