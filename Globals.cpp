#include "Buddy Pounce.h"
#include "stdafx.h"

std::vector< Pounce * > Pounces;
std::vector< Medium >   Mediums;
std::vector< TCHAR * >  Sections;

EventEx SectionEvent;
EventEx ConnectEvent;
EventEx ContactEvent;
EventEx CloseThreads;
EventEx PounceEvent;

plugin_function_send pluginsend = NULL;

TCHAR szConfig[ MAX_PATH ];
TCHAR szPrefixMesg[ MAX_MESSAGE ];
TCHAR szSufixMesg[ MAX_MESSAGE ];
TCHAR szMediumIgnore[ MAX_IGNORE_LIST ];

TCHAR *szBP48       = NULL;
TCHAR *szBP32       = NULL;
TCHAR *szDonateMesg = ( TCHAR * ) szNotAvail;
TCHAR *szHelpText   = ( TCHAR * ) szNotAvail;

OnlineStatus AcceptableStatus = Online;

HINSTANCE hInstance         = NULL;
HANDLE    hThread           = NULL;
HANDLE    hBuddyCheckThread = NULL;
HBITMAP   hBitmap           = NULL;
HBITMAP   hPaypalImage      = NULL;
HBITMAP   hBPSectionImage   = NULL;

QWORD qwStartTime = 0;
QWORD qwEndTime   = 0;

DWORD dwDelay     = 5;
DWORD dwThreshold = 5;
DWORD dwBP48Size  = 0;
DWORD dwBP32Size  = 0;

int iSectionID   = -1;
int iBroadcastID = -1;
int iParentID    = -1;
int iEventID     = -1;
int iInterval    = 1;
int iWindowOp    = 0;

bool bUseExtended   = true;
bool bUseTime       = false;
bool bUseSection    = true;
bool bUseMsgDet     = true;
bool bConfirmOMS    = true;
bool bSendOnLogin   = false;
bool bSeperateLines = true;
bool bSendOnline    = false;

HWND hwTab       = NULL;
HWND hwPrefsWnd  = NULL;
HWND hwPounceWnd = NULL;
HWND hwMesgWnd   = NULL;
HWND hwHelpWnd   = NULL;
HWND hwDonateWnd = NULL;

cLOG *Logger = NULL;