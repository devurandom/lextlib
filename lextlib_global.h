/*
	This file is part of lextlib.
	Copyright (C) 1992-2007  Trolltech ASA.
	Copyright (C) 2005-2009  Warzone Resurrection Project

	lextlib is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	lextlib is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with lextlib; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/*! \file lextlib_global.h
 *  \brief Platform detection, workarounds and compat fixes
 *
 *  OS and CC detection code shamelessly stolen from Qt4 (Qt/qglobal.h) by Dennis.
 *  This has been stripped down, feel free to add checks as you need them.
 */

#ifndef WZGLOBAL_H
#define WZGLOBAL_H


#if defined(HAVE_CONFIG_H)
#  include "config.h"
#elif defined(__MACOSX__)
#  include "config-macosx.h"
#endif


/* ---- Platform detection ---- */


/*
   The operating system, must be one of: (LUAX_OS_x)

     DARWIN             - Darwin OS (synonym for LUAX_OS_MAC)
     OS2                - OS/2
     OS2EMX             - XFree86 on OS/2 (not PM)
     WIN32              - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
     CYGWIN             - Cygwin
     SOLARIS            - Sun Solaris
     HPUX               - HP-UX
     ULTRIX             - DEC Ultrix
     LINUX              - Linux
     FREEBSD            - FreeBSD
     GNU_kFREEBSD       - GNU/kFreeBSD
     NETBSD             - NetBSD
     OPENBSD            - OpenBSD
     BSDI               - BSD/OS
     IRIX               - SGI Irix
     OSF                - HP Tru64 UNIX
     SCO                - SCO OpenServer 5
     UNIXWARE           - UnixWare 7, Open UNIX 8
     AIX                - AIX
     HURD               - GNU Hurd
     DGUX               - DG/UX
     RELIANT            - Reliant UNIX
     DYNIX              - DYNIX/ptx
     QNX                - QNX
     QNX6               - QNX RTP 6.1
     LYNX               - LynxOS
     BSD4               - Any BSD 4.4 system
     UNIX               - Any UNIX BSD/SYSV system
*/

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define LUAX_OS_DARWIN
#  define LUAX_OS_BSD4
#  ifdef __LP64__
#    define LUAX_OS_DARWIN64
#  else
#    define LUAX_OS_DARWIN32
#  endif
#elif defined(__CYGWIN__)
#  define LUAX_OS_CYGWIN
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define LUAX_OS_OS2EMX
#  else
#    define LUAX_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define LUAX_OS_WIN32
#  define LUAX_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define LUAX_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define LUAX_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define LUAX_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define LUAX_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define LUAX_OS_ULTRIX
#elif defined(sinix)
#  define LUAX_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define LUAX_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define LUAX_OS_FREEBSD
#  define LUAX_OS_BSD4
#elif defined(__FreeBSD_kernel__) && !defined(__FreeBSD__)
   /* We're running a non-FreeBSD system with a FreeBSD kernel. Find out what C
    * library we're using to detect the system we're running on. */
#  include <stdlib.h>
#  if defined(__GLIBC__)
     /* We're running GNU/kFreeBSD */
#    define LUAX_OS_GNU_kFREEBSD
#  endif
#elif defined(__NetBSD__)
#  define LUAX_OS_NETBSD
#  define LUAX_OS_BSD4
#elif defined(__OpenBSD__)
#  define LUAX_OS_OPENBSD
#  define LUAX_OS_BSD4
#elif defined(__bsdi__)
#  define LUAX_OS_BSDI
#  define LUAX_OS_BSD4
#elif defined(__sgi)
#  define LUAX_OS_IRIX
#elif defined(__osf__)
#  define LUAX_OS_OSF
#elif defined(_AIX)
#  define LUAX_OS_AIX
#elif defined(__Lynx__)
#  define LUAX_OS_LYNX
#elif defined(__GNU__)
#  define LUAX_OS_HURD
#elif defined(__DGUX__)
#  define LUAX_OS_DGUX
#elif defined(__QNXNTO__)
#  define LUAX_OS_QNX6
#elif defined(__QNX__)
#  define LUAX_OS_QNX
#elif defined(_SEQUENT_)
#  define LUAX_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define LUAX_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define LUAX_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define LUAX_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define LUAX_OS_INTEGRITY
#elif defined(__MAKEDEPEND__)
#else
#  error "Warzone has not been tested on this OS. Please contact warzone-dev@gna.org"
#endif /* LUAX_OS_x */

