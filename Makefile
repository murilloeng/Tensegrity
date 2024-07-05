#qt
qln = /home/murillo/Qt/6.6.1/gcc_64/lib
qin = /home/murillo/Qt/6.6.1/gcc_64/include
uic = /home/murillo/Qt/6.6.1/gcc_64/libexec/uic
qde = /home/murillo/Qt/6.6.1/gcc_64/mkspecs/linux-g++

#compiler
CXX = g++
LIBS += -L $(qln)
WARS += -Wall -Wno-unused-variable -Wno-unused-result
INCS += -I .. -I ../external/cpp/inc -I $(qde) -I $(qin)
LIBS += -l lapack -l quadrule -l GLEW -l GL -l glut -l freetype
LIBS += -l Qt6Core -l Qt6Gui -l Qt6Widgets -l Qt6OpenGL -l Qt6OpenGLWidgets
WARS += -Wno-format-security -Wno-return-type -Wno-unused-function -Wformat-overflow=0
CXXFLAGS += -std=c++20 -fPIC -pipe -fopenmp -MT $@ -MMD -MP -MF $(subst .o,.d, $@) $(DEFS) $(INCS) $(WARS)

#mode
ifneq ($(m), r)
	mode = debug
	CXXFLAGS += -ggdb3
else
	mode = release
	CXXFLAGS += -Ofast
endif

#ouput
out = dist/$(mode)/tensegrity.out

#interfaces
uif := $(sort $(shell find -path './ui/*.ui'))

#sources
src := $(sort $(shell find -path './src/*.cpp'))

#qt files
uig := $(subst ./ui/, build/uic/, $(patsubst %.ui, %.hpp, $(uif)))

#objects
obj := $(sort $(subst ./src/, build/$(mode)/, $(addsuffix .o, $(basename $(src)))))

#dependencies
dep := $(subst .o,.d, $(obj))

#rules
all : $(out)
	@echo 'build($(mode)): complete!'

run : $(out)
	@./$(out)

uic : $(uig)

debug : 
	@gdb $(out) -x gdb.txt

math :
	@cd ../Math && make -f Makefile m=$m

canvas :
	@cd ../Canvas && make -f Makefile m=$m

$(out) : math canvas $(uig) $(obj)
	@echo 'executable($(mode)): $@'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) -fopenmp -o $(out) -Wl,-rpath,$(qln) $(obj) ../Math/dist/$(mode)/libmath.so ../Canvas/dist/$(mode)/libcanvas.so $(LIBS)

build/uic/%.hpp : ui/%.ui
	@echo 'uicing($(mode)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(uic) $< -o $@

build/$(mode)/%.o : src/%.cpp build/$(mode)/%.d
	@echo 'compiling($(mode)): $<'
	@mkdir -p $(dir $@) && rm -rf $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(dep) : ;

-include $(dep)

clean :
	@rm -rf dist/$(mode)
	@rm -rf build/$(mode)
	@echo 'clean($(mode)): complete!'

print-% :
	@echo $* = $($*)

.PHONY : all run gui fea clean print-%