#include "File Class.h"
#include "stdafx.h"

#ifdef MEMMAN
#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif
#endif

/*********************************************
 *  *
 *  List of open file handles (static)  *
 *  *
 *********************************************/

FileHandles FileClass::Files;

/*********************************************
 *  *
 *  Get File Handle from 'File' var   *
 *  *
 *********************************************/

HANDLE FileClass::GetFile( void ) {
  if ( File )
    return File->hFile;
  else
    return NULL;
}

/*********************************************
 *  *
 *  Get Mutex Handle from 'File' var  *
 *  *
 *********************************************/

HANDLE FileClass::GetMutex( void ) {
  if ( File )
    return File->hMutex;
  else
    return NULL;
}

/*********************************************
 *  *
 *  Default constructor  *
 *  *
 *********************************************/

FileClass::FileClass( ) {
  File      = NULL;
  qwPointer = 0;
}

/*********************************************
 *  *
 *  Copy constructor  *
 *  *
 *********************************************/

FileClass::FileClass( FileClass *Right ) {
  File      = NULL;
  qwPointer = 0;
  *this     = *Right;
}

/*********************************************
 *  *
 *  'Open File' constructor   *
 *  *
 *********************************************/

FileClass::FileClass(
  TCHAR *szFile, DWORD dwAccess, DWORD dwShare, DWORD dwDisposition, DWORD dwFlags ) {
  File      = NULL;
  qwPointer = 0;
  Open( szFile, dwAccess, dwShare, dwDisposition, dwFlags );
}

/*********************************************
 *  *
 *  Deconstructor   *
 *  *
 *********************************************/

FileClass::~FileClass( ) {
  Close( );
}

bool FileClass::IsOpen( void ) {
  return ( File && ( File->hFile && File->hMutex ) );
}

/*********************************************
 *  *
 *  Open File function  *
 *  *
 *  szFile - File Name  *
 *  dwAccess - Access Desired  *
 *  dwShare - Share Mode  *
 *  dwDiposition - Open Mode  *
 *  dwFlags - Attributes & Flags *
 *  *
 *********************************************/

bool FileClass::Open(
  TCHAR *szFile, DWORD dwAccess, DWORD dwShare, DWORD dwDisposition, DWORD dwFlags ) {
  TCHAR szFile_FullPath[ MAX_PATH ];
  ZeroMemory( szFile_FullPath, sizeof( TCHAR ) * MAX_PATH );
  GetFullPathName( szFile, MAX_PATH, szFile_FullPath, NULL );

  if ( File )
    Close( );

  if ( FindFile( szFile_FullPath, dwAccess, &File ) ) {
    qwPointer = 0;
    UseFile( &File );
    return true;
  }

  if ( ( File = new FileHandle ) == NULL )
    return false;

  StructInit( *File );

  if ( ( File->hFile = CreateFile(
           szFile_FullPath, dwAccess, dwShare, NULL, dwDisposition, dwFlags, NULL ) ) != NULL ) {
    TCHAR szDigest[ 34 ];
    ZeroMemory( szDigest, sizeof( TCHAR ) * 34 );
    MD5Class MD5( szFile_FullPath, szDigest );
    if ( ( File->hMutex = CreateMutex( NULL, FALSE, szDigest ) ) != NULL ) {
      qwPointer           = 0;
      File->dwPermission  = dwAccess;
      File->dwShareMode   = dwShare;
      File->dwFlags       = dwFlags;
      File->dwDisposition = dwDisposition;

      UseFile( &File );

      memcpy( File->szFileName, szFile_FullPath, MAX_PATH );
      Files.push_back( File );

      CreateMap( );
    } else {
      CloseHandle( &File->hFile );
      delete_ptr( File );
    }
  } else {
    delete_ptr( File );
  }

  return true;
}

/*********************************************
 *  *
 *  Close File function  *
 *  *
 *  Decrement ref count  *
 *  and clear local variables  *
 *  close if no other references *
 *  *
 *********************************************/

bool FileClass::Close( void ) {
  if ( File ) {
    if ( ( --File->uRefCount ) == 0 ) {
      FHIterator FileIt;
      foreach ( FileIt, Files ) {
        if ( *FileIt == File ) {
          Files.erase( FileIt );
          break;
        }
      }

      if ( Files.empty( ) )
        Files.clear( );

      FlushMap( );
      DestroyMap( );
      CloseHandle( &File->hFile );
      CloseHandle( &File->hMutex );
      delete_ptr( File );
    }
    File      = NULL;
    qwPointer = 0;
  }
  return true;
}

/*********************************************
 *  *
 *  Find File function  *
 *  *
 *  Find file in vector list  *
 *  *
 *********************************************/

