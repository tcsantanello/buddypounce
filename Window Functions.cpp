#include "Buddy Pounce.h"
#include "stdafx.h"
#include <shellapi.h>
#include <windowsx.h>

#pragma warning( disable : 4311 )

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

HWND FindMeThatWindow( TCHAR *szRecipient, TCHAR *szMedium, TCHAR *szSender ) {
  if ( ( szRecipient != NULL ) && ( szMedium != NULL ) & ( szSender != NULL ) ) {
    TCHAR *szTitle = new TCHAR[ strlen( szRecipient ) + strlen( szMedium ) + strlen( szSender ) +
                                strlen( " :  - " ) + 1 ];

    TCHAR *szClass = new TCHAR[ strlen( "Trillian private " ) + strlen( szMedium ) + 1 ];
    HWND   hwRet   = NULL;

    if ( szTitle && szClass ) {
      sprintf( szTitle, "%s : %s - %s", szRecipient, szMedium, szSender );
      sprintf( szClass, "Trillian private %s", szMedium );

      hwRet = FindWindow( szClass, szTitle );
      if ( !hwRet ) {
        sprintf( szClass, "ico%s", szMedium );
        hwRet = FindWindow( szClass, szTitle );
      }
    }

    delete_ptr( szTitle );
    delete_ptr( szClass );

    return hwRet;
  }
  return NULL;
}

BOOL CenterWindow( HWND hWnd, HWND hwParant ) {
  RECT WinRect;
  RECT DeskRect;
  GetWindowRect( hWnd, &WinRect );
  GetWindowRect( hwParant, &DeskRect );

  int WinWidth  = WinRect.right - WinRect.left;
  int WinHeight = WinRect.bottom - WinRect.top;

  int DeskWidth  = DeskRect.right - DeskRect.left;
  int DeskHeight = DeskRect.bottom - DeskRect.top;

  return MoveWindow( hWnd,
                     ( DeskWidth / 2 ) - ( WinWidth / 2 ),
                     ( DeskHeight / 2 ) - ( WinHeight / 2 ),
                     WinWidth,
                     WinHeight,
                     TRUE );
}

/***************************************************************************************************
 *
 *   Preferences Dialog
 *
 ***************************************************************************************************/

int PrefsTabDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND        hwTab = GetDlgItem( hWnd, IDC_TAB );
  static HWND hwFocus;
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      TCITEM tcItem;

      ZeroMemory( &tcItem, sizeof( TCITEM ) );
      tcItem.mask = TCIF_TEXT;

      tcItem.pszText = "Pounces";
      TabCtrl_InsertItem( hwTab, 0, &tcItem );
      tcItem.pszText = "Settings";
      TabCtrl_InsertItem( hwTab, 1, &tcItem );
      tcItem.pszText = "Messages";
      TabCtrl_InsertItem( hwTab, 2, &tcItem );
      tcItem.pszText = "Help";
      TabCtrl_InsertItem( hwTab, 3, &tcItem );
      tcItem.pszText = "Donate";
      TabCtrl_InsertItem( hwTab, 4, &tcItem );

      hwPounceWnd =
        CreateDialog( hInstance, MAKEINTRESOURCE( IDD_POUNCES ), hWnd, ( DLGPROC ) PounceDialog );

      hwPrefsWnd =
        CreateDialog( hInstance, MAKEINTRESOURCE( IDD_PREFS ), hWnd, ( DLGPROC ) PrefsDialog );

      hwMesgWnd =
        CreateDialog( hInstance, MAKEINTRESOURCE( IDD_MESG ), hWnd, ( DLGPROC ) MesgDialog );

      hwHelpWnd =
        CreateDialog( hInstance, MAKEINTRESOURCE( IDD_HELP ), hWnd, ( DLGPROC ) HelpDialog );

      hwDonateWnd =
        CreateDialog( hInstance, MAKEINTRESOURCE( IDD_DONATE ), hWnd, ( DLGPROC ) DonateDialog );

      ShowWindow( hwPounceWnd, SW_SHOW );

      RECT rcTab;
      TabCtrl_GetItemRect( hwTab, 0, &rcTab );

      SetWindowPos(
        hwPounceWnd, 0, rcTab.left + 15, rcTab.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      SetWindowPos(
        hwPrefsWnd, 0, rcTab.left + 15, rcTab.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      SetWindowPos(
        hwMesgWnd, 0, rcTab.left + 15, rcTab.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      SetWindowPos(
        hwHelpWnd, 0, rcTab.left + 15, rcTab.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
      SetWindowPos(
        hwDonateWnd, 0, rcTab.left + 15, rcTab.bottom + 15, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

      break;
    }
    case WM_NOTIFY: {
      NMHDR &nmHdr = *( ( LPNMHDR )( lParam ) );
      switch ( nmHdr.code ) {
        case TCN_SELCHANGE: {
          switch ( TabCtrl_GetCurSel( hwTab ) ) {
            case 0: {
              ShowWindow( hwPrefsWnd, SW_HIDE );
              ShowWindow( hwPounceWnd, SW_SHOW );
              ShowWindow( hwMesgWnd, SW_HIDE );
              ShowWindow( hwHelpWnd, SW_HIDE );
              ShowWindow( hwDonateWnd, SW_HIDE );
              hwFocus = hwPounceWnd;
              break;
            }
            case 1: {
              ShowWindow( hwPounceWnd, SW_HIDE );
              ShowWindow( hwPrefsWnd, SW_SHOW );
              ShowWindow( hwMesgWnd, SW_HIDE );
              ShowWindow( hwHelpWnd, SW_HIDE );
              ShowWindow( hwDonateWnd, SW_HIDE );
              hwFocus = hwPrefsWnd;
              break;
            }
            case 2: {
              ShowWindow( hwPounceWnd, SW_HIDE );
              ShowWindow( hwPrefsWnd, SW_HIDE );
              ShowWindow( hwMesgWnd, SW_SHOW );
              ShowWindow( hwHelpWnd, SW_HIDE );
              ShowWindow( hwDonateWnd, SW_HIDE );
              hwFocus = hwMesgWnd;
              break;
            }
            case 3: {
              ShowWindow( hwPounceWnd, SW_HIDE );
              ShowWindow( hwPrefsWnd, SW_HIDE );
              ShowWindow( hwMesgWnd, SW_HIDE );
              ShowWindow( hwHelpWnd, SW_SHOW );
              ShowWindow( hwDonateWnd, SW_HIDE );
              hwFocus = hwHelpWnd;
              break;
            }
            case 4: {
              ShowWindow( hwPounceWnd, SW_HIDE );
              ShowWindow( hwPrefsWnd, SW_HIDE );
              ShowWindow( hwMesgWnd, SW_HIDE );
              ShowWindow( hwHelpWnd, SW_HIDE );
              ShowWindow( hwDonateWnd, SW_SHOW );
              hwFocus = hwDonateWnd;
              break;
            }
          }
          break;
        }
      }
      break;
    }
    case WM_COMMAND: {
      break;
    }
    case WM_SETFOCUS: {
      SetFocus( hwFocus );
      break;
    }
  }
  return 0;
}

int DonateDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      SetWindowText( GetDlgItem( hWnd, IDC_MESG ), szDonateMesg );
      SendMessage(
        GetDlgItem( hWnd, IDC_PPBUTTON ), BM_SETIMAGE, IMAGE_BITMAP, ( LPARAM ) hPaypalImage );
      break;
    }
    case WM_COMMAND: {
      int iEvent = HIWORD( wParam );
      int iID    = LOWORD( wParam );

      if ( iID == IDC_PPBUTTON ) {
        if ( ( UINT ) ShellExecute( hWnd, "open", szDonate, NULL, NULL, SW_SHOW ) <= 32 ) {
          MessageBox( hWnd, szDonate, "Error Launching URL", MB_OK );
        }
      }
    }
    case WM_CLOSE: {
      break;
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Preferences Tab Dialog
 *
 ***************************************************************************************************/

int MesgDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hwPrefix = GetDlgItem( hWnd, IDC_PREFIX );
  HWND hwSufix  = GetDlgItem( hWnd, IDC_SUCCEED );

  switch ( uMsg ) {
    case WM_INITDIALOG: {
      SendMessage( hwPrefix, EM_LIMITTEXT, LEN( szPrefixMesg ) - 1, 0 );
      SendMessage( hwSufix, EM_LIMITTEXT, LEN( szSufixMesg ) - 1, 0 );

      SetWindowText( hwPrefix, szPrefixMesg );
      SetWindowText( hwSufix, szSufixMesg );
      CheckDlgButton( hWnd, IDC_SEPERATE, ( bSeperateLines ) ? BST_CHECKED : BST_UNCHECKED );

      break;
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   My Confirmation Dlg
 *
 ***************************************************************************************************/

int ConfirmDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  TCHAR szTemp[ 10 ];
  if ( uMsg == WM_COMMAND ) {
    if ( IDOK == ( int ) LOWORD( wParam ) ) {
      bConfirmOMS = ( IsDlgButtonChecked( hWnd, IDC_GOAWAY ) == BST_CHECKED ) ? false : true;
      itoa( bConfirmOMS, szTemp, 10 );
      WritePrivateProfileString( "GLOBAL", "ConfirmOMS", szTemp, szConfig );

      if ( hwPrefsWnd != NULL ) {
        CheckDlgButton( hwPrefsWnd, IDC_CONFIRM, ( bConfirmOMS ) ? BST_CHECKED : BST_UNCHECKED );
      }
      EndDialog( hWnd, 1 );
    } else if ( IDCANCEL == ( int ) LOWORD( wParam ) ) {
      EndDialog( hWnd, 0 );
    }
  } else if ( uMsg == WM_INITDIALOG ) {
    CenterWindow( hWnd, GetDesktopWindow( ) );
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Help Dlg
 *
 ***************************************************************************************************/

int HelpDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  if ( uMsg == WM_INITDIALOG ) {
    SetWindowText( GetDlgItem( hWnd, IDC_HELPVER ), szHelpText );
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Fill Mediums Ignore String
 *
 ***************************************************************************************************/

TCHAR *GetMediumsIgnoreList( HWND hWnd, TCHAR *szMediumsIgnore ) {
  UINT uCount = ( UINT ) SendMessage( hWnd, LB_GETSELCOUNT, 0, 0 );
  int *iItems = new int[ uCount + 1 ];
  if ( iItems ) {
    SendMessage( hWnd, LB_GETSELITEMS, ( WPARAM ) uCount, ( LPARAM ) iItems );

    ZeroMemory( szMediumsIgnore, LEN( szMediumsIgnore ) );

    for ( UINT uIter = 0; uIter < uCount; uIter++ ) {
      UINT   uItemLen = ( UINT ) SendMessage( hWnd, LB_GETTEXTLEN, iItems[ uIter ], 0 );
      TCHAR *szItem   = new TCHAR[ uItemLen + 1 ];
      if ( szItem ) {
        if ( SendMessage( hWnd, LB_GETTEXT, iItems[ uIter ], ( LPARAM ) szItem ) ) {
          sprintf( szMediumsIgnore, "%s,%s", szMediumsIgnore, szItem );
        }
      }
      delete_ptr( szItem );
    }
  }
  delete_ptr( iItems );
  strcpy( szMediumsIgnore, ( szMediumsIgnore + 1 ) );
  return szMediumsIgnore;
}

/***************************************************************************************************
 *
 *   Fill Mediums Ignore List
 *
 ***************************************************************************************************/

void MediumsFillListBox( HWND hWnd, TCHAR *szDefaults ) {
  std::vector< Medium >::iterator MIterator;
  UINT                            uCount = 0;
  for ( MIterator = Mediums.begin( ); MIterator != Mediums.end( ); MIterator++, uCount++ ) {
    TCHAR *szMedium = MIterator->szMedium;
    if ( ( strcmp( szMedium, szMetaContact ) != 0 ) ) {
      SendMessage( hWnd, LB_ADDSTRING, 0, ( LPARAM ) szMedium );
      if ( strstr( szDefaults, szMedium ) != NULL ) {
        UINT uSelItem = ( UINT ) SendMessage( hWnd, LB_FINDSTRING, -1, ( LPARAM ) szMedium );
        SendMessage( hWnd, LB_SETSEL, TRUE, uSelItem );
      }
    }
  }
}

/***************************************************************************************************
 *
 *   Preferences Tab Dialog
 *
 ***************************************************************************************************/

int PrefsDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hwInterval      = GetDlgItem( hWnd, IDC_INTERVAL );
  HWND hwStart         = GetDlgItem( hWnd, IDC_START );
  HWND hwEnd           = GetDlgItem( hWnd, IDC_END );
  HWND hwDelay         = GetDlgItem( hWnd, IDC_WAITSEC );
  HWND hwOfflineIgnore = GetDlgItem( hWnd, IDC_OFFLINEIGNORE );
  HWND hwThreshold     = GetDlgItem( hWnd, IDC_THRESHOLD );

  switch ( uMsg ) {
    case WM_INITDIALOG: {
      SendMessage( hwInterval, EM_LIMITTEXT, 3, 0 );
      SetDlgItemInt( hWnd, IDC_INTERVAL, iInterval, FALSE );

      SendMessage( hwDelay, EM_LIMITTEXT, 3, 0 );
      SetDlgItemInt( hWnd, IDC_WAITSEC, dwDelay, FALSE );

      SendMessage( hwThreshold, EM_LIMITTEXT, 3, 0 );
      SetDlgItemInt( hWnd, IDC_THRESHOLD, dwThreshold, FALSE );

      SYSTEMTIME SysTime;
      ZeroMemory( &SysTime, sizeof( SYSTEMTIME ) );

      SysTime = QWORDToSystemTime( qwStartTime );
      if ( DateTime_SetSystemtime( hwStart, GDT_VALID, &SysTime ) ) {
        hwStart;
      }

      SysTime = QWORDToSystemTime( qwEndTime );
      if ( DateTime_SetSystemtime( hwEnd, GDT_VALID, &SysTime ) ) {
        hwEnd;
      }

      CheckDlgButton( hWnd, IDC_TIME, ( bUseTime ) ? BST_CHECKED : BST_UNCHECKED );
      SendMessage( hWnd,
                   WM_COMMAND,
                   MAKEWPARAM( IDC_TIME, BN_CLICKED ),
                   ( LPARAM ) GetDlgItem( hWnd, IDC_TIME ) );

      CheckDlgButton( hWnd, IDC_SECTION, ( bUseSection ) ? BST_CHECKED : BST_UNCHECKED );
      CheckDlgButton( hWnd, IDC_LOGINONLY, ( bSendOnLogin ) ? BST_CHECKED : BST_UNCHECKED );
      CheckDlgButton(
        hWnd, IDC_ACCEPTABLE, ( AcceptableStatus == Online ) ? BST_CHECKED : BST_UNCHECKED );
      CheckDlgButton( hWnd, IDC_ONLINEONLY, ( bSendOnline == true ) ? BST_CHECKED : BST_UNCHECKED );
      CheckDlgButton(
        hWnd, IDC_DEBUGLOG, ( Logger && Logger->IsEnabled( ) ) ? BST_CHECKED : BST_UNCHECKED );

      HWND hwWindowCombo = GetDlgItem( hWnd, IDC_WINDOWOP );
      for ( int iCount = 0, iEnd = sizeof( iOperations ) / sizeof( int ); iCount < iEnd;
            iCount++ ) {
        SendMessage( hwWindowCombo, CB_INSERTSTRING, -1, ( LPARAM ) szOperations[ iCount ] );
      }
      SendMessage( hwWindowCombo, CB_SETCURSEL, ( WPARAM ) iWindowOp, 0 );

      if ( !bUseExtended ) {
        EnableWindow( GetDlgItem( hWnd, IDC_MESGDET ), FALSE );
        EnableWindow( GetDlgItem( hWnd, IDC_CONFIRM ), FALSE );
        EnableWindow( GetDlgItem( hWnd, IDC_OFFLINEIGNORE ), FALSE );
        EnableWindow( GetDlgItem( hWnd, IDC_MYSTATIC ), FALSE );
        EnableWindow( GetDlgItem( hWnd, IDC_MYSTATIC2 ), FALSE );
      } else {
        CheckDlgButton( hWnd, IDC_MESGDET, ( bUseMsgDet ) ? BST_CHECKED : BST_UNCHECKED );
        CheckDlgButton( hWnd, IDC_CONFIRM, ( bConfirmOMS ) ? BST_CHECKED : BST_UNCHECKED );
      }

      MediumsFillListBox( hwOfflineIgnore, szMediumIgnore );

      break;
    }
    case WM_COMMAND: {
      switch ( ( int ) LOWORD( wParam ) ) {
        case IDC_TIME: {
          if ( HIWORD( wParam ) == BN_CLICKED ) {
            if ( IsDlgButtonChecked( hWnd, IDC_TIME ) != BST_CHECKED ) {
              EnableWindow( hwStart, FALSE );
              EnableWindow( hwEnd, FALSE );
            } else {
              EnableWindow( hwStart, TRUE );
              EnableWindow( hwEnd, TRUE );
            }
          }
          break;
        }
      }
      break;
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Pounces Tab Dialog
 *
 ***************************************************************************************************/

int PounceDialog( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  HWND hwListView = GetDlgItem( hWnd, IDC_LIST );
  UINT uSelection = ListView_GetSelectionMark( hwListView );
  UINT uSelCount  = ListView_GetSelectedCount( hwListView );
  switch ( uMsg ) {
    case WM_INITDIALOG:
      PrefsDialog_InitColumns( hWnd );
      PostMessage( hWnd, WM_UPDATELIST, 0, 0 );
      break;
    case WM_COMMAND:
      switch ( ( int ) LOWORD( wParam ) ) {
        case IDC_DELETE: {
          if ( uSelCount >= 1 ) {
            for ( int iPos = 0, uCount = ListView_GetItemCount( hwListView ); iPos <= uCount;
                  iPos++ ) {
              UINT uStyle = ListView_GetItemState( hwListView, iPos, LVIS_SELECTED );
              if ( uStyle & LVIS_SELECTED ) {
                UINT uEntry = Pounces[ iPos ]->uEntryID;
                PouncesFindAndErase( Pounces[ iPos ] );
                EraseINIEntry( uEntry );
                ListView_DeleteItem( hwListView, iPos );
                iPos = -1;
              }
            }
          }
          break;
        }
        case IDC_ADD: {
          if ( AddEditDialog(
                 hInstance, MAKEINTRESOURCE( IDD_ADD ), hWnd, ( DLGPROC ) AddPounceDlg, NULL ) ) {
            PostMessage( hWnd, WM_UPDATELIST, 0, 0 );
          }
          break;
        }
        case IDC_EDIT: {
          if ( ( uSelection != ( UINT ) -1 ) && ( uSelCount == 1 ) ) {
            DialogBoxParam( hInstance,
                            MAKEINTRESOURCE( IDD_EDIT ),
                            hWnd,
                            ( DLGPROC ) EditPounceDlg,
                            ( LPARAM ) Pounces[ uSelection ] );
            PostMessage( hWnd, WM_UPDATELIST, 0, 0 );
          }
          break;
        }
      }
      break;

    case WM_SETFOCUS: {
      UpdateWindow( hwListView );
      break;
    }

    case WM_UPDATELIST: {
      PrefsDialog_InitItems( hWnd );
      break;
    }

    case WM_NOTIFY: {
      NMHDR &nmHdr = *( ( LPNMHDR ) lParam );
      if ( nmHdr.hwndFrom == hwListView ) {
        switch ( nmHdr.code ) {
          case NM_DBLCLK: {
            NMITEMACTIVATE &nmItemActivate = *( ( LPNMITEMACTIVATE )( lParam ) );
            if ( nmItemActivate.iItem != -1 ) {
              SendMessage( hWnd,
                           WM_COMMAND,
                           MAKEWPARAM( IDC_EDIT, BN_CLICKED ),
                           ( LPARAM ) GetDlgItem( hWnd, IDC_EDIT ) );
            }
            break;
          }
          case NM_RCLICK: {
            if ( uSelection != ( UINT ) -1 ) {
              HMENU hMenu    = LoadMenu( hInstance, MAKEINTRESOURCE( IDR_POUNCES_MENU ) );
              HMENU hSubMenu = GetSubMenu( hMenu, 0 );

              POINT pt;
              GetCursorPos( &pt );

              TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, hWnd, NULL );

              DestroyMenu( hMenu );
            } else {
              HMENU hMenu    = LoadMenu( hInstance, MAKEINTRESOURCE( IDR_MAIN_POUNCE_MENU ) );
              HMENU hSubMenu = GetSubMenu( hMenu, 0 );

              POINT pt;
              GetCursorPos( &pt );

              TrackPopupMenu( hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, 0, hWnd, NULL );

              DestroyMenu( hMenu );
            }
            break;
          }
          default:
            break;
        }
      }
      break;
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Preferences Add Pounce Dialog
 *
 ***************************************************************************************************/

BOOL AddEditDialog(
  HINSTANCE hInst, LPCTSTR lpTemplate, HWND hwParent, DLGPROC DialogProc, LPARAM lParam ) {
  KILLONFAIL( ( DialogBoxParam( hInst, lpTemplate, hwParent, DialogProc, lParam ) >= 0 ),
              "Error Creating Dialog (ADDEDIT)",
              return FALSE; );
  return TRUE;
}

int AddPounceDlg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      if ( GetParent( hWnd ) == HWND_DESKTOP )
        CenterWindow( hWnd, GetDesktopWindow( ) );
      SetWindowText( hWnd, szAddNewPounce );
      SendMessage( GetDlgItem( hWnd, IDC_MESSAGE ), EM_LIMITTEXT, MAX_MESSAGE - 1, 0 );
      std::vector< Medium >::iterator MIterator;

      for ( MIterator = Mediums.begin( ); MIterator != Mediums.end( ); MIterator++ ) {
        SendMessage(
          GetDlgItem( hWnd, IDC_MEDIUM ), CB_ADDSTRING, 0, ( LPARAM ) MIterator->szMedium );
      }
      SendMessage( GetDlgItem( hWnd, IDC_MEDIUM ), CB_SETCURSEL, 0, 0 );
      SendMessage( hWnd,
                   WM_COMMAND,
                   MAKEWPARAM( IDC_MEDIUM, CBN_SELCHANGE ),
                   ( LPARAM ) GetDlgItem( hWnd, IDC_MEDIUM ) );
      SendMessage( GetDlgItem( hWnd, IDC_SENDON ), DTM_SETSYSTEMTIME, GDT_NONE, NULL );
      break;
    }
    case WM_COMMAND:
      switch ( ( int ) LOWORD( wParam ) ) {
        case IDC_SENDONAFTER: {
          HWND  hwSendOn = GetDlgItem( hWnd, IDC_SENDONAFTER );
          TCHAR szString[ 14 ];

          GetWindowText( hwSendOn, szString, LEN( szString ) );

          if ( !strcmp( szString, szSendOn ) ) {
            SetWindowText( hwSendOn, szSendOnAfter );
          } else {
            SetWindowText( hwSendOn, szSendOn );
          }
          break;
        }
        case IDOK: {
          HWND hwTo          = GetDlgItem( hWnd, IDC_TO );
          HWND hwFrom        = GetDlgItem( hWnd, IDC_FROM );
          HWND hwMedium      = GetDlgItem( hWnd, IDC_MEDIUM );
          HWND hwPause       = GetDlgItem( hWnd, IDC_PAUSE );
          HWND hwSendOn      = GetDlgItem( hWnd, IDC_SENDON );
          HWND hwSendOnAfter = GetDlgItem( hWnd, IDC_SENDONAFTER );

          TCHAR szString[ 14 ];

          GetWindowText( hwSendOnAfter, szString, LEN( szString ) );

          UINT uSelCount   = ListBox_GetSelCount( hwTo );
          UINT uCount      = 0;
          int *iSelections = new int[ uSelCount + 1 ];
          if ( iSelections ) {
            ListBox_GetSelItems( hwTo, uSelCount, iSelections );

            for ( ; uCount < uSelCount; uCount++ ) {
              Pounce *NewPounce    = new Pounce;
              DWORD   dwTextLength = ListBox_GetTextLen( hwTo, iSelections[ uCount ] );
              TCHAR * szTemp       = new TCHAR[ dwTextLength + 1 ];

              if ( szTemp && NewPounce ) {
                ZeroMemory( szTemp, sizeof( TCHAR ) * dwTextLength );
                ZeroMemory( NewPounce, sizeof( Pounce ) );

                ListBox_GetText( hwTo, iSelections[ uCount ], szTemp );

                if ( bUseExtended ) {
                  if ( !Sections.empty( ) ) {
                    std::vector< TCHAR * >::iterator _iterator;
                    foreach ( _iterator, Sections ) {
                      TCHAR *Section = *_iterator;
                      if ( Section != NULL ) {
                        TCHAR *szEnd = strstr( szTemp, Section );
                        if ( szEnd != NULL ) {
                          *( szEnd - 2 ) = NULL;
                          break;
                        }
                      }
                    }
                  }
                }

                TCHAR *sBegin = StrRevChr( szTemp, '(' ) + 1;
                TCHAR *sEnd   = StrRevChr( szTemp, ')' );

                if ( ( sBegin != szTemp ) && ( sEnd != szTemp ) ) {
                  strncpy( NewPounce->to, sBegin, sEnd - sBegin );
                } else {
                  sscanf( szTemp, "%*[^()](%[^()])", NewPounce->to );
                }

                if ( !strcmp( szString, szSendOn ) ) {
                  NewPounce->bSendAfterOk = false;
                } else {
                  NewPounce->bSendAfterOk = true;
                }

                if ( !strlen( NewPounce->to ) )
                  break;
                if ( !GetWindowText( hwFrom, NewPounce->from, LEN( NewPounce->from ) ) )
                  break;
                if ( !GetWindowText( hwMedium, NewPounce->medium, LEN( NewPounce->medium ) ) )
                  break;
                if ( !GetDlgItemText( hWnd, IDC_MESSAGE, NewPounce->mesg, LEN( NewPounce->mesg ) ) )
                  break;

                NewPounce->bPause = ( IsDlgButtonChecked( hWnd, IDC_PAUSE ) == BST_CHECKED );

                NewPounce->bCurrentSession =
                  ( IsDlgButtonChecked( hWnd, IDC_CURRENTSESSION ) == BST_CHECKED );

                SYSTEMTIME SysTime;
                ZeroMemory( &SysTime, sizeof( SYSTEMTIME ) );

                if ( GDT_NONE !=
                     SendMessage( hwSendOn, DTM_GETSYSTEMTIME, 0, ( LPARAM ) &SysTime ) ) {
                  NewPounce->qwSendOn = SystemTimeToQWORD( SysTime );
                }

                NewPounce->qwTimestamp = SystemTimeToQWORD( GetLocalTime( ) );

                AddPounce( NewPounce, true );
              }
              delete_ptr( szTemp );
            }
          }
          delete_ptr( iSelections );
          EndDialog( hWnd, 2 );
          break;
        }
        case IDCANCEL: {
          EndDialog( hWnd, 1 );
          break;
        }
        case IDC_MEDIUM: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_SELCHANGE: {
              HWND hwTo     = GetDlgItem( hWnd, IDC_TO );
              HWND hwFrom   = GetDlgItem( hWnd, IDC_FROM );
              HWND hwMedium = GetDlgItem( hWnd, IDC_MEDIUM );

              TCHAR szMedium[ 25 ];
              GetWindowText( hwMedium, szMedium, LEN( szMedium ) );
              SendMessage( hwFrom, CB_RESETCONTENT, 0, 0 );
              SendMessage( hwTo, LB_RESETCONTENT, 0, 0 );

              std::vector< Medium >::iterator  MIterator;
              std::vector< Contact >::iterator ContIterator;
              foreach ( MIterator, Mediums ) {
                if ( !strcmp_wrap( MIterator->szMedium, szMedium ) ) {
                  std::vector< Account >::iterator AccIterator;
                  foreach ( AccIterator, MIterator->Accounts ) {
                    SendMessage(
                      GetDlgItem( hWnd, IDC_FROM ), CB_ADDSTRING, 0, ( LPARAM ) AccIterator->name );
                  }
                  if ( SendMessage( hwFrom, CB_GETCOUNT, 0, 0 ) == 1 ) {
                    SendMessage( hwFrom, CB_SETCURSEL, 0, 0 );
                    SendMessage( hWnd, WM_COMMAND, MAKEWPARAM( IDC_FROM, CBN_SELCHANGE ), 0 );
                  }
                  break;
                }
              }
              break;
            }
          }
          break;
        }
        case IDC_FROM: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_SELCHANGE: {
              HWND hwTo     = GetDlgItem( hWnd, IDC_TO );
              HWND hwFrom   = GetDlgItem( hWnd, IDC_FROM );
              HWND hwMedium = GetDlgItem( hWnd, IDC_MEDIUM );

              TCHAR szMedium[ MAX_MESSAGE ];
              TCHAR szFrom[ MAX_MESSAGE ];
              GetWindowText( hwMedium, szMedium, LEN( szMedium ) );
              GetWindowText( hwFrom, szFrom, LEN( szFrom ) );

              SendMessage( hwTo, LB_RESETCONTENT, 0, 0 );

              std::vector< Medium >::iterator      MIterator;
              std::vector< Account >::iterator     AccIterator;
              std::vector< Contact >::iterator     ContIterator;
              std::vector< ContactInfo >::iterator InfoIterator;
              foreach ( MIterator, Mediums ) {
                if ( !strcmp_wrap( MIterator->szMedium, szMedium ) ) {
                  foreach ( AccIterator, MIterator->Accounts ) {
                    if ( !strcmp_wrap( AccIterator->name, szFrom ) ) {
                      foreach ( ContIterator, MIterator->Contacts ) {
                        foreach ( InfoIterator, ContIterator->Config ) {
                          if ( ( InfoIterator->iConnID == AccIterator->iConnId ) ||
                               ( ( InfoIterator->iConnID == -1 ) &&
                                 ( strcmp_wrap( AccIterator->section, InfoIterator->section ) ==
                                   0 ) ) ) {
                            if ( !ContIterator->real_name )
                              break;
                            TCHAR  szString[ 1024 ];
                            TCHAR *RealName = ContIterator->real_name;
                            TCHAR *Name     = ContIterator->name;
                            TCHAR *szSection =
                              ( InfoIterator->section ) ? InfoIterator->section : "My Contacts";
                            if ( ( bUseExtended ) && ( szSection != NULL ) ) {
                              sprintf( szString, "%s (%s) [%s]", Name, RealName, szSection );
                            } else {
                              sprintf( szString, "%s (%s)", Name, RealName );
                            }
                            SendMessage(
                              GetDlgItem( hWnd, IDC_TO ), LB_ADDSTRING, 0, ( LPARAM ) szString );
                            break;
                          }
                        }
                      }
                      return 0;
                    }
                  }
                }
              }
              break;
            }
          }
          break;
        }
        case IDC_TO: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_EDITCHANGE: {
              TCHAR szTemp[ MAX_MESSAGE ];
              GetWindowText( GetDlgItem( hWnd, IDC_TO ), szTemp, LEN( szTemp ) );
              DWORD dwSel =
                ( DWORD ) SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_GETEDITSEL, 0, 0 );
              WORD dStart = LOWORD( dwSel );
              WORD dEnd   = HIWORD( dwSel );

              if ( SendMessage(
                     GetDlgItem( hWnd, IDC_TO ), CB_SELECTSTRING, -1, ( LPARAM ) szTemp ) ==
                   CB_ERR ) {
                SetWindowText( GetDlgItem( hWnd, IDC_TO ), szTemp );
                if ( dwSel != CB_ERR ) {
                  SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_SETEDITSEL, 0, dwSel );
                }
              }

              if ( dEnd < strlen( szTemp ) && dwSel != CB_ERR ) {
                SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_SETEDITSEL, 0, dwSel );
              } else {
                SendMessage( GetDlgItem( hWnd, IDC_TO ),
                             CB_SETEDITSEL,
                             0,
                             MAKELPARAM( strlen( szTemp ), -1 ) );
              }

              break;
            }
          }
          break;
        }
      }
      break;
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Preferences Edit Pounce Dialog
 *
 ***************************************************************************************************/

