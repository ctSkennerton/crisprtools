#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(crisprtools, 0.1.8, c.skennerton@gmail.com)

AC_CONFIG_AUX_DIR(build)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([foreign])
LT_INIT
LT_LANG([C++])


# extra configure options
#
#

# enable rendering of images with graphviz 
AC_ARG_ENABLE([rendering],
    [AS_HELP_STRING([--enable-rendering],
    [Enables images being drawn in the output files])],
    [case "${enableval}" in
        yes) rendering=true ;;
        no)  rendering=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-rendering]) ;;
    esac],[rendering=false])


#check that graphviz is installed before defining RENDERING
GV_LIBS=
GV_BOOL=false
if test x$rendering = xtrue; then
    AC_CHECK_PROGS([GV_PROGS], [neato fdp sfdp dot circo twopi], [no])
    #check that graphviz is installed before defining RENDERING
    AC_CHECK_LIB([gvc],[gvContext],
                 [GV_LIBS="-lgvc $GV_LIBS" 
                  GV_BOOL=true
                 AC_DEFINE([HAVE_LIBGVC],[1],[Defines to one if the gvc library (-lgvc) is found])],
                 [GV_BOOL=false])
    AC_CHECK_LIB([cdt],[dtopen],
                 [GV_LIBS="-lcdt $GV_LIBS"
                  GV_BOOL=true
                  AC_DEFINE([HAVE_LIBCDT],[1],[Defines to one if the cdt library (-lcdt) is found])],
                 [GV_BOOL=false])
    AC_CHECK_LIB([graph],[agnode],
                 [GV_LIBS="-lgraph $GV_LIBS"
                  GV_BOOL=true
                  AC_DEFINE([HAVE_LIBGRAPH],[1],[Defines to one if the graph library (-lgraph) is found])],
                 [GV_BOOL=false])
    if test x$GV_PROGS != xno; then
        
        AC_DEFINE([RENDERING],[1],[Defines to 1 if image rendering is to be performed])
        AC_DEFINE_UNQUOTED([DEFAULT_RENDERING_ALGORITHM],["${GV_PROGS}"],[Sets the Default Graphviz algorithm])
    else
        echo "*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*"
        echo "None of the Graphviz executables could"
        echo "be found in your PATH!"
        echo "Your installation will not conatin"
        echo "rendering options!"
        echo "*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*"
    fi
fi
AM_CONDITIONAL([FOUND_GRAPHVIZ_LIBRARIES],[test $GV_BOOL = true])
AC_SUBST(GV_LIBS)


# Checks for different libs, headers, functions & programs
#
#

# Checks for compilers.


AC_PROG_CXX
AC_PROG_CC

AX_LIBCRISPR
if test $HAVE_LIBCRISPR = no; then
AC_MSG_ERROR([Cannot find libcrispr])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h unistd.h getopt.h])

# Check for presence of pdfLaTeX
AC_CHECK_PROG(PDFLATEX, pdflatex, pdflatex)
if test -z "$PDFLATEX"; then
    AC_MSG_WARN([Unable to create PDF version of the user manual.])
fi

AM_CONDITIONAL([HAVE_PDFLATEX], test -n "$PDFLATEX")


# only perform the check if rendering
if test x$rendering = xtrue; then
    # Checks for Graphviz program neato
    AC_CHECK_PROG([NEATO],[neato],[yes],[no])
    if test x$NEATO = xyes; then
        AC_DEFINE([HAVE_NEATO],[1],[Defines to 1 if the program 'neato' is found in PATH])
    fi 

    # Checks for Graphviz program dot
    AC_CHECK_PROG([DOT],[dot],[yes],[no])
    if test x$DOT = xyes; then
        AC_DEFINE([HAVE_DOT],[1],[Defines to 1 if the program 'dot' is found in PATH])
    fi 

    # Checks for Graphviz program fdp
    AC_CHECK_PROG([FDP],[fdp],[yes],[no])
    if test x$FDP = xyes; then
        AC_DEFINE([HAVE_FDP],[1],[Defines to 1 if the program 'fdp' is found in PATH])
    fi 

    # Checks for Graphviz program sfdp
    AC_CHECK_PROG([SFDP],[sfdp],[yes],[no])
    if test x$SFDP = xyes; then
        AC_DEFINE([HAVE_SFDP],[1],[Defines to 1 if the program 'sfdp' is found in PATH])
    fi 

    # Checks for Graphviz program circo
    AC_CHECK_PROG([CIRCO],[circo],[yes],[no])
    if test x$CIRCO = xyes; then
        AC_DEFINE([HAVE_CIRCO],[1],[Defines to 1 if the program 'circo' is found in PATH])
    fi 

    # Checks for Graphviz program twopi
    AC_CHECK_PROG([TWOPI],[twopi],[yes],[no])
    if test x$FDP = xyes; then
        AC_DEFINE([HAVE_TWOPI],[1],[Defines to 1 if the program 'twopi' is found in PATH])
    fi 
fi


AC_OUTPUT(Makefile src/Makefile man/Makefile)
