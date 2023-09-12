//maker
#include "win/Maker.h"

void setup_dlls(Maker& maker)
{
	maker.m_path_dll.clear();
}
void setup_libs_debug(Maker& maker)
{
	return;
}
void setup_libs_release(Maker& maker)
{
	return;
}
void setup_libs(Maker& maker)
{
	maker.m_libs.clear();
	maker.m_libs += "glu32.lib ";
	maker.m_libs += "opengl32.lib ";
	maker.m_mode.compare("debug") == 0 ? setup_libs_debug(maker) : setup_libs_release(maker);
}

int main(int argc, char** argv)
{
	//setup
	Maker maker;
	maker.m_out = "chair";
	maker.setup(argc, argv);
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