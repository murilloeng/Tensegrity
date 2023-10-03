cl /nologo /std:c++20 /EHsc /c /openmp /D "NDEBUG" /O2 /MD /I . win/make.cpp /Fo:win/make.obj
link /nologo /out:make.exe win/make.obj ../Maker/dist/Maker.lib