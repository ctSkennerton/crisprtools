## Process this file with automake to produce Makefile.in

bin_PROGRAMS = crisprtools
crisprtools_CXXFLAGS = @LIBCRISPR_CPPFLAGS@ -Werror -Wall -pedantic
crisprtools_LDFLAGS = @LIBCRISPR_LD_FLAGS@
crisprtools_LDFLAGS = @GV_LIBS@ @LIBCRISPR_LIBS@
crisprtools_SOURCES = \
	main.cpp \
	MergeTool.cpp \
	MergeTool.h \
	SplitTool.cpp \
	SplitTool.h \
	FilterTool.cpp \
	FilterTool.h \
	ExtractTool.cpp \
	ExtractTool.h \
	SanitiseTool.cpp \
	SanitiseTool.h \
	StatTool.cpp \
	StatTool.h \
	Utils.cpp \
	Utils.h \
	Rainbow.cpp \
	Rainbow.h \
	RemoveTool.h \
	RemoveTool.cpp 
    
if FOUND_GRAPHVIZ_LIBRARIES
crisprtools_SOURCES += DrawTool.cpp DrawTool.h CrisprGraph.cpp CrisprGraph.h 
endif



