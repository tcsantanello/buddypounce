/*
 * BuddyPounce.cpp
 *
 * Author: Thomas Santanello
 *
 */
#include "Buddy Pounce.h"
#include "stdafx.h"

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

int __declspec( dllexport ) plugin_version( void ) {
  return 1;
}

int __declspec( dllexport ) plugin_main( char *event, void *data ) {
  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );
  if ( !_stricmp( event, "initialLoad" ) )
    return plugin_initialLoad( data ); // Plugin Added
  else if ( !_stricmp( event, "load" ) )
    return plugin_load( data ); // Plugin Initialized (online)
  else if ( !_stricmp( event, "start" ) )
    return plugin_start( data ); // Plugin Started
  else if ( !_stricmp( event, "stop" ) )
    return plugin_stop( data ); // Plugin Stopped
  else if ( !_stricmp( event, "informationRequest" ) )
    return plugin_info_request( data ); // Plugin Information Request
  else if ( !_stricmp( event, "prefsShow" ) )
    return plugin_show_prefs( data ); // Show Preferences
  else if ( !_stricmp( event, "prefsAction" ) )
    return plugin_prefs_action( data ); // Preferences Action
  else if ( !_stricmp( event, "unload" ) )
    return plugin_unload( data ); // Plugin Offline
  else if ( !_stricmp( event, "finalUnload" ) )
    return plugin_finalUnload( data ); // Plugin Removed
  return 0;
}

int plugin_unload( void *data ) {
  pluginsend = NULL;
  if ( hPaypalImage )
    DeleteObject( hPaypalImage );
  if ( hBPSectionImage )
    DeleteObject( hBPSectionImage );
  if ( hBitmap )
    DeleteObject( hBitmap );
  delete_ptr( Logger );
  return 1;
}

int plugin_finalUnload( void *data ) {
  return 1;
}

int plugin_initialLoad( void *data ) {
  return 1;
}

int plugin_info_request( void *data ) {
  struct plugin_info_t &PlugInfo = *( struct plugin_info_t * ) data;
  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );

  LoadPluginData( PlugInfo );
  PlugInfo.plugin_send( MYGUID, "pluginInformationUpdate", &PlugInfo );
  return 1;
}

void LoadPluginData( struct plugin_info_t &PlugInfo ) {
  strcpy_wrap( PlugInfo.guid, BPGUID );
  strcpy_wrap( PlugInfo.company, szCompany );
  strcpy_wrap( PlugInfo.version, szVersion );
  strcpy_wrap( PlugInfo.name, szPluginName );
  strcpy_wrap( PlugInfo.description, szDescription );

  if ( PlugInfo.struct_size >= sizeof( plugin_info_t ) ) {
    if ( szBP32 ) {
      PlugInfo.png_image_32     = ( unsigned char * ) szBP32;
      PlugInfo.png_image_32_len = dwBP32Size;
    }
    if ( szBP48 ) {
      PlugInfo.png_image_48     = ( unsigned char * ) szBP48;
      PlugInfo.png_image_48_len = dwBP48Size;
    }
  }
}