#if defined(LUAX_OS_WIN32) || defined(LUAX_OS_WIN64)
#  define LUAX_OS_WIN
#endif /* LUAX_OS_WIN32 */

#if defined(LUAX_OS_DARWIN)
#  define LUAX_OS_MAC /* LUAX_OS_MAC is mostly for compatibility, but also more clear */
#  define LUAX_OS_MACX /* LUAX_OS_MACX is only for compatibility.*/
#  if defined(LUAX_OS_DARWIN64)
#     define LUAX_OS_MAC64
#  elif defined(LUAX_OS_DARWIN32)
#     define LUAX_OS_MAC32
#  endif
#endif /* LUAX_OS_DARWIN */

#if defined(LUAX_OS_MSDOS) || defined(LUAX_OS_OS2) || defined(LUAX_OS_WIN)
#  undef LUAX_OS_UNIX
#elif !defined(LUAX_OS_UNIX)
#  define LUAX_OS_UNIX
#endif /* LUAX_OS_* */


/*
   The compiler, must be one of: (LUAX_CC_x)

     MSVC     - Microsoft Visual C/C++, Intel C++ for Windows
     GNU      - GNU C++
     INTEL    - Intel C++ for Linux, Intel C++ for Windows
     TINYC    - Fabrice Bellard's Tiny C Compiler

   Should be sorted most to least authoritative.
*/

#if defined(_MSC_VER)
#  define LUAX_CC_MSVC
/* All ISO C89 compliant compilers _should_ define the macro __STDC__, MSVC
 * however is known _not_ to do this, so work around that here. */
#  if !defined(__STDC__)
#    define __STDC__ 1
#  endif
/* Visual C++.Net issues for _MSC_VER >= 1300 */
#  if _MSC_VER >= 1300
#    define LUAX_CC_MSVC_NET
#  endif
/* Intel C++ disguising as Visual C++: the `using' keyword avoids warnings */
#  if defined(__INTEL_COMPILER)
#    define LUAX_CC_INTEL
#  endif
/* x64 does not support mmx intrinsics on windows */
#  if (defined(ZS_OS_WIN64) && defined(_M_X64))
#    undef ZS_HAVE_SSE
#    undef ZS_HAVE_SSE2
#    undef ZS_HAVE_MMX
#    undef ZS_HAVE_3DNOW
#  endif

#elif defined(__GNUC__)
#  define LUAX_CC_GNU
#  if defined(__MINGW32__)
#    define LUAX_CC_MINGW
#  endif
#  if defined(__INTEL_COMPILER)
/* Intel C++ also masquerades as GCC 3.2.0 */
#    define LUAX_CC_INTEL
#  endif

#elif defined(__TINYC__)
#  define LUAX_CC_TINYC

#else
#  error "Warzone has not been tested on this compiler. Please contact warzone-dev@gna.org"
#endif /* LUAX_CC_x */


/*
   The window system, must be one of: (LUAX_WS_x)

     MACX     - Mac OS X
     WIN32    - Windows
     X11      - X Window System
     QNX      - QNX
*/

#if defined(_WIN32_X11_)
#  define LUAX_WS_X11

#elif defined(LUAX_OS_WIN32)
#  define LUAX_WS_WIN32
#  if defined(LUAX_OS_WIN64)
#    define LUAX_WS_WIN64
#  endif

