#include "Buddy Pounce.h"
#include "stdafx.h"

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

BOOL SendPounce( Pounce *CurrentPounce, ContactInfo &CurrentInfo, TCHAR *szMedium ) {
  QWORD qwThreshold   = ( dwThreshold * NANOSECONDS );
  QWORD qwDelay       = ( dwDelay * NANOSECONDS );
  QWORD qwCurrent     = SystemTimeToQWORD( GetLocalTime( ) );
  QWORD qwOnlineDiff  = qwCurrent - CurrentInfo.qwOnline;
  QWORD qwLastMsgDiff = qwCurrent - CurrentInfo.qwLastMesg;
  bool  bDeleteItem   = false;

  if ( ( qwOnlineDiff >= qwDelay ) && ( qwLastMsgDiff >= qwThreshold ) ) {
    if ( ( strcmp( CurrentPounce->medium, "MSN" ) ==
           0 ) && // MSN Medium is tricky with repeated messages
         ( qwOnlineDiff <
           ( 10 * NANOSECONDS ) ) ) { // if they are to close the messages get ignored
      return FALSE;
    }
    struct message_t message;
    trillianInitialize( message );
    message.connection_id = CurrentInfo.iConnID;
    message.medium        = szMedium;
    message.name          = CurrentPounce->to;
    message.type          = "outgoing";
    message.text          = "";

    if ( plugin_send( BPGUID, "messageSend", &message ) == 0 ) {
      message.text = CurrentPounce->mesg;
      if ( SendMesg( &message,
                     CurrentPounce->qwTimestamp,
                     CurrentInfo.dwPounceSum,
                     CurrentInfo.bPounceEnd ) ) {
        CurrentInfo.qwLastMesg = SystemTimeToQWORD( GetLocalTime( ) );
        EraseINIEntry( CurrentPounce->uEntryID );
        DelSectionItem( CurrentPounce );
        bDeleteItem = true;
      }
    }
  }
  return bDeleteItem;
}

void ScanPounces( void ) {
  std::vector< Contact >::iterator     ContIterator;
  std::vector< Pounce * >::iterator    PounceIt;
  std::vector< Account >::iterator     AccIterator;
  std::vector< Medium >::iterator      MedIterator;
  std::vector< ContactInfo >::iterator InfoIterator;

  WRITE_LOG( Logger )( "Scanning Pounces" );
  foreach ( PounceIt, Pounces ) {
    bool bPounceSent = false;

    TCHAR *szMesg = GetMD5( ( *PounceIt )->mesg );
    WRITE_LOG( Logger )( "Checking Pounce: %s", szMesg );

    if ( ( *PounceIt )->bPause )
      continue;
    foreach ( MedIterator, Mediums ) {
      if ( !strcmp_wrap( ( *PounceIt )->medium, MedIterator->szMedium ) ) {
        foreach ( ContIterator, MedIterator->Contacts ) {
          if ( !strcmp_wrap( ( *PounceIt )->to, ContIterator->real_name ) ) {
            foreach ( InfoIterator, ContIterator->Config ) {
              if ( InfoIterator->Status != Offline ) {
                if ( ( ( bSendOnLogin ) && ( InfoIterator->bLoggedIn ) ) ||
                     ( ( !bSendOnLogin ) ) ) {
                  foreach ( AccIterator, MedIterator->Accounts ) {
                    if ( ( ( !bSendOnline ) && ( AccIterator->Status >= Away ) ) ||
                         ( ( bSendOnline ) && ( AccIterator->Status == Online ) ) ) {
                      if ( ( !strcmp_wrap( ( *PounceIt )->from, AccIterator->name ) ) &&
                           ( InfoIterator->iConnID == AccIterator->iConnId ) ) {

                        TCHAR *szContact = GetMD5( ContIterator->real_name );
                        WRITE_LOG( Logger )( "Found Contact for %s: %s", szMesg, szContact );
                        delete_ptr( szContact );

                        BOOL bSendOK = FALSE;
                        if ( 0 == ( *PounceIt )->qwSendOn ) {
                          bSendOK =
                            SendPounce( ( *PounceIt ), ( *InfoIterator ), MedIterator->szMedium );
                        } else {
                          SYSTEMTIME CurrTime = GetLocalTime( );
                          SYSTEMTIME SendOn   = QWORDToSystemTime( ( *PounceIt )->qwSendOn );
                          TimeStripTime( &CurrTime );
                          TimeStripTime( &SendOn );

                          if ( ( *PounceIt )->bSendAfterOk ) {
                            if ( SystemTimeToQWORD( CurrTime ) >= SystemTimeToQWORD( SendOn ) ) {
                              bSendOK = SendPounce(
                                ( *PounceIt ), ( *InfoIterator ), MedIterator->szMedium );
                            }
                          } else {
                            if ( SystemTimeToQWORD( CurrTime ) == SystemTimeToQWORD( SendOn ) ) {
                              bSendOK = SendPounce(
                                ( *PounceIt ), ( *InfoIterator ), MedIterator->szMedium );
                            }
                          }
                        }
                        if ( bSendOK ) {
                          WRITE_LOG( Logger )( "Sent: %s", szMesg );
                          delete_ptr( ( *PounceIt ) );
                          Pounces.erase( PounceIt );
                          --PounceIt;

                          if ( hwPounceWnd != NULL )
                            PostMessage( hwPounceWnd, WM_UPDATELIST, 0, 0 );

                          SendEvent( );

                          PounceEvent.Reset( );
                          break;
                        } else {
                          WRITE_LOG( Logger )( "(NOTICE) Error Sending: %s", szMesg );
                        }
                      }
                    }
                    if ( bPounceSent )
                      break;
                  }
                }
              }
              if ( bPounceSent )
                break;
            }
          }
          if ( bPounceSent )
            break;
        }
      }
    }
    delete_ptr( szMesg );
  }
}

