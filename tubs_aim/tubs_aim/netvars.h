#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define GET_NETVAR(t,n) c_netvars::get().get_netvar(t, n)

struct RecvProp;

struct RecvTable {
	uintptr_t m_pProps;
	int       m_nProps;
	void*     m_pDecoder;
	uintptr_t m_pNetTableName;
	bool      m_bInitialized;
	bool      m_bInMainList;
};

struct RecvProp {
	uintptr_t m_pVarName;
	char      pad_0[0x24];
	uintptr_t m_pDataTable;
	int       m_Offset;
	char      pad_1[0xC];
};

struct ClientClass {
	void*	  m_pCreateFn;
	void*	  m_pCreateEventFn;
	uintptr_t m_pNetworkName;
	uintptr_t m_pRecvTable;
	uintptr_t m_pNext;
	int       m_ClassID;
};

class c_netvars {
public:
	static auto get() -> c_netvars& {
		static auto singleton = c_netvars();
		return singleton;
	}
	auto init() -> bool;
	auto get_netvar(std::string table, std::string name) const->std::ptrdiff_t;
	auto dump() -> void;
private:
	auto get_entry(std::string name, uintptr_t table_ptr, uintptr_t offset = 0) const->std::ptrdiff_t;
	auto get_table(std::string table_name) const->uintptr_t;
	auto dump_table(std::ostream& stream, uintptr_t table_ptr, uintptr_t offset = 0, int level = 0) const -> void;
private:
	std::vector< std::pair< uintptr_t, std::string > > m_tables;
};
