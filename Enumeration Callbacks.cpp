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
 *   Prepare A ContactList Enumeration
 *
 ***************************************************************************************************/

int PrepContactEnum( ttkCallback CallbackFunction,
                     TCHAR *     section,
                     void *      userData,
                     TCHAR *     szMedium ) {
  struct contactlist_enum_t ContactList;
  trillianInitialize( ContactList );

  ContactList.callback      = CallbackFunction;
  ContactList.connection_id = -1;
  ContactList.medium        = szMedium;
  ContactList.data          = section; // userData
  if ( bUseExtended ) {
    ContactList.section = section;
  }

  TCHAR *szSection = GetMD5( section );
  WRITE_LOG( Logger )
  ( "================  Enumerating Contacts  (%s)  ================", szSection );
  delete_ptr( szSection );

  return plugin_send( BPGUID, "contactlistEnumerate", &ContactList );
}

/***************************************************************************************************
 *
 *   Prepare A Connection List Enumeration
 *
 ***************************************************************************************************/

int PrepConnectionEnum( ttkCallback CallbackFunction, void *userData, TCHAR *szMedium ) {
  struct connection_enum_t ConnectionList;
  trillianInitialize( ConnectionList );

  ConnectionList.callback = CallbackFunction;
  ConnectionList.medium   = szMedium;
  ConnectionList.data     = userData;

  WRITE_LOG( Logger )( "================   Enumerating Connections   ================" );

  return plugin_send( BPGUID, "connectionEnumerate", &ConnectionList );
}

/***************************************************************************************************
 *
 *   Expand ContactList to Include All Sections with this Enumeration Wrapper
 *
 ***************************************************************************************************/

int PrepSectionEnum( ttkCallback CallbackFunction, void *userData ) {
  struct contactlist_enum_t ContactList;
  trillianInitialize( ContactList );

  ContactList.callback = CallbackFunction;
  ContactList.data     = userData;

  WRITE_LOG( Logger )( "================   Enumerating Sections   ================" );

  return plugin_send( BPGUID, "contactlistSectionEnumerate", &ContactList );
}

