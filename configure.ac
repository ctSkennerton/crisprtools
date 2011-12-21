dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(crisprtools, 0.0.1)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.10])

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

# enable rendering of images with graphviz 
AC_ARG_ENABLE([rendering],
    [AS_HELP_STRING([--enable-rendering],
    [Enables images being drawn in the output files])],
    [case "${enableval}" in
        yes) rendering=true ;;
        no)  rendering=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-rendering]) ;;
    esac],[rendering=false])


AC_PROG_CXX

AX_LIB_XERCES
if test $HAVE_XERCES = no; then
AC_MSG_ERROR([Cannot find Xerces-c])
else
AM_CONDITIONAL([XERCES_FOUND], [test $HAVE_XERCES = yes])
fi

CAN_DRAW=no
#check that graphviz is installed before defining RENDERING
if test x$rendering = xtrue; then
    AC_CHECK_LIB([gvc],[gvContext])
    AC_CHECK_LIB([cdt],[dtopen])
    AC_CHECK_LIB([graph],[agnode])
    AC_CHECK_PROGS([GV_PROGS], [neato fdp sfdp dot circo twopi], [no])
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

AM_CONDITIONAL([FOUND_GRAPHVIZ_LIBRARIES],[test x$CAN_DRAW != xno])


AC_CHECK_HEADERS([getopt.h stdlib.h])

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







AC_OUTPUT([
Makefile
src/Makefile
])
