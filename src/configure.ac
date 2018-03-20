AC_INIT(MATE, 1.0)
AC_PREFIX_DEFAULT([.])

AC_PROG_CC([gcc g++ cc])
AC_PROG_CXX([[g++ gcc cc]])
AC_LANG([C++])

function test_papi {
  if test "x$PAPI_INC_DIR" == "x" ; then
    AC_MSG_WARN([PAPI include directory not specified])
  fi

  if test "x$PAPI_LIB_DIR" == "x" ; then
    AC_MSG_WARN([PAPI library directory not specified])
  fi
}

function test_dyn {
  if test "x$DYNINST_INC_DIR" == "x" ; then
    AC_MSG_WARN([Dyninst include directory not specified])
  fi

  if test "x$DYNINST_LIB_DIR" == "x" ; then
    AC_MSG_WARN([Dyninst library directory not specified])
  fi
}

function test_libdwarf {
  if test "x$LIBDWARF_INC_DIR" == "x" ; then
    AC_MSG_WARN([Libdwarf include directory not specified])
  fi

  if test "x$LIBDWARF_LIB_DIR" == "x" ; then
    AC_MSG_WARN([Libdwarf library directory not specified])
  fi
}

function test_libelf {
  if test "x$LIBELF_INC_DIR" == "x" ; then
    AC_MSG_WARN([Libelf include directory not specified])
  fi

  if test "x$LIBELF_LIB_DIR" == "x" ; then
    AC_MSG_WARN([Libelf library directory not specified])
  fi
}

function test_libiberty {
  if test "x$LIBIBERTY_INC_DIR" == "x" ; then
    AC_MSG_WARN([Libiberty include directory not specified])
  fi

  if test "x$LIBIBERTY_LIB_DIR" == "x" ; then
    AC_MSG_WARN([Libiberty library directory not specified])
  fi
}

function test_libdw {
  if test "x$LIBDW_LIB_DIR" == "x" ; then
    AC_MSG_WARN([Libdw library directory not specified])
  fi
}


############################################################
#                         ARGS                             #
############################################################

# PAPI
AC_ARG_WITH(papi,
            [AS_HELP_STRING([--with-papi],[PAPI directory (must contain ./lib and ./include)])],
            [PAPI_INC_DIR="${withval}/include";
            PAPI_LIB_DIR="${withval}/lib";],)

AC_ARG_WITH(papi-incdir,
            [AS_HELP_STRING([--with-papi-incdir],[PAPI include directory])],
            [PAPI_INC_DIR="${withval}";],)

AC_ARG_WITH(papi-libdir,
            [AS_HELP_STRING([--with-papi-libdir],[PAPI library directory])],
            [PAPI_LIB_DIR="${withval}";],)

# Libiberty ARGS
AC_ARG_WITH(libiberty,
            [AS_HELP_STRING([--with-libiberty],[LIBIBERTY directory (must contain ./lib and ./include)])],
            [LIBIBERTY_INC_DIR="${withval}/include";
            LIBIBERTY_LIB_DIR="${withval}/lib";],)

AC_ARG_WITH(libiberty-libdir,
            [AS_HELP_STRING([--with-libiberty-libdir],[LIBIBERTY library directory])],
            [LIBIBERTY_LIB_DIR="${withval}"],)

AC_ARG_WITH(libiberty-incdir,
            [AS_HELP_STRING([--with-libiberty-incdir],[LIBIBERTY include directory])],
            [LIBIBERTY_INC_DIR="${withval}";],)


# Libelf ARGS
AC_ARG_WITH(libelf,
            [AS_HELP_STRING([--with-libelf],[LIBELF directory (must contain ./lib and ./include)])],
            [LIBELF_INC_DIR="${withval}/include";
            LIBELF_LIB_DIR="${withval}/lib";],)

AC_ARG_WITH(libelf-incdir,
            [AS_HELP_STRING([--with-libelf-incdir],[LIBELF include directory])],
            [LIBELF_INC_DIR="${withval}";],)

AC_ARG_WITH(libelf-libdir,
            [AS_HELP_STRING([--with-libelf-libdir],[LIBELF library directory])],
            [LIBELF_LIB_DIR="${withval}";],)



# Libdwarf ARGS
AC_ARG_WITH(libdwarf,
            [AS_HELP_STRING([--with-libdwarf],[LIBDWARF directory (must contain ./lib and ./include)])],
            [LIBDWARF_INC_DIR="${withval}/include";
            LIBDWARF_LIB_DIR="${withval}/lib";],)

AC_ARG_WITH(libdwarf-incdir,
            [AS_HELP_STRING([--with-libdwarf-incdir],[LIBDWARF include directory])],
            [LIBDWARF_INC_DIR="${withval}";],)

AC_ARG_WITH(libdwarf-libdir,
            [AS_HELP_STRING([--with-libdwarf-libdir],[LIBDWARF library directory])],
            [LIBDWARF_LIB_DIR="${withval}";],)


## Libdw ARG
##AC_ARG_WITH(libdw-libdir,
##            [AS_HELP_STRING([--with-libdw-libdir],[LIBDW include directory])],
##            [LIBDW_LIB_DIR="${withval}";],)


