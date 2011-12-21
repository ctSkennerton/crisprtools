## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_LDFLAGS = @XERCES_LDFLAGS@
if XERCES_FOUND
LIBS += -lxerces-c
endif


AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = crisprtools

crisprtools_SOURCES = \
	main.cpp \
	StlExt.h \
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
	CrassXML.h \
	CrassXML.cpp \
	CrisprException.h \
	CrisprException.cpp \
    Utils.cpp \
    Utils.h \
    Rainbow.cpp \
    Rainbow.h

if FOUND_GRAPHVIZ_LIBRARIES
crisprtools_SOURCES += DrawTool.cpp \
DrawTool.h \
CrisprGraph.cpp \
CrisprGraph.h 
endif

crisprtools_LDFLAGS = 

crisprtools_LDADD =  