bool FileClass::FindFile( TCHAR *szFile, DWORD dwPermission, FileHandle **File ) {
  FHIterator FileIt;
  *File = NULL;

  foreach ( FileIt, Files ) {
    FileHandle *CurrentFile = *FileIt;
    if ( ( !stricmp( CurrentFile->szFileName, szFile ) ) &&
         ( CurrentFile->dwPermission == dwPermission ) ) {
      *File = *FileIt;
      return true;
    }
  }
  return false;
}

/*********************************************
 *  *
 *  Use File function  *
 *  *
 *  Increment file ref count  *
 *  for usage  *
 *  *
 *********************************************/

void FileClass::UseFile( FileHandle **File ) {
  if ( File && *File ) {
    ( *File )->uRefCount++;
  }
}

/*********************************************
 *  *
 *  Get Access function  *
 *  *
 *  Lock Mutex for exclusive access   *
 *  *
 *********************************************/

bool FileClass::GetAccess( void ) {
  if ( ( GetMutex( ) ) && ( File->hFile ) ) {
    return ( WaitForSingleObject( GetMutex( ), INFINITE ) == WAIT_OBJECT_0 );
  }
  return false;
}

/*********************************************
 *  *
 *  Release Access function   *
 *  *
 *  Unlock Mutex   *
 *  *
 *********************************************/

bool FileClass::ReleaseAccess( void ) {
  if ( ( GetMutex( ) ) && ( File->hFile ) ) {
    return ( ReleaseMutex( GetMutex( ) ) != 0 );
  }
  return true;
}

/*********************************************
 *  *
 *  Get File Attributes in DWORD format  *
 *  *
 *********************************************/

DWORD FileClass::GetAttributes( void ) {
  BY_HANDLE_FILE_INFORMATION FileInfo;
  ZeroMemory( &FileInfo, sizeof( BY_HANDLE_FILE_INFORMATION ) );
  if ( GetFileInformationByHandle( File->hFile, &FileInfo ) )
    return FileInfo.dwFileAttributes;
  else
    return 0;
}

/*********************************************
 *  *
 *  Get File Time in QWORD format   *
 *  *
 *********************************************/

QWORD FileClass::GetTime( void ) {
  BY_HANDLE_FILE_INFORMATION FileInfo;
  ZeroMemory( &FileInfo, sizeof( BY_HANDLE_FILE_INFORMATION ) );

  if ( GetFileInformationByHandle( File->hFile, &FileInfo ) ) {
    return MAKEQWORD( FileInfo.ftLastWriteTime.dwLowDateTime,
                      FileInfo.ftLastWriteTime.dwHighDateTime );
  } else
    return 0;
}

/*********************************************
 *  *
 *  Get File Size in QWORD format   *
 *  *
 *********************************************/

QWORD FileClass::GetSize( void ) {
  BY_HANDLE_FILE_INFORMATION FileInfo;
  ZeroMemory( &FileInfo, sizeof( BY_HANDLE_FILE_INFORMATION ) );

  if ( GetFileInformationByHandle( File->hFile, &FileInfo ) ) {
    return MAKEQWORD( FileInfo.nFileSizeLow, FileInfo.nFileSizeHigh );
  } else
    return 0;
}

/*********************************************
 *  *
 *  Get File Information by   *
 *  BY_HANDLE_FILE_INFORMATION  *
 *  *
 *********************************************/

BY_HANDLE_FILE_INFORMATION FileClass::GetInformation( void ) {
  BY_HANDLE_FILE_INFORMATION FileInfo;
  ZeroMemory( &FileInfo, sizeof( BY_HANDLE_FILE_INFORMATION ) );
  GetFileInformationByHandle( File->hFile, &FileInfo );
  return FileInfo;
}

/*********************************************
 *  *
 *  Close HANDLE and NULLify  *
 *  on Success  *
 *  *
 *********************************************/

bool FileClass::CloseHandle( HANDLE *hObject ) {
  if ( ::CloseHandle( *hObject ) ) {
    *hObject = NULL;
    return true;
  } else {
    return false;
  }
}

/*********************************************
 *  *
 *  Move File and reopen if successful  *
 *  try to reopen on failure as  *
 *  well   *
 *  *
 *********************************************/

bool FileClass::Move( TCHAR *szDest ) {
  bool bRet = false;
  if ( GetAccess( ) ) {
    CloseHandle( &File->hFile );

    if ( MoveFile( File->szFileName, szDest ) ) {
      File->hFile = CreateFile( szDest,
                                File->dwPermission,
                                File->dwShareMode,
                                NULL,
                                File->dwDisposition,
                                File->dwFlags,
                                NULL );

      if ( File->hFile != NULL ) {
        strcpy( File->szFileName, szDest );
        bRet = true;
      }
    } else {
      File->hFile = CreateFile( File->szFileName,
                                File->dwPermission,
                                File->dwShareMode,
                                NULL,
                                File->dwDisposition,
                                File->dwFlags,
                                NULL );
    }

    ReleaseAccess( );
  }
  return bRet;
}