int plugin_load( void *data ) {
  struct plugin_info_t &PlugInfo = *( struct plugin_info_t * ) data;

  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );
  if ( &PlugInfo != NULL ) {
    LoadCommonControls( );

    if ( szDonateMesg == szNotAvail )
      szDonateMesg =
        LocknLoadResource( ( HMODULE ) hInstance, MAKEINTRESOURCE( IDR_DONATE ), "TEXT", NULL );
    if ( szHelpText == szNotAvail )
      szHelpText =
        LocknLoadResource( ( HMODULE ) hInstance, MAKEINTRESOURCE( IDR_HELPFILE ), "TEXT", NULL );
    if ( !szBP48 )
      szBP48 =
        LocknLoadResource( ( HMODULE ) hInstance, MAKEINTRESOURCE( IDR_BP48 ), "PNG", &dwBP48Size );
    if ( !szBP32 )
      szBP32 =
        LocknLoadResource( ( HMODULE ) hInstance, MAKEINTRESOURCE( IDR_BP32 ), "PNG", &dwBP32Size );
    if ( !hPaypalImage )
      hPaypalImage = LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_PAYPAL ) );
    if ( !hBPSectionImage )
      hBPSectionImage = LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_ADD ) );
    if ( !hBitmap )
      hBitmap = LoadBitmap( hInstance, MAKEINTRESOURCE( IDB_LOGO ) );
    if ( !Logger )
      Logger = new cLOG( "BuddyPounce.log" );

    KILLONFAIL( ( szDonateMesg ) && ( szHelpText ), "Error loading dialog resources", return -1; );

    LoadPluginData( PlugInfo );

    strcpy_wrap( szConfig, PlugInfo.config_directory );
    strcat( szConfig, szConfigFile );

    pluginsend  = PlugInfo.plugin_send;
    hwTab       = NULL;
    hwPrefsWnd  = NULL;
    hwPounceWnd = NULL;

    TCHAR szTemp[ MAX_MESSAGE ];
    ZeroMemory( szTemp, sizeof( TCHAR ) * MAX_MESSAGE );

    bUseExtended = ( PlugInfo.struct_size >= dwBaseExtSize );

    GetPrivateProfileString( "Global", "Interval", "1", szTemp, LEN( szTemp ), szConfig );
    iInterval = atoi( szTemp );

    GetPrivateProfileString( "Global", "WindowOp", "0", szTemp, LEN( szTemp ), szConfig );
    iWindowOp = atoi( szTemp );

    GetPrivateProfileString( "Global", "Delay", "5", szTemp, LEN( szTemp ), szConfig );
    dwDelay = strtoul( szTemp, NULL, 10 );

    GetPrivateProfileString( "Global", "PouncePause", "5", szTemp, LEN( szTemp ), szConfig );
    dwThreshold = strtoul( szTemp, NULL, 10 );

    GetPrivateProfileString(
      "Global", "Start", "127097856001390000", szTemp, LEN( szTemp ), szConfig );
    qwStartTime = _atoi64( szTemp );

    GetPrivateProfileString(
      "Global", "End", "127097856001390000", szTemp, LEN( szTemp ), szConfig );
    qwEndTime = _atoi64( szTemp );

    GetPrivateProfileString(
      "Global", "PRECEDE", szDefaultPrefix, szPrefixMesg, LEN( szPrefixMesg ), szConfig );
    GetPrivateProfileString(
      "Global", "SUCCEED", szDefaultSuffix, szSufixMesg, LEN( szSufixMesg ), szConfig );

    GetPrivateProfileString( "GLOBAL", "UseSeperateLines", "0", szTemp, LEN( szTemp ), szConfig );
    bSeperateLines = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "UseTime", "0", szTemp, LEN( szTemp ), szConfig );
    bUseTime = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "UseSection", "1", szTemp, LEN( szTemp ), szConfig );
    bUseSection = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "UseMesgDetection", "1", szTemp, LEN( szTemp ), szConfig );
    bUseMsgDet = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "ConfirmOMS", "1", szTemp, LEN( szTemp ), szConfig );
    bConfirmOMS = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "SendOnLogin", "0", szTemp, LEN( szTemp ), szConfig );
    bSendOnLogin = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "SendOnline", "0", szTemp, LEN( szTemp ), szConfig );
    bSendOnline = atoi( szTemp ) ? true : false;

    GetPrivateProfileString( "GLOBAL", "AcceptableStates", "2", szTemp, LEN( szTemp ), szConfig );
    AcceptableStatus = ( OnlineStatus ) atoi( szTemp );

    GetPrivateProfileString( "GLOBAL", "DebugLog", "0", szTemp, LEN( szTemp ), szConfig );
    if ( Logger )
      Logger->Enable( atoi( szTemp ) ? true : false );

    GetPrivateProfileString(
      "GLOBAL", "IgnoreMediums", "", szMediumIgnore, LEN( szMediumIgnore ), szConfig );
    if ( strlen( szMediumIgnore ) == 0 )
      strcpy( szMediumIgnore, "YAHOO,ICQ,JABBER" );

    iSectionID   = -1;
    iBroadcastID = -1;
    iParentID    = -1;

    ConvertToNewLine( szPrefixMesg );
    ConvertToNewLine( szSufixMesg );
  }
  return 1;
}

int plugin_start( void *data ) {
  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );

  DWORD dwThreadID  = 0;
  hBuddyCheckThread = CreateThread( NULL, 0, BuddyCheck, NULL, 0, &dwThreadID );
  hThread           = CreateThread( NULL, 0, BuddyThread, NULL, 0, &dwThreadID );
  CloseThreads.Create( TRUE, FALSE, "BPCloseThreads" );

