/*
 *  Self-test demonstration program
 *
 *  Based on XySSL: Copyright (C) 2006-2008  Christophe Devine
 *
 *  Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <string.h>
#include <stdio.h>

#include "polarssl/config.h"

#include "polarssl/md2.h"
#include "polarssl/md4.h"
#include "polarssl/md5.h"
#include "polarssl/sha1.h"
#include "polarssl/sha2.h"
#include "polarssl/sha4.h"
#include "polarssl/arc4.h"
#include "polarssl/des.h"
#include "polarssl/aes.h"
#include "polarssl/camellia.h"
#include "polarssl/base64.h"
#include "polarssl/bignum.h"
#include "polarssl/rsa.h"
#include "polarssl/x509.h"
#include "polarssl/xtea.h"

int main( int argc, char *argv[] )
{
    int ret, v;

    if( argc == 2 && strcmp( argv[1], "-quiet" ) == 0 )
        v = 0;
    else
    {
        v = 1;
        printf( "\n" );
    }

#if defined(POLARSSL_MD2_C)
    if( ( ret = md2_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_MD4_C)
    if( ( ret = md4_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_MD5_C)
    if( ( ret = md5_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_SHA1_C)
    if( ( ret = sha1_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_SHA2_C)
    if( ( ret = sha2_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_SHA4_C)
    if( ( ret = sha4_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_ARC4_C)
    if( ( ret = arc4_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_DES_C)
    if( ( ret = des_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_AES_C)
    if( ( ret = aes_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_BASE64_C)
    if( ( ret = base64_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_BIGNUM_C)
    if( ( ret = mpi_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_RSA_C)
    if( ( ret = rsa_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_X509_C)
    if( ( ret = x509_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_XTEA_C)
    if( ( ret = xtea_self_test( v ) ) != 0 )
        return( ret );
#endif

#if defined(POLARSSL_CAMELLIA_C)
    if( ( ret = camellia_self_test( v ) ) != 0 )
        return( ret );
#endif

    if( v != 0 )
    {
        printf( "  [ All tests passed ]\n\n" );
#ifdef WIN32
        printf( "  Press Enter to exit this program.\n" );
        fflush( stdout ); getchar();
#endif
    }

    return( ret );
}
