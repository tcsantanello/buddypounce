#pragma once

#include "Constants.h"
#include "Functions.h"
#include "Logger Class.h"
#include "Macros.h"
#include "Mem-Manager.h"
#include "Structures.h"
#include "plugin.h"
#include <commctrl.h>
#include <windows.h>

#include <vector>

extern std::vector< Pounce * > Pounces;
extern std::vector< Medium >   Mediums;
extern std::vector< TCHAR * >  Sections;

extern int          iInterval;
extern QWORD        qwStartTime;
extern QWORD        qwEndTime;
extern TCHAR        szPrefixMesg[ MAX_MESSAGE ];
extern TCHAR        szSufixMesg[ MAX_MESSAGE ];
extern bool         bUseTime;
extern bool         bUseSection;
extern bool         bUseMsgDet;
extern bool         bConfirmOMS;
extern bool         bSendOnLogin;
extern bool         bSeperateLines;
extern DWORD        dwDelay;
extern DWORD        dwThreshold;
extern DWORD        dwBP48Size;
extern DWORD        dwBP32Size;
extern TCHAR        szMediumIgnore[ MAX_IGNORE_LIST ];
extern OnlineStatus AcceptableStatus;

extern TCHAR * szDonateMesg;
extern TCHAR * szHelpText;
extern TCHAR * szBP48;
extern TCHAR * szBP32;
extern HBITMAP hPaypalImage;
extern HBITMAP hBPSectionImage;

extern TCHAR                szConfig[ MAX_PATH ];
extern plugin_function_send pluginsend;
extern HINSTANCE            hInstance;
extern HBITMAP              hBitmap;

extern HANDLE hThread;
extern HANDLE hBuddyCheckThread;

extern EventEx SectionEvent;
extern EventEx ConnectEvent;
extern EventEx ContactEvent;
extern EventEx CloseThreads;
extern EventEx PounceEvent;

extern int iSectionID;
extern int iBroadcastID;
extern int iParentID;
extern int iEventID;
extern int iWindowOp;

extern bool bUseExtended;
extern bool bSendOnLogin;
extern bool bSeperateLines;
extern bool bSendOnline;

extern HWND hwTab;
extern HWND hwPrefsWnd;
extern HWND hwPounceWnd;
extern HWND hwMesgWnd;
extern HWND hwHelpWnd;
extern HWND hwDonateWnd;

extern cLOG *Logger;