/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

# include "extras/safestring.h"

// All data here will be dynamically allocated.
static char _pass [ 33 ] ;

void mcr_SafeString_init ( mcr_SafeString * ssPt )
{
	if ( ! ssPt )
	{
		dmsg ;
		return ;
	}
	memset ( ssPt, 0, sizeof ( mcr_SafeString ) ) ;
	mcr_String_init ( & ssPt->text ) ;
}

void mcr_SafeString_init_with ( mcr_SafeString * ssPt,
		const char * text, int cryptic )
{
	if ( ! ssPt )
	{
		dmsg ;
		return ;
	}
	mcr_SafeString_init ( ssPt ) ;
	mcr_SafeString_set ( ssPt, text, cryptic ) ;
}

int mcr_SafeString_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
		return lhs < rhs ? -1 : 1 ;
	const mcr_SafeString * lPt = lhs, * rPt = rhs ;
	mcr_Array lStr, rStr ;
	const mcr_Array * lStrPt = & lStr, * rStrPt = & rStr ;
	mcr_String_init ( & lStr ) ;
	mcr_String_init ( & rStr ) ;
	if ( lPt->cryptic )
		lStr = mcr_SafeString_get ( lPt ) ;
	else
		lStrPt = & lPt->text ;
	if ( rPt->cryptic )
		rStr = mcr_SafeString_get ( rPt ) ;
	else
		rStrPt = & rPt->text ;
	int ret ;
	if ( lStrPt->used != rStrPt->used )
		ret = lStrPt->used < rStrPt->used ? -1 : 1 ;
	else
		ret = memcmp ( lStrPt->array, rStrPt->array, lStrPt->used ) ;
	mcr_Array_free ( & lStr ) ;
	mcr_Array_free ( & rStr ) ;
	return ret ;
}

void mcr_SafeString_copy ( void * dstPt, void * srcPt )
{
	dassert ( dstPt ) ;
	if ( ! srcPt )
	{
		mcr_SafeString_init ( dstPt ) ;
		return ;
	}
	mcr_SafeString * dPt = dstPt, * sPt = srcPt ;
	if ( sPt->cryptic )
	{
		mcr_Array txt = mcr_SafeString_get ( sPt ) ;
		mcr_SafeString_set ( dPt, txt.array, 1 ) ;
		mcr_Array_free ( & txt ) ;
	}
	else
	{
		mcr_SafeString_set ( dPt, sPt->text.array, 0 ) ;
	}
}

void mcr_SafeString_free ( mcr_SafeString * ssPt )
{
	if ( ! ssPt )
	{
		dmsg ;
		return ;
	}
	mcr_Array_free ( & ssPt->text ) ;
}

mcr_Array mcr_SafeString_get ( const mcr_SafeString * ssPt )
{
	dassert ( ssPt ) ;
	return mcr_SafeString_from_password ( ssPt, _pass ) ;
}

mcr_Array mcr_SafeString_from_password ( const mcr_SafeString * ssPt,
		const char * pass )
{
	dassert ( ssPt ) ;
	if ( ssPt->cryptic && ssPt->text.used )
	{
		return mcr_String_from_encrypted ( & ssPt->text, ssPt->tag,
				( const unsigned char * ) pass, ssPt->iv ) ;
	}
	mcr_Array ret ;
	mcr_String_init ( & ret ) ;
	mcr_String_nfrom_string ( & ret, ssPt->text.array,
			ssPt->text.used ) ;
	return ret ;
}

void mcr_SafeString_set ( mcr_SafeString * ssPt,
		const char * text, int cryptic )
{
	dassert ( ssPt ) ;
	if ( text )
		mcr_String_from_string ( & ssPt->text, text ) ;
	else
		mcr_String_from_string ( & ssPt->text, "\0" ) ;

	if ( cryptic && text && _pass [ 0 ] != '\0' )
	{
		mcr_randomize ( ssPt->iv, sizeof ( ssPt->iv ) - 1 ) ;
		ssPt->iv [ MCR_AES_BLOCK_SIZE ] = '\0' ;
		mcr_String_to_encrypted ( & ssPt->text, ssPt->tag,
				( unsigned char * ) _pass, ssPt->iv ) ;
		ssPt->cryptic = cryptic ;
	}
	else
	{
		memset ( ssPt->iv, 0, sizeof ( ssPt->iv ) ) ;
		memset ( ssPt->tag, 0, sizeof ( ssPt->tag ) ) ;
		ssPt->cryptic = 0 ;
	}
}