#define CLOSETHREADS CloseThreads.CloseHandle( )
#define CLOSEBPCHECK TerminateThread( hBuddyCheckThread, 0 )
#define CLOSETHREAD TerminateThread( hThread, 0 )

  KILLONFAIL( ( HANDLE ) CloseThreads, "Error Creating Thread Event", CLOSETHREADS; CLOSEBPCHECK;
              CLOSETHREAD;
              return -1; );
  KILLONFAIL( hBuddyCheckThread, "Error Creating Vector Thread", CLOSETHREADS; CLOSEBPCHECK;
              CLOSETHREAD;
              return -1; );
  KILLONFAIL( hThread, "Error Creating Pounce Thread", CLOSETHREADS; CLOSEBPCHECK; CLOSETHREAD;
              return -1; );

#undef CLOSETHREADS
#undef CLOSEBPCHECK
#undef CLOSETHREAD

  SetThreadPriority( hBuddyCheckThread, THREAD_BASE_PRIORITY_IDLE );
  SetThreadPriority( hThread, THREAD_BASE_PRIORITY_IDLE );

  ToggleSection( bUseSection );
  ToggleMesgDetection( bUseMsgDet );

  RegEvents( TRUE );
  InitPrefs( );

  return 1;
}

int plugin_stop( void *data ) {
  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );

  ToggleSection( FALSE );
  ToggleMesgDetection( FALSE );
  RegEvents( FALSE );

  if ( hwTab != NULL ) {
    PluginDestroyWindow( hwTab );
    hwTab = NULL;
  }

  CloseThreads.Set( );
  if ( WaitForSingleObject( hThread, 2000 ) == WAIT_TIMEOUT ) {
    if ( TerminateThread( hThread, 0 ) ) {
      hThread = NULL;
      PounceEvent.CloseHandle( );
    }
  }

  if ( WaitForSingleObject( hBuddyCheckThread, 0 ) == WAIT_TIMEOUT ) {
    if ( TerminateThread( hBuddyCheckThread, 0 ) ) {
      hBuddyCheckThread = NULL;
      ConnectEvent.CloseHandle( );
      ContactEvent.CloseHandle( );
    }
  }

  if ( !Pounces.empty( ) ) {
    std::vector< Pounce * >::iterator _iterator;
    foreach ( _iterator, Pounces ) {
      Pounce *CurrentPounce = *_iterator;
      if ( CurrentPounce->bCurrentSession ) {
        EraseINIEntry( CurrentPounce->uEntryID );
      }
      DelSectionItem( CurrentPounce );
      delete_ptr( CurrentPounce );
    }
    Pounces.clear( );
  }

  if ( !Mediums.empty( ) ) {
    std::vector< Medium >::iterator MedIterator;
    foreach ( MedIterator, Mediums ) {
      if ( !MedIterator->Contacts.empty( ) ) {
        std::vector< Contact >::iterator ContIterator;
        foreach ( ContIterator, MedIterator->Contacts ) {
          delete_ptr( ContIterator->real_name );
          delete_ptr( ContIterator->name );
        }
        MedIterator->Contacts.clear( );
      }
      if ( !MedIterator->Accounts.empty( ) ) {
        std::vector< Account >::iterator AccIterator;
        foreach ( AccIterator, MedIterator->Accounts ) {
          delete_ptr( AccIterator->name );
          delete_ptr( AccIterator->section );
        }
        MedIterator->Accounts.clear( );
      }
      delete_ptr( MedIterator->szMedium );
    }
    Mediums.clear( );
  }

  return 1;
}

int plugin_show_prefs( void *data ) {
  struct plugin_prefs_show_t *pPrefs = ( struct plugin_prefs_show_t * ) data;

  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );
  if ( pPrefs != NULL ) {
    if ( !stricmp( pPrefs->pref_name, szPluginName ) ) {
      sprintf( pPrefs->prefs_info.name, "   %s", szPluginTitle );
      sprintf( pPrefs->prefs_info.description, "   %s", szPluginDesc );
      pPrefs->prefs_info.bitmap = hBitmap;

      if ( pPrefs->show == 1 ) {
        if ( hwTab == NULL ) {
          hwTab = PluginCreateDialog(
            hInstance, MAKEINTRESOURCE( IDD_PREFS_DLG ), pPrefs->hwnd, ( DLGPROC ) PrefsTabDialog );
          SetWindowPos( hwTab, 0, pPrefs->x, pPrefs->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
        }

        ShowWindow( hwTab, SW_SHOW );
      } else if ( pPrefs->show == 0 ) {
        if ( hwTab != NULL )
          ShowWindow( hwTab, SW_HIDE );
      }
    }
  }
  return 1;
}

