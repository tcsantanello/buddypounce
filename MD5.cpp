#include "MD5.h"
#include "stdafx.h"

#ifdef _DEBUG
#define DEBUG_NEW new ( __FILE__, __LINE__ )
// #define DEBUG_DELETE delete(__FILE__, __LINE__ )
#else
#define DEBUG_NEW new
// #define DEBUG_DELETE delete
#endif

#define new DEBUG_NEW
//#define delete DEBUG_DELETE

MD5Class::MD5Class( TCHAR *szString, UCHAR szDigest[ 17 ] ) {
  ZeroMemory( szDigest, sizeof( UCHAR ) * 17 );
  MDString( szString, szDigest );
}

MD5Class::MD5Class( TCHAR *szString, TCHAR szDigest[ 34 ] ) {
  UCHAR uszDigest[ 17 ];
  ZeroMemory( uszDigest, sizeof( UCHAR ) * 17 );
  MDString( szString, uszDigest );
  ZeroMemory( szDigest, sizeof( TCHAR ) * 34 );
  FormatDigest( szDigest, uszDigest );
}

void MD5Class::FormatDigest( TCHAR szDigest[ 33 ], UCHAR uszDigest[ 17 ] ) {
  ZeroMemory( szDigest, sizeof( TCHAR ) * 33 );
  for ( UINT uCount = 0; uCount < 16; uCount++ )
    sprintf( szDigest, "%s%02x", szDigest, uszDigest[ uCount ] );
}

const TCHAR *MD5Class::GetDigestStr( TCHAR szDigest[ 33 ] ) {
  GetDigest( );
  FormatDigest( szDigest, MD5Val.uc_Digest );
  return szDigest;
}

const UCHAR *MD5Class::GetDigest( void ) {
  Final( MD5Val.uc_Digest );
  return MD5Val.uc_Digest;
}

void MD5Class::_64BitFold( void ) {
  MD5Val.dwBitCount[ 0 ] ^= MD5Val.dwBitCount[ 2 ];
  MD5Val.dwBitCount[ 1 ] ^= MD5Val.dwBitCount[ 3 ];
}

void MD5Class::MDString( TCHAR *szString, int iStrlen ) {
  Update( ( UCHAR * ) szString, ( ( iStrlen == -1 ) ? ( DWORD ) strlen( szString ) : iStrlen ) );
}

void MD5Class::MDString( TCHAR *szString, UCHAR szDigest[ 17 ], int iStrlen ) {
  Init( );
  Update( ( UCHAR * ) szString, ( ( iStrlen == -1 ) ? ( DWORD ) strlen( szString ) : iStrlen ) );
  Final( szDigest );
  Init( );
}

void MD5Class::MDString( TCHAR *szString, TCHAR szDigest[ 33 ], int iStrlen ) {
  UCHAR uszDigest[ 16 ];
  MDString( szString, uszDigest, ( ( iStrlen == -1 ) ? ( int ) strlen( szString ) : iStrlen ) );
  FormatDigest( szDigest, uszDigest );
  Init( );
}