# Dyninst ARGS 
AC_ARG_WITH(dyninst, 	
	[AS_HELP_STRING([--with-dyninst],[DYNINST directory (must contain ./lib and ./include])],
	[DYNINST_LIB_DIR="${withval}/lib"
	 DYNINST_INC_DIR="${withval}/include"],[])

AC_ARG_WITH(dyninst-libdir,
	[AS_HELP_STRING([--with-dyninst-libdir],[Directory to look for dyninst libraries])],
	[DYNINST_LIB_DIR="${withval}"])

AC_ARG_WITH(dyninst-incdir,
	[AS_HELP_STRING([--with-dyninst-incdir],[Directory to look for dyninst headers])],
	[DYNINST_INC_DIR="${withval}"],)

############################################################
#                        FLAGS                             #
############################################################

# --------------------- Dyninst -------------------------- # 
if test "x$DYNINST_INC_DIR" != "x" ; then
  CXXFLAGS="-I${DYNINST_INC_DIR} ${CFLAGS}"
fi

if test "x$DYNINST_LIB_DIR" != "x" ; then
  LDFLAGS="-L${DYNINST_LIB_DIR} ${LDFLAGS}"
fi
# -------------------------------------------------------- #
# ----------------------- PAPI --------------------------- # 

if test "x$PAPI_INC_DIR" != "x" ; then
   CXXFLAGS="-I${PAPI_INC_DIR} ${CXXFLAGS}"
fi

if test "x$PAPI_LIB_DIR" != "x" ; then
   LDFLAGS="-L${PAPI_LIB_DIR} ${LDFLAGS}"
fi

# -------------------------------------------------------- #
# ----------------------- Libs --------------------------- # 

if test "x$LIBDWARF_INC_DIR" != "x" ; then
   CXXFLAGS="-I${LIBDWARF_INC_DIR} ${CXXFLAGS}"
fi

if test "x$LIBDWARF_LIB_DIR" != "x" ; then
   LDFLAGS="-L${LIBDWARF_LIB_DIR} ${LDFLAGS}"
fi

if test "x$LIBELF_INC_DIR" != "x" ; then
   CXXFLAGS="-I${LIBELF_INC_DIR} ${CXXFLAGS}"
fi

if test "x$LIBELF_LIB_DIR" != "x" ; then
   LDFLAGS="-L${LIBELF_LIB_DIR} ${LDFLAGS}"
fi

if test "x$LIBIBERTY_INC_DIR" != "x" ; then
   CXXFLAGS="-I${LIBIBERTY_INC_DIR} ${CXXFLAGS}"
fi

if test "x$LIBIBERTY_LIB_DIR" != "x" ; then
   LDFLAGS="-L${LIBIBERTY_LIB_DIR} ${LDFLAGS}"
fi

if test "x$LIBDW_LIB_DIR" != "x" ; then
   LDFLAGS="-L${LIBDW_LIB_DIR} ${LDFLAGS}"
fi

############################################################
#                      CHECKING                            #
############################################################

# Checking headers for Dyninst
AC_CHECK_HEADER([BPatch.h], [ 
		# Check if Dyninst is version 5.0+ if BPatch_process.h is found that means so
	AC_CHECK_HEADER([BPatch_process.h],
    	[AC_SUBST([DYNINST_VER], [5])],
    	[AC_SUBST([DYNINST_VER], [4])],
    	[// Do not use default includes])],
	[test_dyn; AC_MSG_ERROR([Cannot find DyninstAPI library include files.])],
	[// Do not use default includes])

# Checking Libelf and Libiberty headers
AC_CHECK_HEADER(libelf.h,[],[],[// Do not use default includes])
AC_CHECK_HEADER(libiberty.h,[],[],[// Do not use default includes])
AC_CHECK_HEADER(papiStdEventDefs.h,[],[AC_MSG_ERROR([Cannot find Papi include files.])],[// Do not use default includes])

# Checking libraries. Beware of the order:
AC_CHECK_LIB([papi], [PAPI_accum], 
             [], [test_papi; AC_MSG_ERROR([PAPI library not found])])

AC_CHECK_LIB([elf], [elf_getshdrstrndx],
            [],[test_libelf; AC_MSG_ERROR([elf library not found])])

AC_CHECK_LIB([iberty], [cplus_demangle],
            [],[test_libiberty; AC_MSG_ERROR([iberty library not found])])

#AC_CHECK_LIB([dw], [dwarf_begin],
#            [], [test_libdw; AC_MSG_ERROR([libdw library not found: libdw.so must be in the system])], [-ldwarf])

AC_CHECK_LIB([dwarf], [dwarf_next_cu_header_c],
            [],[test_libdwarf; AC_MSG_ERROR([dwarf library not found])])

AC_CHECK_LIB([dyninstAPI], [_init],
            [],[AC_MSG_ERROR([dyninstAPI library not found])])

AC_CHECK_LIB([common], [cplus_demangle_fill_name],
            [],[AC_MSG_ERROR([common library not found])])

AC_CHECK_LIB([symtabAPI], [getSymtabReaderFactory],
            [],[AC_MSG_ERROR([symtabAPI library not found])])

############################################################

AC_SUBST([LIBS], ["${LIBS}"])
AC_SUBST([SHELL], [$SHELL])
AC_SUBST([ARCH], [$(uname -m)])

AC_SUBST([DYNINST_IDIR], ["${DYNINST_INC_DIR}"])
AC_SUBST([DYNINST_LDIR], ["${DYNINST_LIB_DIR}"])

AC_SUBST([LDFLAGS], ["${LDFLAGS}"])
AC_SUBST([CXXFLAGS], ["${CXXFLAGS}"])

AC_CONFIG_FILES([make.configure])
AC_OUTPUT()
