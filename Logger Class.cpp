#include "Logger Class.h"
#include "stdafx.h"

#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
// #define DEBUG_DELETE delete(__FILE__, __LINE__ )
#else
#define DEBUG_NEW new
// #define DEBUG_DELETE delete
#endif

#define new DEBUG_NEW
//#define delete DEBUG_DELETE

/**************************************************************
 *
 *   cLog Static Variable Members
 *
 **************************************************************/

FileClass cLOG::File;
bool      cLOG::bEnable = true;
TCHAR     cLOG::szFile[ MAX_PATH ];

/*********************************************
 *  *
 * Logging Facility Default Constructor  *
 *  *
 *  Default to enabled  *
 *  *
 *********************************************/

cLOG::cLOG( ) {
  Enable( true );
}

/*********************************************
 *  *
 * Logging Facility Copy Constructor   *
 *  *
 *  *
 *********************************************/

cLOG::cLOG( cLOG *Right ) {
  *this = *Right;
}

/*********************************************
 *  *
 * Logging Facility Default Constructor  *
 *  Load szFile for writing &  *
 *   set desired log level  *
 *  *
 *  Default to log enabled   *
 *  *
 *********************************************/

cLOG::cLOG( IN char *szFile ) {
  strcpy( this->szFile, szFile );
  OpenLog( );
  Enable( true );
}

/*********************************************
 *  *
 *  Logging Facility Deconstructor   *
 *  *
 *  Cleanup logger and disable  *
 *  *
 *********************************************/

cLOG::~cLOG( ) {
  Enable( false );
  CloseLog( );
}

/*********************************************
 *  *
 *  Logging Facility Enabler  *
 *  *
 *  returns true on successful change  *
 *   (returns true 100% of the time)  *
 *  *
 *********************************************/

bool cLOG::Enable( IN bool bEnableLog ) {
  if ( ( bEnableLog ) && ( !bEnable ) ) {
    OpenLog( );
  } else if ( ( !bEnableLog ) && ( bEnable ) ) {
    CloseLog( );
  }
  return ( bEnable = bEnableLog );
}

/*********************************************
 *  *
 *  Logging Facility Log Openner   *
 *  *
 *  Open szFile & set pointer to EOF  *
 *   returns function success   *
 *  *
 *********************************************/

bool cLOG::OpenLog( void ) {
  if ( !File.IsOpen( ) ) {
    if ( File.Open( szFile, GENERIC_WRITE, FILE_SHARE_READ, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL ) ) {
      File.SetFilePointer64( 0, FILE_END );
      return true;
    }
  } else {
    File.SetFilePointer64( 0, FILE_END );
    return true;
  }
  return false;
}

/*********************************************
 *  *
 *  Logging Facility Log Closser   *
 *   returns success  *
 *  *
 *********************************************/

bool cLOG::CloseLog( void ) {
  return File.Close( );
}

/*********************************************
 *  *
 *  Logging Facility Write Line  *
 *  Variable params accepted  *
 *  *
 *   Log Line Level (iLogLevel)   *
 *   szString (Format String)   *
 *   '...' variable params  *
 *  *
 *   Line formatted as   *
 *  '(Log Level)[ Time ] String '  *
 *  *
 *********************************************/

bool cLOG::Printf( IN char *szString... ) {
  if ( ( szString != NULL ) && ( bEnable ) && ( strlen( szString ) != 0 ) ) {
    __time64_t TimeT;
    _time64( &TimeT );
    struct tm CurrentTime        = *_localtime64( &TimeT );
    char *    szFDate            = _ctime64( &TimeT );
    *( strchr( szFDate, '\n' ) ) = NULL;
    DWORD dwBytes                = File.printf( "[ %s ] ", szFDate );

    va_list valist;
    va_start( valist, szString );
    dwBytes += File.vprintf_remove_chars( "\r\n", szString, valist );
    va_end( valist );
    dwBytes += File.printf( "\r\n" );

    return ( dwBytes - ( 21 + strlen( szFDate ) ) ) > 0;
  }
  return false;
}

/*********************************************
 *  *
 *  Logging Facility Error Logger   *
 *  record error as Emergency  *
 *  *
 *********************************************/

bool cLOG::RecordLastError( void ) {
  TCHAR *szBuffer;
  if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      GetLastError( ),
                      MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                      ( LPTSTR ) &szBuffer,
                      0,
                      NULL ) ) {
    return Printf( szBuffer );
  }
  return false;
}