#elif defined(LUAX_OS_MAC)
#  define LUAX_WS_MAC
#  define LUAX_WS_MACX
#  if defined(LUAX_OS_MAC64)
#    define LUAX_WS_MAC64
#  elif defined(LUAX_OS_MAC32)
#    define LUAX_WS_MAC32
#  endif

#elif defined(LUAX_OS_QNX)
#  define LUAX_WS_QNX

#elif defined(LUAX_OS_UNIX)
#  define LUAX_WS_X11

#else
#  error "Warzone has not been tested on this window system. Please contact warzone-dev@gna.org"
#endif /* LUAX_WS_x */

#if defined(LUAX_WS_WIN16) || defined(LUAX_WS_WIN32)
#  define LUAX_WS_WIN
#endif


/*
   The supported C standard, must be one of: (LUAX_Cxx)

     99       - ISO/IEC 9899:1999 / C99
*/
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define LUAX_C99
#endif /* LUAX_Cxx */

/*
   The supported C++ standard, must be one of: (LUAX_CXXxx)

     98       - ISO/IEC 14882:1998 / C++98
*/
#if defined(__cplusplus)
# define LUAX_CXX98
#endif /* LUAX_CXXxx */


/*
   Convenience macros to test the versions of gcc.
   Copied from glibc's features.h.
*/
#if defined(LUAX_CC_GNU) && defined __GNUC__ && defined __GNUC_MINOR__
#  define LUAX_CC_GNU_PREREQ(maj, min) \
          ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#  define LUAX_CC_GNU_PREREQ(maj, min) 0
#endif


/*
   Convenience macros to test the versions of icc.
*/
#if defined(LUAX_CC_INTEL) && defined __ICC
#  define LUAX_CC_INTEL_PREREQ(maj, min) \
          ((__ICC) >= ((maj) * 100) + (min))
#else
#  define LUAX_CC_INTEL_PREREQ(maj, min) 0
#endif




/* ---- Declaration attributes ---- */


/*!
 * \def LUAX_DECL_DEPRECATED
 *
 * The LUAX_DECL_DEPRECATED macro can be used to trigger compile-time warnings
 * with newer compilers when deprecated functions are used.
 *
 * For non-inline functions, the macro gets inserted at front of the
 * function declaration, right before the return type:
 *
 * \code
 * LUAX_DECL_DEPRECATED void deprecatedFunctionA();
 * LUAX_DECL_DEPRECATED int deprecatedFunctionB() const;
 * \endcode
 *
 * For functions which are implemented inline,
 * the LUAX_DECL_DEPRECATED macro is inserted at the front, right before the return
 * type, but after "static", "inline" or "virtual":
 *
 * \code
 * LUAX_DECL_DEPRECATED void deprecatedInlineFunctionA() { .. }
 * virtual LUAX_DECL_DEPRECATED int deprecatedInlineFunctionB() { .. }
 * static LUAX_DECL_DEPRECATED bool deprecatedInlineFunctionC() { .. }
 * inline LUAX_DECL_DEPRECATED bool deprecatedInlineFunctionD() { .. }
 * \endcode
 *
 * You can also mark whole structs or classes as deprecated, by inserting the
 * LUAX_DECL_DEPRECATED macro after the struct/class keyword, but before the
 * name of the struct/class:
 *
 * \code
 * class LUAX_DECL_DEPRECATED DeprecatedClass { };
 * struct LUAX_DECL_DEPRECATED DeprecatedStruct { };
 * \endcode
 *
 * \note
 * Description copied from KDE4, code copied from Qt4.
 *
 */
#if LUAX_CC_GNU_PREREQ(3,2) || LUAX_CC_INTEL_PREREQ(10,0)
#  define LUAX_DECL_DEPRECATED __attribute__((__deprecated__))
#elif defined(LUAX_CC_MSVC) && defined(LUAX_CC_MSVC_NET)
#  define LUAX_DECL_DEPRECATED __declspec(deprecated)
#else
#  define LUAX_DECL_DEPRECATED
#endif


