#ifndef _MD5_
#define _MD5_

#include <stdio.h>
#include <windows.h>

#define StructInit( structure )                                                                    \
  ZeroMemory( &( structure ), sizeof( ( structure ) ) );                                           \
  ( structure ).struct_size = sizeof( ( structure ) );

class MD5Class {
  /*************************************************************
   *  *
   *  Private Variables  *
   *  *
   *************************************************************/

 private:
  typedef struct MD5 {
    int   struct_size;
    DWORD dwState[ 4 ];
    DWORD dwBitCount[ 2 ];
    UCHAR uc_Buffer[ 65 ];
    UCHAR uc_Digest[ 17 ];
  } MD5;

  const static DWORD DefState_1 = 0x67452301;
  const static DWORD DefState_2 = 0xefcdab89;
  const static DWORD DefState_3 = 0x98badcfe;
  const static DWORD DefState_4 = 0x10325476;

  const static DWORD FF1 = 7;
  const static DWORD FF2 = 12;
  const static DWORD FF3 = 17;
  const static DWORD FF4 = 22;

  const static DWORD GG1 = 5;
  const static DWORD GG2 = 9;
  const static DWORD GG3 = 14;
  const static DWORD GG4 = 20;

  const static DWORD HH1 = 4;
  const static DWORD HH2 = 11;
  const static DWORD HH3 = 16;
  const static DWORD HH4 = 23;

  const static DWORD II1 = 6;
  const static DWORD II2 = 10;
  const static DWORD II3 = 15;
  const static DWORD II4 = 21;

  MD5 MD5Val;

  UCHAR szPadding[ 65 ];

  /*************************************************************
   *  *
   *  Public Functions  *
   *  *
   *************************************************************/

 public:
  MD5Class( ) { Init( ); }
  MD5Class( TCHAR *, UCHAR[] );
  MD5Class( TCHAR *, TCHAR[] );

  void MDString( TCHAR *, UCHAR[], int iStrlen = -1 );
  void MDString( TCHAR *, TCHAR[], int iStrlen = -1 );
  void MDString( TCHAR *, int iStrlen = -1 );

  void _64BitFold( void );

  const UCHAR *GetDigest( void );
  const TCHAR *GetDigestStr( TCHAR[] );

  void FormatDigest( TCHAR[], UCHAR[] );

  /*************************************************************
   *  *
   *  Private Functions  *
   *  *
   *************************************************************/

 private:
  DWORD F( DWORD &, DWORD &, DWORD & );
  DWORD G( DWORD &, DWORD &, DWORD & );
  DWORD H( DWORD &, DWORD &, DWORD & );
  DWORD I( DWORD &, DWORD &, DWORD & );

  DWORD RotateLeft( DWORD, DWORD );
  void  FF( DWORD &, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD );
  void  GG( DWORD &, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD );
  void  HH( DWORD &, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD );
  void  II( DWORD &, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD );

  void Decode( DWORD *, UCHAR *, DWORD );
  void Encode( UCHAR *, DWORD *, DWORD );
  void Transform( DWORD[], UCHAR[] );

 protected:
  void Init( void );
  void Update( UCHAR *, DWORD );
  void Final( UCHAR[] );
};

#endif