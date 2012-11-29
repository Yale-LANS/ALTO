#pragma once

#include "config.h"
#include "connection.h"
#include "epoll.h"
#include "stats.h"
#include "tcp_listen_socket.h"
#include "tracker_input.h"
#include "udp_listen_socket.h"
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <map>
#include <sql/database.h>
#include <virtual_binary.h>

/* Include P4P API library */
#include <p4p/p4p.h>
#include <p4p/isp_peerguidance.h>

typedef std::string ISP_ID;
typedef std::string PID_ID;
typedef std::vector<const p4p::P4PPeerBase*> P4PPeerList;

class Cserver
{
public:
	class peer_key_c
	{
	public:
		peer_key_c()
		{
		}

		peer_key_c(int host, int uid)
		{
			host_ = host;
#ifdef PEERS_KEY
			uid_ = uid;
#endif
		}

		bool operator<(peer_key_c v) const
		{
#ifdef PEERS_KEY
			return host_ < v.host_ || host_ == v.host_ && uid_ < v.uid_;
#else
			return host_ < v.host_;
#endif
		}

		int host_;
#ifdef PEERS_KEY
		int uid_;
#endif
	};

	struct t_peer
	{
		t_peer(int addr, unsigned short pt)
			: p4p_peer()
		{
			mtime = 0;
			ipa = (unsigned int) addr;
			port = pt;
		}
		
		t_peer(const t_peer& rhs) { 
			downloaded = rhs.downloaded;
			uploaded = rhs.uploaded;
			mtime = rhs.mtime;
			uid = rhs.uid;
			port = rhs.port;
			left = rhs.left;
			peer_id = rhs.peer_id;
			p4p_peer = rhs.p4p_peer;
		}
		
		t_peer& operator=(const t_peer& rhs) { 
			downloaded = rhs.downloaded;
			uploaded = rhs.uploaded;
			mtime = rhs.mtime;
			uid = rhs.uid;
			port = rhs.port;
			left = rhs.left;
			peer_id = rhs.peer_id;
			p4p_peer = rhs.p4p_peer;
			return *this;
		}
		
		unsigned int ipa;
		long long downloaded;
		long long uploaded;
		time_t mtime;
		int uid;
		short port;
		bool left;
		boost::array<char, 20> peer_id;
		p4p::P4PPeerBase p4p_peer;
	};

	typedef std::map<peer_key_c, t_peer> t_peers;

	struct t_deny_from_host
	{
		unsigned int begin;
		bool marked;
	};
	
	struct t_file
	{
		void clean_up(time_t t, Cserver&);
		void debug(std::ostream&) const;
		std::string select_peers(const Ctracker_input&) const;
		void p4p_select_peers(P4PPeerList *out, void* tracker_input = NULL);

		t_file(const Cserver& server)
			: server_ref(&server)
		{
			completed = 0;
			dirty = true;
			fid = 0;
			leechers = 0;
			seeders = 0;
			isp_view_manager = new p4p::ISPViewManager();
			for (unsigned int i = 0; i < server_ref->m_isp_list.size(); i++) {
				isp_view_manager->createISPView(server_ref->m_isp_list[i]);
			}
		}

		t_peers peers;
		time_t ctime;
		int completed;
		int fid;
		int leechers;
		int seeders;
		bool dirty;
		std::map<p4p::P4PPeerBase*, t_peer*> peer_map;
		
		/* P4P ISP View Manager */
		p4p::ISPViewManager *isp_view_manager;

		const Cserver* server_ref;

		t_file(const t_file& rhs) { copy(rhs); }
		t_file& operator=(const t_file& rhs) { return copy(rhs); }

		t_file& copy(const t_file& rhs)
		{
			peers = rhs.peers;
			ctime = rhs.ctime;
			completed = rhs.completed;
			fid = rhs.fid;
			leechers = rhs.leechers;
			seeders = rhs.seeders;
			dirty = rhs.dirty;
			server_ref = rhs.server_ref;
			isp_view_manager = rhs.isp_view_manager;
			return *this;
		}
	};