/*! \def LUAX_DECL_FORMAT
 * GCC: "The format attribute specifies that a function takes printf, scanf, strftime or strfmon
 *       style arguments which should be type-checked against a format string."
 */
#if LUAX_CC_GNU_PREREQ(2,5) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_FORMAT(archetype, string_index, first_to_check) \
          __attribute__((__format__(archetype, string_index, first_to_check)))
#else
#  define LUAX_DECL_FORMAT(archetype, string_index, first_to_check)
#endif


/*!
 * \def LUAX_DECL_NORETURN
 * "A few standard library functions, such as abort and exit, cannot return. GCC knows this
 *  automatically. Some programs define their own functions that never return.
 *  You can declare them noreturn to tell the compiler this fact."
 */
#if LUAX_CC_GNU_PREREQ(2,5) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_NORETURN __attribute__((__noreturn__))
#else
#  define LUAX_DECL_NORETURN
#endif


/*!
 * \def LUAX_DECL_CONST
 * GCC: "Many functions do not examine any values except their arguments, and have no effects
 *       except the return value. Basically this is just slightly more strict class than
 *       the pure attribute below, since function is not allowed to read global memory."
 */
#if LUAX_CC_GNU_PREREQ(2,5) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_CONST __attribute__((__const__,__warn_unused_result__))
#else
#  define LUAX_DECL_CONST
#endif


/*!
 * \def LUAX_DECL_PURE
 * GCC: "Many functions have no effects except the return value and their return value depends
 *       only on the parameters and/or global variables. Such a function can be subject to
 *       common subexpression elimination and loop optimization just as an arithmetic operator
 *       would be."
 */
#if LUAX_CC_GNU_PREREQ(2,96) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_PURE __attribute__((__pure__))
#else
#  define LUAX_DECL_PURE
#endif


/*!
 * \def LUAX_DECL_UNUSED
 * GCC: "This attribute, attached to a function, means that the function is meant to be possibly
 *       unused. GCC will not produce a warning for this function."
 */
#if LUAX_CC_GNU_PREREQ(3,2) || LUAX_CC_INTEL_PREREQ(10,0)
#  define LUAX_DECL_UNUSED __attribute__((__unused__))
#else
#  define LUAX_DECL_UNUSED
#endif


/*!
 * \def LUAX_DECL_WARN_UNUSED_RESULT
 * GCC: "The warn_unused_result attribute causes a warning to be emitted if a caller of the
 *       function with this attribute does not use its return value. This is useful for
 *       functions where not checking the result is either a security problem or always a bug,
 *       such as realloc."
 */
#if defined(LUAX_CC_GNU) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
#  define LUAX_DECL_WARN_UNUSED_RESULT
#endif


/*! \def LUAX_DECL_MAY_ALIAS
 * GCC: "Accesses to objects with types with this attribute are not subjected to type-based alias
 *       analysis, but are instead assumed to be able to alias any other type of objects,
 *       just like the char type. See -fstrict-aliasing for more information on aliasing issues."
 */
#if LUAX_CC_GNU_PREREQ(3,3) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_MAY_ALIAS __attribute__((__may_alias__))
#else
#  define LUAX_DECL_MAY_ALIAS
#endif


/*!
 * \def LUAX_DECL_RESTRICT
 * Apply the "restrict" keyword found in the C99 revision of the standard.
 * The compiler may assume that the memory referenced by a "restrict" pointer is not aliased
 * by any other pointer. Thus this forms the opposite of LUAX_DECL_MAY_ALIAS.
 */
#if defined(LUAX_C99) && LUAX_CC_GNU_PREREQ(4,1) && !defined(LUAX_CC_INTEL)
#  define LUAX_DECL_RESTRICT restrict
#elif defined(LUAX_CC_MSVC) && defined(LUAX_CC_MSVC_NET)
#  define LUAX_DECL_RESTRICT __restrict
#else
#  define LUAX_DECL_RESTRICT
#endif


/*! \def LUAX_DECL_THREAD
 * Declares a variable to be local to the running thread, and not shared between threads.
 */
