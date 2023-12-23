//maker
#include "../Maker/inc/Maker.hpp"

void setup_dlls(Maker& maker)
{
	maker.m_path_dll.push_back(maker.m_edll + "glew32.dll");
	maker.m_path_dll.push_back(maker.m_edll + "libblas.dll");
	maker.m_path_dll.push_back(maker.m_edll + "freeglut.dll");
	maker.m_path_dll.push_back(maker.m_edll + "freetype.dll");
	maker.m_path_dll.push_back(maker.m_edll + "liblapack.dll");
	maker.m_path_dll.push_back(maker.m_edll + "libquadmath-0.dll");
	maker.m_path_dll.push_back(maker.m_edll + "libgfortran-3.dll");
	maker.m_path_dll.push_back(maker.m_edll + "libgcc_s_sjlj-1.dll");
}
void setup_libs_debug(Maker& maker)
{
	maker.m_libs += "../Math/dist/debug/libmath.lib ";
	maker.m_libs += "../Canvas/lib/dist/debug/libcanvas.lib ";
}
void setup_libs_release(Maker& maker)
{
	maker.m_libs += "../Math/dist/release/libmath.lib ";
	maker.m_libs += "../Canvas/lib/dist/release/libcanvas.lib ";
}
void setup_libs(Maker& maker)
{
	maker.m_libs += maker.m_elib + "glew32.lib ";
	maker.m_libs += maker.m_elib + "libblas.lib ";
	maker.m_libs += maker.m_elib + "freeglut.lib ";
	maker.m_libs += maker.m_elib + "freetype.lib ";
	maker.m_libs += maker.m_elib + "liblapack.lib ";
	maker.m_mode.compare("debug") == 0 ? setup_libs_debug(maker) : setup_libs_release(maker);
}

int main(int argc, char** argv)
{
	//setup
	Maker maker;
	maker.m_out = "tensegrity";
	maker.setup(argc, argv);
	maker.m_incs += "/I ../ /I ../Math/ ";
	//build
	if(!maker.m_clean)
	{
		setup_libs(maker);
		setup_dlls(maker);
		maker.build_src();
		maker.build_dll();
		maker.build_exe();
		maker.build_run();
		maker.build_debug();
	}
	if(maker.m_clean)
	{
		maker.build_clean();
	}
	//return
	return EXIT_SUCCESS;
}