void MD5Class::Transform( DWORD dwState[ 4 ], UCHAR szBlock[ 64 ] ) {
  DWORD dwA = dwState[ 0 ], dwB = dwState[ 1 ], dwC = dwState[ 2 ], dwD = dwState[ 3 ];

  DWORD dwX[ 16 ];

  Decode( dwX, szBlock, 64 );

  FF( dwA, dwB, dwC, dwD, dwX[ 0 ], FF1, 0xd76aa478 );  /* 1 */
  FF( dwD, dwA, dwB, dwC, dwX[ 1 ], FF2, 0xe8c7b756 );  /* 2 */
  FF( dwC, dwD, dwA, dwB, dwX[ 2 ], FF3, 0x242070db );  /* 3 */
  FF( dwB, dwC, dwD, dwA, dwX[ 3 ], FF4, 0xc1bdceee );  /* 4 */
  FF( dwA, dwB, dwC, dwD, dwX[ 4 ], FF1, 0xf57c0faf );  /* 5 */
  FF( dwD, dwA, dwB, dwC, dwX[ 5 ], FF2, 0x4787c62a );  /* 6 */
  FF( dwC, dwD, dwA, dwB, dwX[ 6 ], FF3, 0xa8304613 );  /* 7 */
  FF( dwB, dwC, dwD, dwA, dwX[ 7 ], FF4, 0xfd469501 );  /* 8 */
  FF( dwA, dwB, dwC, dwD, dwX[ 8 ], FF1, 0x698098d8 );  /* 9 */
  FF( dwD, dwA, dwB, dwC, dwX[ 9 ], FF2, 0x8b44f7af );  /* 10 */
  FF( dwC, dwD, dwA, dwB, dwX[ 10 ], FF3, 0xffff5bb1 ); /* 11 */
  FF( dwB, dwC, dwD, dwA, dwX[ 11 ], FF4, 0x895cd7be ); /* 12 */
  FF( dwA, dwB, dwC, dwD, dwX[ 12 ], FF1, 0x6b901122 ); /* 13 */
  FF( dwD, dwA, dwB, dwC, dwX[ 13 ], FF2, 0xfd987193 ); /* 14 */
  FF( dwC, dwD, dwA, dwB, dwX[ 14 ], FF3, 0xa679438e ); /* 15 */
  FF( dwB, dwC, dwD, dwA, dwX[ 15 ], FF4, 0x49b40821 ); /* 16 */

  GG( dwA, dwB, dwC, dwD, dwX[ 1 ], GG1, 0xf61e2562 );  /* 17 */
  GG( dwD, dwA, dwB, dwC, dwX[ 6 ], GG2, 0xc040b340 );  /* 18 */
  GG( dwC, dwD, dwA, dwB, dwX[ 11 ], GG3, 0x265e5a51 ); /* 19 */
  GG( dwB, dwC, dwD, dwA, dwX[ 0 ], GG4, 0xe9b6c7aa );  /* 20 */
  GG( dwA, dwB, dwC, dwD, dwX[ 5 ], GG1, 0xd62f105d );  /* 21 */
  GG( dwD, dwA, dwB, dwC, dwX[ 10 ], GG2, 0x02441453 ); /* 22 */
  GG( dwC, dwD, dwA, dwB, dwX[ 15 ], GG3, 0xd8a1e681 ); /* 23 */
  GG( dwB, dwC, dwD, dwA, dwX[ 4 ], GG4, 0xe7d3fbc8 );  /* 24 */
  GG( dwA, dwB, dwC, dwD, dwX[ 9 ], GG1, 0x21e1cde6 );  /* 25 */
  GG( dwD, dwA, dwB, dwC, dwX[ 14 ], GG2, 0xc33707d6 ); /* 26 */
  GG( dwC, dwD, dwA, dwB, dwX[ 3 ], GG3, 0xf4d50d87 );  /* 27 */
  GG( dwB, dwC, dwD, dwA, dwX[ 8 ], GG4, 0x455a14ed );  /* 28 */
  GG( dwA, dwB, dwC, dwD, dwX[ 13 ], GG1, 0xa9e3e905 ); /* 29 */
  GG( dwD, dwA, dwB, dwC, dwX[ 2 ], GG2, 0xfcefa3f8 );  /* 30 */
  GG( dwC, dwD, dwA, dwB, dwX[ 7 ], GG3, 0x676f02d9 );  /* 31 */
  GG( dwB, dwC, dwD, dwA, dwX[ 12 ], GG4, 0x8d2a4c8a ); /* 32 */

  HH( dwA, dwB, dwC, dwD, dwX[ 5 ], HH1, 0xfffa3942 );  /* 33 */
  HH( dwD, dwA, dwB, dwC, dwX[ 8 ], HH2, 0x8771f681 );  /* 34 */
  HH( dwC, dwD, dwA, dwB, dwX[ 11 ], HH3, 0x6d9d6122 ); /* 35 */
  HH( dwB, dwC, dwD, dwA, dwX[ 14 ], HH4, 0xfde5380c ); /* 36 */
  HH( dwA, dwB, dwC, dwD, dwX[ 1 ], HH1, 0xa4beea44 );  /* 37 */
  HH( dwD, dwA, dwB, dwC, dwX[ 4 ], HH2, 0x4bdecfa9 );  /* 38 */
  HH( dwC, dwD, dwA, dwB, dwX[ 7 ], HH3, 0xf6bb4b60 );  /* 39 */
  HH( dwB, dwC, dwD, dwA, dwX[ 10 ], HH4, 0xbebfbc70 ); /* 40 */
  HH( dwA, dwB, dwC, dwD, dwX[ 13 ], HH1, 0x289b7ec6 ); /* 41 */
  HH( dwD, dwA, dwB, dwC, dwX[ 0 ], HH2, 0xeaa127fa );  /* 42 */
  HH( dwC, dwD, dwA, dwB, dwX[ 3 ], HH3, 0xd4ef3085 );  /* 43 */
  HH( dwB, dwC, dwD, dwA, dwX[ 6 ], HH4, 0x04881d05 );  /* 44 */
  HH( dwA, dwB, dwC, dwD, dwX[ 9 ], HH1, 0xd9d4d039 );  /* 45 */
  HH( dwD, dwA, dwB, dwC, dwX[ 12 ], HH2, 0xe6db99e5 ); /* 46 */
  HH( dwC, dwD, dwA, dwB, dwX[ 15 ], HH3, 0x1fa27cf8 ); /* 47 */
  HH( dwB, dwC, dwD, dwA, dwX[ 2 ], HH4, 0xc4ac5665 );  /* 48 */

  II( dwA, dwB, dwC, dwD, dwX[ 0 ], II1, 0xf4292244 );  /* 49 */
  II( dwD, dwA, dwB, dwC, dwX[ 7 ], II2, 0x432aff97 );  /* 50 */
  II( dwC, dwD, dwA, dwB, dwX[ 14 ], II3, 0xab9423a7 ); /* 51 */
  II( dwB, dwC, dwD, dwA, dwX[ 5 ], II4, 0xfc93a039 );  /* 52 */
  II( dwA, dwB, dwC, dwD, dwX[ 12 ], II1, 0x655b59c3 ); /* 53 */
  II( dwD, dwA, dwB, dwC, dwX[ 3 ], II2, 0x8f0ccc92 );  /* 54 */
  II( dwC, dwD, dwA, dwB, dwX[ 10 ], II3, 0xffeff47d ); /* 55 */
  II( dwB, dwC, dwD, dwA, dwX[ 1 ], II4, 0x85845dd1 );  /* 56 */
  II( dwA, dwB, dwC, dwD, dwX[ 8 ], II1, 0x6fa87e4f );  /* 57 */
  II( dwD, dwA, dwB, dwC, dwX[ 15 ], II2, 0xfe2ce6e0 ); /* 58 */
  II( dwC, dwD, dwA, dwB, dwX[ 6 ], II3, 0xa3014314 );  /* 59 */
  II( dwB, dwC, dwD, dwA, dwX[ 13 ], II4, 0x4e0811a1 ); /* 60 */
  II( dwA, dwB, dwC, dwD, dwX[ 4 ], II1, 0xf7537e82 );  /* 61 */
  II( dwD, dwA, dwB, dwC, dwX[ 11 ], II2, 0xbd3af235 ); /* 62 */
  II( dwC, dwD, dwA, dwB, dwX[ 2 ], II3, 0x2ad7d2bb );  /* 63 */
  II( dwB, dwC, dwD, dwA, dwX[ 9 ], II4, 0xeb86d391 );  /* 64 */

  dwState[ 0 ] += dwA;
  dwState[ 1 ] += dwB;
  dwState[ 2 ] += dwC;
  dwState[ 3 ] += dwD;
}