int EditPounceDlg( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
  Pounce *EditPounce = ( Pounce * ) GetProp( hWnd, "EditPounce" );
  switch ( uMsg ) {
    case WM_INITDIALOG: {
      if ( GetParent( hWnd ) == NULL )
        CenterWindow( hWnd, GetDesktopWindow( ) );
      SetWindowText( hWnd, szEditExisting );

      SetProp( hWnd, "EditPounce", ( EditPounce = ( Pounce * ) lParam ) );
      SendMessage( GetDlgItem( hWnd, IDC_MESSAGE ), EM_LIMITTEXT, MAX_MESSAGE - 1, 0 );

      bool bDone    = false;
      HWND hwMedium = GetDlgItem( hWnd, IDC_MEDIUM );
      HWND hwFrom   = GetDlgItem( hWnd, IDC_FROM );

      std::vector< Medium >::iterator MIterator;
      foreach ( MIterator, Mediums ) {
        SendMessage( hwMedium, CB_ADDSTRING, 0, ( LPARAM ) MIterator->szMedium );
      }

      SendMessage( hwMedium, CB_SELECTSTRING, -1, ( LPARAM ) EditPounce->medium );
      SendMessage( hWnd, WM_COMMAND, MAKEWPARAM( IDC_MEDIUM, CBN_SELCHANGE ), 0 );

      SendMessage( hwFrom, CB_SELECTSTRING, -1, ( LPARAM ) EditPounce->from );
      SendMessage( hWnd, WM_COMMAND, MAKEWPARAM( IDC_FROM, CBN_SELCHANGE ), 0 );

      SetDlgItemText( hWnd, IDC_MESSAGE, EditPounce->mesg );

      TCHAR szTemp[ MAX_MESSAGE ];
      sprintf( szTemp, "%s", EditPounce->to );
      SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_SELECTSTRING, -1, ( LPARAM ) szTemp );

      CheckDlgButton( hWnd, IDC_PAUSE, ( EditPounce->bPause ) ? BST_CHECKED : BST_UNCHECKED );
      CheckDlgButton(
        hWnd, IDC_CURRENTSESSION, ( EditPounce->bCurrentSession ) ? BST_CHECKED : BST_UNCHECKED );
      if ( EditPounce->qwSendOn ) {
        SYSTEMTIME SysTime;
        ZeroMemory( &SysTime, sizeof( SYSTEMTIME ) );
        SysTime = QWORDToSystemTime( EditPounce->qwSendOn );
        SendMessage(
          GetDlgItem( hWnd, IDC_SENDON ), DTM_SETSYSTEMTIME, GDT_VALID, ( LPARAM ) &SysTime );
      } else {
        SendMessage( GetDlgItem( hWnd, IDC_SENDON ), DTM_SETSYSTEMTIME, GDT_NONE, NULL );
      }

      break;
    }
    case WM_COMMAND:
      switch ( ( int ) LOWORD( wParam ) ) {
        case IDC_SENDONAFTER: {
          HWND  hwSendOn = GetDlgItem( hWnd, IDC_SENDONAFTER );
          TCHAR szString[ 14 ];

          GetWindowText( hwSendOn, szString, LEN( szString ) );

          if ( !strcmp( szString, szSendOn ) ) {
            SetWindowText( hwSendOn, szSendOnAfter );
          } else {
            SetWindowText( hwSendOn, szSendOn );
          }
          break;
        }
        case IDOK: {
          HWND hwTo          = GetDlgItem( hWnd, IDC_TO );
          HWND hwFrom        = GetDlgItem( hWnd, IDC_FROM );
          HWND hwMedium      = GetDlgItem( hWnd, IDC_MEDIUM );
          HWND hwPause       = GetDlgItem( hWnd, IDC_PAUSE );
          HWND hwSendOn      = GetDlgItem( hWnd, IDC_SENDON );
          HWND hwSendOnAfter = GetDlgItem( hWnd, IDC_SENDONAFTER );

          TCHAR szString[ 14 ];
          TCHAR szTemp[ MAX_MESSAGE ];
          GetWindowText( hwSendOnAfter, szString, LEN( szString ) );

          if ( !strcmp( szString, szSendOn ) ) {
            EditPounce->bSendAfterOk = false;
          } else {
            EditPounce->bSendAfterOk = true;
          }

          GetWindowText( hwTo, szTemp, LEN( szTemp ) );

          if ( bUseExtended ) {
            if ( !Sections.empty( ) ) {
              std::vector< TCHAR * >::iterator _iterator;
              foreach ( _iterator, Sections ) {
                TCHAR *Section = *_iterator;
                TCHAR *szEnd   = strstr( szTemp, Section );
                if ( szEnd != NULL ) {
                  *( --szEnd ) = NULL;
                  break;
                }
              }
            }
          }

          TCHAR *sBegin = StrRevChr( szTemp, '(' ) + 1;
          TCHAR *sEnd   = StrRevChr( szTemp, ')' );

          if ( ( sBegin != szTemp ) && ( sEnd != szTemp ) ) {
            strncpy( EditPounce->to, sBegin, sEnd - sBegin );
          } else {
            sscanf( szTemp, "%*[^()](%[^()])", EditPounce->to );
          }

          if ( !strlen( EditPounce->to ) )
            break;
          if ( !GetWindowText( hwFrom, EditPounce->from, LEN( EditPounce->from ) ) )
            break;
          if ( !GetWindowText( hwMedium, EditPounce->medium, LEN( EditPounce->medium ) ) )
            break;
          if ( !GetDlgItemText( hWnd, IDC_MESSAGE, EditPounce->mesg, LEN( EditPounce->mesg ) ) )
            break;

          EditPounce->bPause = ( IsDlgButtonChecked( hWnd, IDC_PAUSE ) == BST_CHECKED );

          EditPounce->bCurrentSession =
            ( IsDlgButtonChecked( hWnd, IDC_CURRENTSESSION ) == BST_CHECKED );

          SYSTEMTIME SysTime;
          ZeroMemory( &SysTime, sizeof( SYSTEMTIME ) );

          if ( GDT_NONE != SendMessage( hwSendOn, DTM_GETSYSTEMTIME, 0, ( LPARAM ) &SysTime ) ) {
            EditPounce->qwSendOn = SystemTimeToQWORD( SysTime );
          } else {
            EditPounce->qwSendOn = 0;
          }

          UpdateINIEntry( EditPounce );

          UpdateSectionItemText( EditPounce );

          EditPounce = NULL;
          EndDialog( hWnd, 2 );
          break;
        }
        case IDCANCEL: {
          EditPounce = NULL;
          EndDialog( hWnd, 1 );
          break;
        }
        case IDC_MEDIUM: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_SELCHANGE: {
              HWND hwTo     = GetDlgItem( hWnd, IDC_TO );
              HWND hwFrom   = GetDlgItem( hWnd, IDC_FROM );
              HWND hwMedium = GetDlgItem( hWnd, IDC_MEDIUM );

              TCHAR szMedium[ 25 ];
              GetWindowText( hwMedium, szMedium, LEN( szMedium ) );
              SendMessage( hwFrom, CB_RESETCONTENT, 0, 0 );
              SendMessage( hwTo, CB_RESETCONTENT, 0, 0 );

              std::vector< Medium >::iterator  MIterator;
              std::vector< Contact >::iterator ContIterator;
              foreach ( MIterator, Mediums ) {
                if ( !strcmp_wrap( MIterator->szMedium, szMedium ) ) {
                  std::vector< Account >::iterator AccIterator;
                  foreach ( AccIterator, MIterator->Accounts ) {
                    SendMessage(
                      GetDlgItem( hWnd, IDC_FROM ), CB_ADDSTRING, 0, ( LPARAM ) AccIterator->name );
                  }
                  if ( SendMessage( hwFrom, CB_GETCOUNT, 0, 0 ) == 1 ) {
                    SendMessage( hwFrom, CB_SETCURSEL, 0, 0 );
                    SendMessage( hWnd, WM_COMMAND, MAKEWPARAM( IDC_FROM, CBN_SELCHANGE ), 0 );
                  }
                  break;
                }
              }
              break;
            }
          }
          break;
        }
        case IDC_FROM: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_SELCHANGE: {
              HWND hwTo     = GetDlgItem( hWnd, IDC_TO );
              HWND hwFrom   = GetDlgItem( hWnd, IDC_FROM );
              HWND hwMedium = GetDlgItem( hWnd, IDC_MEDIUM );

              TCHAR szMedium[ MAX_MESSAGE ];
              TCHAR szFrom[ MAX_MESSAGE ];
              GetWindowText( hwMedium, szMedium, LEN( szMedium ) );
              GetWindowText( hwFrom, szFrom, LEN( szFrom ) );

              SendMessage( hwTo, CB_RESETCONTENT, 0, 0 );

              std::vector< Medium >::iterator      MIterator;
              std::vector< Account >::iterator     AccIterator;
              std::vector< Contact >::iterator     ContIterator;
              std::vector< ContactInfo >::iterator InfoIterator;
              foreach ( MIterator, Mediums ) {
                if ( !strcmp_wrap( MIterator->szMedium, szMedium ) ) {
                  foreach ( AccIterator, MIterator->Accounts ) {
                    if ( !strcmp_wrap( AccIterator->name, szFrom ) ) {
                      foreach ( ContIterator, MIterator->Contacts ) {
                        foreach ( InfoIterator, ContIterator->Config ) {
                          if ( ( InfoIterator->iConnID == AccIterator->iConnId ) ||
                               ( ( InfoIterator->iConnID == -1 ) &&
                                 ( strcmp_wrap( AccIterator->section, InfoIterator->section ) ==
                                   0 ) ) ) {
                            if ( !ContIterator->real_name )
                              break;
                            TCHAR  szString[ 1024 ];
                            TCHAR *RealName = ContIterator->real_name;
                            TCHAR *Name     = ContIterator->name;
                            TCHAR *szSection =
                              ( InfoIterator->section ) ? InfoIterator->section : "My Contacts";
                            if ( ( bUseExtended ) && ( szSection != NULL ) ) {
                              sprintf( szString, "%s (%s) [%s]", Name, RealName, szSection );
                            } else {
                              sprintf( szString, "%s (%s)", Name, RealName );
                            }
                            SendMessage(
                              GetDlgItem( hWnd, IDC_TO ), CB_ADDSTRING, 0, ( LPARAM ) szString );
                            break;
                          }
                        }
                      }
                      return 0;
                    }
                  }
                }
              }
              break;
            }
          }
        }
        case IDC_TO: {
          switch ( ( int ) HIWORD( wParam ) ) {
            case CBN_EDITCHANGE: {
              TCHAR szTemp[ MAX_MESSAGE ];
              GetWindowText( GetDlgItem( hWnd, IDC_TO ), szTemp, LEN( szTemp ) );
              DWORD dwSel =
                ( DWORD ) SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_GETEDITSEL, 0, 0 );
              WORD dStart = LOWORD( dwSel );
              WORD dEnd   = HIWORD( dwSel );

              if ( SendMessage(
                     GetDlgItem( hWnd, IDC_TO ), CB_SELECTSTRING, -1, ( LPARAM ) szTemp ) ==
                   CB_ERR ) {
                SetWindowText( GetDlgItem( hWnd, IDC_TO ), szTemp );
                if ( dwSel != CB_ERR ) {
                  SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_SETEDITSEL, 0, dwSel );
                }
              }

              if ( dEnd < strlen( szTemp ) && dwSel != CB_ERR ) {
                SendMessage( GetDlgItem( hWnd, IDC_TO ), CB_SETEDITSEL, 0, dwSel );
              } else {
                SendMessage( GetDlgItem( hWnd, IDC_TO ),
                             CB_SETEDITSEL,
                             0,
                             MAKELPARAM( strlen( szTemp ), -1 ) );
              }

              break;
            }
          }
          break;
        }
      }
      break;
  }
  return 0;
}
