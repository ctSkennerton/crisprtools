dnl Process this file with autoconf to produce a configure script.

AC_INIT(crisprtools, 0.0.1)

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR(build)
AC_CONFIG_MACRO_DIR(m4)
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_SRCDIR([src/main.cpp])

LT_INIT
LT_LANG([C++])

# extra configure options
#
#

#DEBUG
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],
    [Turn on debugging preprocessor flags])],
    [case "${enableval}" in
        yes) debug=true ;;
        no)  debug=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],[debug=false])

AM_CONDITIONAL([DEBUG],[test x$debug = xtrue])

if test x$debug = xtrue; then
    AC_DEFINE([DEBUG],[1],[define to 1 for debug statments])
fi




AC_PROG_CXX

AX_LIB_XERCES
if test $HAVE_XERCES = no; then
    AC_MSG_ERROR([Cannot find Xerces-c])
else
    AM_CONDITIONAL([XERCES_FOUND], [test $HAVE_XERCES = yes])
fi



GV_LIBS=yes
#check that graphviz is installed before defining RENDERING
AC_CHECK_LIB([gvc],[gvContext],,[$GV_LIBS=no])
AC_CHECK_LIB([cdt],[dtopen],,[$GV_LIBS=no])
AC_CHECK_LIB([graph],[agnode],,[$GV_LIBS=no])

# if we don't have the libraries then check for the executables
GV_PROG=no

#if test x$GV_LIBS = xno; then
#    AC_CHECK_PROGS([GV_PROGS], [neato fdp sfdp dot circo twopi], [no])
#    if test x$GV_PROGS != xno; then
#        $GV_PROG=yes
#        AC_DEFINE([GRAPHVIZ_EXECUTABLES_FOUND],[1],[Defines to 1 if image rendering is to be performed])
#        AC_DEFINE_UNQUOTED([DEFAULT_RENDERING_ALGORITHM],["${GV_PROGS}"],[Sets the Default Graphviz algorithm])
#    fi
#fi

AM_CONDITIONAL([FOUND_GRAPHVIZ_LIBRARIES],[test x$GV_LIBS = xyes])
AM_CONDITIONAL([FOUND_GRAPHVIZ_EXECUTABLES],[test x$GV_PROG = xyes])

AC_CHECK_HEADERS([getopt.h stdlib.h])

# only perform the check if rendering
if test x$GV_PROG = xyes; then
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







AC_OUTPUT([
Makefile
src/Makefile
src/libcrispr/Makefile
])
