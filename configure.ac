AC_INIT(myCCexamples, 0.0.1, parasyte@sleepingstill.com)
#AC_INIT(src/)
AM_INIT_AUTOMAKE(myCPPexamples, 0.0.1)
AM_CONFIG_HEADER(config.h)
###########################################
###########################################
#RPM
dnl For my rpm.m4 macros
RPM_RELEASE=1
AC_SUBST(RPM_RELEASE)

AM_RPM_INIT
dnl Enable or disable the rpm making rules in Makefile.am
AM_CONDITIONAL(MAKE_RPMS, test x$make_rpms = xtrue)

#for c++: dnl This is a little hack to make this work with rpm better (see mysql++.spec.in)
test -z "$CXXFLAGS" && CXXFLAGS="${CFLAGS}"
#
#debug mode
AC_ARG_ENABLE(debug,
	AC_HELP_STRING(--enable-debug,Output debugging information to STDOUT (default=no)),
	enable_debug=yes,enable_debug=no)

if [! test "x$enable_debug" != "xyes"]; then
	CFLAGS="$CFLAGS -DDEBUG"
	CXXFLAGS="$CXXFLAGS -DDEBUG"
fi
#copyright statement
COPYRIGHT='"COPYRIGHT 2004, Karl N. Redman (parasyte@sleepingstill.com)"'
AC_DEFINE_UNQUOTED(COPYRIGHT,$COPYRIGHT, GNU and SleepingStill copyright statement)
LICENSE='"This application comes with ABSOLUTELY NO WARRANTY; \nfor details see the licensing agreement distributed with this application"'
AC_DEFINE_UNQUOTED(LICENSE,$LICENSE, GNU and SleepingStill license statement)
#
#######################################
#######################################
#debuging output parameters (for debug.h)
DEBUG_PRINTF="printf"
DEBUG_PRINTF_INCLUDE="<stdio.h>"
AC_DEFINE_UNQUOTED(DEBUG_PRINTF, $DEBUG_PRINTF,Specify the printf type of function call used in debug.h)
AC_DEFINE_UNQUOTED(DEBUG_PRINTF_INCLUDE, $DEBUG_PRINTF_INCLUDE, Specify the include file to support the printf statement)
##
#######################################
#######################################
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_LIBTOOL
#AC_PROG_INSTALL
##
dnl *************************************************
dnl gettext support
dnl *************************************************

COPYRIGHT='"COPYRIGHT 2004, Karl N. Redman (parasyte@sleepingstill.com)"'
AC_DEFINE_UNQUOTED(COPYRIGHT,$COPYRIGHT, GNU and SleepingStill copyright statement)

GETTEXT_PACKAGE="myCPPexamples"
##AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,$GETTEXT_PACKAGE, gettext support)

dnl Add the languages which your application supports here.
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

dnl Set PACKAGE_LOCALE_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, ${ac_default_prefix}/${DATADIRNAME}/locale, package locale dir)
else
  AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, ${prefix}/${DATADIRNAME}/locale, package locale dir)
fi

AC_LANG_CPLUSPLUS

dnl *************************************************
dnl libraries support
dnl *************************************************

#PKG_CHECK_MODULES(GTKMM,[gtkmm-2.0 >= 2.2.8])
#AC_SUBST(GTKMM_CFLAGS)
#AC_SUBST(GTKMM_LIBS)

AC_CHECK_HEADER(pthread.h,[],AC_MSG_ERROR([pthread.h is required to build this project]))

PTHREAD_LIBS=error
AC_MSG_CHECKING(for old style FreeBSD -pthread flag)
AC_EGREP_CPP(yes,
        [#if defined(__FreeBSD_cc_version) && __FreeBSD_cc_version <= 500001
          yes
        #endif
        ], AC_MSG_RESULT(yes) CFLAGS="$CFLAGS -D_THREAD_SAFE" PTHREAD_LIBS="-pthread",
        AC_MSG_RESULT(no))
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(pthread, pthread_attr_init, PTHREAD_LIBS="-lpthread")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(pthreads, pthread_attr_init, PTHREAD_LIBS="-lpthreads")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_LIB(c_r, pthread_attr_init, PTHREAD_LIBS="-lc_r")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_CHECK_FUNC(pthread_attr_init, PTHREAD_LIBS="")
fi
if test "x$PTHREAD_LIBS" = xerror; then
        AC_MSG_ERROR(*** Unable to locate working posix thread library ***)
fi

AC_SUBST(PTHREAD_LIBS)



##
dnl Use -Wall if we have gcc.
#changequote(,)dnl
#if test "x$GCC" = "xyes"; then
  #case " $CFLAGS " in
  #*[\ \	]-Wall[\ \	]*) ;;
  #*) CFLAGS="$CFLAGS -Wall" ;;
  #esac
#fi
#changequote([,])dnl
##
AC_OUTPUT(Makefile \
extradist/Makefile \
src/Makefile \
src/iteration/Makefile \
src/overload/Makefile \
src/streambuf/Makefile \
src/template/Makefile \
src/threadDeath/Makefile \
src/virtual/Makefile \
docs/Makefile \
docs/html/Makefile \
myproject.spec)
##
##
echo "--- Now run make ---"
echo ""