int plugin_prefs_action( void *data ) {
  struct plugin_prefs_action_t *pPrefsAction = ( struct plugin_prefs_action_t * ) data;

  KILLONFAIL( IsPluginValid( ), "Error: Plugin Invalid!", return -1; );
  switch ( pPrefsAction->type ) {
    case 0: // Cancel
      if ( hwTab != NULL ) {
        PluginDestroyWindow( hwTab );
        hwTab       = NULL;
        hwPrefsWnd  = NULL;
        hwPounceWnd = NULL;
      }
      break;
    case 2: // OK
    case 1: // Apply
      if ( hwTab != NULL ) {
        HWND  hwIgnoreList = GetDlgItem( hwPrefsWnd, IDC_OFFLINEIGNORE );
        HWND  hwStart      = GetDlgItem( hwPrefsWnd, IDC_START );
        HWND  hwEnd        = GetDlgItem( hwPrefsWnd, IDC_END );
        HWND  hwPrefix     = GetDlgItem( hwMesgWnd, IDC_PREFIX );
        HWND  hwSufix      = GetDlgItem( hwMesgWnd, IDC_SUCCEED );
        TCHAR szTemp[ MAX_MESSAGE ];
        ZeroMemory( szTemp, sizeof( TCHAR ) * MAX_MESSAGE );

        bSeperateLines =
          ( IsDlgButtonChecked( hwMesgWnd, IDC_SEPERATE ) == BST_CHECKED ) ? true : false;
        WritePrivateProfileString(
          "GLOBAL", "UseSeperateLines", itoa( bSeperateLines, szTemp, 10 ), szConfig );

        AcceptableStatus =
          ( OnlineStatus )( IsDlgButtonChecked( hwPrefsWnd, IDC_ACCEPTABLE ) == BST_CHECKED )
            ? Online
            : Away;
        WritePrivateProfileString(
          "GLOBAL", "AcceptableStates", itoa( AcceptableStatus, szTemp, 10 ), szConfig );

        iInterval = ( int ) GetDlgItemInt( hwPrefsWnd, IDC_INTERVAL, FALSE, FALSE );
        WritePrivateProfileString( "GLOBAL", "Interval", itoa( iInterval, szTemp, 10 ), szConfig );

        iWindowOp =
          ( int ) SendMessage( GetDlgItem( hwPrefsWnd, IDC_WINDOWOP ), CB_GETCURSEL, NULL, NULL );
        WritePrivateProfileString( "GLOBAL", "WindowOp", itoa( iWindowOp, szTemp, 10 ), szConfig );

        dwDelay = ( DWORD ) GetDlgItemInt( hwPrefsWnd, IDC_WAITSEC, FALSE, FALSE );
        WritePrivateProfileString( "GLOBAL", "Delay", ultoa( dwDelay, szTemp, 10 ), szConfig );

        dwThreshold = ( DWORD ) GetDlgItemInt( hwPrefsWnd, IDC_THRESHOLD, FALSE, FALSE );
        WritePrivateProfileString(
          "GLOBAL", "PouncePause", ultoa( dwThreshold, szTemp, 10 ), szConfig );

        GetWindowText( hwPrefix, szPrefixMesg, LEN( szPrefixMesg ) );
        WritePrivateProfileString(
          "GLOBAL", "PRECEDE", ConvertFromNewLine( szPrefixMesg ), szConfig );
        ConvertToNewLine( szPrefixMesg );

        GetWindowText( hwSufix, szSufixMesg, LEN( szSufixMesg ) );
        WritePrivateProfileString(
          "GLOBAL", "SUCCEED", ConvertFromNewLine( szSufixMesg ), szConfig );
        ConvertToNewLine( szSufixMesg );

        bUseTime = ( IsDlgButtonChecked( hwPrefsWnd, IDC_TIME ) == BST_CHECKED ) ? true : false;
        itoa( bUseTime, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "UseTime", szTemp, szConfig );

        bConfirmOMS =
          ( IsDlgButtonChecked( hwPrefsWnd, IDC_CONFIRM ) == BST_CHECKED ) ? true : false;
        itoa( bConfirmOMS, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "ConfirmOMS", szTemp, szConfig );

        bUseSection =
          ( IsDlgButtonChecked( hwPrefsWnd, IDC_SECTION ) == BST_CHECKED ) ? true : false;
        itoa( bUseSection, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "UseSection", szTemp, szConfig );

        bSendOnLogin =
          ( IsDlgButtonChecked( hwPrefsWnd, IDC_LOGINONLY ) == BST_CHECKED ) ? true : false;
        itoa( bSendOnLogin, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "SendOnLogin", szTemp, szConfig );

        bSendOnline =
          ( IsDlgButtonChecked( hwPrefsWnd, IDC_ONLINEONLY ) == BST_CHECKED ) ? true : false;
        itoa( bSendOnline, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "SendOnline", szTemp, szConfig );

        if ( Logger ) {
          Logger->Enable(
            ( IsDlgButtonChecked( hwPrefsWnd, IDC_DEBUGLOG ) == BST_CHECKED ) ? true : false );
          itoa( Logger->IsEnabled( ), szTemp, 10 );
          WritePrivateProfileString( "GLOBAL", "DebugLog", szTemp, szConfig );
        }

        ToggleSection( bUseSection );

        bUseMsgDet =
          ( IsDlgButtonChecked( hwPrefsWnd, IDC_MESGDET ) == BST_CHECKED ) ? true : false;
        itoa( bUseMsgDet, szTemp, 10 );
        WritePrivateProfileString( "GLOBAL", "UseMesgDetection", szTemp, szConfig );
        ToggleMesgDetection( bUseMsgDet );

        SYSTEMTIME sysStartTime;
        ZeroMemory( &sysStartTime, sizeof( SYSTEMTIME ) );
        SYSTEMTIME sysEndTime;
        ZeroMemory( &sysEndTime, sizeof( SYSTEMTIME ) );

        DateTime_GetSystemtime( hwStart, &sysStartTime );
        TimeStripDate( &sysStartTime );
        DateTime_GetSystemtime( hwEnd, &sysEndTime );
        TimeStripDate( &sysEndTime );

        qwStartTime = SystemTimeToQWORD( sysStartTime );
        qwEndTime   = SystemTimeToQWORD( sysEndTime );

        WritePrivateProfileString(
          "GLOBAL", "START", _ui64toa( qwStartTime, szTemp, 10 ), szConfig );

        WritePrivateProfileString( "GLOBAL", "END", _ui64toa( qwEndTime, szTemp, 10 ), szConfig );

        WritePrivateProfileString( "GLOBAL",
                                   "IgnoreMediums",
                                   GetMediumsIgnoreList( hwIgnoreList, szMediumIgnore ),
                                   szConfig );

        if ( pPrefsAction->type == 2 ) {
          PluginDestroyWindow( hwTab );
          hwTab       = NULL;
          hwPrefsWnd  = NULL;
          hwPounceWnd = NULL;
        }
      }
      break;
  }
  return 1;
}

