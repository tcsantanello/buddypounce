#include "Constants.h"
#include "plugin.h"

#pragma once

typedef unsigned __int64 QWORD;
typedef enum OnlineStatus { Offline, Invisible, DND, Away, Online } OnlineStatus;

typedef union _FILETIMEEXT_ {
  FILETIME ft;
  QWORD    Quad;
} FILETIMEEXT;

typedef struct _SectionData_ {
  ttkCallback Callback;
  void *      UserData;
  TCHAR *     szMedium;
} SectionData;

typedef struct _OnlineCBStruct_ {
  int    iConnectionID;
  TCHAR *szUser;
  TCHAR *szMedium;
  bool   bOnline;

  bool bInList;
  bool bDone;
  int  iRepCount;
} OnlineCBStruct;

typedef struct _Pounce {
  TCHAR medium[ 25 ];
  TCHAR from[ 1024 ];
  TCHAR to[ 1024 ];
  TCHAR mesg[ MAX_MESSAGE ];

  bool bPause;
  bool bSendAfterOk;
  bool bCurrentSession;

  QWORD qwSendOn;
  QWORD qwTimestamp;

  UINT uEntryID;
  int  iUniqueID;

  struct menu_entry_t *menu_entry;
  struct MenuEntry *   MenuEntry;
} Pounce;

typedef struct MenuEntry {
  TCHAR * szCommand;
  Pounce *ItemPounce;
} MenuEntry;

typedef struct ContactInfo {
  TCHAR *section;
  int    iConnID;

  OnlineStatus Status;
  bool         bConfirmed;
  bool         bLoggedIn;
  bool         bPounceEnd;
  DWORD        dwPounceSum;

  QWORD qwOnline;
  QWORD qwLastMesg;

  HWND  hwLastWinID;
  bool  bChange;
  short sDelCount;
} ContactInfo;

typedef struct Contact {
  TCHAR *                    name;
  TCHAR *                    real_name;
  std::vector< ContactInfo > Config;
} Contact;

typedef struct Account {
  TCHAR *      name;
  TCHAR *      section;
  OnlineStatus Status;
  bool         bOnline;
  int          iConnId;
  bool         bChange;
  short        sDelCount;
} Account;

typedef struct Medium {
  TCHAR *                szMedium;
  std::vector< Contact > Contacts;
  std::vector< Account > Accounts;
} Medium;

class EventEx {
 private:
  HANDLE hEvent;
  long   lRefCount;

 public:
  EventEx( ) {
    hEvent    = NULL;
    lRefCount = 0;
  }
  EventEx( IN BOOL bManualReset, IN BOOL bInitialState, IN LPCSTR lpName ) {
    hEvent = NULL;
    Create( bManualReset, bInitialState, lpName );
  }
  ~EventEx( ) {
    if ( hEvent ) {
      CloseHandle( );
    }
  }
  BOOL CloseHandle( void ) {
    if ( !lRefCount ) {
      return ::CloseHandle( hEvent );
    }
    return FALSE;
  }
  BOOL Create( IN BOOL bManualReset, IN BOOL bInitialState, IN LPCSTR lpName ) {
    if ( !hEvent ) {
      if ( ( hEvent = CreateEvent( NULL, bManualReset, bInitialState, lpName ) ) != NULL ) {
        if ( bInitialState ) {
          SetEvent( hEvent );
          lRefCount++;
        } else {
          ResetEvent( hEvent );
        }
        return TRUE;
      }
    }
    return FALSE;
  }
  BOOL Set( void ) {
    BOOL bRet = FALSE;
    if ( hEvent ) {
      bRet = SetEvent( hEvent );
      lRefCount++;
    }
    return bRet;
  }
  BOOL Reset( void ) {
    BOOL bRet = FALSE;
    if ( !( --lRefCount ) ) {
      bRet      = ResetEvent( hEvent );
      lRefCount = 0;
    }
    return bRet;
  }

  operator HANDLE( ) { return hEvent; }
};