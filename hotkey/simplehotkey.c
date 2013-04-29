
# include "hotkey/simplehotkey.h"

namespace macro
{
	Hotkey::Hotkey ( const ISignal * callbackPt, ISignal * interceptPt,
		const int interceptMods, const bool enable,
		const bool specify, const bool blocking )
	{
		// First, we should create the internal hotkey to modify.
		setIntercept ( interceptPt, interceptMods ) ;
		setCallbackObject ( callbackPt ) ;
		this->specify ( specify ) ;
		block ( blocking ) ;
		this->enable ( enable ) ;
	}

	Hotkey::Hotkey ( IHotkey::callback_t callback, ISignal * interceptPt,
		const int interceptMods, const bool enable,
		const bool specify, const bool blocking )
	{
		// First, we should create the internal hotkey to modify.
		setIntercept ( interceptPt, interceptMods ) ;
		setCallback ( callback ) ;
		this->specify ( specify ) ;
		block ( blocking ) ;
		this->enable ( enable ) ;
	}
	Hotkey::Hotkey ( ITrigger * callbackPt, ISignal * interceptPt,
					const int interceptMods, const bool enable,
					const bool specify, const bool blocking )
	{
		// First, we should create the internal hotkey to modify.
		setIntercept ( interceptPt, interceptMods ) ;
		setTriggerObject ( callbackPt ) ;
		this->specify ( specify ) ;
		block ( blocking ) ;
		this->enable ( enable ) ;
	}

	Hotkey::Hotkey ( const std::shared_ptr<IHotkey> & hotty )
		: _hot ( hotty )
	{
		_ensureMember ( ) ;
	}

	Hotkey::Hotkey ( const Hotkey & copytron )
		: _hot ( copytron._hot )
	{
		_ensureMember ( ) ;
	}

	Hotkey & Hotkey::operator = ( const Hotkey & copytron )
	{
		if ( & copytron != this )
		{
			_hot = copytron._hot ;
//			_hot.reset ( copytron._hot.get ( ) ) ;
		}
		return * this ;
	}

	const std::shared_ptr<IHotkey> Hotkey::get ( ) const
	{
		return _hot ;
	}
	void Hotkey::set ( const std::shared_ptr<IHotkey> & newHotkey )
	{
		_hot = newHotkey ;
//		_hot.reset ( newHotkey.get ( ) ) ;
		_ensureMember ( ) ;
	}

	//
	// Functions to modify how registering is handled.
	//
	void Hotkey::specify ( const bool useSpecific )
	{
		_hot->specify ( useSpecific ) ;
	}
	bool Hotkey::isSpecified ( ) const
	{
		return _hot->isSpecified ( ) ;
	}
//	bool Hotkey::isRegistered ( ) const
//	{
//		return _hot->isRegistered ( ) ;
//	}
//	bool Hotkey::canRegister ( ) const
//	{
//		return _hot->canRegister ( ) ;
//	}

