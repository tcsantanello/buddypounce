#include "Structures.h"
#include "plugin.h"

#pragma once

/********************************************************************************************************/

bool   IsValid( void * );     //
bool   IsReadable( void * );  //  Ptr Functions
bool   IsWriteable( void * ); //
TCHAR *LocknLoadResource( HINSTANCE, LPCTSTR, LPCTSTR, DWORD * );
void   LoadCommonControls( void );

/********************************************************************************************************/

/***************************************************************************************************/

BOOL SendMesg( struct message_t *, QWORD, DWORD &, bool & );
BOOL FormatAndParse( TCHAR **, struct message_t *, QWORD, DWORD &, bool & );

HWND FindMeThatWindow( TCHAR *, TCHAR *, TCHAR * );
void RegEvents( BOOL );
void SendEvent( void );

/***************************************************************************************************
 *
 *   Section Creation/Deletion/Toggle (/Callback)
 *
 ***************************************************************************************************/

BOOL InitBPSection( void );
BOOL DenitBPSection( void );

void ToggleSection( BOOL );

int BPSectionCallback( int, char *, char *, void *, void * );
int SectionItemCallback( int, char *, char *, void *, void * );

BOOL AddSectionItem( Pounce * );
BOOL DelSectionItem( Pounce * );
BOOL UpdateSectionItemText( Pounce * );

/***************************************************************************************************
 *
 *   String Functions (String Functions.cpp)
 *
 ***************************************************************************************************/

int    strcmp_wrap( const char *, const char * );
char * strcpy_wrap( char *, const char * );
BOOL   stralnum( char * );
TCHAR *BPNum( UINT );
TCHAR *StrRevChr( TCHAR *, TCHAR );

TCHAR *      ConvertFromNewLine( TCHAR * );
TCHAR *      ConvertToNewLine( TCHAR * );
TCHAR *      StripNewLine( TCHAR * );
BOOL         StripHTML( TCHAR *, TCHAR * );
TCHAR *      StrRemove( TCHAR *, TCHAR *, TCHAR );
UINT         GetCharCount( TCHAR *, TCHAR );
DWORD        CreateCheckSum( TCHAR * );
OnlineStatus DecodeStatus( const TCHAR * );

/***************************************************************************************************
 *
 *   Trillian Plugin Functions (start,stop,load,unload,finalunload,initialload,etc...)
 *
 ***************************************************************************************************/

void LoadPluginData( struct plugin_info_t & );
int  plugin_send( char *, char *, void * );
int  plugin_initialLoad( void *data );
int  plugin_load( void *data );
int  plugin_start( void *data );
int  plugin_info_request( void *data );
int  plugin_stop( void *data );
int  plugin_show_prefs( void *data );
int  plugin_prefs_action( void *data );
int  plugin_unload( void *data );
int  plugin_finalUnload( void *data );

/***************************************************************************************************
 *
 *   Trillian Plugin Prefs Window Functions
 *
 ***************************************************************************************************/

HWND PluginCreateDialog( HINSTANCE, LPCTSTR, HWND, DLGPROC );
void PluginDestroyWindow( HWND );

/***************************************************************************************************
 *
 *   INI Loading, Reading, Updating & Erasing
 *
 ***************************************************************************************************/

UINT   ReadProfile( void );
Pounce ReadINIEntry( UINT );
BOOL   AddINIEntry( Pounce * );
BOOL   UpdateINIEntry( Pounce * );
BOOL   EraseINIEntry( UINT );

/***************************************************************************************************
 *
 *   (Vector) Pounce Deletion
 *
 ***************************************************************************************************/

BOOL PouncesFindAndErase( Pounce * );

/***************************************************************************************************
 *
 *   Trillian Plugin Preferences Init ( Trillian Message & ListView Init)
 *
 ***************************************************************************************************/

TCHAR *GetMediumsIgnoreList( HWND, TCHAR * );
void   MediumsFillListBox( HWND, TCHAR * );

