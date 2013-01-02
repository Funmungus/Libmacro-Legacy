/*! \file signalfactory.h
 * \brief SignalFactory, Create signals from type name.
 */

# ifndef FUN_SIGNAL_FACTORY_H
# define FUN_SIGNAL_FACTORY_H

# include "signal/signal.h"

namespace funlibrary
{

	//! \brief Create signals dynamically, from the name of a signal type.
	class  SignalFactory
	{
	public :
		/*! \brief Get a new signal from the type name.
		 * 
		 * The returned signal will be made with the new operator, or
		 * Null if no correct signal is found for the name.
		 * \param signalName This signal name should be the same as
		 * what is returned from its ISignal#type() function, and
		 * its public name variable.
		 */
		static SIGNAL_API ISignal * get ( std::string signalName ) ;
		/*! \brief Get a new signal of inVal's type, and copy inVal into the new signal.
		 * 
		 * If no signal is found for this type, NULL is returned.
		 */
		static SIGNAL_API ISignal * copy ( const ISignal * inVal ) ;
	protected :
	private :
		static SignalFactory _instance ;
		SignalFactory ( ) ;
		typedef ISignal * (* SignalReturn) ( void ) ;
		typedef ISignal * (* CopyReturn) ( const ISignal * ) ;
		// Map from keyString to keycode.
		static std::map < std::string, SignalReturn > _functionMap ;
		static std::map < std::string, CopyReturn > _copyMap ;

		static ISignal * getKey ( ) ;
		static ISignal * getSpace ( ) ;
		static ISignal * getMods ( ) ;
		static ISignal * getScript ( ) ;
		static ISignal * getCom ( ) ;
		static ISignal * getNoOp ( ) ;
		static ISignal * getKeyString ( ) ;
		static ISignal * copyKey ( const ISignal * inVal ) ;
		static ISignal * copySpace ( const ISignal * inVal ) ;
		static ISignal * copyMods ( const ISignal * inVal ) ;
		static ISignal * copyScript ( const ISignal * inVal ) ;
		static ISignal * copyCom ( const ISignal * inVal ) ;
		static ISignal * copyNoOp ( const ISignal * inVal ) ;
		static ISignal * copyKeyString ( const ISignal * inVal ) ;

		static void add ( std::string lowerName, SignalReturn func , CopyReturn cFunc) ;
	};
}