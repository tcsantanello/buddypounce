#ifndef _LOGGER_CLASS_
#define _LOGGER_CLASS_

#include "File Class.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#ifndef WRITE_LOG
#define WRITE_LOG( x )                                                                             \
  if ( x != NULL )                                                                                 \
  x->Printf
#endif

class cLOG {
  /*********************************************************************
   *
   *  Log Class Member Variables
   *
   *********************************************************************/

 private:
  static FileClass File;
  static bool      bEnable;
  static TCHAR     szFile[ MAX_PATH ];

  /*********************************************************************
   *
   *  Log Class Member Functions
   *
   *********************************************************************/

 public:
  cLOG( );
  cLOG( cLOG * );
  cLOG( IN char * );
  ~cLOG( );

  inline bool IsEnabled( void ) { return bEnable; }
  bool        Enable( IN bool );
  bool        OpenLog( void );
  bool        CloseLog( void );

  bool Printf( IN char *... );

  bool  RecordLastError( void );
  cLOG &operator=( cLOG &Right ) { return *this; }
};

#endif