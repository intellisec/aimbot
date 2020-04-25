#include "stdafx.h"
#include "dllmain.h"

auto c_netvars::init() -> bool {
	m_tables.clear();

	auto get_list = memory->FindPattern(client, "A1 ? ? ? ? 8B ? 0C 85 C0 74 18");

	if (!get_list)
		return false;

	auto list = memory->Read<uintptr_t>(memory->Read<uintptr_t>(get_list + 0x1));

	for (auto client_class = list; !!client_class; client_class = memory->Read<uintptr_t>(client_class + offsetof(struct ClientClass, m_pNext))) {
		auto recv_table = memory->Read<uintptr_t>(client_class + offsetof(struct ClientClass, m_pRecvTable));
		auto table_name = memory->Read<uintptr_t>(recv_table + offsetof(struct RecvTable, m_pNetTableName));

		char name[64]{};
		memory->Read(table_name, name, sizeof(name));
		m_tables.push_back({ recv_table, name });
	}

	return !m_tables.empty();
}

auto c_netvars::get_table(std::string table_name) const -> uintptr_t {
	if (m_tables.empty()) return 0;

	for (auto table : m_tables) {
		if (strstr(table_name.c_str(), table.second.c_str())) {
			return table.first;
		}
	}

	return 0;
}

auto c_netvars::get_entry(std::string name, uintptr_t table_ptr, uintptr_t offset/* = 0*/) const -> std::ptrdiff_t {
	std::ptrdiff_t ret{};

	auto table = memory->Read<RecvTable>(table_ptr);

	if (table.m_nProps < 0)
		return 0;

	auto props_size = table.m_nProps * sizeof(RecvProp);

	auto props = (RecvProp*)malloc(props_size);
	memset(props, 0, props_size);
	memory->Read(table.m_pProps, props, props_size);

	for (int i{}; i < table.m_nProps; ++i) {
		RecvProp prop = props[i];

		char prop_name[64]{};
		memory->Read(prop.m_pVarName, prop_name, sizeof(prop_name));

		if (isdigit(prop_name[0]))
			continue;

		if (prop.m_pDataTable) {
			auto child = memory->Read<RecvTable>(prop.m_pDataTable);
			if (child.m_nProps)
			{
				auto tmp = get_entry(name, prop.m_pDataTable, offset + prop.m_Offset);
				if (tmp) ret = tmp;
			}
		}

		if (!strstr(prop_name, name.c_str()))
			continue;

		ret = offset + prop.m_Offset;
		break;
	}

	free(props);
	return ret;
}

std::ptrdiff_t c_netvars::get_netvar(std::string data, std::string name) const {
	std::ptrdiff_t ret{};
	auto table = get_table(data);

	ret = get_entry(name, table);
#ifdef DEBUG
	printf("%s:\t\t 0x%05x\n", name.c_str(), ret);
#endif
	return ret;
}

/*
 * DUMP
 */
auto c_netvars::dump() -> void
{
	if (m_tables.empty()) return;

	auto outfile = std::ofstream("C:\\tubs\\netvar_dump.txt");

	for (auto table : m_tables) {
		dump_table(outfile, table.first);
	}

	outfile.close();
}

auto c_netvars::dump_table(std::ostream& stream, uintptr_t table_ptr, uintptr_t offset/* = 0*/, int level/* = 0*/) const -> void
{
	auto table = memory->Read<RecvTable>(table_ptr);

	if (table.m_nProps < 0)
		return;

	char name[64]{};
	memory->Read(table.m_pNetTableName, name, sizeof(name));
	stream << std::string(level, '_') << name << ":" << std::endl;
	level++;

	auto props_size = table.m_nProps * sizeof(RecvProp);

	auto props = (RecvProp*)malloc(props_size);
	memset(props, 0, props_size);
	memory->Read(table.m_pProps, props, props_size);

	for (int i{}; i < table.m_nProps; ++i) {
		RecvProp prop = props[i];

		char prop_name[64]{};
		memory->Read(prop.m_pVarName, prop_name, sizeof(prop_name));

		if (isdigit(prop_name[0]))
			continue;

		if (prop.m_pDataTable) {
			auto child = memory->Read<RecvTable>(prop.m_pDataTable);
			if (child.m_nProps)
			{
				dump_table(stream, prop.m_pDataTable, offset + prop.m_Offset, level);
			}
		}

		sprintf_s(prop_name, "%s = 0x%X", prop_name, offset + prop.m_Offset);
		stream << std::string(level, '_') << prop_name << std::endl;
	}

	free(props);
}