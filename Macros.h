#pragma once

#define LEN( x ) sizeof( x ) / sizeof( TCHAR )
#define MAKEQWORD( low, high )                                                                     \
  ( ( QWORD )( ( DWORD )( ( low ) &0xffffffff ) ) |                                                \
    ( ( QWORD )( ( DWORD )( ( high ) &0xffffffff ) ) << 32 ) )
#define LOWQUADPART( x ) ( ( DWORD )( ( x ) &0xffffffff ) )
#define HIGHQUADPART( x ) ( ( DWORD )( ( x ) >> 32 ) )
#define delete_ptr( pointer )                                                                      \
  if ( pointer )                                                                                   \
    delete pointer;                                                                                \
  pointer = NULL;
//#define delete_ptr(pointer) do { delete pointer; pointer=NULL; } while (0);

#define foreach( var, array ) for ( var = array.begin( ); var != array.end( ); var++ )

#define MyMessage( )                                                                               \
  ( BOOL ) DialogBox(                                                                              \
    hInstance, MAKEINTRESOURCE( IDD_CONFIRM ), HWND_DESKTOP, ( DLGPROC ) ConfirmDialog )

#define stdfind( x, val ) std::find( x.begin( ), x.end( ), val )

#define IsPluginValid( ) ( DonateCheckSum == CreateCheckSum( ( TCHAR * ) szDonate ) )

#define KILLONFAIL( opbool, szMesg, Failop )                                                       \
                                                                                                   \
  if ( !( opbool ) ) {                                                                             \
    if ( szMesg )                                                                                  \
      MessageBox( NULL, szMesg, "(Buddy Pounce) Error:  ", MB_OK );                                \
    if ( GetLastError( ) != ERROR_SUCCESS ) {                                                      \
      ProcessError( );                                                                             \
    }                                                                                              \
    Failop;                                                                                        \
  }
