#include "Buddy Pounce.h"
#include "stdafx.h"

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

/***************************************************************************************************
 *
 *   Decode Status to OnlineStatus
 *
 ***************************************************************************************************/

OnlineStatus DecodeStatus( const TCHAR *szStatus ) {
  if ( !stricmp( szStatus, szOfflinestr ) ) {
    return Offline;
  } else if ( !stricmp( szStatus, szInvisiblestr ) ) {
    return Invisible;
  } else if ( !stricmp( szStatus, szDNDstr ) ) {
    return DND;
  } else if ( !stricmp( szStatus, szOnlinestr ) ) {
    return Online;
  } else {
    return Away;
  }
}

/***************************************************************************************************/

DWORD CreateCheckSum( TCHAR *szString ) {
  DWORD dwSum = 0;
  while ( ( *szString != '\0' ) && ( szString ) ) {
    dwSum = ( ( dwSum ) &01 ) ? ( dwSum >> 1 ) + 0x8000 : dwSum >> 1;
    dwSum += *szString;
    szString++;
  }
  return dwSum;
}

UINT GetCharCount( TCHAR *szString, TCHAR cChar ) {
  UINT uCount = 0;
  do {
    if ( *szString == cChar )
      uCount++;
    szString++;
  } while ( ( szString != NULL ) && ( *szString != NULL ) );
  return uCount;
}

UINT GetDigits( DWORD dwValue ) {
  UINT uDigits = 1;
  UINT uPos    = 1;
  while ( ( dwValue % uPos ) ) {
    uPos *= 10;
    uDigits++;
  }
  return uDigits;
}

TCHAR *BPNum( UINT uNum ) {
  DWORD  dwLen  = 2 + GetDigits( uNum ) + 1;
  TCHAR *szTemp = new TCHAR[ dwLen + 1 ];
  if ( szTemp ) {
    ZeroMemory( szTemp, sizeof( TCHAR ) * dwLen );

    strcpy_wrap( szTemp, "BP" );
    _ultoa( uNum, ( szTemp + 2 ), 10 );
  }
  return szTemp;
}

int strcmp_wrap( const char *left, const char *right ) {
  if ( left == NULL && right == NULL ) {
    return 0;
  } else if ( ( left == NULL ) || ( !( IsValid( ( void * ) left ) ) ) ) {
    return -1;
  } else if ( ( right == NULL ) || ( !( IsValid( ( void * ) right ) ) ) ) {
    return 1;
  } else {
    return strcmp( left, right );
  }
}

char *strcpy_wrap( char *left, const char *right ) {
  if ( ( ( left != NULL ) && ( right != NULL ) ) &&
       ( ( IsValid( ( void * ) left ) ) && ( IsValid( ( void * ) right ) ) ) ) {
    return strcpy( left, right );
  }
  return NULL;
}

BOOL stralnum( char *str ) {
  while ( ( *str != NULL ) && ( str != NULL ) ) {
    if ( ( !isalnum( *str ) ) && ( *str != '@' ) ) {
      return FALSE;
    }
    str++;
  }
  return TRUE;
}

TCHAR *StripNewLine( TCHAR *szString ) {
  TCHAR *szNewString = new TCHAR[ strlen( szString ) + 1 ];
  TCHAR *szFormated  = szNewString;
  TCHAR *szConvert   = szString;

  while ( ( *szConvert != NULL ) && ( szConvert != NULL ) ) {
    if ( *szConvert == '\n' ) {
      *szFormated = ' ';
      szFormated++;
    } else if ( *szConvert != '\r' ) {
      *szFormated = *szConvert;
      szFormated++;
    }

    szConvert++;
  }

  *szFormated = NULL;
  strcpy( szString, szNewString );
  delete_ptr( szNewString );
  return szString;
}

TCHAR *ConvertFromNewLine( TCHAR *szString ) {
  TCHAR *szNewString = new TCHAR[ strlen( szString ) + 1 ];
  TCHAR *szFormated  = szNewString;
  TCHAR *szConvert   = szString;

  while ( ( *szConvert != NULL ) && ( szConvert != NULL ) ) {
    if ( *szConvert == '\n' ) {
      *szFormated = 1;
    } else if ( *szConvert == '\r' ) {
      *szFormated = 2;
    } else {
      *szFormated = *szConvert;
    }
    szConvert++;
    szFormated++;
  }

  *szFormated = NULL;
  strcpy( szString, szNewString );
  delete_ptr( szNewString );
  return szString;
}

TCHAR *ConvertToNewLine( TCHAR *szString ) {
  if ( szString == NULL )
    return NULL;
  DWORD  dwStrLen    = ( DWORD ) strlen( szString );
  TCHAR *szNewString = new TCHAR[ dwStrLen + 1 ];
  TCHAR *szFormated  = szNewString;
  TCHAR *szConvert   = szString;

  while ( ( *szConvert != NULL ) && ( szConvert != NULL ) ) {
    if ( *szConvert == 1 ) {
      *szFormated = '\n';
    } else if ( *szConvert == 2 ) {
      *szFormated = '\r';
    } else {
      *szFormated = *szConvert;
    }
    szConvert++;
    szFormated++;
  }

  *szFormated = NULL;
  strcpy( szString, szNewString );
  delete_ptr( szNewString );
  return szString;
}

TCHAR *StrRevChr( TCHAR *szString, TCHAR tcFind ) {
  TCHAR *szRevString = szString;
  while ( *( szRevString + 1 ) != NULL )
    szRevString++;

  while ( szRevString != szString ) {
    if ( szRevString[ 0 ] == tcFind )
      break;
    szRevString--;
  }
  return szRevString;
}

BOOL StripHTML( TCHAR *szSrc, TCHAR *szDest ) {
  TCHAR *szSrcStart  = szSrc;
  TCHAR *szDestStart = szDest;
  bool   bCopy       = true;

  do {
    if ( *szSrc == '<' ) {
      if ( ( ( toupper( *( szSrc + 1 ) ) == 'B' ) && ( toupper( *( szSrc + 2 ) ) == 'R' ) &&
             ( *( szSrc + 3 ) == '>' ) ) ||
           ( ( toupper( *( szSrc + 1 ) ) == 'P' ) && ( *( szSrc + 2 ) == '>' ) ) ) {
        *szDest = '\r';
        szDest++;
        *szDest = '\n';
        szDest++;
      }
      bCopy = false;
    } else if ( *szSrc == '>' ) {
      bCopy = true;
    } else if ( bCopy == true ) {
      *szDest = *szSrc;
      szDest++;
    }
    szSrc++;
  } while ( ( *szSrc != NULL ) && ( szSrc != NULL ) );

  *szDest = NULL;
  return ( strlen( szDest ) > 0 );
}

TCHAR *StrRemove( TCHAR *szSrc, TCHAR *szDest, TCHAR cChar ) {
  TCHAR *szSrcStart  = szSrc;
  TCHAR *szDestStart = szDest;

  do {
    if ( *szSrc != cChar ) {
      *szDest = *szSrc;
      szDest++;
    }
    szSrc++;
  } while ( ( *szSrc != NULL ) && ( szSrc != NULL ) );

  *szDest = NULL;
  return szDestStart;
}
