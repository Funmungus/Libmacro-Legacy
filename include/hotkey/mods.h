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

/*! \file hotkey/mods.h
 * \brief mcr_Mods, signal to modify \ref mcr_internalMods, and
 * handle known modifiers
 * */

# ifndef MCR_MODS_H
# define MCR_MODS_H

# include "hotkey/def.h"

//! \brief Modify \ref mcr_InternalMods with either set or release.
MCR_API extern mcr_ISignal mcr_iMod ;

/*! \brief Pair of modifier bits and whether to set or release those
 * modifiers <br>
 * May be sent with \ref mcr_iMod
 * */
typedef struct mcr_Mod
{
	//! \brief Bit values representing a set of modifiers.
	unsigned int modifiers ;
	/*! \brief \ref MCR_DOWN will set the modifier, \ref MCR_TOGGLE
	 * will change the state from what it currently is, otherwise it is
	 * released */
	mcr_KeyUpType up_type ;
} mcr_Mod ;

MCR_API int mcr_Mod_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_Mod_compare ( const void * lhs, const void * rhs ) ;

//
// Generic, known modifiers.
//
/*!
 * \brief Enumeration for known and historical modifiers.
 *
 * To emulate how an operating system combines the names of some
 * modifiers, the names of those modifiers will map to the same value.
 * <br>
 * Some of these are actually hardware switches, depending on the
 * keyboard. If relying on keyboard or key codes to set modifiers, some
 * may never be used.
 * <br>
 * These values may change. Other values may also be
 * defined in other modules.
 * */
typedef enum mcr_ModTypes
{
	// Alt and option are always the same here.
	MCR_ALT		= 0x000001,
	MCR_OPTION	= 0x000001,
	MCR_ALTGR	= 0x000002,
	MCR_AMIGA	= 0x000004,
	MCR_CMD		= 0x000008,
	MCR_CODE	= 0x000100,
	MCR_COMPOSE	= 0x000200,
	MCR_CTRL	= 0x000400,
	MCR_FN		= 0x000800,
	MCR_FRONT	= 0x001000,
	MCR_GRAPH	= 0x002000,
	MCR_HYPER	= 0x004000,
	MCR_META	= 0x008000,
	MCR_SHIFT	= 0x010000,
	MCR_SUPER	= 0x020000,
	MCR_SYMBOL	= 0x040000,
	MCR_TOP		= 0x080000,
	MCR_WIN		= 0x100000
} mcr_ModTypes ;

MCR_API void mcr_Mods_load_contract ( ) ;

// Names
MCR_API unsigned int mcr_Mod_from ( const char * name ) ;
MCR_API const char * mcr_Mod_name (
		const unsigned int modifier ) ;
MCR_API size_t mcr_Mod_count ( ) ;
MCR_API void mcr_Mod_all ( unsigned int * modBuffer,
		const size_t bufferLength ) ;
// Echo
MCR_API mcr_Mod mcr_Mod_from_echo ( const int echo ) ;
MCR_API int mcr_Mod_echo ( const mcr_Mod modifier ) ;
MCR_API size_t mcr_Mod_echo_count ( ) ;
MCR_API void mcr_Mod_echo_all ( int * echoBuffer,
		const size_t bufferLength ) ;
// Key
MCR_API unsigned int mcr_Mod_from_key ( const int key ) ;
MCR_API int mcr_Mod_key_key ( const unsigned int modifier ) ;
MCR_API size_t mcr_Mod_key_count ( ) ;
MCR_API void mcr_Mod_key_all ( int * keyBuffer,
		const size_t bufferLength ) ;

//
// Modify modifiers
//
MCR_API unsigned int mcr_Mod_combine ( const unsigned int * modsArray,
		const size_t length ) ;
MCR_API void mcr_Mod_modify ( mcr_Mod * modPt,
		const unsigned int modifier, const mcr_KeyUpType modifierKeyUp ) ;
MCR_API unsigned int mcr_Mod_modify_bits ( const unsigned int mods,
		const unsigned int modifier, const mcr_KeyUpType keyUp ) ;
MCR_API unsigned int mcr_Mod_add ( const unsigned int mods,
		const unsigned int newMod ) ;
MCR_API int mcr_Mod_has ( const unsigned int mods,
		const unsigned int modVal ) ;
