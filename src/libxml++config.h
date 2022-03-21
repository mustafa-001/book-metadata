#ifndef _LIBXMLPP_CONFIG_H
#define _LIBXMLPP_CONFIG_H 1

#if 0
#include <glibmmconfig.h>
#endif 

#ifdef GLIBMM_CONFIGURE
/* compiler feature tests that are used during compile time and run-time
   by libxml++ only. */
#undef LIBXMLCPP_EXCEPTIONS_ENABLED
#endif /* GLIBMM_CONFIGURE */

#ifdef GLIBMM_MSC
#define LIBXMLCPP_EXCEPTIONS_ENABLED 1
#endif /* GLIBMM_MSC */

#ifdef GLIBMM_DLL
  #if defined(LIBXMLPP_BUILD) && defined(_WINDLL)
    // Do not dllexport as it is handled by gendef on MSVC
    #define LIBXMLPP_API
  #elif !defined(LIBXMLPP_BUILD)
    #define LIBXMLPP_API __declspec(dllimport)
  #else
    /* Build a static library */
    #define LIBXMLPP_API
  #endif /* LIBXMLPP_BUILD - _WINDLL */
#else
  #define LIBXMLPP_API
#endif /* GLIBMM_DLL */

#define LIBXMLPP_API
#define LIBXMLCPP_EXCEPTIONS_ENABLED 1

#endif /* _LIBXMLPP_CONFIG_H */

