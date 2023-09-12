//std
#include <omp.h>
#include <cstring>
#include <algorithm>

//maker
#include "Maker.h"

//constructors
Maker::Maker(void) : 
	m_lib(false), m_gui(false), m_run(false), m_clean(false), m_debug(false), m_mode("debug")
{
	return;
}

//destructor
Maker::~Maker(void)
{
	return;
}

//build
void Maker::build_uic(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_uic.size(); i++)
	{
		const std::filesystem::path uic = m_path_uic[i];
		const std::filesystem::path inc = uic_to_inc(uic);
		const std::string cmd = "uic " + uic.string() + " -o " + inc.string();
		if(!std::filesystem::exists(inc) || std::filesystem::last_write_time(inc) < std::filesystem::last_write_time(uic))
		{
			#pragma omp critical
			{
				printf("uicing: %s\n", uic.string().c_str());
			}
			if(!std::filesystem::exists(inc.parent_path()))
			{
				std::filesystem::create_directories(inc.parent_path());
			}
			if(system(cmd.c_str()))
			{
				printf("Error build uic %s!\n", uic.string().c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
}
void Maker::build_inc(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_inc.size(); i++)
	{
		const std::filesystem::path inc = m_path_inc[i];
		const std::filesystem::path src = inc_to_src(inc);
		const std::string cmd = "moc " + inc.string() + " -o" + src.string() + " -nw";
		if(!std::filesystem::exists(src) || std::filesystem::last_write_time(src) < std::filesystem::last_write_time(inc))
		{
			#pragma omp critical
			{
				printf("mocing: %s\n", inc.string().c_str());
			}
			if(!std::filesystem::exists(src.parent_path()))
			{
				std::filesystem::create_directories(src.parent_path());
			}
			if(system(cmd.c_str()))
			{
				printf("Error build header %s!\n", inc.string().c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
}
void Maker::build_rsc(void)
{
	for(int i = 0; i < m_path_rsc.size(); i++)
	{
		const std::filesystem::path rsc = m_path_rsc[i];
		const std::filesystem::path src = rsc_to_src(rsc);
		const std::string cmd = "rcc " + rsc.string() + " -o " + src.string();
		if(!std::filesystem::exists(src) || std::filesystem::last_write_time(src) < std::filesystem::last_write_time(rsc))
		{
			printf("rscing: %s\n", rsc.string().c_str());
			if(!std::filesystem::exists(src.parent_path()))
			{
				std::filesystem::create_directories(src.parent_path());
			}
			if(system(cmd.c_str()))
			{
				printf("Error build resource %s!\n", rsc.string().c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
}
void Maker::build_src(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_src.size(); i++)
	{
		//data
		char log_path[50];
		const unsigned a = omp_get_thread_num();
		std::vector<std::filesystem::path> list;
		const std::filesystem::path src = m_path_src[i];
		const std::filesystem::path obj = src_to_obj(src);
		//check
		find_dependencies(list, src);
		bool build = !std::filesystem::exists(obj);
		sprintf(log_path, "build\\log_%02d.txt", a);
		build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(src);
		for(const std::filesystem::path& inc : list)
		{
			build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(inc);
		}
		const std::string cmd = m_cxx + m_flags + m_defs + m_incs + " /Fo:" + obj.string() + " " + src.string() + " >> " + log_path;
		//build
		if(build)
		{
			if(!std::filesystem::exists(obj.parent_path()))
			{
				std::filesystem::create_directories(obj.parent_path());
			}
			#pragma omp critical
			{
				printf("compiling(%s): %s\n", m_mode.c_str(), src.string().c_str());
			}
			if(system(cmd.c_str()))
			{
				#pragma omp critical
				{
					print_file(log_path);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	join_logs();
}
void Maker::build_moc(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_inc.size(); i++)
	{
		//data
		char log_path[50];
		const unsigned a = omp_get_thread_num();
		std::vector<std::filesystem::path> list;
		const std::filesystem::path inc = m_path_inc[i];
		const std::filesystem::path src = inc_to_src(inc);
		const std::filesystem::path obj = inc_to_obj(inc);
		//check
		find_dependencies(list, src);
		bool build = !std::filesystem::exists(obj);
		sprintf(log_path, "build\\log_moc_%02d.txt", a);
		build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(src);
		for(const std::filesystem::path& inc : list)
		{
			build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(inc);
		}
		const std::string cmd = m_cxx + m_flags + m_defs + m_incs + " /Fo:" + obj.string() + " " + src.string() + " >> " + log_path;
		//build
		if(build)
		{
			if(!std::filesystem::exists(obj.parent_path()))
			{
				std::filesystem::create_directories(obj.parent_path());
			}
			#pragma omp critical
			{
				printf("compiling(%s): %s\n", m_mode.c_str(), src.string().c_str());
			}
			if(system(cmd.c_str()))
			{
				#pragma omp critical
				{
					print_file(log_path);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	join_logs("build/log_moc.txt");
}
void Maker::build_rcc(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_rsc.size(); i++)
	{
		//data
		char log_path[50];
		const unsigned a = omp_get_thread_num();
		std::vector<std::filesystem::path> list;
		const std::filesystem::path rsc = m_path_rsc[i];
		const std::filesystem::path src = rsc_to_src(rsc);
		const std::filesystem::path obj = rsc_to_obj(rsc);
		//check
		find_dependencies(list, src);
		bool build = !std::filesystem::exists(obj);
		sprintf(log_path, "build\\log_rcc_%02d.txt", a);
		build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(src);
		for(const std::filesystem::path& inc : list)
		{
			build = build || std::filesystem::last_write_time(obj) < std::filesystem::last_write_time(inc);
		}
		const std::string cmd = m_cxx + m_flags + m_defs + m_incs + " /Fo:" + obj.string() + " " + src.string() + " >> " + log_path;
		//build
		if(build)
		{
			if(!std::filesystem::exists(obj.parent_path()))
			{
				std::filesystem::create_directories(obj.parent_path());
			}
			#pragma omp critical
			{
				printf("compiling(%s): %s\n", m_mode.c_str(), src.string().c_str());
			}
			if(system(cmd.c_str()))
			{
				#pragma omp critical
				{
					print_file(log_path);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	join_logs("build\\log_rcc.txt");
}
void Maker::build_dll(void)
{
	omp_set_num_threads(16);
	#pragma omp parallel for
	for(int i = 0; i < m_path_dll.size(); i++)
	{
		std::filesystem::path ext = m_path_dll[i];
		std::filesystem::path dll = ext_to_dll(ext);
		const std::string cmd = "copy /y " + replace(ext, "/", "\\").string() + " " + replace(dll, "/", "\\").string() + " > nul";
		if(!std::filesystem::exists(dll) || std::filesystem::last_write_time(dll) < std::filesystem::last_write_time(ext))
		{
			if(system(cmd.c_str()))
			{
				printf("Error coping dll %s!\n", ext.string().c_str());
				exit(EXIT_FAILURE);
			}
		}
	}
}
void Maker::build_lib(void)
{
	//data
	std::string objs;
	const std::string dir = "dist/" + m_mode;
	//objects
	for(const std::filesystem::path& path : m_path_src)
	{
		objs += src_to_obj(path).string() + " ";
	}
	//temp
	FILE* file = fopen("objs", "w");
	fprintf(file, "%s", objs.c_str());
	fclose(file);
	//setup
	if(!std::filesystem::exists(dir.c_str()))
	{
		std::filesystem::create_directories(dir);
	}
	//build
	std::string out = "dist/" + m_mode + "/lib" + m_out + ".lib ";
	std::string cmd = "lib /nologo /out:" + out + "@objs";
	if(system(cmd.c_str()))
	{
		printf("Error building library!\n");
		exit(EXIT_FAILURE);
	}
	printf("library(%s): %s\n", m_mode.c_str(), out.c_str());
	//delete
	std::filesystem::remove("objs");
}
void Maker::build_exe(void)
{
	//data
	std::string objs;
	const std::string dir = "dist/" + m_mode;
	//objects
	for(const std::filesystem::path& path : m_path_src)
	{
		objs += src_to_obj(path).string() + " ";
	}
	if(m_gui)
	{
		for(const std::filesystem::path& path : m_path_inc)
		{
			objs += inc_to_obj(path).string() + " ";
		}
		for(const std::filesystem::path& path : m_path_rsc)
		{
			objs += rsc_to_obj(path).string() + " ";
		}
	}
	//temp
	FILE* file = fopen("objs", "w");
	fprintf(file, "%s", objs.c_str());
	fclose(file);
	//build
	std::string out = "dist/" + m_mode + "/" + m_out + ".exe ";
	std::string cmd = "link /nologo " + m_lnks + "/out:" + out + "@objs " + m_libs;
	if(system(cmd.c_str()))
	{
		printf("Error building executable!\n");
		exit(EXIT_FAILURE);
	}
	printf("executable(%s): dist/%s/%s.exe\n", m_mode.c_str(), m_mode.c_str(), m_out.c_str());
	//delete
	std::filesystem::remove("objs");
}
void Maker::build_run(void)
{
	if(m_run && system(("dist\\" + m_mode + "\\" + m_out + ".exe").c_str()))
	{
		printf("Error running executable!\n");
		exit(EXIT_FAILURE);
	}
}
void Maker::build_clean(void)
{
	if(std::filesystem::exists("dist/" + m_mode))
	{
		std::filesystem::remove_all("dist/" + m_mode);
	}
	if(std::filesystem::exists("build/" + m_mode))
	{
		std::filesystem::remove_all("build/" + m_mode);
	}
	if(m_gui)
	{
		if(std::filesystem::exists("build/uic"))
		{
			std::filesystem::remove_all("build/uic");
		}
		if(std::filesystem::exists("build/moc"))
		{
			std::filesystem::remove_all("build/moc");
		}
	}
}
void Maker::build_debug(void)
{
	if(m_debug && system(("devenv /debugexe dist/debug/" + m_out + ".exe").c_str()))
	{
		printf("Error launching debug environment!\n");
		exit(EXIT_FAILURE);
	}
}

//setup
void Maker::setup(int argc, char** argv)
{
	//data
	m_cxx = "cl ";
	m_incs = "/I . ";
	m_elib = "../external/cpp/lib/x64/";
	m_edll = "../external/cpp/dll/x64/";
	m_qlib = "C:/Qt/6.4.3/msvc2019_64/lib/";
	m_lnks = "/IGNORE:4099 /STACK:104857600 ";
	m_defs = "/D \"NOMINMAX\" /D \"_USE_MATH_DEFINES\" ";
	m_flags = "/nologo /std:c++20 /EHsc /c /openmp /diagnostics:caret ";
	if(m_gui)
	{
		m_flags += "/Zc:__cplusplus ";
		m_incs += "/I C:/Qt/6.4.3/msvc2019_64/include ";
		m_defs += "/D QT_CORE_LIB /D QT_GUI_LIB /D QT_WIDGETS_LIB ";
		m_defs += "/D QT_SHARED /D QT_OPENGL_LIB /D QT_CONSOLE /D QT_PRINTSUPPORT_LIB ";
	}
	//args
	for(unsigned i = 0; i < argc; i++)
	{
		m_run = m_run || strcmp(argv[i], "run") == 0;
		m_clean = m_clean || strcmp(argv[i], "clean") == 0;
		m_debug = m_debug || strcmp(argv[i], "debug") == 0;
		if(strcmp(argv[i], "m=r") == 0) m_mode = "release";
	}
	//mode
	if(m_mode.compare("debug") != 0)
	{
		m_flags += "/D \"NDEBUG\" /O2 /MD ";
	}
	else
	{
		m_lnks += "/debug ";
		std::string lib = m_lib ? ("lib" + m_out) : m_out;
		m_flags += "/Zi /FS /D \"_DEBUG\" /Od /MDd /RTC1 /Fddist/debug/" + lib + ".pdb ";
	}
	//dist
	if(!std::filesystem::exists("dist/" + m_mode))
	{
		std::filesystem::create_directories("dist/" + m_mode);
	}
	//files
	find_files(m_path_uic, "ui/", ".ui");
	find_files(m_path_inc, "inc/", ".h");
	find_files(m_path_src, "src/", ".cpp");
	find_files(m_path_rsc, "rsc/", ".qrc");
}

//log
void Maker::print_file(std::string path)
{
	FILE* file = fopen(path.c_str(), "r");
	while(!feof(file))
	{
		printf("%c", fgetc(file));
	}
	fclose(file);
}
void Maker::join_logs(std::string log_file)
{
	char cmd[200];
	char log_path[50];
	bool join = false;
	for(unsigned i = 0; i < omp_get_num_threads(); i++)
	{
		sprintf(log_path, "build\\log_%02d.txt", i);
		join = join || std::filesystem::exists(log_path);
	}
	if(log_file.empty()) log_file = "build\\log.txt";
	sprintf(cmd, "copy /b build\\log_*.txt %s > nul & del build\\log_*.txt", log_file.c_str());
	if(join && system(cmd))
	{
		printf("Error joining logs!\n");
		exit(EXIT_FAILURE);
	}
}

//find
void Maker::find_files(std::vector<std::filesystem::path>& list, std::string dir, std::string ext)
{
	if(std::filesystem::exists(dir) && !std::filesystem::is_empty(dir))
	{
		for(const std::filesystem::directory_entry& item : std::filesystem::recursive_directory_iterator(dir))
		{
			std::string str = item.path().string();
			if(str.compare(str.length() - ext.length(), ext.length(), ext) == 0)
			{
				list.push_back(replace(str, "\\", "/"));
			}
		}
	}
}
void Maker::find_dependencies(std::vector<std::filesystem::path>& list, std::filesystem::path path)
{
	//open
	const char* str = "#include \"";
	FILE* file = fopen(path.string().c_str(), "r");
	//read
	char buffer[1000];
	while(!feof(file))
	{
		fgets(buffer, 1000, file);
		if(!strncmp(buffer, str, strlen(str)))
		{
			std::string str = buffer;
			strip(strip(strip(str, "#include "), "\""), "\n");
			if(std::filesystem::exists("../" + str))
			{
				list.push_back("../" + str);
				find_dependencies(list, "../" + str);
			}
		}
	}
	std::sort(list.begin(), list.end() );
	list.erase(std::unique(list.begin(), list.end()), list.end());
	//close
	fclose(file);
}

//string
std::filesystem::path Maker::inc_to_obj(std::filesystem::path path)
{
	path = replace(path, ".h", ".obj");
	path = replace(path, "inc/", "build/" + m_mode + "/moc/");
	return path;
}
std::filesystem::path Maker::src_to_obj(std::filesystem::path path)
{
	path = replace(path, ".cpp", ".obj");
	path = replace(path, "src/", "build/" + m_mode + "/obj/");
	return path;
}
std::filesystem::path Maker::rsc_to_obj(std::filesystem::path path)
{
	path = replace(path, ".qrc", ".obj");
	path = replace(path, "rsc/", "build/" + m_mode + "/rsc/", 1);
	return path;
}
std::filesystem::path Maker::ext_to_dll(std::filesystem::path path)
{
	return replace(path, "../external/cpp/dll/x64/", "dist/" + m_mode + "/");
}
std::filesystem::path Maker::uic_to_inc(std::filesystem::path path)
{
	path = replace(path, ".ui", ".h");
	path = replace(path, "ui/", "build/uic/");
	return path;
}
std::filesystem::path Maker::inc_to_src(std::filesystem::path path)
{
	path = replace(path, ".h", ".cpp");
	path = replace(path, "inc/", "build/moc/");
	return path;
}
std::filesystem::path Maker::rsc_to_src(std::filesystem::path path)
{
	path = replace(path, ".qrc", ".cpp");
	path = replace(path, "rsc/", "build/rsc/", 1);
	return path;
}

//string
std::string& Maker::strip(std::string& str, std::string s1)
{
	return replace(str, s1, "");
}
std::string& Maker::replace(std::string& str, std::string s1, std::string s2, unsigned max)
{
	for(unsigned i = 0; i < max; i++)
	{
		size_t index = str.find(s1);
		if(index == std::string::npos) break;
		str.replace(index, s1.length(), s2);
	}
	return str;
}
std::filesystem::path& Maker::replace(std::filesystem::path& path, std::string s1, std::string s2, unsigned max)
{
	std::string str = path.string();
	replace(str, s1, s2, max);
	path = str;
	return path;
}