#include "Buddy Pounce.h"
#include "stdafx.h"

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

BOOL EraseINIEntry( UINT uEntry ) {
  UINT  uSize = ( UINT ) Pounces.size( );
  TCHAR szTemp[ MAX_MESSAGE ];
  if ( uSize > 0 ) {
    TCHAR *szSection = BPNum( uSize );
    WritePrivateProfileString( szSection, NULL, NULL, szConfig );
    WritePrivateProfileString( "GLOBAL", "Pounces", _ultoa( uSize - 1, szTemp, 10 ), szConfig );

    std::vector< Pounce * >::iterator _iterator = Pounces.begin( );
    for ( UINT uCount = 0; _iterator != Pounces.end( ); _iterator++ ) {
      Pounce *PounceEntry = *_iterator;
      if ( uEntry != PounceEntry->uEntryID ) {
        PounceEntry->uEntryID = ++uCount;
        UpdateINIEntry( PounceEntry );
      }
    }
    delete_ptr( szSection );
  }
  return FALSE;
}

BOOL UpdateINIEntry( Pounce *UpdatePounce ) {
  TCHAR sz_bCurrentSession[ 2 ];
  TCHAR sz_bPause[ 2 ];
  TCHAR sz_qwSendOn[ 65 ];
  TCHAR sz_qwTimestamp[ 65 ];

  UINT uEntry = UpdatePounce->uEntryID;

  ConvertFromNewLine( UpdatePounce->mesg );

  TCHAR *szSection = BPNum( uEntry );
  itoa( UpdatePounce->bCurrentSession, sz_bCurrentSession, 10 );
  itoa( UpdatePounce->bPause, sz_bPause, 10 );
  _ui64toa( UpdatePounce->qwSendOn, sz_qwSendOn, 10 );
  _ui64toa( UpdatePounce->qwTimestamp, sz_qwTimestamp, 10 );

  if ( ( WritePrivateProfileString( szSection, "To", UpdatePounce->to, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "From", UpdatePounce->from, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "Medium", UpdatePounce->medium, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "Mesg", UpdatePounce->mesg, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "CurrentSession", sz_bCurrentSession, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "SendOn", sz_qwSendOn, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "Pause", sz_bPause, szConfig ) ) &&
       ( WritePrivateProfileString( szSection, "TimeStamp", sz_qwTimestamp, szConfig ) ) ) {

    ConvertToNewLine( UpdatePounce->mesg );
    delete_ptr( szSection );
    return TRUE;
  }
  delete_ptr( szSection );
  ConvertToNewLine( UpdatePounce->mesg );
  return FALSE;
}

BOOL AddINIEntry( Pounce *NewPounce ) {
  TCHAR sz_bCurrentSession[ 2 ];
  TCHAR sz_bPause[ 2 ];
  TCHAR sz_qwSendOn[ 65 ];
  TCHAR sz_qwTimestamp[ 65 ];
  TCHAR szTemp[ MAX_MESSAGE ];

  UINT uMax = GetPrivateProfileInt( "GLOBAL", "Pounces", 0, szConfig ) + 1;

  ConvertFromNewLine( NewPounce->mesg );

  if ( WritePrivateProfileString( "GLOBAL", "Pounces", _ultoa( uMax, szTemp, 10 ), szConfig ) ) {
    TCHAR *szSection = BPNum( uMax );
    itoa( NewPounce->bCurrentSession, sz_bCurrentSession, 10 );
    itoa( NewPounce->bPause, sz_bPause, 10 );
    _ui64toa( NewPounce->qwSendOn, sz_qwSendOn, 10 );
    _ui64toa( NewPounce->qwTimestamp, sz_qwTimestamp, 10 );

    if ( ( WritePrivateProfileString( szSection, "To", NewPounce->to, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "From", NewPounce->from, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "Medium", NewPounce->medium, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "Mesg", NewPounce->mesg, szConfig ) ) &&
         ( WritePrivateProfileString(
           szSection, "CurrentSession", sz_bCurrentSession, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "SendOn", sz_qwSendOn, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "TimeStamp", sz_qwTimestamp, szConfig ) ) &&
         ( WritePrivateProfileString( szSection, "Pause", sz_bPause, szConfig ) ) ) {

      delete_ptr( szSection );
      ConvertToNewLine( NewPounce->mesg );
      NewPounce->uEntryID = uMax;
      return TRUE;
    }
    delete_ptr( szSection );
  }
  ConvertToNewLine( NewPounce->mesg );
  return FALSE;
}

