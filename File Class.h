#ifndef _FILE_CLASS_
#define _FILE_CLASS_

#include "MD5.h"
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <windows.h>

typedef unsigned __int64 QWORD;
#define MAKEQWORD( low, high )                                                                     \
  ( ( QWORD )( ( DWORD )( ( low ) &0xffffffff ) ) |                                                \
    ( ( QWORD )( ( DWORD )( ( high ) &0xffffffff ) ) << 32 ) )
#define LOWQUADPART( x ) ( ( DWORD )( ( x ) &0xffffffff ) )
#define HIGHQUADPART( x ) ( ( DWORD )( ( x ) >> 32 ) )
//#define delete_ptr(pointer) do { delete pointer; pointer=NULL; } while (0);
#define delete_ptr( pointer )                                                                      \
  if ( pointer )                                                                                   \
    delete pointer;                                                                                \
  pointer = NULL;
#define foreach( var, array ) for ( var = array.begin( ); var != array.end( ); var++ )

typedef union INT64Conv {
  FILETIME       FileTime;
  QWORD          qwTime;
  ULARGE_INTEGER ulInt;
} INT64Conv;

typedef struct FileHandle {
  int    struct_size;
  TCHAR  szFileName[ MAX_PATH ];
  UINT   uRefCount;
  DWORD  dwPermission;
  DWORD  dwShareMode;
  DWORD  dwFlags;
  DWORD  dwDisposition;
  HANDLE hFile;
  HANDLE hMutex;
  struct {
    DWORD dwSize;
    void *pBuffer;
  };
} FileHandle;

typedef std::vector< FileHandle * > FileHandles;
typedef FileHandles::iterator       FHIterator;

class FileClass {
 private:
  static FileHandles Files;
  FileHandle *       File;
  QWORD              qwPointer;

 public:
 private:
  static bool FindFile( TCHAR *, DWORD, FileHandle ** );
  static void UseFile( FileHandle ** );

  bool GetAccess( void );
  bool ReleaseAccess( void );
  bool CloseHandle( HANDLE * );

  HANDLE GetFile( void );
  HANDLE GetMutex( void );

  QWORD GetOffset( void ) { return qwPointer; }
  QWORD SetOffset( QWORD Pos ) {
    QWORD qwOld = qwPointer;
    qwPointer   = Pos;
    return qwOld;
  }

  QWORD SetOffset( DWORD Low, DWORD High ) { return SetOffset( MAKEQWORD( Low, High ) ); }

  bool GetData( void *, DWORD, DWORD );
  bool SetData( void *, DWORD, DWORD );

  void  FlushMap( void );
  void *CreateMap( void );
  void  DestroyMap( void );

 public:
  FileClass( );
  FileClass( TCHAR *, DWORD, DWORD, DWORD, DWORD );
  FileClass( FileClass * );
  ~FileClass( );

  FileClass &operator=( FileClass &Right ) {
    File      = Right.File;
    qwPointer = Right.qwPointer;

    File->uRefCount++;

    return *this;
  }

  bool         IsOpen( void );
  const TCHAR *GetFileName( void ) { return ( File ) ? File->szFileName : NULL; }

  bool Open( TCHAR *, DWORD, DWORD, DWORD, DWORD );
  bool Close( void );

  DWORD GetAttributes( void );
  bool  SetAttrib( DWORD );

  QWORD GetTime( void );
  QWORD GetSize( void );

  BY_HANDLE_FILE_INFORMATION
  GetInformation( void );

  bool Move( TCHAR * );

  bool Lock( QWORD, QWORD );
  bool UnLock( QWORD, QWORD );

  bool FlushBuffers( void );
  bool SetEOF( void );

  int printf( char *... );
  int vprintf( char *, va_list );
  int vprintf_remove_chars( char *, char *, va_list );

  bool  ReadFile( LPVOID, DWORD, DWORD *, bool bKind = true );
  bool  WriteFile( LPVOID, DWORD, DWORD *, bool bKind = true );
  QWORD SetFilePointer64( QWORD, DWORD, bool bKind = true );
  QWORD GetFilePointer64( bool bKind = true );
};

#endif