void mcr_SafeString_set_cryptic ( mcr_SafeString * ssPt,
		int cryptic )
{
	dassert ( ssPt ) ;
	if ( cryptic != ssPt->cryptic && ssPt->text.used )
	{
		if ( cryptic )
		{
			mcr_String_to_encrypted ( & ssPt->text, ssPt->tag,
					( unsigned char * ) _pass, ssPt->iv ) ;
		}
		else
		{
			mcr_Array str = mcr_String_from_encrypted ( & ssPt->text,
					ssPt->tag, ( unsigned char * ) _pass, ssPt->iv ) ;
			free ( ssPt->text.array ) ;
			memset ( ssPt->iv, 0, sizeof ( ssPt->iv ) ) ;
			memset ( ssPt->tag, 0, sizeof ( ssPt->tag ) ) ;
			ssPt->text = str ;
		}
		ssPt->cryptic = cryptic ;
	}
}

void mcr_SafeString_recrypt ( mcr_SafeString * ssPt,
		char * oldPass )
{
	dassert ( ssPt ) ;
	if ( ! ssPt->cryptic || MCR_STR_EMPTY ( ssPt->text ) )
		return ;
	mcr_Array textVal = mcr_SafeString_from_password ( ssPt, oldPass ) ;
	mcr_SafeString_set ( ssPt, textVal.array, ssPt->cryptic ) ;
	mcr_Array_free ( & textVal ) ;
}

void mcr_String_to_encrypted ( mcr_Array * toEncryptPt,
		unsigned char * tag, unsigned char * pass, unsigned char * iv )
{
	dassert ( toEncryptPt ) ;
	dassert ( tag ) ;
	dassert ( pass ) ;
	dassert ( iv ) ;
	if ( MCR_STR_EMPTY ( * toEncryptPt ) )
	{
		return ;
	}
	mcr_Array plain ;
	mcr_String_init ( & plain ) ;
	mcr_String_nfrom_string ( & plain, toEncryptPt->array,
			toEncryptPt->used ) ;

	/* Guessing encrypted text length may be
	 * ( length of plain text * 1.5 ) */
	if ( mcr_Array_resize ( toEncryptPt, toEncryptPt->used +
			( toEncryptPt->used / 2 ) ) )
	{
		toEncryptPt->used = mcr_encrypt ( ( unsigned char * ) plain.array,
				( unsigned int ) plain.used - 1, NULL, 0, pass, iv,
				( unsigned char * ) toEncryptPt->array, tag ) ;
	}
	else
		dmsg ;
	mcr_Array_free ( & plain ) ;
}

mcr_Array mcr_String_from_encrypted ( const mcr_Array * encryptedPt,
		const unsigned char * tag, const unsigned char * pass,
		const unsigned char * iv )
{
	dassert ( encryptedPt ) ;
	dassert ( tag ) ;
	dassert ( pass ) ;
	dassert ( iv ) ;
	mcr_Array ret ;
	mcr_String_init ( & ret ) ;
	if ( MCR_STR_EMPTY ( * encryptedPt ) )
		return ret ;
	if ( ! mcr_Array_resize ( & ret, encryptedPt->used ) )
	{
		dmsg ;
		return ret ;
	}

	ret.used = ( size_t ) mcr_decrypt (
			( unsigned char * ) encryptedPt->array,
			( int ) encryptedPt->used, NULL, 0, tag, pass, iv,
			( unsigned char * ) ret.array ) ;
	if ( ret.used == ( size_t ) -1 || ! ret.used )
	{
		dmsg ;
		ret.used = 0 ;
		mcr_String_from_string ( & ret, "\0" ) ;
	}
	else
	{
		unsigned char nullChar = '\0' ;
		mcr_Array_push ( & ret, & nullChar ) ;
	}
	return ret ;
}

void mcr_SafeString_set_password ( char * pass )
{
	if ( pass )
	{
		strncpy ( _pass, pass, 32 ) ;
	}
	else
	{
		memset ( _pass, 0, sizeof ( _pass ) ) ;
	}
}

void mcr_safestring_initialize ( )
{
	mcr_randomize ( ( unsigned char * ) _pass, 32 ) ;
	_pass [ 32 ] = '\0' ;
}

void mcr_safestring_cleanup ( void )
{
}