/*********************************************
 *  *
 * Set File Attributes from DWORD format  *
 *  *
 *********************************************/

bool FileClass::SetAttrib( DWORD dwAttrib ) {
  return ( SetFileAttributes( File->szFileName, dwAttrib ) != 0 );
}

/*********************************************
 *  *
 *  Lock File block   *
 *  *
 *********************************************/

bool FileClass::Lock( QWORD qwOffset, QWORD qwBytes ) {
  return ( LockFile( File->hFile,
                     LOWQUADPART( qwOffset ),
                     HIGHQUADPART( qwOffset ),
                     LOWQUADPART( qwBytes ),
                     HIGHQUADPART( qwBytes ) ) != 0 );
}

/*********************************************
 *  *
 *   Unlock File block   *
 *  *
 *********************************************/

bool FileClass::UnLock( QWORD qwOffset, QWORD qwBytes ) {
  return ( UnlockFile( File->hFile,
                       LOWQUADPART( qwOffset ),
                       HIGHQUADPART( qwOffset ),
                       LOWQUADPART( qwBytes ),
                       HIGHQUADPART( qwBytes ) ) != 0 );
}

/*********************************************
 *  *
 *   Flush Buffers File  *
 *  *
 *********************************************/

bool FileClass::FlushBuffers( void ) {
  if ( GetFile( ) ) {
    if ( GetAccess( ) ) {
      bool bRet = ( FlushFileBuffers( File->hFile ) != 0 );
      ReleaseAccess( );
      return bRet;
    }
  }
  return false;
}

/*********************************************
 *  *
 *   Set File End of File  *
 *  *
 *********************************************/

bool FileClass::SetEOF( void ) {
  if ( GetFile( ) ) {
    if ( GetAccess( ) ) {
      SetFilePointer64( GetOffset( ), FILE_BEGIN, false );
      bool bRet = ( SetEndOfFile( File->hFile ) != 0 );
      ReleaseAccess( );
      return bRet;
    }
  }
  return false;
}

/*********************************************
 *  *
 *   Read File   *
 *  *
 *********************************************/

bool FileClass::ReadFile( OUT LPVOID lpBuffer,
                          IN DWORD   dwReadLen,
                          OUT DWORD *dwBytesRead,
                          IN bool    bKind ) {
  bool bRet = false;
  if ( GetFile( ) ) {
    if ( bKind ) {
      if ( GetAccess( ) ) {
        if ( !GetData( lpBuffer, dwReadLen, LOWQUADPART( GetOffset( ) ) ) ) {
          DestroyMap( );
          bRet = ReadFile( lpBuffer, dwReadLen, dwBytesRead, false );
          CreateMap( );
        } else {
          if ( dwBytesRead )
            *dwBytesRead = dwReadLen;
          bRet = true;
        }
        ReleaseAccess( );
      }
    } else {
      SetFilePointer64( GetOffset( ), FILE_BEGIN, bKind );

      DWORD dwBytes = 0;
      bRet          = ( ::ReadFile( GetFile( ), lpBuffer, dwReadLen, &dwBytes, NULL ) != 0 );

      if ( ( bRet ) && ( dwBytesRead ) )
        *dwBytesRead = dwBytes;

      SetFilePointer64( GetFilePointer64( bKind ), FILE_BEGIN, bKind );
    }
  }
  return bRet;
}

/*********************************************
 *  *
 *   Write File   *
 *  *
 *********************************************/

bool FileClass::WriteFile( IN LPVOID lpBuffer,
                           IN DWORD  dwWriteLen,
                           OUT DWORD *dwBytesWritten,
                           IN bool    bKind ) {
  bool bRet = false;
  if ( GetFile( ) ) {
    if ( bKind ) {
      if ( GetAccess( ) ) {
        if ( !SetData( lpBuffer, dwWriteLen, LOWQUADPART( GetOffset( ) ) ) ) {
          DestroyMap( );
          bRet = WriteFile( lpBuffer, dwWriteLen, dwBytesWritten, false );
          CreateMap( );
        } else {
          if ( dwBytesWritten )
            *dwBytesWritten = dwWriteLen;
          bRet = true;
        }
        ReleaseAccess( );
      }
    } else {
      SetFilePointer64( GetOffset( ), FILE_BEGIN, bKind );

      DWORD dwBytes = 0;
      bRet          = ( ::WriteFile( GetFile( ), lpBuffer, dwWriteLen, &dwBytes, NULL ) != 0 );

      if ( ( bRet ) && ( dwBytesWritten ) )
        *dwBytesWritten = dwBytes;

      SetFilePointer64( GetFilePointer64( bKind ), FILE_BEGIN, bKind );
    }
  }
  return bRet;
}