	//
	// Functions to modify how signals are intercepted ( This is logically after hotkeys are registered ).
	//
	void Hotkey::setCallbackObject ( const ISignal * newCallbackPt )
	{
		_hot->setCallbackObject ( newCallbackPt ) ;
	}
	const ISignal * Hotkey::getCallbackObject ( ) const
	{
		return _hot->getCallbackObject ( ) ;
	}
	void Hotkey::setCallback ( IHotkey::callback_t newCallback )
	{
		_hot->setCallback ( newCallback ) ;
	}
	IHotkey::callback_t Hotkey::getCallback ( ) const
	{
		return _hot->getCallback ( ) ;
	}
	void Hotkey::setTriggerObject ( ITrigger * newCallbackPt )
	{
		_hot->setTriggerObject ( newCallbackPt ) ;
	}
	const ITrigger * Hotkey::getTriggerObject ( ) const
	{
		return _hot->getTriggerObject ( ) ;
	}
	bool Hotkey::hasCallback ( ) const
	{
		return _hot->hasCallback ( ) ;
	}
	void Hotkey::setIntercept ( ISignal * interceptPt, const int interceptingMods )
	{
		setIntercept ( interceptPt ) ;
		setInterceptMods ( interceptingMods ) ;
	}
	void Hotkey::setIntercept ( ISignal * interceptPt )
	{
//		_ensureMember ( ) ;
		// setIntercept ( string ) will ensure member.
		if ( interceptPt == NULL )
		{
			setIntercept ( Signal<ISignal>::Name ) ;
		}
		else
		{
			setIntercept ( interceptPt->type ( ) ) ;
			_hot->setIntercept ( interceptPt ) ;
		}
	}
	void Hotkey::setInterceptMods ( const int interceptMods )
	{
		_hot->setInterceptMods ( interceptMods ) ;
	}
	void Hotkey::setIntercept ( const std::string & interceptTypeName )
	{
		_ensureMember ( ) ;
		// If same type, no need to go through any other hassle.
		if ( containsAt ( _hot->getIntercept ( )->type ( ).c_str ( ), 0,
						interceptTypeName.c_str ( ) ) )
			return ;
		// store previous
		int m = getInterceptMods ( ) ;
		bool spec = isSpecified ( ) ;
		bool en = isEnabled ( ) ;
		bool blk = isBlocking ( ) ;
		ISignal * sig = const_cast < ISignal * > ( getCallbackObject ( ) ) ;
		callback_t caller = getCallback ( ) ;
		ITrigger * trig = getTriggerObject ( ) ;
		// Set new hotkey from new signal type. This may be NULL.
		_hot = hotFromSignal ( interceptTypeName ) ;
		_ensureMember ( ) ;
		// set the old values
		setInterceptMods ( m ) ;
		if ( sig != NULL )
		{
			setCallbackObject ( sig ) ;
		}
		else if ( caller != NULL )
		{
			setCallback ( caller ) ;
		}
		else
		{
			setTriggerObject ( trig ) ;
		}
		block ( blk ) ;
		specify ( spec ) ;
		enable ( en ) ;
	}
	const ISignal * Hotkey::getIntercept ( ) const
	{
		return _hot->getIntercept ( ) ;
	}
	int Hotkey::getInterceptMods ( ) const
	{
		return _hot->getInterceptMods ( ) ;
	}
	void Hotkey::clear ( )
	{
		_hot->clear ( ) ;
	}
	void Hotkey::enable ( const bool enable )
	{
		_hot->enable ( enable ) ;
	}
	bool Hotkey::isEnabled ( ) const
	{
		return _hot->isEnabled ( ) ;
	}
	void Hotkey::block ( const bool enable )
	{
		_hot->block ( enable ) ;
	}
	bool Hotkey::isBlocking ( ) const
	{
		return _hot->isBlocking ( ) ;
	}

	class StagedHotkey ;
	std::map<std::string, hotkey_return_t> defaultHotMap ( )
	{
		std::string names [ ] =
		{
			Signal<ISignal>::Name,
			Signal<Alarm>::Name, Signal<HIDEcho>::Name,
			Signal<Key>::Name, Signal<MoveCursor>::Name,
			Signal<NoOp>::Name, Signal<Scroll>::Name,
			Signal<KeyMod>::Name, Signal<SignalMod>::Name,
			Signal<Hotkey>::Name, Signal<StagedHotkey>::Name
		} ;
		std::map<std::string, hotkey_return_t> ret ;
		hotkey_return_t fncs [ ] =
		{
			& getObject<IHotkey, Hot<ISignal>>,
			& getObject<IHotkey, Hot<Alarm>>, & getObject<IHotkey, Hot<HIDEcho>>,
			& getObject<IHotkey, Hot<Key>>, & getObject<IHotkey, Hot<MoveCursor>>,
			& getObject<IHotkey, Hot<NoOp>>, 	& getObject<IHotkey, Hot<Scroll>>,
			& getObject<IHotkey, Hot<KeyMod>>, & getObject<IHotkey, Hot<SignalMod>>,
			& getObject<IHotkey, Hot<Hotkey>>, 	& getObject<IHotkey, Hot<StagedHotkey>>
		} ;
		unsigned int len = sizeof names / sizeof ( std::string ) ;
		std::string name, origin ;
		for ( unsigned int i = 0 ; i < len ; i++ )
		{
			origin = name = names [ i ] ;
			std::transform ( name.begin ( ), name.end ( ), name.begin ( ), toupper ) ;
			ret.erase ( name ) ;
			ret.insert ( std::make_pair ( name, fncs [ i ] ) ) ;
			std::transform ( name.begin ( ), name.end ( ), name.begin ( ), tolower ) ;
			ret.erase ( name ) ;
			ret.insert ( std::make_pair ( name, fncs [ i ] ) ) ;
			ret.erase ( origin ) ;
			ret.insert ( std::make_pair ( origin, fncs [ i ] ) ) ;
		}
		return ret ;
	}
	std::map<std::string, hotkey_return_t> HotMap = defaultHotMap ( ) ;
}