DWORD WINAPI BuddyThread( LPVOID lpParameter ) {
  PounceEvent.Create( TRUE, FALSE, "BPounce" );
  ReadProfile( );
  KILLONFAIL( ( ( HANDLE ) PounceEvent ), "Error Creating BPEvent.", CloseThreads.Set( );
              ExitThread( 0 ); );
  while ( WaitForSingleObject( ( HANDLE ) CloseThreads, 10 ) != WAIT_OBJECT_0 ) {
    if ( WaitForSingleObject( ( HANDLE ) PounceEvent, 10 ) == WAIT_OBJECT_0 ) {
      if ( ( ( bUseTime ) && ( IsInTime( qwStartTime, GetLocalTime( ), qwEndTime ) ) ) ||
           ( !bUseTime ) ) {
        ScanPounces( );
      }
      Sleep( dwSleep * iInterval );
    }
  }
  PounceEvent.CloseHandle( );
  ExitThread( 0 );
}

DWORD WINAPI BuddyCheck( LPVOID lpParam ) {
  ConnectEvent.Create( TRUE, TRUE, "BPConnection" );
  ContactEvent.Create( TRUE, TRUE, "BPContact" );
  SectionEvent.Create( TRUE, TRUE, "BPSection" );
  HANDLE hEvents[] = { ( HANDLE ) ConnectEvent, ( HANDLE ) ContactEvent, ( HANDLE ) SectionEvent };
  DWORD  dwCount   = sizeof( hEvents ) / sizeof( HANDLE );

  KILLONFAIL( ( ( HANDLE ) ConnectEvent && ( HANDLE ) ContactEvent && ( HANDLE ) SectionEvent ),
              "Error Creating Necessary Events!",
              CloseThreads.Set( );
              ContactEvent.CloseHandle( );
              ConnectEvent.CloseHandle( );
              SectionEvent.CloseHandle( );
              return -1; );

  while ( WaitForSingleObject( ( HANDLE ) CloseThreads, 10 ) != WAIT_OBJECT_0 ) {
    if ( WaitForMultipleObjects( dwCount, hEvents, TRUE, 10 ) == WAIT_OBJECT_0 ) {
      KILLONFAIL( ( PrepConnectionEnum( ( ttkCallback ) GetConnectionList_Enum ) == 0 ),
                  "Error Running ConnectionEnum",
                  CloseThreads.Set( );
                  ContactEvent.CloseHandle( );
                  ConnectEvent.CloseHandle( );
                  SectionEvent.CloseHandle( );
                  return -1; );
      if ( bUseExtended ) {
        KILLONFAIL(
          ( PrepSectionEnum( ( ttkCallback ) GetSections_Enum, ( void * ) GetContacts_Enum ) == 0 ),
          "Error Running ConnectionEnum",
          CloseThreads.Set( );
          ContactEvent.CloseHandle( );
          ConnectEvent.CloseHandle( );
          SectionEvent.CloseHandle( );
          return -1; );
      } else {
        KILLONFAIL( ( PrepContactEnum( ( ttkCallback ) GetContacts_Enum ) == 0 ),
                    "Error Running Contact Enum",
                    CloseThreads.Set( );
                    ContactEvent.CloseHandle( );
                    ConnectEvent.CloseHandle( );
                    SectionEvent.CloseHandle( );
                    return -1; );
      }
      Sleep( dwSleep );
    }
  }
  if ( WaitForMultipleObjects( dwCount, hEvents, TRUE, INFINITE ) == WAIT_OBJECT_0 ) {
    ContactEvent.CloseHandle( );
    ConnectEvent.CloseHandle( );
    SectionEvent.CloseHandle( );
  }
  ExitThread( 0 );
}