bool FileClass::GetData( void *pRead, DWORD dwLength, DWORD dwPos ) {
  if ( ( dwLength + dwPos ) <= ( File->dwSize ) ) {
    BYTE *pPtr = ( BYTE * ) File->pBuffer;
    memcpy( pRead, &pPtr[ dwPos ], dwLength );
    return true;
  }
  return false;
}

bool FileClass::SetData( void *pWrite, DWORD dwLength, DWORD dwPos ) {
  if ( ( dwLength + dwPos ) <= ( File->dwSize ) ) {
    BYTE *pPtr = ( BYTE * ) File->pBuffer;
    memcpy( &pPtr[ dwPos ], pWrite, dwLength );
    return true;
  }
  return false;
}

void FileClass::FlushMap( void ) {
  FlushViewOfFile( File->pBuffer, 0 );
}

void *FileClass::CreateMap( void ) {
  QWORD qwSize = GetSize( );
  File->dwSize = LOWQUADPART( qwSize );
  HANDLE hMap  = CreateFileMapping( File->hFile, NULL, PAGE_READWRITE, 0, File->dwSize, NULL );
  if ( hMap == NULL ) {
    File->dwSize = 0;
    return NULL;
  }
  return ( File->pBuffer = MapViewOfFile( hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0 ) );
}

void FileClass::DestroyMap( void ) {
  UnmapViewOfFile( File->pBuffer );
  File->pBuffer = NULL;
}

/*********************************************
 *  *
 *  Get Current FILE Pointer  *
 *  *
 *********************************************/

QWORD FileClass::GetFilePointer64( IN bool bKind ) {
  QWORD qwRet = 0;
  if ( GetFile( ) ) {
    if ( bKind ) {
      if ( GetAccess( ) ) {
        qwRet = GetFilePointer64( false );
        ReleaseAccess( );
      }
    } else {
      DWORD dwHigh = 0;
      DWORD dwLow  = SetFilePointer( GetFile( ), 0, ( LONG * ) &dwHigh, FILE_CURRENT );
      qwRet        = MAKEQWORD( dwLow, dwHigh );
    }
  }
  return qwRet;
}

/*********************************************
 *  *
 *  Set Current FILE Pointer  *
 *  *
 *********************************************/

QWORD FileClass::SetFilePointer64( IN QWORD qwPos, IN DWORD dwMethod, IN bool bKind ) {
  QWORD qwRet = GetFilePointer64( bKind );
  if ( GetFile( ) ) {
    if ( bKind ) {
      if ( GetAccess( ) ) {
        qwRet = SetFilePointer64( qwPos, dwMethod, false );
        ReleaseAccess( );
      }
    } else {
      DWORD dwLow  = LOWQUADPART( qwPos );
      DWORD dwHigh = HIGHQUADPART( qwPos );

      SetFilePointer( GetFile( ), dwLow, ( LONG * ) &dwHigh, dwMethod );
      SetOffset( GetFilePointer64( bKind ) );
    }
  }
  return qwRet;
}

/*********************************************
 *  *
 *  Write formated string to file   *
 *  *
 *********************************************/

int FileClass::printf( char *format... ) {
  va_list VarArgs;
  va_start( VarArgs, format );

  return vprintf( format, VarArgs );
}

/*********************************************
 *  *
 *  Write formated string to file   *
 *  *
 *********************************************/

int FileClass::vprintf( char *format, va_list VarArgs ) {
  return vprintf_remove_chars( NULL, format, VarArgs );
}

/*********************************************
 *  *
 *  Write formated string to file   *
 *   BUT remove unwanted Chars   *
 *  *
 *********************************************/

int FileClass::vprintf_remove_chars( char *szChars, char *format, va_list VarArgs ) {
  TCHAR *szString = NULL;
  DWORD  dwLen    = ( DWORD ) strlen( format ); // 1

  do {
    delete_ptr( szString );
    szString = new TCHAR[ ( ++dwLen ) + 1 ];
    ZeroMemory( szString, sizeof( TCHAR ) * ( dwLen + 1 ) );
  } while ( _vsnprintf( szString, dwLen, format, VarArgs ) == -1 );
  va_end( VarArgs );

  if ( szChars )
    do {
      for ( char *pPointer = szString; *pPointer != NULL; pPointer++ ) {
        if ( *pPointer == *szChars ) {
          strcpy( pPointer, ( pPointer + 1 ) );
          pPointer--;
          dwLen--;
        }
      }
      szChars++;
    } while ( *szChars );

  dwLen = ( DWORD ) strlen( szString );
  if ( szString && dwLen ) {
    if ( !WriteFile( szString, dwLen, &dwLen ) ) {
      dwLen = 0;
    }
  }

  delete_ptr( szString );
  return dwLen;
}