void ProcessError( void ) {
  char *lpMessage;
  FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 GetLastError( ),
                 MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ),
                 ( LPTSTR ) &lpMessage,
                 0,
                 NULL );
  MessageBox( 0, lpMessage, "Buddy Pounce", MB_ICONEXCLAMATION | MB_OK );
  LocalFree( lpMessage );
}

int plugin_send( char *guid, char *event, void *data ) {
  return ( pluginsend != NULL ) ? pluginsend( guid, event, data ) : -1;
}

HWND PluginCreateDialog( HINSTANCE hInst, LPCTSTR lpTemplate, HWND hwParent, DLGPROC dlgDialog ) {
  struct dialog_entry_t det;
  trillianInitialize( det );

  det.hwnd = CreateDialog( hInst, lpTemplate, hwParent, dlgDialog );

  if ( det.hwnd != NULL ) {
    plugin_send( MYGUID, "dialogAdd", &det );
    return det.hwnd;
  }
  return FALSE;
}

void PluginDestroyWindow( HWND hWnd ) {
  struct dialog_entry_t det;

  trillianInitialize( det );
  det.hwnd = hWnd;

  plugin_send( MYGUID, "dialogRemove", &det );

  if ( DestroyWindow( hWnd ) ) {
    // Destroyed
  }
}

BOOL InitPrefs( void ) {
  struct plugin_prefs_t Prefs;
  trillianInitialize( Prefs );
  Prefs.enabled   = true;
  Prefs.pref_name = szPluginName;
  return plugin_send( BPGUID, "prefsInitialize", &Prefs );
}

BOOL APIENTRY DllMain( HANDLE hModule, DWORD dwReason, LPVOID lpReserved ) {
  switch ( dwReason ) {
    case DLL_PROCESS_ATTACH: {
      hInstance = ( HINSTANCE ) hModule;
      break;
    }
    case DLL_PROCESS_DETACH: {
#ifdef MEMMAN
#ifdef _DEBUG
      MemManager.DumpUnfreed( );
#endif
#endif
      break;
    }
  }
  return 1;
}