	struct t_user
	{
		t_user()
		{
			can_leech = true;
			completes = 0;
			incompletes = 0;
			peers_limit = 0;
			torrent_pass_version = 0;
			torrents_limit = 0;
			wait_time = 0;
		}

		bool can_leech;
		bool marked;
		int uid;
		int completes;
		int incompletes;
		int peers_limit;
		int torrent_pass_version;
		int torrents_limit;
		int wait_time;
	};

	typedef std::map<std::string, t_file> t_files;
	typedef std::map<unsigned int, t_deny_from_host> t_deny_from_hosts;
	typedef std::map<int, t_user> t_users;
	typedef std::map<std::string, t_user*> t_users_torrent_passes;

	int test_sql();
	void accept(const Csocket&);
	t_user* find_user_by_torrent_pass(const std::string&, const std::string& info_hash);
	t_user* find_user_by_uid(int);
	void read_config();
	void write_db_files();
	void write_db_users();
	void read_db_deny_from_hosts();
	void read_db_files();
	void read_db_files_sql();
	void read_db_users();
	void clean_up();
	
	/* P4P */
	void add_isp(std::string loc_ip, int loc_port, std::string dis_ip, int dis_port);
	std::string insert_peer(const Ctracker_input&, bool udp, t_user*);
	std::string debug(const Ctracker_input&) const;
	std::string statistics() const;
	Cvirtual_binary select_peers(const Ctracker_input&) const;
	Cvirtual_binary scrape(const Ctracker_input&);
	int run();
	static void term();
	Cserver(Cdatabase&, const std::string& table_prefix, bool use_sql, const std::string& conf_file);

	const t_file* file(const std::string& id) const
	{
		t_files::const_iterator i = m_files.find(id);
		return i == m_files.end() ? NULL : &i->second;
	}

	const Cconfig& config() const
	{
		return m_config;
	}

	long long secret() const
	{
		return m_secret;
	}

	Cstats& stats()
	{
		return m_stats;
	}

	time_t time() const
	{
		return m_time;
	}
	
	std::vector<p4p::ISP*> m_isp_list; /* ISP List */
	std::map<const p4p::ISP*, p4p::ISPGenericPeeringGuidance*> m_guidance_map;
	std::string m_guidance_server_ip;
	unsigned short m_guidance_server_port;
	
private:
	enum
	{
		column_files_completed,
		column_files_fid,
		column_files_leechers,
		column_files_seeders,
		column_users_uid,
		table_announce_log,
		table_config,
		table_deny_from_hosts,
		table_files,
		table_files_users,
		table_scrape_log,
		table_users,
	};

	typedef boost::ptr_list<Cconnection> t_connections;
	typedef std::list<Ctcp_listen_socket> t_tcp_sockets;
	typedef std::list<Cudp_listen_socket> t_udp_sockets;

	static void sig_handler(int v);
	std::string column_name(int v) const;
	std::string table_name(int) const;

	Cconfig m_config;
	Cstats m_stats;
	bool m_read_users_can_leech;
	bool m_read_users_peers_limit;
	bool m_read_users_torrent_pass;
	bool m_read_users_torrent_pass_version;
	bool m_read_users_torrents_limit;
	bool m_read_users_wait_time;
	bool m_use_sql;
	time_t m_clean_up_time;
	time_t m_read_config_time;
	time_t m_read_db_deny_from_hosts_time;
	time_t m_read_db_files_time;
	time_t m_read_db_users_time;
	time_t m_time;
	time_t m_write_db_files_time;
	time_t m_write_db_users_time;
	int m_fid_end;
	long long m_secret;
	t_connections m_connections;
	Cdatabase& m_database;
	Cepoll m_epoll;
	t_deny_from_hosts m_deny_from_hosts;
	t_files m_files;
	t_users m_users;
	t_users_torrent_passes m_users_torrent_passes;
	std::string m_announce_log_buffer;
	std::string m_conf_file;
	std::string m_files_users_updates_buffer;
	std::string m_scrape_log_buffer;
	std::string m_table_prefix;
	std::string m_users_updates_buffer;
	
	static void update_isps(Cserver* server);		/* Thread routine for updating ISP information */
	static void update_guidance(Cserver* server);		/* Thread routine for updating Peering guidance */
};
