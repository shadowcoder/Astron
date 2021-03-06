#pragma once
#include <unordered_map>
#include <unordered_set>

#include "StateServer.h"

/* Helper Functions */
// unpack_db_fields reads the field_count and following fields into a required map and ram map
// from a DBSERVER_GET_ALL_RESP or DBSERVER_GET_FIELDS_RESP message.
// Returns false if unpacking failed for some reason.
bool unpack_db_fields(DatagramIterator &dg, DCClass* dclass,
                      std::unordered_map<DCField*, std::vector<uint8_t> > &required,
                      std::map<DCField*, std::vector<uint8_t> > &ram);

class LoadingObject;

class DBStateServer : public StateServer
{
		friend class LoadingObject;

	public:
		DBStateServer(RoleConfig roleconfig);
		~DBStateServer();

		virtual void handle_datagram(Datagram &in_dg, DatagramIterator &dgi);

	private:
		channel_t m_db_channel; // database control channel
		std::unordered_map<uint32_t, LoadingObject*> m_loading; // loading but not active objects

		// m_next_context is the next context to send to the db. Invariant: always post-increment.
		uint32_t m_next_context;
		// m_context_datagrams is a map of "context sent to db" to datagram response stubs to send
		// back to the caller. It stores the data used to correctly route the response while the
		// dbss is waiting on the db.
		std::unordered_map<uint32_t, Datagram> m_context_datagrams;

		std::unordered_map<uint32_t, std::unordered_set<uint32_t> > m_inactive_loads;

		// handle_activate parses any DBSS_ACTIVATE_* message and spawns a LoadingObject to handle it.
		void handle_activate(DatagramIterator &dgi, bool has_other);

		// receive_object gives responsibility of a DistributedObject to the dbss
		// primarily used by a LoadingObject when the object is finished loading.
		void receive_object(DistributedObject* obj);
		// discard_loader tells the dbss to forget about a LoadingObject, either
		// because the object finished loading, or because the object failed to load.
		void discard_loader(uint32_t do_id);
};
