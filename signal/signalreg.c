/* signal/signalreg.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/signal.h"

static mcr_IRegistry _isigReg ;

void mcr_Signal_load_contract ( )
{
	mcr_ISignal * sigs [ ] =
	{
		& mcr_iAlarm, & mcr_iHIDEcho, & mcr_iKey,
		& mcr_iMoveCursor, & mcr_iNoOp, & mcr_iScroll
	} ;
	const char * names [ ] =
	{
		"Alarm", "Echo", "Key",
		"MoveCursor", "NoOp", "Scroll"
	} ;
	const char * echoAdd [ ] =
	{
		"HIDEcho", "hid echo", "hid_echo"
	} ;
	const char * mcAdd [ ] =
	{
		"move cursor", "move_cursor"
	} ;
	int len = sizeof ( sigs ) / sizeof ( mcr_ISignal * ) ;
	for ( int i = 0 ; i < len ; i ++ )
	{
		mcr_ISignal_set_name ( sigs [ i ], names [ i ] ) ;
	}
	mcr_ISignal_add_names ( & mcr_iHIDEcho, echoAdd, 3 ) ;
	mcr_ISignal_add_names ( & mcr_iMoveCursor, mcAdd, 2 ) ;
}

//
// Signal development: Add signal type to end, replace signal type,
//
size_t mcr_ISignal_register ( mcr_ISignal * newType )
{
	dassert ( newType ) ;
	if ( mcr_iregister ( & _isigReg, ( mcr_Interface * ) newType ) ==
			( size_t ) -1 )
	{
		dmsg ;
		return -1 ;
	}
	return newType->interface.id ;
}

size_t mcr_ISignal_register_with ( mcr_ISignal * newType,
		const char * name, const char ** addNames,
		size_t bufferLen )
{
	if ( mcr_ISignal_register ( newType ) == ( size_t ) -1 )
	{
		dmsg ;
		return -1 ;
	}
	if ( ! mcr_ISignal_set_names ( newType, name, addNames, bufferLen ) )
	{
		dmsg ;
	}
	return newType->interface.id ;
}

mcr_ISignal * mcr_ISignal_get ( size_t typeId )
{
	return ( mcr_ISignal * ) mcr_iget ( & _isigReg, typeId ) ;
}

mcr_ISignal * mcr_ISignal_from_name ( const char * typeName )
{
	return ( mcr_ISignal * ) mcr_ifrom_name ( & _isigReg, typeName ) ;
}

size_t mcr_ISignal_get_id ( const char * typeName )
{
	return mcr_iget_id ( & _isigReg, typeName ) ;
}

const char * mcr_ISignal_get_name ( size_t id )
{
	return mcr_iget_name ( & _isigReg, id ) ;
}

int mcr_ISignal_set_name ( mcr_ISignal * sigPt,
		const char * name )
{
	if ( ! mcr_iset_name ( & _isigReg, ( mcr_Interface * ) sigPt, name ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_ISignal_add_name ( mcr_ISignal * sigPt,
		const char * name )
{
	if ( ! mcr_iadd_name ( & _isigReg, ( mcr_Interface * ) sigPt, name ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

int mcr_ISignal_add_names ( mcr_ISignal * sigPt,
		const char ** names, size_t bufferLen )
{
	dassert ( sigPt ) ;
	dassert ( names ) ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < bufferLen && names [ i ] != NULL ; i ++ )
	{
		if ( ! mcr_ISignal_add_name ( sigPt, names [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

int mcr_ISignal_set_names ( mcr_ISignal * sigPt,
		const char * name,
		const char ** extraNames, size_t bufferLen )
{
	int ret = 1 ;
	if ( ! mcr_ISignal_set_name ( sigPt, name ) )
	{
		dmsg ;
		ret = 0 ;
	}
	if ( ! mcr_ISignal_add_names ( sigPt, extraNames, bufferLen ) )
	{
		dmsg ;
		ret = 0 ;
	}
	return ret ;
}

int mcr_ISignal_rename ( const char * oldName,
		const char * newName )
{
	if ( ! mcr_irename ( & _isigReg, oldName, newName ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

size_t mcr_ISignal_count ( )
{
	return _isigReg.set.used ;
}

void mcr_ISignal_get_all ( mcr_ISignal ** buffer, size_t bufferLength )
{
	mcr_iget_all ( & _isigReg, ( mcr_Interface ** ) buffer, bufferLength ) ;
}

void mcr_ISignal_clear ( )
{
	mcr_iclear ( & _isigReg ) ;
}

void mcr_signalreg_initialize ( )
{
	mcr_iinit_maps ( & _isigReg ) ;
}

void mcr_signalreg_cleanup ( void )
{
	mcr_ISignal_clear ( ) ;
}
