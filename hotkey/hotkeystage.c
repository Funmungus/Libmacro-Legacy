//
//# include "hotkey/hotkeystage.h"
//
//namespace macro
//{
//	const std::string HotkeyStage::Name = "Stage" ;
//
//	HotkeyStage::HotkeyStage ( const ISignal * intercept, const Mods & interceptMods,
//		const bool activated, const bool blocking, const int measurementError )
//		: activated ( activated ), blocking ( blocking ),
//		interceptMods ( interceptMods ), measurementError ( measurementError ),
//		_intercept ( nullptr ), _type ( NONE )
//	{
//		setIntercept ( intercept ) ;
//	}
//
//	HotkeyStage::HotkeyStage ( const HotkeyStage & copytron )
//		: activated ( copytron.activated ), blocking ( copytron.blocking ),
//		interceptMods ( copytron.interceptMods ),
//		measurementError ( copytron.measurementError ), _intercept ( nullptr ),
//		_type ( NONE )
//	{
//		setIntercept ( copytron._intercept.get ( ) ) ;
//	}
//
//	HotkeyStage & HotkeyStage::operator= ( const HotkeyStage & rhs )
//	{
//		if ( this != & rhs )
//		{
//			activated = rhs.activated ;
//			blocking = rhs.blocking ;
//			measurementError = rhs.measurementError ;
//			setIntercept ( rhs._intercept.get ( ) ) ;
//			interceptMods = rhs.interceptMods ;
//		}
//		return * this ;
//	}
//
//	void HotkeyStage::getIntercept ( std::unique_ptr < ISignal > * outVal ) const
//	{
//		if ( outVal == NULL ) return ;
//
//		switch ( _type )
//		{
//		case KEY :
//			outVal->reset ( new KeySignal ( * ( KeySignal * ) _intercept.get ( ) ) ) ;
//			break ;
//		case SPACE :
//			outVal->reset ( new SpaceSignal ( * ( SpaceSignal * ) _intercept.get ( ) ) ) ;
//			break ;
//		case NONE :
//			outVal->reset ( nullptr ) ;
//			break ;
//		}
//	}
//	void HotkeyStage::setIntercept ( const ISignal * newIntercept )
//	{
//		const KeySignal * kp = dynamic_cast < const KeySignal * > ( newIntercept ) ;
//		if ( kp != NULL )
//		{
//			_intercept.reset ( new KeySignal ( * kp ) ) ;
//			_type = KEY ;
//			return ;
//		}
//		const SpaceSignal * sp = dynamic_cast < const SpaceSignal * > ( newIntercept ) ;
//		if ( sp != NULL )
//		{
//			_intercept.reset ( new SpaceSignal ( * sp ) ) ;
//			_type = SPACE ;
//			return ;
//		}
//		_type = NONE ;
//	}
//	InterceptType HotkeyStage::getInterceptType ( ) const
//	{
//		return _type ;
//	}
//
//	bool HotkeyStage::activateMe ( const KeySignal * key, const Mods * mod )
//	{
//		if ( _type != KEY ) return false ;
//		KeySignal * mKey = static_cast < KeySignal * > ( _intercept.get ( ) ) ;
//
//		// For key, current activation affects how key up is managed.
//		if ( ( mKey->getKey ( ) == key->getKey ( ) || mKey->getScan ( ) == key->getScan ( ) )
//			&& interceptMods == * mod )
//		{
//			//if ( activated )
//				//return true ;
//
//			KeyUpType me = mKey->getKeyUp ( ), them = key->getKeyUp ( ) ;
//			if ( me == BOTH || them == BOTH || me == them )
//			{
//				activated = true ;
//				return true ;
//			}
//		}
//
//		activated = false ;
//		return false ;
//	}
//
//	bool HotkeyStage::activateMe ( const SpaceSignal * space, const Mods * mod )
//	{
//		if ( _type != SPACE ) return false ; 	// Non-space has no effect.
//		// In space there is no key up, so current activation does not matter.
//		SpaceSignal * mSpace = static_cast < SpaceSignal * > ( _intercept.get ( ) ) ;
//
//		int me = mSpace->getEvent ( ), them = space->getEvent ( ) ;
//		if ( me == them && interceptMods == * mod )
//		{
//			//if ( activated )
//			//return true ;
//
//			if ( mSpace->resembles ( * space, measurementError ) )
//			{
//				activated = true ;
//				return true ;
//			}
//		}
//		activated = false ;
//		return false ;
//	}
//
//}