void PrefsDialog_InitColumns( HWND hWnd ) {
  HWND hwListView = GetDlgItem( hWnd, IDC_LIST );

  LVCOLUMN lvColumn;
  ZeroMemory( &lvColumn, sizeof( lvColumn ) );

  lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

  lvColumn.iSubItem = 0;
  lvColumn.pszText  = "To";
  lvColumn.cx       = 75;
  ListView_InsertColumn( hwListView, 0, &lvColumn );

  lvColumn.iSubItem = 1;
  lvColumn.pszText  = "From";
  lvColumn.cx       = 75;
  ListView_InsertColumn( hwListView, 1, &lvColumn );

  lvColumn.iSubItem = 2;
  lvColumn.pszText  = "Protocol";
  lvColumn.cx       = 60;
  ListView_InsertColumn( hwListView, 2, &lvColumn );

  lvColumn.iSubItem = 3;
  lvColumn.pszText  = "Message";
  lvColumn.cx       = 118;
  ListView_InsertColumn( hwListView, 3, &lvColumn );

  ListView_SetExtendedListViewStyle( hwListView,
                                     ListView_GetExtendedListViewStyle( hwListView ) |
                                       LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP );
}

void PrefsDialog_InitItems( HWND hWnd ) {
  LVITEM lvItem;
  ZeroMemory( &lvItem, sizeof( lvItem ) );
  lvItem.mask = LVIF_TEXT;

  ListView_DeleteAllItems( GetDlgItem( hWnd, IDC_LIST ) );
  std::vector< Pounce * >::iterator _iterator;

  UINT  uCount = 0;
  TCHAR szMessage[ MAX_MESSAGE ];
  for ( _iterator = Pounces.begin( ); _iterator != Pounces.end( ); _iterator++, uCount++ ) {
    Pounce *CurrentPounce = *_iterator;
    lvItem.iItem          = uCount;

    lvItem.iSubItem = 0;
    lvItem.pszText  = CurrentPounce->to;
    ListView_InsertItem( GetDlgItem( hWnd, IDC_LIST ), &lvItem );

    lvItem.iSubItem = 1;
    lvItem.pszText  = CurrentPounce->from;
    ListView_SetItem( GetDlgItem( hWnd, IDC_LIST ), &lvItem );

    lvItem.iSubItem = 2;
    lvItem.pszText  = CurrentPounce->medium;
    ListView_SetItem( GetDlgItem( hWnd, IDC_LIST ), &lvItem );

    strcpy( szMessage, CurrentPounce->mesg );
    lvItem.iSubItem = 3;
    lvItem.pszText  = StripNewLine( szMessage );
    ListView_SetItem( GetDlgItem( hWnd, IDC_LIST ), &lvItem );
  }
}

BOOL PouncesFindAndErase( Pounce *DelMe ) {
  std::vector< Pounce * >::iterator _iterator;
  for ( _iterator = Pounces.begin( ); _iterator != Pounces.end( ); _iterator++ ) {
    Pounce *CurrentPounce = *( _iterator );
    if ( CurrentPounce->uEntryID == DelMe->uEntryID ) {
      DelSectionItem( CurrentPounce );

      Pounces.erase( _iterator );

      if ( CurrentPounce->menu_entry != NULL )
        delete_ptr( CurrentPounce->menu_entry );
      if ( CurrentPounce->MenuEntry != NULL )
        delete_ptr( CurrentPounce->MenuEntry );
      delete_ptr( CurrentPounce );
      return TRUE;
    }
  }
  return FALSE;
}
