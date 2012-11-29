#ifndef TRACKER_H
#define TRACKER_H

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <netinet/ip.h>

#ifndef NO_P4P
	#include <p4p/p4p.h>
	#include <p4p/app/p4papp.h>

	using namespace p4p;
	using namespace p4p::app;
#endif

/* Forward declaration */
class Peer;
class Channel;

struct TrackerInfo
{
	int channels;
	int peers;

	TrackerInfo& operator+=(const TrackerInfo& rhs);
	TrackerInfo& operator-=(const TrackerInfo& rhs);
	TrackerInfo operator+(const TrackerInfo& rhs) const;
	TrackerInfo operator-(const TrackerInfo& rhs) const;
};

std::ostream& operator<<(std::ostream& os, const TrackerInfo& rhs);

/* Tracker data structure, maintain all channels and direct
 * peer events to the appropriate channel. */
class Tracker
{
public:

#ifndef NO_P4P
	Tracker(const ISPManager& isp_mgr, P4PUpdateManager* update_mgr, std::vector<std::string>& pidmap_filenames, const double intra_pid_pct, const double intra_isp_pct);
#else
	Tracker();
#endif
	virtual ~Tracker();

	bool createChannel(const std::string& contentID);

#ifndef NO_P4P
	bool createChannel(const std::string& contentID,
			   const PeeringGuidanceMatrixOptions& options,
			   const std::string& optEngineAddr = "",
			   unsigned short optEnginePort = 0);
#endif

	bool removeChannel(const std::string& contentID);

	TrackerInfo getInfo() const;

	bool peerJoin(const std::string& peerID,
		      const std::string& contentID,
		      const in_addr& addr,
		      unsigned short port);

	/* Peer selection */
	bool peerQuery(const std::string& peerID,
		       const std::string& contentID,
		       unsigned int num_peers,
		       std::vector<Peer*>& peers);

	bool peerReport(const std::string& peerID,
			const std::string& contentID,
			bool downloading,
			float upcapacity, float downcapacity);

	bool peerLeave(const std::string& peerID,
		       const std::string& contentID);

	Peer* findPeer(const std::string& peerID,
		       const std::string& contentID);

private:
	/* Mapping from channel ID to channel data structure */
	typedef std::map<std::string, Channel*> ChannelCollection;

	Channel* findChannel(const std::string& channelID) const;

	ChannelCollection	m_channels;		/**< List of channels */
	unsigned int		m_num_peers;		/**< Total number of peers */

#ifndef NO_P4P
	std::vector<std::string>	m_pidmap_filenames;
	double 			m_intra_pid_pct;
	double 			m_intra_isp_pct;

	const ISPManager&	m_isp_mgr;		/**< Manager for P4P information from ISPs */	
	P4PUpdateManager*	m_update_mgr;		/**< Manager for updating/refreshing P4P information */
#endif
};

#endif
