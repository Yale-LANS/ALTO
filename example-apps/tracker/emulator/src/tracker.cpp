#include "tracker.h"

#include "peer.h"
#include "channel.h"

TrackerInfo& TrackerInfo::operator+=(const TrackerInfo& rhs)
{
	channels	+= rhs.channels;
	peers		+= rhs.peers;
	return *this;
}

TrackerInfo& TrackerInfo::operator-=(const TrackerInfo& rhs)
{
	channels	-= rhs.channels;
	peers		-= rhs.peers;
	return *this;
}

TrackerInfo TrackerInfo::operator+(const TrackerInfo& rhs) const
{
	TrackerInfo result = *this;
	result += rhs;
	return result;
}

TrackerInfo TrackerInfo::operator-(const TrackerInfo& rhs) const
{
	TrackerInfo result = *this;
	result -= rhs;
	return result;
}

std::ostream& operator<<(std::ostream& os, const TrackerInfo& rhs)
{
	return os
		<< "\tNumChannels: " << rhs.channels
		<< "\tNumPeers: " << rhs.peers;
}

#ifndef NO_P4P
Tracker::Tracker(const ISPManager& isp_mgr, P4PUpdateManager* update_mgr, std::vector<std::string>& pidmap_filenames, const double intra_pid_pct, const double intra_isp_pct)
	: m_num_peers(0),
	  m_pidmap_filenames(pidmap_filenames),
	  m_intra_pid_pct(intra_pid_pct),
	  m_intra_isp_pct(intra_isp_pct),
	  m_isp_mgr(isp_mgr),
	  m_update_mgr(update_mgr)
{
}
#else
Tracker::Tracker()
	: m_num_peers(0)
{
}
#endif

Tracker::~Tracker()
{
	/* Clean resources for all channels */
	for (ChannelCollection::const_iterator itr = m_channels.begin(); itr != m_channels.end(); ++itr)
		delete itr->second;
}

TrackerInfo Tracker::getInfo() const
{
	TrackerInfo result;
	result.channels = m_channels.size();
	result.peers = m_num_peers;
	return result;
}

Channel* Tracker::findChannel(const std::string& channelID) const
{
	ChannelCollection::const_iterator itr = m_channels.find(channelID);
	if (itr == m_channels.end())
		return NULL;

	return itr->second;
}

#ifndef NO_P4P
bool Tracker::createChannel(const std::string& contentID, const PeeringGuidanceMatrixOptions& options, const std::string& optEngineAddr, unsigned short optEnginePort)
{
	/* Ensure channel doesn't already exist */
	if (findChannel(contentID) != NULL)
		return false;

	/* Create new channel */
	m_channels[contentID] = new Channel(contentID, m_isp_mgr, m_update_mgr, options, optEngineAddr, optEnginePort);
	return true;
}
#endif

bool Tracker::createChannel(const std::string& contentID)
{
	/* Ensure channel doesn't already exist */
	if (findChannel(contentID) != NULL)
		return false;

	/* Create new channel */
	m_channels[contentID] = new Channel(contentID);
	return true;
}

bool Tracker::removeChannel(const std::string& contentID)
{
	/* Look for existing channel */
	ChannelCollection::iterator itr = m_channels.find(contentID);
	if (itr == m_channels.end())
		return false;

	/* Clean resources */
	delete itr->second;
	m_channels.erase(itr);
	return true;
}

bool Tracker::peerJoin(const std::string& peerID,
		       const std::string& contentID,
		       const in_addr& addr,
		       unsigned short port)
{
	/* Find the channel */
	Channel* channel = findChannel(contentID);
	if (!channel)
		return false;

	/* Create peer */
#ifdef NO_P4P
	Peer* newPeer = new Peer(peerID, addr, port);
#else
	Peer* newPeer = new Peer(peerID, addr, port, m_pidmap_filenames, m_intra_pid_pct, m_intra_isp_pct);
#endif
	if (!channel->addPeer(newPeer))
	{
		delete newPeer;
		return false;
	}

	++m_num_peers;
	return true;
}

bool Tracker::peerQuery(const std::string& peerID,
			const std::string& contentID,
			unsigned int num_peers,
			std::vector<Peer*>& peers)
{
	/* Find the channel */
	Channel* channel = findChannel(contentID);
	if (!channel)
		return false;

	return channel->selectPeers(peerID, num_peers, peers);
}

bool Tracker::peerReport(const std::string& peerID,
			 const std::string& contentID,
			 bool downloading,
			 float upcap, float downcap)
{
	/* Find the channel */
	Channel* channel = findChannel(contentID);
	if (!channel)
		return false;

	return channel->updatePeerStats(peerID, downloading, upcap, downcap);
}

bool Tracker::peerLeave(const std::string& peerID, const std::string& contentID)
{
	/* Find the channel */
	Channel* channel = findChannel(contentID);
	if (!channel)
		return false;

	if (!channel->deletePeer(peerID))
		return false;

	--m_num_peers;
	return true;
}

Peer* Tracker::findPeer(const std::string& peerID, const std::string& contentID)
{
	/* Find the channel */
	Channel* channel = findChannel(contentID);
	if (!channel)
		return NULL;
	
	return channel->findPeer(peerID);
}


