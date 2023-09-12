#pragma once

//std
#include <vector>
#include <climits>
#include <filesystem>

class Maker
{
public:
	//constructor
	Maker(void);

	//destructor
	~Maker(void);

	//build
	void build_uic(void);
	void build_inc(void);
	void build_rsc(void);
	void build_src(void);
	void build_moc(void);
	void build_rcc(void);
	void build_dll(void);
	void build_lib(void);
	void build_exe(void);
	void build_run(void);
	void build_clean(void);
	void build_debug(void);

	//setup
	void setup(int, char**);

	//log
	void print_file(std::string);
	void join_logs(std::string = "");

	//find
	void find_files(std::vector<std::filesystem::path>&, std::string, std::string);
	void find_dependencies(std::vector<std::filesystem::path>&, std::filesystem::path);

	//transform
	std::filesystem::path inc_to_obj(std::filesystem::path);
	std::filesystem::path src_to_obj(std::filesystem::path);
	std::filesystem::path rsc_to_obj(std::filesystem::path);
	std::filesystem::path ext_to_dll(std::filesystem::path);
	static std::filesystem::path uic_to_inc(std::filesystem::path);
	static std::filesystem::path inc_to_src(std::filesystem::path);
	static std::filesystem::path rsc_to_src(std::filesystem::path);

	//string
	static std::string& strip(std::string&, std::string);
	static std::string& replace(std::string&, std::string, std::string, unsigned = UINT_MAX);
	static std::filesystem::path& replace(std::filesystem::path&, std::string, std::string, unsigned = UINT_MAX);

	//data
	bool m_lib, m_run, m_gui, m_clean, m_debug;
	std::vector<std::filesystem::path> m_path_inc, m_path_src, m_path_uic, m_path_dll, m_path_rsc;
	std::string m_cxx, m_out, m_mode, m_defs, m_incs, m_lnks, m_libs, m_elib, m_edll, m_qlib, m_flags;
};