int GetSections_Enum( int windowID, char *subwindow, char *event, void *data, void *userData ) {
  struct contactlist_entry_t *Entry = ( struct contactlist_entry_t * ) data;
  if ( !_stricmp( event, "enum_start" ) ) {
    SectionEvent.Reset( );
  } else if ( !_stricmp( event, "enum_finish" ) ) {
    SectionEvent.Set( );
  } else if ( !_stricmp( event, "enum_add" ) ) {
    PrepContactEnum( ( ttkCallback ) userData, Entry->section );
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Fill Contacts vector with enumeration information
 *
 ***************************************************************************************************/

int GetContacts_Enum( int windowID, char *subwindow, char *event, void *data, void *userData ) {
  struct contactlist_entry_t *Entry = ( struct contactlist_entry_t * ) data;
  if ( !_stricmp( event, "enum_start" ) ) {
    ContactEvent.Reset( );
  } else if ( !_stricmp( event, "enum_finish" ) ) {
    std::vector< Medium >::iterator      MedIterator;
    std::vector< Contact >::iterator     ContactIterator;
    std::vector< ContactInfo >::iterator InfoIterator;
    foreach ( MedIterator, Mediums ) {
      foreach ( ContactIterator, MedIterator->Contacts ) {
        foreach ( InfoIterator, ContactIterator->Config ) {
          if ( !InfoIterator->bChange ) { // Account must have been removed or "closed"
            if ( ( InfoIterator->sDelCount ) &&
                 ( InfoIterator->sDelCount == MedIterator->Accounts.size( ) ) &&
                 ( ContactIterator->real_name ) &&
                 ( strcmp_wrap( InfoIterator->section, ( TCHAR * ) userData ) == 0 ) ) {

              TCHAR *szRet = GetMD5( ContactIterator->real_name );
              WRITE_LOG( Logger )( "Removing Config : %s (%d)", szRet, InfoIterator->iConnID );
              delete_ptr( szRet );

              ContactIterator->Config.erase( InfoIterator );
              --InfoIterator;
            } else {
              InfoIterator->sDelCount++;
            }
          } else {
            InfoIterator->bChange   = false;
            InfoIterator->sDelCount = 0;
          }
        }
        if ( ( !ContactIterator->Config.size( ) ) && ( ContactIterator->real_name ) ) {
          Contact &RemContact = *ContactIterator;
          ContactIterator->Config.clear( );

          TCHAR *szRet = GetMD5( ContactIterator->real_name );
          WRITE_LOG( Logger )( "Removing Contact: %s", szRet );
          delete_ptr( szRet );

          MedIterator->Contacts.erase( ContactIterator-- );

          delete_ptr( RemContact.name );
          delete_ptr( RemContact.real_name );
        }
      }
    }
    ContactEvent.Set( );
  } else if ( !_stricmp( event, "enum_add" ) ) {
    if ( ( Entry->real_name != NULL ) && ( Entry->medium != NULL ) && ( Entry->status != NULL ) ) {
      std::vector< Medium >::iterator      MedIterator;
      std::vector< Contact >::iterator     ContactIterator;
      std::vector< ContactInfo >::iterator InfoIterator;
      foreach ( MedIterator, Mediums ) {
        if ( !strcmp_wrap( MedIterator->szMedium, Entry->medium ) ) {
          foreach ( ContactIterator, MedIterator->Contacts ) {
            if ( !strcmp_wrap( ContactIterator->real_name, Entry->real_name ) ) {
              foreach ( InfoIterator, ContactIterator->Config ) {
                InfoIterator->bChange = true;
                if ( InfoIterator->iConnID == Entry->connection_id ) {
                  bool bPrevOnline = ( InfoIterator->Status > Offline );
                  if ( stricmp( Entry->status, szOfflinestr ) ) {
                    if ( stricmp( Entry->status, szOnlinestr ) == 0 ) {
                      InfoIterator->Status = Online;
                    } else {
                      InfoIterator->Status = Away;
                    }
                  } else {
                    InfoIterator->Status = Offline;
                  }
                  if ( ( bPrevOnline == false ) && ( InfoIterator->Status > Offline ) ) {
                    InfoIterator->bLoggedIn = true;
                    InfoIterator->qwOnline  = SystemTimeToQWORD( GetLocalTime( ) );
                    if ( ( bSendOnLogin ) && ( !Pounces.empty( ) ) ) {
                      ScanPounces( );
                    }
                  } else if ( ( bPrevOnline == true ) && ( InfoIterator->Status == Offline ) ) {
                    InfoIterator->qwOnline = 0;
                  } else if ( ( bPrevOnline == true ) && ( InfoIterator->Status > Offline ) ) {
                    InfoIterator->bLoggedIn = false;
                  }

                  if ( ( bSendOnLogin ) && ( InfoIterator->Status > Offline ) &&
                       ( !Pounces.empty( ) ) ) {
                    ScanPounces( );
                  }
                  return 0;
                }
              }
              break;
            }
          }
          break;
        }
      }

      if ( MedIterator == Mediums.end( ) ) {
        Medium NewMedium;
        ZeroMemory( &NewMedium, sizeof( Medium ) );

        DWORD dwMediumLen  = ( DWORD ) strlen( Entry->medium );
        NewMedium.szMedium = new TCHAR[ dwMediumLen + 1 ];

        if ( NewMedium.szMedium ) {
          WRITE_LOG( Logger )( "Adding   Medium : %s", Entry->medium );

          memcpy( NewMedium.szMedium, Entry->medium, dwMediumLen + 1 );

          Mediums.push_back( NewMedium );
          MedIterator     = Mediums.end( ) - 1;
          ContactIterator = MedIterator->Contacts.end( );

          if ( !strcmp_wrap( Entry->medium, szMetaContact ) ) {
            Account NewAccount;
            ZeroMemory( &NewAccount, sizeof( Account ) );

            DWORD dwNameLen = ( DWORD ) strlen( szMetaAccount );
            NewAccount.name = new TCHAR[ dwNameLen + 1 ];
            memcpy( NewAccount.name, szMetaAccount, dwNameLen + 1 );

            NewAccount.Status  = Online;
            NewAccount.bOnline = true;
            NewAccount.iConnId = Entry->connection_id;

            MedIterator->Accounts.push_back( NewAccount );
          }
        }
      }

      if ( ContactIterator == MedIterator->Contacts.end( ) ) {
        Contact NewContact;
        DWORD   dwNameStrLen     = ( DWORD ) strlen( Entry->name );
        DWORD   dwRealNameStrLen = ( DWORD ) strlen( Entry->real_name );

        NewContact.name      = new char[ dwNameStrLen + 1 ];
        NewContact.real_name = new char[ dwRealNameStrLen + 1 ];

        if ( NewContact.name ) {
          memcpy( NewContact.name, Entry->name, dwNameStrLen );
          NewContact.name[ dwNameStrLen ] = NULL;
        }
        if ( NewContact.real_name ) {
          memcpy( NewContact.real_name, Entry->real_name, dwRealNameStrLen );
          NewContact.real_name[ dwRealNameStrLen ] = NULL;
        }

        TCHAR *szRet = GetMD5( Entry->real_name );
        WRITE_LOG( Logger )( "Adding   Contact: %s", szRet );
        delete_ptr( szRet );

        MedIterator->Contacts.push_back( NewContact );
        ContactIterator = MedIterator->Contacts.end( );
        --ContactIterator;
        InfoIterator = ContactIterator->Config.end( );
      }

      if ( InfoIterator == ContactIterator->Config.end( ) ) {
        ContactInfo Info;
        ZeroMemory( &Info, sizeof( Info ) );

        if ( stricmp( Entry->status, szOfflinestr ) ) {
          if ( stricmp( Entry->status, szOnlinestr ) == 0 )
            Info.Status = Online;
          else
            Info.Status = Away;
        } else
          Info.Status = Offline;

        Info.iConnID     = Entry->connection_id;
        Info.bConfirmed  = false;
        Info.hwLastWinID = NULL;
        Info.bChange     = true;

        Info.section = NULL;
        if ( ( bUseExtended ) && ( Entry->section ) ) {
          Info.section = Entry->section;
        }

        if ( Info.Status > Offline ) {
          Info.qwOnline  = SystemTimeToQWORD( GetLocalTime( ) );
          Info.bLoggedIn = true;
        } else {
          Info.qwOnline = 0;
        }

        Info.qwLastMesg = 0;

        TCHAR *szRet = GetMD5( Entry->real_name );
        WRITE_LOG( Logger )( "Adding   Config : %s (%d)", szRet, Entry->connection_id );
        delete_ptr( szRet );

        ContactIterator->Config.push_back( Info );
      }
      if ( ( bSendOnLogin ) && ( InfoIterator->Status > Offline ) && ( !Pounces.empty( ) ) ) {
        ScanPounces( );
      }
    }
  }
  return 0;
}

/***************************************************************************************************
 *
 *   Fill Mediums & Accounts vectors with ConnectionList enumeration information
 *
 ***************************************************************************************************/

int GetConnectionList_Enum(
  int windowID, char *subwindow, char *event, void *data, void *userData ) {
  struct connection_entry_t *Entry = ( struct connection_entry_t * ) data;
  if ( !_stricmp( event, "enum_start" ) ) {
    ConnectEvent.Reset( );
  } else if ( !_stricmp( event, "enum_finish" ) ) {
    std::vector< Medium >::iterator  MedIterator;
    std::vector< Account >::iterator AccIterator;
    foreach ( MedIterator, Mediums ) {
      foreach ( AccIterator, MedIterator->Accounts ) {
        if ( ( !AccIterator->bChange ) &&
             ( strcmp_wrap( AccIterator->name,
                            szMetaAccount ) ) ) { // Account must have been removed or "closed"
          if ( AccIterator->sDelCount == sDelThreshold ) {
            TCHAR *szRet = GetMD5( AccIterator->name );
            WRITE_LOG( Logger )( "Removing Account: %s (%s)", szRet, AccIterator->section );
            delete_ptr( szRet );

            delete_ptr( AccIterator->name );
            delete_ptr( AccIterator->section );

            MedIterator->Accounts.erase( AccIterator );
            --AccIterator;
          } else {
            AccIterator->sDelCount++;
          }
        } else {
          AccIterator->bChange   = false;
          AccIterator->sDelCount = 0;
        }
      }
      if ( ( !MedIterator->Accounts.size( ) ) && ( !MedIterator->Contacts.size( ) ) ) {
        WRITE_LOG( Logger )( "Removing Medium : %s", MedIterator->szMedium );
        MedIterator->Accounts.clear( );
      }
    }
    ConnectEvent.Set( );
  } else if ( !_stricmp( event, "enum_add" ) ) {
    if ( ( Entry->name != NULL ) && ( Entry->medium != NULL ) ) {
      std::vector< Medium >::iterator  MedIterator;
      std::vector< Account >::iterator AccIterator;
      foreach ( MedIterator, Mediums ) {
        if ( !strcmp_wrap( MedIterator->szMedium, Entry->medium ) ) {
          foreach ( AccIterator, MedIterator->Accounts ) {
            if ( !strcmp_wrap( AccIterator->name, Entry->name ) ) {
              AccIterator->Status  = DecodeStatus( Entry->status );
              AccIterator->bChange = true;
              return 0;
            }
          }
          break;
        }
      }
      if ( MedIterator == Mediums.end( ) ) {
        Medium NewMedium;
        ZeroMemory( &NewMedium, sizeof( Medium ) );

        DWORD dwMediumLen  = ( DWORD ) strlen( Entry->medium );
        NewMedium.szMedium = new TCHAR[ dwMediumLen + 1 ];
        memcpy( NewMedium.szMedium, Entry->medium, dwMediumLen + 1 );

        WRITE_LOG( Logger )( "Adding   Medium : %s", Entry->medium );

        Mediums.push_back( NewMedium );
        MedIterator = Mediums.end( ) - 1;
        AccIterator = MedIterator->Accounts.end( );
      }

      if ( AccIterator == MedIterator->Accounts.end( ) ) {
        Account NewAccount;
        ZeroMemory( &NewAccount, sizeof( Account ) );

        DWORD dwNameLen = ( DWORD ) strlen( Entry->name );
        NewAccount.name = new TCHAR[ dwNameLen + 1 ];
        memcpy( NewAccount.name, Entry->name, dwNameLen + 1 );

        if ( Entry->section ) {
          DWORD dwSectionLen = ( DWORD ) strlen( Entry->section );
          NewAccount.section = new TCHAR[ dwSectionLen ];
          if ( NewAccount.section ) {
            memcpy( NewAccount.section, Entry->section, dwSectionLen + 1 );
          }
        }

        NewAccount.Status  = DecodeStatus( Entry->status );
        NewAccount.bOnline = ( stricmp( Entry->status, szOfflinestr ) != 0 );
        NewAccount.iConnId = Entry->connection_id;
        NewAccount.bChange = true;

        TCHAR *szAccount = GetMD5( NewAccount.name );
        TCHAR *szSection = GetMD5( NewAccount.section );
        WRITE_LOG( Logger )
        ( "Adding   Account: %s (%s)", szAccount, ( ( szSection ) ? ( szSection ) : "0" ) );
        delete_ptr( szAccount );
        delete_ptr( szSection );

        MedIterator->Accounts.push_back( NewAccount );
      }
    }
  }
  return 0;
}