void MD5Class::Init( void ) {
  ZeroMemory( szPadding, sizeof( UCHAR ) * 64 );
  szPadding[ 0 ] = 0x80;
  StructInit( MD5Val );

  MD5Val.dwState[ 0 ] = DefState_1;
  MD5Val.dwState[ 1 ] = DefState_2;
  MD5Val.dwState[ 2 ] = DefState_3;
  MD5Val.dwState[ 3 ] = DefState_4;
}

void MD5Class::Decode( DWORD *dwOutput, UCHAR *szInput, DWORD dwLen ) {
  DWORD dwCount = 0, dwSubScript = 0;

  for ( ; dwSubScript < dwLen; dwCount++, dwSubScript += 4 ) {
    dwOutput[ dwCount ] = ( ( ( DWORD ) szInput[ dwSubScript ] ) ) |
                          ( ( ( DWORD ) szInput[ dwSubScript + 1 ] ) << 8 ) |
                          ( ( ( DWORD ) szInput[ dwSubScript + 2 ] ) << 16 ) |
                          ( ( ( DWORD ) szInput[ dwSubScript + 3 ] ) << 24 );
  }
}

void MD5Class::Encode( UCHAR *szOutput, DWORD *dwInput, DWORD dwLen ) {
  DWORD dwCount = 0, dwSubScript = 0;

  for ( ; dwSubScript < dwLen; dwCount++, dwSubScript += 4 ) {
    szOutput[ dwSubScript ]     = ( UCHAR )( ( dwInput[ dwCount ] ) & 0xff );
    szOutput[ dwSubScript + 1 ] = ( UCHAR )( ( dwInput[ dwCount ] >> 8 ) & 0xff );
    szOutput[ dwSubScript + 2 ] = ( UCHAR )( ( dwInput[ dwCount ] >> 16 ) & 0xff );
    szOutput[ dwSubScript + 3 ] = ( UCHAR )( ( dwInput[ dwCount ] >> 24 ) & 0xff );
  }
}

