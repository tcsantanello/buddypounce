========================================================================
    DYNAMIC LINK LIBRARY : Buddy Pounce Project Overview
========================================================================

AppWizard has created this Buddy Pounce DLL for you.  
This file contains a summary of what you will find in each of the files that
make up your Buddy Pounce application.


Buddy Pounce.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

Buddy Pounce.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it  
	will not produce a .lib file when it is built. If you wish this project 
	to be a project dependency of some other project, you will either need to 
	add code to export some symbols from the DLL so that an export library 
	will be produced, or you can set the Ignore Input Library property to Yes 
	on the General propert page of the Linker folder in the project's Property 
	Pages dialog box.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Buddy Pounce.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////


/***************************************************************************************************
*
*			Check for Online Users for message sending routine
*
***************************************************************************************************/
 
int ContactList_Enum(int windowID, char *subwindow, char *event, void *data, void *userData){
	struct contactlist_entry_t *Entry = (struct contactlist_entry_t *) data;
	if ( ! _stricmp(event,"enum_start") ) {
		iContactEnum ++;
	} else if ( ! _stricmp(event,"enum_finish") ) {
		iContactEnum --;
	} else if ( ! _stricmp(event,"enum_add") ) {
		if ( ( Entry->real_name != NULL ) && ( Entry->medium != NULL ) && ( Entry->status != NULL ) ){
			std::vector<Pounce *>::iterator _iterator;
			for ( _iterator = Pounces.begin(); _iterator != Pounces.end(); _iterator++) {
				Pounce *CurrentPounce=*_iterator;
				if ( ! _stricmp(Entry->medium,CurrentPounce->medium) ){
					if ( ! _stricmp(Entry->real_name,CurrentPounce->to) ) {
						if ( _stricmp(Entry->status,szOfflinestr) ) {
							if ( false == CurrentPounce->bonline ){
								if ( CurrentPounce->qwOnline == 0 ) {
									CurrentPounce->bonline=true;
									CurrentPounce->qwOnline=SystemTimeToQWORD(GetLocalTime());
								}
							}
						} else if ( Entry->connection_id >= 0 ) {
							CurrentPounce->bonline=false;
							CurrentPounce->qwOnline=0;
						}
					}
				}
			}
		}
	}
	return 0;
}

/***************************************************************************************************
*
*			Check Accounts & User status for message sending routine
*
***************************************************************************************************/
 
