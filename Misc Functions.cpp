#include "Buddy Pounce.h"
#include "stdafx.h"
#include <Wingdi.h>

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

/********************************************************************************
 *   *
 *  Load & Lock Resource   *
 *   *
 ********************************************************************************/

void LoadCommonControls( void ) {
  INITCOMMONCONTROLSEX icc;
  ZeroMemory( &icc, sizeof( INITCOMMONCONTROLSEX ) );
  icc.dwSize = sizeof( INITCOMMONCONTROLSEX );
  icc.dwICC =
    ICC_DATE_CLASSES | ICC_TAB_CLASSES | ICC_WIN95_CLASSES | ICC_USEREX_CLASSES | ICC_UPDOWN_CLASS;
  InitCommonControlsEx( &icc );
}

TCHAR *LocknLoadResource( HINSTANCE hInst, LPCTSTR lpName, LPCTSTR lpSection, DWORD *dwSize ) {
  HRSRC   hrSrc = FindResource( hInst, lpName, lpSection );
  HGLOBAL hGlob = NULL;
  if ( NULL != hrSrc ) {
    if ( ( hGlob = LoadResource( ( HMODULE ) hInst, hrSrc ) ) ) {
      if ( dwSize )
        *dwSize = SizeofResource( ( HMODULE ) hInst, hrSrc );
      return ( TCHAR * ) LockResource( hGlob );
    }
  }
  if ( dwSize )
    *dwSize = 0;
  return NULL;
}

/********************************************************************************
 *   *
 *   Verify Available Operations for Pointers   *
 *   *
 ********************************************************************************/

bool IsReadable( void *pPtr ) {
  __try {
    char temp = *( ( char * ) pPtr );
    return true;
  } __except ( ( GetExceptionCode( ) == EXCEPTION_ACCESS_VIOLATION )
                 ? ( EXCEPTION_EXECUTE_HANDLER )
                 : ( EXCEPTION_CONTINUE_SEARCH ) ) {
    return false;
  }
}

bool IsWriteable( void *pPtr ) {
  __try {
    *( ( char * ) pPtr ) = *( ( char * ) pPtr );
    return true;
  } __except ( ( GetExceptionCode( ) == EXCEPTION_ACCESS_VIOLATION )
                 ? ( EXCEPTION_EXECUTE_HANDLER )
                 : ( EXCEPTION_CONTINUE_SEARCH ) ) {
    return false;
  }
}

bool IsValid( void *pPtr ) {
  return IsReadable( pPtr ) || IsWriteable( pPtr );
}

/***************************************************************************************************
 *
 *   Parse 3 * MAX_MESSAGE to ~CUT_MESSAGE_AT Characters  (Keep words intact)
 *
 ***************************************************************************************************/

TCHAR *ParseMessageString( TCHAR *szString, TCHAR **szEnd, TCHAR *szBegin ) {
  if ( szEnd == NULL )
    return NULL;
  if ( strlen( szBegin ) > CUT_MESSAGE_AT ) {
    TCHAR *pStart = szBegin + CUT_MESSAGE_AT;
    *szEnd        = pStart;

    while ( ( **szEnd != ' ' ) && ( **szEnd != '\n' ) && ( **szEnd != '\r' ) ) {
      if ( --*szEnd == szString )
        break;
    }
    int iLeftDiff = abs( ( int ) ( pStart - *szEnd ) ) * -1;

    *szEnd = pStart;

    while ( ( **szEnd != ' ' ) && ( **szEnd != '\n' ) && ( **szEnd != '\r' ) ) {
      if ( ++*szEnd == ( szString + strlen( szString ) ) )
        break;
    }
    int iRightDiff = abs( ( int ) ( *szEnd - pStart ) ) * +1;

    if ( ( iLeftDiff <= 25 ) && ( iRightDiff <= 25 ) ) {
      *szEnd = pStart + ( ( abs( iLeftDiff ) < abs( iRightDiff ) ) ? iLeftDiff : iRightDiff );
    } else if ( ( iLeftDiff > 25 ) && ( iRightDiff <= 25 ) ) {
      *szEnd = pStart + iRightDiff;
    } else if ( ( iLeftDiff <= 25 ) && ( iRightDiff > 25 ) ) {
      *szEnd = pStart + iLeftDiff;
    } else {
      *szEnd = pStart;
    }
  } else {
    *szEnd = szBegin + strlen( szBegin );
  }

  return *szEnd;
}

