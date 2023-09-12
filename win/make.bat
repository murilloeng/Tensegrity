cl /nologo /std:c++20 /EHsc /c /openmp /D "NDEBUG" /O2 /MD /I . win/make.cpp /Fo:win/make.obj
cl /nologo /std:c++20 /EHsc /c /openmp /D "NDEBUG" /O2 /MD /I . win/Maker.cpp /Fo:win/Maker.obj
link /nologo /out:make.exe win/make.obj win/Maker.obj