BOOL InitPrefs( void );
void PrefsDialog_InitColumns( HWND );
void PrefsDialog_InitItems( HWND );

/***************************************************************************************************
 *
 *   Buddy Pounce Plugin Preferences Dialog Functions
 *
 ***************************************************************************************************/

BOOL AddEditDialog( HINSTANCE, LPCTSTR, HWND, DLGPROC, LPARAM );

int PrefsDialog( HWND, UINT, WPARAM, LPARAM );
int AddPounceDlg( HWND, UINT, WPARAM, LPARAM );
int EditPounceDlg( HWND, UINT, WPARAM, LPARAM );
int PounceDialog( HWND, UINT, WPARAM, LPARAM );
int PrefsTabDialog( HWND, UINT, WPARAM, LPARAM );
int MesgDialog( HWND, UINT, WPARAM, LPARAM );
int ConfirmDialog( HWND, UINT, WPARAM, LPARAM );
int HelpDialog( HWND, UINT, WPARAM, LPARAM );
int DonateDialog( HWND, UINT, WPARAM, LPARAM );

/***************************************************************************************************
 *
 *   List Enumerations
 *
 ***************************************************************************************************/

int ContactList_Enum( int, char *, char *, void *, void * );
int ConnectionList_Enum( int, char *, char *, void *, void * );

int GetContacts_Enum( int, char *, char *, void *, void * );
int GetConnectionList_Enum( int, char *, char *, void *, void * );
int GetSections_Enum( int, char *, char *, void *, void * );

int CheckUserOnline( int, char *, char *, void *, void * );
int CheckConnection( int, char *, char *, void *, void * );

/***************************************************************************************************
 *
 *   List Enumeration Prep Functions
 *
 ***************************************************************************************************/

int PrepContactEnum( ttkCallback,
                     TCHAR *section  = NULL,
                     void * userdata = NULL,
                     TCHAR *szMedium = "ALL" );
int PrepConnectionEnum( ttkCallback, void *userData = NULL, TCHAR *szMedium = "ALL" );
int PrepSectionEnum( ttkCallback, void * );

/***************************************************************************************************
 *
 *   Thread procedure
 *
 ***************************************************************************************************/

DWORD WINAPI BuddyThread( LPVOID );
DWORD WINAPI BuddyCheck( LPVOID );
DWORD WINAPI RunWindowOperation( LPVOID );
void         DoPoll( void );
void         ScanPounces( void );

/***************************************************************************************************
 *
 *   Error Handling
 *
 ***************************************************************************************************/

void ProcessError( void );

/***************************************************************************************************
 *
 *   Date/Time
 *
 ***************************************************************************************************/

SYSTEMTIME QWORDToSystemTime( QWORD );
QWORD      SystemTimeToQWORD( SYSTEMTIME );
SYSTEMTIME GetLocalTime( void );

void   TimeStripDate( SYSTEMTIME * );
BOOL   IsInTime( QWORD, SYSTEMTIME, QWORD );
void   TimeStripTime( SYSTEMTIME * );
TCHAR *QWORDTimeToString( TCHAR *, QWORD );

/***************************************************************************************************
 *
 *   Message Callback
 *
 ***************************************************************************************************/

TCHAR *  IsConnectionUp( int, TCHAR * );
Contact *GetUser( TCHAR *, TCHAR * );
int      IsConnectionUp( TCHAR *, TCHAR * );

BOOL StartMesgBroadcast( void );
void MessageCallback( int, char *, char *, void *, void * );
BOOL StopMesgBroadcast( void );

void ToggleMesgDetection( BOOL );

/***************************************************************************************************
 *
 *
 *
 ***************************************************************************************************/

// BOOL SendPounce ( Pounce *, QWORD &, QWORD &, Account & );

bool AddPounce( Pounce *, bool );

/**************************************************************
 *
 *
 *
 **************************************************************/

TCHAR *GetMD5( TCHAR * );