BOOL FormatAndParse( TCHAR *           szMessage,
                     struct message_t *Message,
                     QWORD             qwTimestamp,
                     DWORD &           dwPounceSum,
                     bool &            bPounceEnd ) {
  SYSTEMTIME DateTime = QWORDToSystemTime( qwTimestamp );
  BOOL       bSendOK  = true;
  bool       bIgnore  = false;
  TCHAR *    szEnd;
  TCHAR *    szStart   = szMessage;
  TCHAR *    pPosition = szStart;

  if ( GetCharCount( szMessage, '%' ) ) {
    do {
      if ( ( *pPosition == '%' ) && ( *( pPosition + 1 ) != ' ' ) &&
           ( *( pPosition + 1 ) != '%' ) && ( !bIgnore ) ) {
        TCHAR cNextChar = *( pPosition + 1 );
        switch ( cNextChar ) {
          case 'h': { /* %h -- 24 hour */
            TCHAR szHour[ 3 ];
            itoa( DateTime.wHour, szHour, 10 );
            if ( strlen( szHour ) < 2 ) {
              szHour[ 1 ] = szHour[ 0 ];
              szHour[ 0 ] = '0';
              szHour[ 2 ] = NULL;
            }
            TCHAR cTemp = *( pPosition + 2 );
            strncpy( pPosition, szHour, 2 );
            *( pPosition + 2 ) = cTemp;
            break;
          }
          case 'm': { /* %m -- minutes */
            TCHAR szMin[ 3 ];
            itoa( DateTime.wMinute, szMin, 10 );
            if ( strlen( szMin ) < 2 ) {
              szMin[ 1 ] = szMin[ 0 ];
              szMin[ 0 ] = '0';
              szMin[ 2 ] = NULL;
            }
            TCHAR cTemp = *( pPosition + 2 );
            strncpy( pPosition, szMin, 2 );
            *( pPosition + 2 ) = cTemp;
            break;
          }
          case 'N': { /* %N -- 1-12 Month */
            TCHAR szMonth[ 3 ];
            itoa( DateTime.wMonth, szMonth, 10 );
            TCHAR cTemp = *( pPosition + strlen( szMonth ) );
            strncpy( pPosition, szMonth, strlen( szMonth ) );
            *( pPosition + strlen( szMonth ) ) = cTemp;
            strcpy( ( pPosition + strlen( szMonth ) ), ( pPosition + 2 ) );
            break;
          }
          case 'D': { /* %D -- ?x Day */
            TCHAR szDay[ 3 ];
            itoa( DateTime.wDay, szDay, 10 );
            if ( strlen( szDay ) < 2 ) {
              szDay[ 1 ] = szDay[ 0 ];
              szDay[ 0 ] = '0';
              szDay[ 2 ] = NULL;
            }
            TCHAR cTemp = *( pPosition + 2 );
            strncpy( pPosition, szDay, 2 );
            *( pPosition + 2 ) = cTemp;
            break;
          }
          case 'H': { /* %H -- 12 hour */
            TCHAR szHour[ 3 ];
            int   iHour = DateTime.wHour % 12;
            iHour       = ( iHour == 0 ) ? 12 : iHour;

            itoa( iHour, szHour, 10 );
            TCHAR cTemp = *( pPosition + strlen( szHour ) );
            strncpy( pPosition, szHour, strlen( szHour ) );
            *( pPosition + strlen( szHour ) ) = cTemp;
            strcpy( ( pPosition + strlen( szHour ) ), ( pPosition + 2 ) );
            break;
          }
          case 's': { /* %s -- seconds */
            TCHAR szSecond[ 3 ];
            itoa( DateTime.wSecond, szSecond, 10 );
            if ( strlen( szSecond ) < 2 ) {
              szSecond[ 1 ] = szSecond[ 0 ];
              szSecond[ 0 ] = '0';
              szSecond[ 2 ] = NULL;
            }
            TCHAR cTemp = *( pPosition + 2 );
            strncpy( pPosition, szSecond, 2 );
            *( pPosition + 2 ) = cTemp;
            break;
          }
          case 'n': { /* %n -- Jan-Dec */
            TCHAR *szMonths[] = {
              "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
            TCHAR *szTemp = new TCHAR[ strlen( pPosition + 2 ) + 1 ];

            if ( szTemp ) {
              strcpy( szTemp, ( pPosition + 2 ) );
              strcpy( ( pPosition + 3 ), szTemp );
            }
            delete_ptr( szTemp );

            TCHAR cTemp = *( pPosition + 3 );
            strncpy( pPosition, szMonths[ DateTime.wMonth - 1 ], 3 );
            *( pPosition + 3 ) = cTemp;
            break;
          }
          case 'a': { /* %a -- AM/PM */
            TCHAR *szAMPM[] = { "AM", "PM" };
            TCHAR  cTemp    = *( pPosition + 2 );
            strncpy( pPosition, szAMPM[ ( DateTime.wHour > 12 ) ], 2 );
            *( pPosition + 2 ) = cTemp;
            break;
          }
          case 'y': { /* %y -- YYYY */
            TCHAR szYear[ 5 ];
            itoa( DateTime.wYear, szYear, 10 );
            TCHAR *szTemp = new TCHAR[ strlen( pPosition + 2 ) + 1 ];

            if ( szTemp ) {
              strcpy( szTemp, ( pPosition + 2 ) );
              strcpy( ( pPosition + 4 ), szTemp );
            }

            delete_ptr( szTemp );

            TCHAR cTemp = *( pPosition + 4 );
            strncpy( pPosition, szYear, 4 );
            *( pPosition + 4 ) = cTemp;
            break;
          }
          case 'd': { /* %d -- Sun-Sat */
            TCHAR *szDays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
            TCHAR *szTemp   = new TCHAR[ strlen( pPosition + 2 ) + 1 ];

            if ( szTemp ) {
              strcpy( szTemp, ( pPosition + 2 ) );
              strcpy( ( pPosition + 3 ), szTemp );
            }

            delete_ptr( szTemp );

            TCHAR cTemp = *( pPosition + 3 );
            strncpy( pPosition, szDays[ DateTime.wDayOfWeek ], 3 );
            *( pPosition + 3 ) = cTemp;
            break;
          }
          default:
            break;
        }
      } else if ( *pPosition == '%' ) {
        bIgnore = true;
      } else if ( ( bIgnore == true ) && ( *pPosition != '%' ) ) {
        bIgnore = false;
      }
      ++pPosition;
      szEnd = ( szMessage + strlen( szMessage ) );
    } while ( pPosition != szEnd );
  } else {
    szEnd = ( szMessage + strlen( szMessage ) );
  }

  do {
    if ( ParseMessageString( szMessage, &szEnd, szStart ) ) {
      TCHAR *szParsedMesg = new TCHAR[ ( szEnd - szStart ) + 4 ];
      if ( szParsedMesg ) {
        ZeroMemory( szParsedMesg, sizeof( TCHAR ) * ( ( szEnd - szStart ) + 1 ) );
        Message->text = szParsedMesg;

        strncpy( szParsedMesg, szStart, ( szEnd - szStart ) );

        if ( strlen( szParsedMesg ) == 1 )
          strcat( szParsedMesg, "  " );

        dwPounceSum = CreateCheckSum( szParsedMesg );
        bPounceEnd  = ( ( szMessage + strlen( szMessage ) ) == szEnd );
        bSendOK     = bSendOK && ( !plugin_send( BPGUID, "messageSend", Message ) );
        Sleep( 1 * dwSleep );

        szStart = szEnd;
      }
      delete_ptr( szParsedMesg );
    }
  } while ( szEnd != ( szStart + strlen( szStart ) ) );

  return bSendOK;
}

BOOL SendMesg( struct message_t *Message,
               QWORD             qwTimestamp,
               DWORD &           dwPounceSum,
               bool &            bPounceEnd ) {
  TCHAR *szMessage = Message->text;
  BOOL   bSendOK   = FALSE;

  if ( bSeperateLines ) {
    UINT   uNumVar  = GetCharCount( szPrefixMesg, '%' );
    TCHAR *szString = new TCHAR[ MAX_MESSAGE + ( uNumVar * 4 ) + 3 ];
    bool   bNULL    = false;
    if ( szString ) {
      strcpy( szString, szPrefixMesg );
      bSendOK = FormatAndParse( szString, Message, qwTimestamp, dwPounceSum, bNULL );
    }

    delete_ptr( szString );

    uNumVar  = GetCharCount( szMessage, '%' );
    szString = new TCHAR[ MAX_MESSAGE + ( uNumVar * 4 ) + 3 ];
    if ( szString ) {
      strcpy( szString, szMessage );
      bSendOK = FormatAndParse( szString, Message, qwTimestamp, dwPounceSum, bNULL ) && bSendOK;
    }
    delete_ptr( szString );

    uNumVar  = GetCharCount( szSufixMesg, '%' );
    szString = new TCHAR[ MAX_MESSAGE + ( uNumVar * 4 ) + 3 ];
    if ( szString ) {
      strcpy( szString, szSufixMesg );
      bSendOK =
        FormatAndParse( szString, Message, qwTimestamp, dwPounceSum, bPounceEnd ) && bSendOK;
    }

    delete_ptr( szString );
  } else {
    UINT uNumVar = GetCharCount( szMessage, '%' ) + GetCharCount( szPrefixMesg, '%' ) +
                   GetCharCount( szSufixMesg, '%' );

    TCHAR *szString = new TCHAR[ ( 3 * MAX_MESSAGE ) + ( uNumVar * 4 ) + 3 ];
    if ( szString ) {
      sprintf( szString, "%s %s %s", szPrefixMesg, szMessage, szSufixMesg );
      bSendOK = FormatAndParse( szString, Message, qwTimestamp, dwPounceSum, bPounceEnd );
    }
    delete_ptr( szString );
  }
  bPounceEnd = false;
  return bSendOK;
}

/***************************************************************************************************
 *
 *   Register Event
 *
 ***************************************************************************************************/

void RegEvents( BOOL Load ) {
  if ( bUseExtended ) {
    struct event_action_register_t Event;
    trillianInitialize( Event );

    Event.description = szEventName;
    Event.event       = szEventName;
    Event.event_id    = iEventID;

    if ( Load ) {
      plugin_send( BPGUID, "eventsEventRegister", &Event );
      iEventID = Event.event_id;
    } else {
      plugin_send( BPGUID, "eventsEventUnregister", &Event );
    }
  }
}

/***************************************************************************************************
 *
 *   Send Event notification
 *
 ***************************************************************************************************/

void SendEvent( void ) {
  if ( bUseExtended ) {
    struct event_t Event;
    trillianInitialize( Event );

    Event.description = szEventName;
    Event.type        = szEventName;
    Event.discrete    = 1;
    Event.event_id    = iEventID;

    plugin_send( BPGUID, "eventsGenerate", &Event ) &&
      plugin_send( BPGUID, "eventsExecute", &Event );
  }
}

/***************************************************************************************************
 *
 *   Format Qword to readable string (Last Seen)
 *
 ***************************************************************************************************/

TCHAR *QWORDTimeToString( TCHAR *szLastSeen, QWORD qwLastSeen ) {
  SYSTEMTIME sysTime    = QWORDToSystemTime( qwLastSeen );
  TCHAR *    szDays[]   = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  TCHAR *    szMonths[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

  sprintf( szLastSeen,
           "\n\rLast Seen On\n\r%s %s %d %2d:%2d:%2d %4d\n\r",
           szDays[ sysTime.wDayOfWeek ],
           szMonths[ sysTime.wMonth - 1 ],
           sysTime.wDay,
           sysTime.wHour,
           sysTime.wMinute,
           sysTime.wSecond,
           sysTime.wYear );

  return szLastSeen;
}

/***************************************************************************************************
 *
 *   Qword to System Time
 *
 ***************************************************************************************************/

SYSTEMTIME QWORDToSystemTime( QWORD qwDateTime ) {
  FILETIME ftDateTime;
  ZeroMemory( &ftDateTime, sizeof( FILETIME ) );
  SYSTEMTIME sysDateTime;
  ZeroMemory( &sysDateTime, sizeof( SYSTEMTIME ) );

  ftDateTime.dwLowDateTime  = LOWQUADPART( qwDateTime );
  ftDateTime.dwHighDateTime = HIGHQUADPART( qwDateTime );

  if ( !FileTimeToSystemTime( &ftDateTime, &sysDateTime ) ) {
    ZeroMemory( &sysDateTime, sizeof( SYSTEMTIME ) );
  }
  return sysDateTime;
}

/***************************************************************************************************
 *
 *   System Time to QWORD
 *
 ***************************************************************************************************/

QWORD SystemTimeToQWORD( SYSTEMTIME sysDateTime ) {
  FILETIME ftDateTime;
  ZeroMemory( &ftDateTime, sizeof( FILETIME ) );

  KILLONFAIL( SystemTimeToFileTime( &sysDateTime, &ftDateTime ), NULL, return 0; );

  return MAKEQWORD( ftDateTime.dwLowDateTime, ftDateTime.dwHighDateTime );
}

/***************************************************************************************************
 *
 *   Local Time Wrapper
 *
 ***************************************************************************************************/

SYSTEMTIME GetLocalTime( void ) {
  SYSTEMTIME LocalTime;
  ZeroMemory( &LocalTime, sizeof( SYSTEMTIME ) );
  GetLocalTime( &LocalTime );
  return LocalTime;
}

/***************************************************************************************************
 *
 *   Check Systime between QWORD Time
 *
 ***************************************************************************************************/

BOOL IsInTime( QWORD qwStart, SYSTEMTIME sysTime, QWORD qwEnd ) {
  SYSTEMTIME sysStart = QWORDToSystemTime( qwStart );
  SYSTEMTIME sysEnd   = QWORDToSystemTime( qwEnd );

  TimeStripDate( &sysStart );
  FILETIMEEXT ftStart;
  SystemTimeToFileTime( &sysStart, &ftStart.ft );
  TimeStripDate( &sysEnd );
  FILETIMEEXT ftEnd;
  SystemTimeToFileTime( &sysEnd, &ftEnd.ft );
  TimeStripDate( &sysTime );
  FILETIMEEXT ftTime;
  SystemTimeToFileTime( &sysTime, &ftTime.ft );

  if ( ( sysTime.wHour < sysStart.wHour ) && ( sysTime.wHour < sysEnd.wHour ) ) {
    ftStart.Quad -= DAYINNANO;
  }

  if ( ( sysTime.wHour > sysEnd.wHour ) && ( sysTime.wHour > sysStart.wHour ) ) {
    ftEnd.Quad += DAYINNANO;
  }

  if ( ( CompareFileTime( &ftStart.ft, &ftTime.ft ) <= 0 ) &&
       ( CompareFileTime( &ftEnd.ft, &ftTime.ft ) >= 0 ) ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/***************************************************************************************************
 *
 *   Strip Date from Date/Time (Use Current Date to keep it valid)
 *
 ***************************************************************************************************/

void TimeStripDate( SYSTEMTIME *sysTime ) {
  SYSTEMTIME sysLocalTime;
  ZeroMemory( &sysLocalTime, sizeof( SYSTEMTIME ) );
  GetLocalTime( &sysLocalTime );
  sysTime->wDay       = sysLocalTime.wDay;
  sysTime->wDayOfWeek = sysLocalTime.wDayOfWeek;
  sysTime->wMonth     = sysLocalTime.wMonth;
  sysTime->wYear      = sysLocalTime.wYear;
}

void TimeStripTime( SYSTEMTIME *sysTime ) {
  sysTime->wSecond       = 0;
  sysTime->wMinute       = 0;
  sysTime->wMilliseconds = 0;
  sysTime->wHour         = 12;
}

/***************************************************************************************************
 *
 *   Section Callback
 *
 ***************************************************************************************************/

int BPSectionCallback( int windowID, char *subwindow, char *event, void *data, void *userData ) {
  if ( strcmp( event, "list_leftDoubleClick" ) == 0 ) {
    if ( AddEditDialog(
           hInstance, MAKEINTRESOURCE( IDD_ADD ), HWND_DESKTOP, ( DLGPROC ) AddPounceDlg, NULL ) ) {
      if ( hwPounceWnd != NULL )
        PostMessage( hwPounceWnd, WM_UPDATELIST, 0, 0 );
    }
  }
  return 0;
}

int SectionItemCallback( int windowID, char *subwindow, char *event, void *data, void *userData ) {
  if ( strcmp( event, "list_leftDoubleClick" ) == 0 ) {
    DialogBoxParam( hInstance,
                    MAKEINTRESOURCE( IDD_EDIT ),
                    HWND_DESKTOP,
                    ( DLGPROC ) EditPounceDlg,
                    ( LPARAM ) userData );

    if ( hwPounceWnd != NULL ) {
      PostMessage( hwPounceWnd, WM_UPDATELIST, 0, 0 );
    }
  } else if ( strcmp( event, "menu-select" ) == 0 ) {
    struct menu_entry_t &Menu_Entry = *( struct menu_entry_t * ) data;
    if ( ( ( ( MenuEntry * ) userData )->ItemPounce != NULL ) &&
         ( ( ( MenuEntry * ) userData )->szCommand != NULL ) ) {
      MenuEntry *ItemMenuEntry = ( MenuEntry * ) userData;
      Pounce *   CurrentPounce = ( Pounce * ) ItemMenuEntry->ItemPounce;

      if ( strcmp( ItemMenuEntry->szCommand, szRemoveBP ) == 0 ) {
        EraseINIEntry( CurrentPounce->uEntryID );
        PouncesFindAndErase( CurrentPounce );

        if ( hwPounceWnd != NULL ) {
          PostMessage( hwPounceWnd, WM_UPDATELIST, 0, 0 );
        }
      }
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Section Creation
 *
 ***************************************************************************************************/

BOOL InitBPSection( void ) {
  struct list_entry_t ListEntry;
  trillianListInitialize( ListEntry );

  ListEntry.section  = 1;
  ListEntry.expanded = 1;
  ListEntry.tooltip  = szPluginDesc;
  ListEntry.text     = szPluginTitle;

  ListEntry.real_name = szPluginTitle;
  plugin_send( BPGUID, "listAddEntry", &ListEntry );

  iSectionID = ListEntry.section_id;

  ListEntry.tooltip  = "Add New Pounce";
  ListEntry.text     = "Add New Pounce";
  ListEntry.callback = BPSectionCallback;
  ListEntry.section  = 0;

  ListEntry.left_icons = new struct list_bmp_t;
  trillianInitialize( ( *ListEntry.left_icons ) );

  ListEntry.left_icons->num_states = 1;

  ListEntry.left_icons->bitmap      = new HBITMAP;
  ListEntry.left_icons->bitmap[ 0 ] = hBPSectionImage;

  ListEntry.left_icons->location             = new RECT;
  ListEntry.left_icons->location[ 0 ].left   = 0;
  ListEntry.left_icons->location[ 0 ].top    = 0;
  ListEntry.left_icons->location[ 0 ].bottom = 16;
  ListEntry.left_icons->location[ 0 ].right  = 14;

  ListEntry.num_left_icons = 1;

  BOOL bRet = !plugin_send( BPGUID, "listAddEntry", &ListEntry );

  delete_ptr( ListEntry.left_icons->bitmap );
  delete_ptr( ListEntry.left_icons->location );
  delete_ptr( ListEntry.left_icons );

  trillianListInitialize( ListEntry );
  ListEntry.section_id = iSectionID;
  ListEntry.group      = 1;
  ListEntry.tooltip    = "Pounces";
  ListEntry.text       = "Pounces";

  iParentID = plugin_send( BPGUID, "listAddEntry", &ListEntry );

  if ( iParentID >= 0 ) {
    std::vector< Pounce * >::iterator _iterator;
    for ( _iterator = Pounces.begin( ); _iterator != Pounces.end( ); _iterator++ ) {
      Pounce *PounceItem = *_iterator;
      AddSectionItem( PounceItem );
    }
  }

  return ( bRet && iParentID >= 0 );
}

/***************************************************************************************************
 *
 *   Section Entry Addition
 *
 ***************************************************************************************************/

BOOL AddSectionItem( Pounce *PounceData ) {
  if ( ( iParentID >= 0 ) && ( iSectionID >= 0 ) ) {
    struct list_entry_t ListEntry;
    trillianListInitialize( ListEntry );

    TCHAR szTemp[ MAX_MESSAGE ];
    ZeroMemory( szTemp, sizeof( TCHAR ) * MAX_MESSAGE );

    ListEntry.previous_id    = -1;
    ListEntry.section_id     = iSectionID;
    ListEntry.parent_id      = iParentID;
    ListEntry.unique_id      = -1;
    ListEntry.group          = 0;
    ListEntry.section        = 0;
    ListEntry.inline_editing = 0;

    ListEntry.drag_and_drop = 0;
    ListEntry.expanded      = 1;
    ListEntry.selectable    = 1;

    TCHAR *szString = new TCHAR[ strlen( PounceData->to ) + 1 + strlen( PounceData->mesg ) + 1 ];
    if ( szString ) {
      UINT uConfigLen = LEN( szTemp );

      sprintf( szString, "%s %s", PounceData->to, PounceData->mesg );

      UINT uStrLen = ( UINT ) strlen( szString ) + 1;

      strncpy( szTemp, szString, ( uConfigLen < uStrLen ) ? uConfigLen : uStrLen );
    }
    delete_ptr( szString );

    ListEntry.tooltip = szTemp;
    ListEntry.text    = szTemp;

    ListEntry.callback = SectionItemCallback;
    ListEntry.data     = ( void * ) PounceData;

    ListEntry.menu_entry = new struct menu_entry_t;
    if ( ListEntry.menu_entry ) {
      PounceData->menu_entry = ListEntry.menu_entry;
      trillianInitialize( ( *ListEntry.menu_entry ) );

      ListEntry.menu_entry->callback = SectionItemCallback;
      ListEntry.menu_entry->text     = "Remove Pounce";
      ListEntry.menu_entry->type     = MENU_TEXT;
    }

    PounceData->MenuEntry = new MenuEntry;
    if ( PounceData->MenuEntry ) {
      ZeroMemory( PounceData->MenuEntry, sizeof( MenuEntry ) );

      PounceData->MenuEntry->ItemPounce = PounceData;
      PounceData->MenuEntry->szCommand  = szRemoveBP;

      ListEntry.menu_entry->data = ( void * ) PounceData->MenuEntry;

      PounceData->iUniqueID = plugin_send( BPGUID, "listAddEntry", &ListEntry );
    }
    return ( PounceData->iUniqueID >= 0 );
  } else {
    return FALSE;
  }
}

/***************************************************************************************************
 *
 *   Section Entry Update Text
 *
 ***************************************************************************************************/

BOOL UpdateSectionItemText( Pounce *PounceData ) {
  if ( ( iParentID >= 0 ) && ( iSectionID >= 0 ) && ( PounceData->iUniqueID >= 0 ) ) {
    struct list_entry_t ListEntry;
    trillianListInitialize( ListEntry );

    ListEntry.parent_id = iParentID;
    ListEntry.unique_id = PounceData->iUniqueID;

    TCHAR *szString = new TCHAR[ strlen( PounceData->to ) + 1 + strlen( PounceData->mesg ) + 1 ];
    TCHAR  szTemp[ MAX_MESSAGE ];

    if ( szString ) {

      UINT uConfigLen = LEN( szTemp );

      sprintf( szString, "%s %s", PounceData->to, PounceData->mesg );

      UINT uStrLen = ( UINT ) strlen( szString ) + 1;

      strncpy( szTemp, szString, ( uConfigLen < uStrLen ) ? uConfigLen : uStrLen );
    }
    delete_ptr( szString );

    ListEntry.tooltip = szTemp;
    ListEntry.text    = szTemp;

    return ( !plugin_send( BPGUID, "listUpdateText", &ListEntry ) ) &&
           ( !plugin_send( BPGUID, "listUpdateTooltip", &ListEntry ) );
  }
  return FALSE;
}

/***************************************************************************************************
 *
 *   Section Entry Deletion
 *
 ***************************************************************************************************/

BOOL DelSectionItem( Pounce *PounceData ) {
  if ( ( iParentID >= 0 ) && ( iSectionID >= 0 ) && ( PounceData->iUniqueID >= 0 ) ) {
    struct list_entry_t ListEntry;
    trillianListInitialize( ListEntry );

    ListEntry.parent_id = iParentID;
    ListEntry.unique_id = PounceData->iUniqueID;

    delete_ptr( PounceData->MenuEntry );
    delete_ptr( PounceData->menu_entry );
    if ( plugin_send( MYGUID, "listRemoveEntry", &ListEntry ) == 0 ) {
      PounceData->iUniqueID = -1;
      return TRUE;
    } else {
      return FALSE;
    }
  }
  return FALSE;
}

/***************************************************************************************************
 *
 *   Section Deletion
 *
 ***************************************************************************************************/

BOOL DenitBPSection( void ) {
  if ( ( iParentID >= 0 ) && ( iSectionID >= 0 ) ) {
    struct list_entry_t ListEntry;
    trillianListInitialize( ListEntry );

    ListEntry.section_id = iSectionID;

    std::vector< Pounce * >::iterator _iterator;
    for ( _iterator = Pounces.begin( ); _iterator != Pounces.end( ); _iterator++ ) {
      Pounce *PounceItem = *_iterator;
      DelSectionItem( PounceItem );
    }

    plugin_send( BPGUID, "listRemoveEntry", &ListEntry );

    iSectionID = -1;
    iParentID  = -1;
    return TRUE;
  } else {
    return FALSE;
  }
}

/***************************************************************************************************
 *
 *   Toggle Selection Creation/Deletion
 *
 ***************************************************************************************************/

void ToggleSection( BOOL bSection ) {
  if ( bSection ) {
    if ( iSectionID == -1 ) {
      InitBPSection( );
    }
  } else {
    DenitBPSection( );
  }
}

/***************************************************************************************************
 *
 *   Check User Status
 *
 ***************************************************************************************************/

ContactInfo *GetUser( TCHAR *szUser, TCHAR *szMedium, int iConnID ) {
  std::vector< Contact >::iterator     ContIterator;
  std::vector< Medium >::iterator      MedIterator;
  std::vector< ContactInfo >::iterator CIInfo;
  ContactInfo *                        GlobalContact = NULL;

  foreach ( MedIterator, Mediums ) {
    if ( !strcmp_wrap( MedIterator->szMedium, szMedium ) ) {
      foreach ( ContIterator, MedIterator->Contacts ) {
        if ( !strcmp_wrap( ContIterator->real_name, szUser ) ) {
          foreach ( CIInfo, ContIterator->Config ) {
            if ( iConnID == CIInfo->iConnID ) {
              return &( *CIInfo );
            } else if ( CIInfo->iConnID == -1 ) {
              GlobalContact = &( *CIInfo );
            }
          }
        }
      }
    }
  }
  return GlobalContact;
}

/***************************************************************************************************
 *
 *   Check Connection Name & Status
 *
 ***************************************************************************************************/

TCHAR *IsConnectionUp( int iConnectionID, TCHAR *szMedium ) {
  std::vector< Account >::iterator AccIterator;
  std::vector< Medium >::iterator  MedIterator;

  foreach ( MedIterator, Mediums ) {
    if ( !strcmp_wrap( MedIterator->szMedium, szMedium ) ) {
      foreach ( AccIterator, MedIterator->Accounts ) {
        Account CurrentAccount = *AccIterator;
        if ( CurrentAccount.iConnId == iConnectionID ) {
          return CurrentAccount.name;
        }
      }
    }
  }
  return NULL;
}

int IsConnectionUp( TCHAR *szUser, TCHAR *szMedium ) {
  std::vector< Account >::iterator AccIterator;
  std::vector< Medium >::iterator  MedIterator;

  foreach ( MedIterator, Mediums ) {
    if ( !strcmp_wrap( MedIterator->szMedium, szMedium ) ) {
      foreach ( AccIterator, MedIterator->Accounts ) {
        Account CurrentAccount = *AccIterator;
        if ( !strcmp_wrap( CurrentAccount.name, szUser ) ) {
          return CurrentAccount.iConnId;
        }
      }
    }
  }
  return NULL;
}

/***************************************************************************************************
 *
 *   Broadcast messages
 *
 ***************************************************************************************************/

void MessageCallback(
  int iWindowID, char *szSubWindow, char *szEvent, void *vData, void *vUserData ) {
  struct message_t &Message = *( struct message_t * ) vData;
  if ( strcmp( szEvent, "message_broadcast" ) == 0 ) {
    if ( !strcmp( Message.type, "information_standard" ) ) {
      if ( strstr( szMediumIgnore, Message.medium ) == NULL ) {
        ContactInfo *UserContact = GetUser( Message.name, Message.medium, Message.connection_id );
        if ( UserContact != NULL ) {
          TCHAR *szFrom  = IsConnectionUp( Message.connection_id, Message.medium );
          HWND   hwWinID = FindMeThatWindow( Message.name, Message.medium, szFrom );
          if ( ( UserContact->Status == Offline ) && ( strlen( szFrom ) > 0 ) ) {
            if ( ( ( UserContact->bConfirmed ) && ( UserContact->hwLastWinID == hwWinID ) ) ||
                 ( !bConfirmOMS ) ) {
              if ( stricmp( Message.text, szTrillianOfflineMessage ) == 0 ) {
                if ( strstr( szMediumIgnore, Message.medium ) == NULL ) {
                  strcpy( Message.text, szBuddyPounceOfflineMessage );
                }
              } else if ( stricmp( Message.text, szTrillianOfflineMessage_Error ) == 0 ) {
                if ( strstr( szMediumIgnore, Message.medium ) == NULL ) {
                  strcpy( Message.text, szBuddyPounceOfflineMessage_Error );
                }
              }
            }
          }
        }
      }
    } else if ( strcmp( Message.type, "outgoing_privateMessage" ) == 0 ) {
      if ( strstr( szMediumIgnore, Message.medium ) == NULL ) {
        ContactInfo *UserContact = GetUser( Message.name, Message.medium, Message.connection_id );
        if ( UserContact != NULL ) {
          TCHAR *szFrom     = IsConnectionUp( Message.connection_id, Message.medium );
          HWND   hwWinID    = FindMeThatWindow( Message.name, Message.medium, szFrom );
          DWORD  dwCheckSum = CreateCheckSum( Message.text );
          if ( ( UserContact->Status == Offline ) && ( ( szFrom && strlen( szFrom ) > 0 ) ) &&
               ( dwCheckSum != UserContact->dwPounceSum ) ) {

            UserContact->bConfirmed =
              ( bConfirmOMS ) ? ( ( !UserContact->bConfirmed ) ? ( MyMessage( ) != FALSE ) : false )
                              : true;

            if ( UserContact->bConfirmed ) {

              Pounce *NewPounce = new Pounce;
              ZeroMemory( NewPounce, sizeof( Pounce ) );

              if ( NewPounce ) {
                NewPounce->qwTimestamp = SystemTimeToQWORD( GetLocalTime( ) );
                strcpy( NewPounce->from, szFrom );
                strcpy( NewPounce->to, Message.name );
                strcpy( NewPounce->medium, Message.medium );
                StripHTML( Message.text, NewPounce->mesg );

                AddPounce( NewPounce, true );

                if ( hwPounceWnd != NULL ) {
                  PostMessage( hwPounceWnd, WM_UPDATELIST, 0, 0 );
                }
              }
            }
          } else if ( UserContact->Status > Offline ) {
            UserContact->qwLastMesg = SystemTimeToQWORD( GetLocalTime( ) );
            if ( ( UserContact->dwPounceSum == dwCheckSum ) && ( UserContact->bPounceEnd ) &&
                 ( iOperations[ iWindowOp ] && ( hwWinID ) ) &&
                 ( UserContact->hwLastWinID != hwWinID ) ) { // if window was NOT open before
              DWORD dwID = 0;
              CreateThread( NULL, 0, RunWindowOperation, hwWinID, 0, &dwID );
            } else if ( ( dwCheckSum !=
                          UserContact->dwPounceSum ) ) { // if last message was NOT a pounce
              UserContact->hwLastWinID = hwWinID;        // Save window handle
            }
          }
        }
      }
    }
  }
}

DWORD WINAPI RunWindowOperation( LPVOID lpParam ) {
  HWND hwWinID = ( HWND ) lpParam;
  Sleep( 500 );
  switch ( sOpMethod[ iWindowOp ] ) {
    case 0:
      SendMessage( hwWinID, WM_SYSCOMMAND, iOperations[ iWindowOp ], 0 );
      break;
    case 1:
      ShowWindow( hwWinID, iOperations[ iWindowOp ] );
      break;
  }
  ExitThread( 0 );
}

/**************************************************************
*
  (  UserContact->Status >= Away   ) &&   // if User is online
 (   szFrom && ( strlen(szFrom) > 0 )   ) &&   // if sent by valid connection
*
**************************************************************/

/***************************************************************************************************
 *
 *   Start Broadcast messages
 *
 ***************************************************************************************************/

BOOL StopMesgBroadcast( void ) {
  if ( iBroadcastID >= 0 ) {
    struct message_broadcast_t MessageBroadcast;
    trillianInitialize( MessageBroadcast );

    MessageBroadcast.broadcast_id = iBroadcastID;

    if ( plugin_send( BPGUID, "messageBroadcastDisable", &MessageBroadcast ) == 0 ) {
      iBroadcastID = -1;
      return TRUE;
    }
  }
  return FALSE;
}

/***************************************************************************************************
 *
 *   Stop Broadcast messages
 *
 ***************************************************************************************************/

BOOL StartMesgBroadcast( void ) {
  if ( iBroadcastID <= 0 ) {
    struct message_broadcast_t MessageBroadcast;
    trillianInitialize( MessageBroadcast );

    MessageBroadcast.callback = ( ttkCallback ) MessageCallback;

    iBroadcastID = plugin_send( BPGUID, "messageBroadcastEnable", &MessageBroadcast );
    return ( iBroadcastID > 0 );
  }
  return TRUE;
}

/***************************************************************************************************
 *
 *   Toggle Broadcast Messages
 *
 ***************************************************************************************************/

void ToggleMesgDetection( BOOL bBroadcast ) {
  if ( bUseExtended ) {
    if ( bBroadcast ) {
      if ( iBroadcastID <= 0 ) {
        StartMesgBroadcast( );
      }
    } else {
      StopMesgBroadcast( );
    }
  }
}

/**************************************************************
 *
 *   Add Pounce
 *
 **************************************************************/

bool AddPounce( Pounce *InsPounce, bool bINI ) {
  if ( bINI )
    bINI = ( AddINIEntry( InsPounce ) == TRUE );
  Pounces.push_back( InsPounce );
  PounceEvent.Set( );
  return ( AddSectionItem( InsPounce ) == TRUE ) && bINI;
}

/**************************************************************
 *
 *   Get MD5 Sum from string: Caller responsible to delete returned string
 *
 **************************************************************/

TCHAR *GetMD5( TCHAR *szString ) {
  if ( szString ) {
    TCHAR *szRet = new TCHAR[ 34 ];
    ZeroMemory( szRet, sizeof( TCHAR ) * 34 );
    MD5Class MD5( szString, szRet );
    return szRet;
  }
  return NULL;
}