MCR_API unsigned int mcr_Mod_remove ( const unsigned int mods,
		const unsigned int delMod ) ;

//
// Modifier providers for standard types.
//
MCR_API unsigned int mcr_HIDEcho_modify ( mcr_Signal * sigPt,
		unsigned int mods ) ;
MCR_API unsigned int mcr_Key_modify ( mcr_Signal * sigPt,
		unsigned int mods ) ;

//
// Modifier development.
//
// Names
MCR_API int mcr_Mod_set_name ( const unsigned int modifier,
		const char * name ) ;
MCR_API int mcr_Mod_add_name ( const unsigned int modifier,
		const char * name ) ;
MCR_API int mcr_Mod_add_names ( const unsigned int modifier,
		const char ** addNames, size_t bufferLen ) ;
MCR_API int mcr_Mod_set_names ( const unsigned int modifier,
		const char * name, const char ** addNames,
		size_t bufferLen ) ;
MCR_API int mcr_Mod_rename ( const unsigned int modifier,
		const char * newName ) ;
MCR_API int mcr_Mod_rename_by_name ( const char * oldName,
		const char * newName ) ;
MCR_API void mcr_Mod_clear_names ( ) ;
// Echo
MCR_API int mcr_Mod_set_echo ( const mcr_Mod modifiers,
		const int echo ) ;
MCR_API int mcr_Mod_add_echo ( const mcr_Mod modifiers,
		const int echo ) ;
MCR_API int mcr_Mod_add_echos ( const mcr_Mod modifiers,
		const int * addEchos, size_t bufferLen ) ;
MCR_API int mcr_Mod_set_echos ( const mcr_Mod modifiers,
		const int echo, const int * addEchos, size_t bufferLen ) ;
MCR_API int mcr_Mod_reecho ( const mcr_Mod modifiers,
		const int echo ) ;
MCR_API int mcr_Mod_reecho_by_echo ( const int oldEcho,
		const int newEcho ) ;
MCR_API void mcr_Mod_clear_echo ( ) ;
// Key
MCR_API int mcr_Mod_set_key ( const unsigned int modifiers,
		const int key ) ;
MCR_API int mcr_Mod_add_key ( const unsigned int modifiers,
		const int key ) ;
MCR_API int mcr_Mod_add_keys ( const unsigned int modifiers,
		const int * addKeys, size_t bufferLen ) ;
MCR_API int mcr_Mod_set_keys ( const unsigned int modifiers,
		const int key, const int * addKeys, size_t bufferLen ) ;
MCR_API int mcr_Mod_rekey ( const unsigned int modifiers,
		const int key ) ;
MCR_API int mcr_Mod_rekey_by_key ( const int oldKey,
		const int newKey ) ;
MCR_API void mcr_Mod_clear_key ( ) ;
// All
MCR_API void mcr_Mod_clear_all ( ) ;

MCR_API void mcr_mods_initialize ( ) ;
MCR_API void mcr_mods_cleanup ( ) ;

//
// Implement in native directory.
//
MCR_API void mcr_Mods_load_key_contract ( ) ;

# define MCR_MOD_ADD( toModify, modifiers ) \
	( toModify ) |= ( modifiers ) ;

# define MCR_MOD_HAS( modifiers, hasMod ) \
	( ( modifiers & hasMod ) == hasMod )

# define MCR_MOD_REMOVE( toModify, modifiers ) \
	( toModify ) &= ~ ( modifiers ) ;

# define MCR_MOD_MODIFY_BITS( toModify, modifiers, upType ) \
	if ( ( upType ) == MCR_DOWN ) \
	{ \
		MCR_MOD_ADD ( toModify, modifiers ) ; \
	} \
	else if ( ( upType ) == MCR_TOGGLE ) \
	{ \
		if ( MCR_MOD_HAS ( toModify, modifiers ) ) \
		{ \
			MCR_MOD_REMOVE ( toModify, modifiers ) ; \
		} \
		else \
		{ \
			MCR_MOD_ADD ( toModify, modifiers ) ; \
		} \
	} \
	else \
	{ \
		MCR_MOD_REMOVE ( toModify, modifiers ) ; \
	}

# define MCR_MOD_SEND( modPt, success ) \
	MCR_MOD_MODIFY_BITS ( mcr_internalMods, ( modPt )->modifiers, \
			( modPt )->up_type ) ;

# endif