void MD5Class::Update( UCHAR *szInput, DWORD dwLen ) {
  UINT  uIndex;
  DWORD dwPartLen = 64 - ( uIndex = ( ( UINT )( MD5Val.dwBitCount[ 0 ] >> 3 ) & 0x3F ) );

  MD5Val.dwBitCount[ 1 ] +=
    ( ( MD5Val.dwBitCount[ 0 ] += ( dwLen << 3 ) ) < ( dwLen << 3 ) ) + ( dwLen >> 29 );

  if ( dwLen >= dwPartLen ) {
    CopyMemory( &MD5Val.uc_Buffer[ uIndex ], szInput, dwPartLen );
    Transform( MD5Val.dwState, MD5Val.uc_Buffer );
    for ( DWORD dwSub = dwPartLen; ( dwSub + 63 ) < dwLen; dwSub += 64 ) {
      Transform( MD5Val.dwState, &szInput[ dwSub ] );
    }
    CopyMemory( &MD5Val.uc_Buffer, &szInput[ dwSub ], dwLen - dwSub );
  } else {
    CopyMemory( &MD5Val.uc_Buffer[ uIndex ], szInput, dwLen );
  }
}

void MD5Class::Final( UCHAR szDigest[ 17 ] ) {
  UCHAR szBits[ 8 ];
  UINT  uIndex, uPadLen;

  Encode( szBits, MD5Val.dwBitCount, 8 );

  uIndex  = ( UINT )( ( MD5Val.dwBitCount[ 0 ] >> 3 ) & 0x3F );
  uPadLen = ( uIndex < 56 ) ? ( 56 - uIndex ) : ( 120 - uIndex );

  Update( szPadding, uPadLen );
  Update( szBits, 8 );

  Encode( szDigest, MD5Val.dwState, 16 );

  CopyMemory( MD5Val.uc_Digest, szDigest, 16 );
}

DWORD MD5Class::F( DWORD &x, DWORD &y, DWORD &z ) {
  return ( ( x & y ) | ( ( ~x ) & z ) );
}

DWORD MD5Class::G( DWORD &x, DWORD &y, DWORD &z ) {
  return ( ( x & z ) | ( y & ( ~z ) ) );
}

DWORD MD5Class::H( DWORD &x, DWORD &y, DWORD &z ) {
  return ( x ^ y ^ z );
}

DWORD MD5Class::I( DWORD &x, DWORD &y, DWORD &z ) {
  return ( y ^ ( x | ( ~z ) ) );
}

DWORD MD5Class::RotateLeft( DWORD x, DWORD n ) {
  return ( x << n ) | ( x >> ( 32 - n ) );
}

void MD5Class::FF( DWORD &a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac ) {
  a += F( b, c, d ) + x + ac;
  a = RotateLeft( a, s ) + b;
}

void MD5Class::GG( DWORD &a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac ) {
  a += G( b, c, d ) + x + ac;
  a = RotateLeft( a, s ) + b;
}

void MD5Class::HH( DWORD &a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac ) {
  a += H( b, c, d ) + x + ac;
  a = RotateLeft( a, s ) + b;
}

void MD5Class::II( DWORD &a, DWORD b, DWORD c, DWORD d, DWORD x, DWORD s, DWORD ac ) {
  a += I( b, c, d ) + x + ac;
  a = RotateLeft( a, s ) + b;
}