#if defined(LUAX_CC_GNU) || defined(LUAX_CC_INTEL)
#  define LUAX_DECL_THREAD __thread
#elif defined(LUAX_CC_MSVC)
#  define LUAX_DECL_THREAD __declspec(thread)
#else
#  error "Thread local storage attribute required"
#endif


/*! \def LUAX_ASSERT_STATIC_STRING
 * Asserts that the given string is statically allocated.
 */
#if defined(__cplusplus)
#  include <typeinfo>
#  define LUAX_ASSERT_STATIC_STRING(_var) assert(typeid(_var) == typeid(char[]))
#elif defined(LUAX_CC_GNU) || defined(LUAX_CC_INTEL)
#  define LUAX_ASSERT_STATIC_STRING(_var) assert(__builtin_types_compatible_p(typeof(_var), char[]))
#else
#  define LUAX_ASSERT_STATIC_STRING(_var) (void)(_var)
#endif


/* ---- Platform specific setup ---- */


#if defined(LUAX_OS_WIN)
#  if defined(LUAX_CC_MINGW)
#    include <unistd.h>
#    include <sys/param.h>
#    include <w32api.h>
#    define _WIN32_IE IE5
/* Required for alloca */
#    include <malloc.h>

#  elif defined(LUAX_CC_MSVC)
#    if defined(_DEBUG)
#      define DEBUG
#      define _CRTDBG_MAP_ALLOC
#      include <stdlib.h>
#      include <crtdbg.h>
#    endif /* _DEBUG */
#  endif /* LUAX_CC_* */

#  define WIN32_LEAN_AND_MEAN
#  define WIN32_EXTRA_LEAN
#  include <windows.h>

#  if defined(LUAX_CC_MSVC)
/*   notify people we are disabling these warning messages. */
#    pragma message (" *** Warnings 4018,4100,4127,4204,4244,4267,4389 have been squelched. ***")
#    pragma warning (disable : 4018) /* Shut up: '>' : signed/unsigned mismatch */
#    pragma warning (disable : 4100) /* Shut up: unreferenced formal parameter (FIXME) */
#    pragma warning (disable : 4127) /* Shut up: conditional expression is constant (eg. "while(0)") */
#    pragma warning (disable : 4204) /* Shut up: non-constant aggregate initializer */
#    pragma warning (disable : 4244) /* Shut up: conversion from 'float' to 'int', possible loss of data */
#    pragma warning (disable : 4267) /* Shut up: conversion from 'size_t' to 'type', possible loss of data */
#    pragma warning (disable : 4389) /* Shut up: '==' : signed/unsigned mismatch */

#    define strcasecmp _stricmp
#    define strncasecmp _strnicmp
#    define inline __inline
#    define alloca _alloca
#    define fileno _fileno

#    define isnan _isnan
#    define isfinite _finite

#    define PATH_MAX MAX_PATH
#  endif /* LUAX_CC_MSVC */

/* Make sure that PATH_MAX is large enough to use as the size for return
 * buffers for Windows API calls
 */
#  if (PATH_MAX < MAX_PATH)
#    undef PATH_MAX
#    define PATH_MAX MAX_PATH
#  endif

#elif defined(LUAX_OS_UNIX)
#  include <unistd.h>
#  include <alloca.h>
#endif /* LUAX_OS_* */


#if !defined(LUAX_C99)
#	if !defined(va_copy)
/**
 * Implements the interface of the C99 macro va_copy such that we can use it on
 * non-C99 systems as well.
 *
 * This implementation assumes that va_list is just a pointer to the stack
 * frame of the variadic function. This is by far the most common setup, though
 * it might not always work.
 */
#		define va_copy(dest, src) (void)((dest) = (src))
#	endif /* !va_copy */
#	if !defined(inline)
#		define inline __inline
#	endif /* !inline */
#endif /* !LUAX_C99 */


#endif /* WZGLOBAL_H */