Pounce ReadINIEntry( UINT uEntry ) {
  TCHAR sz_bCurrentSession[ 2 ];
  TCHAR sz_bPause[ 2 ];
  TCHAR sz_qwSendOn[ 65 ];
  TCHAR sz_qwTimestamp[ 65 ];

  Pounce BP;
  ZeroMemory( &BP, sizeof( Pounce ) );

  TCHAR *szSection = BPNum( uEntry );

  _ui64toa( SystemTimeToQWORD( GetLocalTime( ) ), sz_qwTimestamp, 10 );

  GetPrivateProfileString( szSection, "From", "", BP.from, LEN( BP.from ), szConfig );
  GetPrivateProfileString( szSection, "Medium", "", BP.medium, LEN( BP.medium ), szConfig );
  GetPrivateProfileString( szSection, "To", "", BP.to, LEN( BP.to ), szConfig );
  GetPrivateProfileString( szSection, "Mesg", "", BP.mesg, LEN( BP.mesg ), szConfig );
  GetPrivateProfileString(
    szSection, "CurrentSession", "0", sz_bCurrentSession, LEN( sz_bCurrentSession ), szConfig );
  GetPrivateProfileString( szSection, "SendOn", "0", sz_qwSendOn, LEN( sz_qwSendOn ), szConfig );
  GetPrivateProfileString(
    szSection, "TimeStamp", sz_qwTimestamp, sz_qwTimestamp, LEN( sz_qwTimestamp ), szConfig );
  GetPrivateProfileString( szSection, "Pause", "0", sz_bPause, LEN( sz_bPause ), szConfig );

  ConvertToNewLine( BP.mesg );

  BP.bCurrentSession = ( atoi( sz_bCurrentSession ) == 1 );
  BP.bPause          = ( atoi( sz_bPause ) == 1 );
  BP.qwSendOn        = _atoi64( sz_qwSendOn );
  BP.qwTimestamp     = _atoi64( sz_qwTimestamp );
  BP.uEntryID        = uEntry;

  delete_ptr( szSection );
  return BP;
}

UINT ReadProfile( void ) {
  QWORD qwMax     = GetPrivateProfileInt( "GLOBAL", "Pounces", 0, szConfig );
  QWORD qwRealMax = 0;
  for ( QWORD qwCount = 1; qwCount <= qwMax; qwCount++ ) {
    TCHAR szString[ MAX_MESSAGE ];
    ZeroMemory( szString, sizeof( TCHAR ) * MAX_MESSAGE );
    TCHAR *szSection = BPNum( ( UINT ) qwCount );
    if ( GetPrivateProfileString(
           szSection, "From", szString, szString, LEN( szString ), szConfig ) ) {
      Pounce *NewPounce = new Pounce;
      *NewPounce        = ReadINIEntry( ( UINT ) qwCount );
      if ( ( strlen( NewPounce->from ) > 0 ) && ( strlen( NewPounce->to ) > 0 ) &&
           ( strlen( NewPounce->medium ) > 0 ) && ( strlen( NewPounce->mesg ) > 0 ) ) {
        AddPounce( NewPounce, false );
        if ( qwCount != ++qwRealMax ) {
          EraseINIEntry( NewPounce->uEntryID );
          NewPounce->uEntryID = ( UINT )( qwRealMax );
          UpdateINIEntry( NewPounce );
        }
      }
    }
    delete_ptr( szSection );
  }
  if ( qwMax != qwRealMax ) {
    TCHAR szTemp[ MAX_MESSAGE ];
    ZeroMemory( szTemp, sizeof( TCHAR ) * MAX_MESSAGE );
    WritePrivateProfileString(
      "GLOBAL", "Pounces", _ultoa( ( UINT ) qwRealMax, szTemp, 10 ), szConfig );
  }
  return ( UINT ) qwMax;
}
