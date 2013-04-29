
# include "macro/factory.h"
# include "macro/macro.h"

namespace macro
{
	bool factoryDefaultMacro ( )
	{
		SignalFactory::replace ( Macro::name, getObject<ISignal, Macro>,
								copyObject<ISignal, Macro> ) ;
		replaceXmlMapping ( Macro::name, macroXR, & SignalXRMap ) ;
		return true ;
	}

	void macroXR ( ISignal * receiver, rapidxml::xml_node < char > * obj )
	{
		// complex types IHotkey and set
		// normal types name, enable, sticky
		if ( receiver == NULL || obj == 0 || obj->name ( ) == NULL ) return ;

		Macro * mcr ;
		std::unique_ptr < ISignal > sig ( SignalFactory::get ( obj->name ( ) ) ) ;
		if ( sig.get ( ) != NULL )
		{
			// only possible to be hotkey
			IHotkey * hkey = dynamic_cast < IHotkey * > ( sig.get ( ) ) ;
			if ( hkey != NULL )
			{
				mcr = static_cast < Macro * > ( receiver ) ;
				mcr->setHotkey ( hkey ) ;
			}
		}
		else if ( containsAt ( obj->name ( ), 0, "set", true ) )
		{
			mcr = static_cast < Macro * > ( receiver ) ;
			// Moving through the signalSet. Every child node should be a signal to add.
			for ( auto child = obj->first_node ( ) ;
				child != 0 ; child = child->next_sibling ( ) )
			{
				if ( child->name ( ) != NULL )
				{
					sig.reset ( SignalFactory::get ( child->name ( ) ) ) ;
					// Received object from factory.
					if ( sig.get ( ) != NULL )
					{
						// Have signal, set its values with its own SignalXR in SignalXRMap.
						SignalXR fnc = SignalXRMap.find ( child->name ( ) )->second ;
						if ( fnc != NULL )
						{
							fnc ( sig.get ( ), child ) ;
						}
						mcr->push_back ( sig.get ( ) ) ;
					}
				}
			}
		}
		else
		{
			signalBaseReceiver ( receiver, obj ) ;
		}
	}

	bool _Init_Macro_Factory_ = factoryDefaultMacro ( ) ;
}