int ConnectionList_Enum(int windowID, char *subwindow, char *event, void *data, void *userData){
	struct connection_entry_t *Entry = (struct connection_entry_t *) data;

	if ( ! _stricmp(event,"enum_start") ) {
		bConnectionListEnum = true;
	} else if ( ! _stricmp(event,"enum_finish") ) {
		bConnectionListEnum = false;
	} else if ( ! _stricmp(event,"enum_add") ) {
		if ( ( Entry->name != NULL ) && ( Entry->medium != NULL ) ) {
			std::vector<Pounce *>::iterator _iterator;
			for ( _iterator = Pounces.begin(); _iterator != Pounces.end(); _iterator++) {
				Pounce *CurrentPounce=*_iterator;
				if ( ! _stricmp(Entry->name,CurrentPounce->from) ) {
					if ( ! _stricmp(Entry->medium,CurrentPounce->medium) ){
						if ( _stricmp(Entry->status,szOfflinestr) ) {
							if ( true == CurrentPounce->bonline ) {
								if ( false == CurrentPounce->bPause ) {
									QWORD qwDelay	= (dwDelay*NANOSECONDS);
									QWORD qwCurrent	= SystemTimeToQWORD(GetLocalTime());
									QWORD qwDiff	= qwCurrent - CurrentPounce->qwOnline;
									bool  bDeleteItem = false;

									if ( qwDiff >= qwDelay ){  
										struct message_t message;
										trillianInitialize(message);

										message.connection_id = Entry->connection_id;
										message.medium		  = Entry->medium;
										message.name		  = CurrentPounce->to;
										message.type		  = "outgoing";

										if ( strlen(szPrefixMesg) > 0 ) {
											message.text	= szPrefixMesg;
											plugin_send(BPGUID,"messageSend",&message);
										}

										message.text		  = CurrentPounce->mesg;

										if ( plugin_send(BPGUID,"messageSend",&message) == 0 ) {
											if ( hwPounceWnd != NULL ) {
												PostMessage(hwPounceWnd,WM_UPDATELIST,0,0);
											}
											EraseINIEntry(CurrentPounce->uEntryID);
											DelSectionItem(CurrentPounce);

											Pounces.erase(_iterator);
											_iterator--;
											bDeleteItem = true;
										}			

										if ( strlen(szSufixMesg) > 0 ) {
											message.text	= szSufixMesg;
											plugin_send(BPGUID,"messageSend",&message);
										}
										
										if ( bDeleteItem ) { 
											delete_ptr(CurrentPounce);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/***************************************************************************************************
*
*			Check User Online with OnlineCBStruct
*
***************************************************************************************************/

int CheckUserOnline(int windowID, char *subwindow, char *event, void *data, void *userData){
	struct contactlist_entry_t *Entry = (struct contactlist_entry_t *) data;
	OnlineCBStruct &User=*(OnlineCBStruct*)userData;
	if ( ! _stricmp(event,"enum_start") ) {
		User.iRepCount++;
	} else if ( ! _stricmp(event,"enum_finish") ) {
		if ( (--User.iRepCount) == 0 ) {
			User.bDone=true;
		}
	} else if ( ! _stricmp(event,"enum_add") ) {
		if ( stricmp(Entry->real_name,User.szUser) == 0 ){
			User.bOnline=(  _stricmp(Entry->status,szOfflinestr) != 0 );
			User.bInList=true;
			User.bDone=true;
		}
	}
	return 0;
}

/***************************************************************************************************
*
*			Check Connection Username with OnlineCBStruct
*
***************************************************************************************************/

int CheckConnection(int windowID, char *subwindow, char *event, void *data, void *userData){
	struct connection_entry_t *Entry = (struct connection_entry_t *) data;
	OnlineCBStruct &User=*(OnlineCBStruct*)userData;
	if ( ! _stricmp(event,"enum_start") ) {
		User.iRepCount++;
	} else if ( ! _stricmp(event,"enum_finish") ) {
		if ( (--User.iRepCount) == 0 ) {
			User.bDone=true;
		}
	} else if ( ! _stricmp(event,"enum_add") ) {
		if ( Entry->connection_id == User.iConnectionID ) {
			if ( strcmp(Entry->medium,User.szMedium) == 0 ){
				User.szUser=Entry->name;
				User.bOnline = true;
				User.bDone = true;
				User.bInList=true;
			}
		}
	}
	return 0;
}


************** Is User Online **************

/*
	if ( bUseExtended ) {
		SectionData userData; ZeroMemory(&userData,sizeof(SectionData));
		userData.Callback=CheckUserOnline;
		userData.UserData=(void*)&Information;
		userData.szMedium=szMedium;

		PrepConnectionEnum((ttkCallback)GetSections_Enum,(void*)&userData,szMedium);
	} else {
		PrepContactEnum(CheckUserOnline,NULL,(void*)&Information,szMedium);
	}

	while ( ( ! Information.iRepCount ) && ( ! Information.bDone ) );
*/

********************************************

************* Is Connection Up *************

/*
	PrepConnectionEnum((ttkCallback)CheckConnection,(void*)&Information);
	while ( ( ! Information.iRepCount ) && ( ! Information.bDone )  );
*/

********************************************

/***************************************************************************************************
*
*			Text width
*
***************************************************************************************************/

long GetAveWidth (  HWND hWnd ){ // uses default text
    TEXTMETRIC  TextMetric;
    HDC     hDC=GetDC(hWnd);

	GetTextMetrics(hDC,&TextMetric);
	UINT uRetVal=TextMetric.tmAveCharWidth + 1;

    ReleaseDC(NULL,hDC);
    return uRetVal;
}