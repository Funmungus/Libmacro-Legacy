
# include "signal/keystring.h"

namespace macro
{
	const std::string KeyString::name = "keystring" ;
	bool ( * KeyString::dispatcher ) ( KeyString & ) ;

	void KeyString::replaceCharacterMapping ( const int character, const int key,
										const bool shifted )
	{
		if ( _keyVals.size ( ) <= ( unsigned char ) character )
		{
			_keyVals.resize ( character + 1, 0 ) ;
		}
		_keyVals [ character ] = key ;
		if ( _needShift.size ( ) <= ( unsigned char ) character )
		{
			_needShift.resize ( character + 1, false ) ;
		}
		_needShift [ character ] = shifted ;

		std::pair < int, bool > mappi ( key, shifted ) ;
		_charMap.erase ( mappi ) ;
		_charMap.insert ( std::pair < std::pair < int, bool >, int >
						( mappi, character ) ) ;
	}

	void KeyString::removeCharacterMapping ( const int character )
	{
		if ( ( unsigned char ) character < _keyVals.size ( ) )
		{
			std::pair < int, bool > mPair
					( _keyVals [ character ], _needShift [ character ] ) ;
			_charMap.erase ( mPair ) ;
			_keyVals [ character ] = 0 ;
		}
	}

	void KeyString::trim ( )
	{
		unsigned int lastIndex = -1 ;
		for ( unsigned int i = _keyVals.size ( ) - 1 ;
			i + 1 > 0 ; i-- )
		{
			if ( _keyVals [ i ] != 0 )
			{
				lastIndex = i ;
			}
		}

		for ( unsigned int i = _keyVals.size ( ) - 1 ;
			i > lastIndex ; i-- )
		{
			std::pair < int, bool > mPair ;
			mPair.first = _keyVals [ i ] ;
			mPair.second = _needShift [ i ] ;
			_charMap.erase ( mPair ) ;
		}

		_keyVals.resize ( lastIndex + 1 ) ;
		_needShift.resize ( lastIndex + 1 ) ;
	}

	void KeyString::clearCharaterMapping ( )
	{
		_charMap.clear ( ) ;
		_keyVals.resize ( 0 ) ;
		_needShift.resize ( 0 ) ;
	}

	// static
	KeyString KeyString::fromKeys ( std::vector<std::unique_ptr<ISignal> > & keySet )
	{
		bool shift = false ;
		bool keyDown = false ;
		unsigned int shiftMod = Mods::get ( Mods::shift ) ;
		unsigned int modVal = 0 ;
		unsigned long long delay = 0, interval = 0 ;
		std::string val = "" ;
		int character ;
//		pair < int, bool > shifted ;

		// Handle Keys ( key code and shifting ), and NoOp ( delay and interval ).
		for ( auto it = keySet.begin ( ) ; it != keySet.end ( ) ; it ++ )
		{
			// Should most often be Key signal or pause
			KeySignal * key = dynamic_cast < KeySignal * > ( it->get ( ) ) ;
			if ( key != NULL )
			{
				modVal = Mods::modFromKey ( key->getKey ( ) ) ;
				if ( ( modVal & shiftMod ) != 0 )
				{
					switch ( key->getKeyUp ( ) )
					{
					case DOWN :
						shift = true ;
						break ;
					case UP :
					case BOTH :
						shift = false ;
						break ;
					}
				}
				else
				{
					switch ( key->getKeyUp ( ) )
					{
					case DOWN :
						character = charFromKey ( key->getKey ( ), shift ) ;
						keyDown = true ;
						val += character ;
						break ;
					case BOTH :
						character = charFromKey ( key->getKey ( ), shift ) ;
						val += character ;
					case UP :
						keyDown = false ;
						break ;
					}
				}
				continue ;
			}
			NoOp * pause = dynamic_cast < NoOp * > ( it->get ( ) ) ;
			if ( pause != NULL )
			{
				if ( keyDown )
				{
					delay = ( delay == 0 ) ?
								pause->get ( ) :
								( delay + pause->get ( ) ) / 2 ;
				}
				else
				{
					interval = ( interval == 0 ) ?
								pause->get ( ) :
								( interval + pause->get ( ) ) / 2 ;
				}
				continue ;
			}
			Mods * mod = dynamic_cast < Mods * > ( it->get ( ) ) ;
			if ( mod != NULL )
			{
				if ( mod->hasMod ( shiftMod ) )
				{
					switch ( mod->getKeyUp ( ) )
					{
					case DOWN :
						shift = false ;
						break ;
					case UP :
					case BOTH :
						shift = true ;
						break ;
					}
				}
				continue ;
			}

		}
		return KeyString ( val, delay, interval ) ;
	}
	void KeyString::toKeys ( std::vector<std::unique_ptr<ISignal> > & retVals )
	{
		retVals.clear ( ) ;

		std::string text = getText ( ) ;
		if ( text.empty ( ) ) return ;

		Mods shiftKey = Mods ( Mods::get ( Mods::shift ), DOWN ) ;
		bool shifted = false, toShift = false ;
		unsigned long long delay = getDelay ( ) ;
		unsigned long long interval = getInterval ( ) ;
		int newKey ;

		for ( auto it = text.begin ( ) ; it != text.end ( ) ; it ++ )
		{
			toShift = needShift ( * it ) ;
			if ( shifted )
			{
				if ( ! toShift )
				{
					shiftKey.setKeyUp ( UP ) ;
					retVals.push_back ( std::unique_ptr < ISignal > ( new Mods ( shiftKey ) ) ) ;
					shifted = false ;
				}
			}
			else
			{
				if ( toShift )
				{
					shiftKey.setKeyUp ( DOWN ) ;
					retVals.push_back ( std::unique_ptr < ISignal > ( new Mods ( shiftKey ) ) ) ;
					shifted = true ;
				}
			}

			newKey = keyFromChar ( * it ) ;
			if ( newKey == 0 ) continue ;

			if ( delay == 0 )
			{
				retVals.push_back ( std::unique_ptr < ISignal > ( new KeySignal ( newKey, 0, BOTH ) ) ) ;
			}
			else
			{
				retVals.push_back ( std::unique_ptr < ISignal > ( new KeySignal ( newKey, 0, DOWN ) ) ) ;
				retVals.push_back ( std::unique_ptr < ISignal > ( new NoOp ( delay ) ) ) ;
				retVals.push_back ( std::unique_ptr < ISignal > ( new KeySignal ( newKey, 0, UP ) ) ) ;
			}
			if ( interval != 0 )
				retVals.push_back ( std::unique_ptr < ISignal > ( new NoOp ( interval ) ) ) ;
		}
	}

	std::string KeyString::type ( ) const
	{
		return name ;
	}

	bool KeyString::send ( )
	{
		if ( dispatch ( ) ) return true ;
		Mods shifty ( Mods::get ( Mods::shift ) ) ;
		KeySignal key ;
		for ( unsigned int pt = 0 ; pt < _text.size ( ) ; )
		{
			bool sendingShifted = needShift ( _text [ pt ] ) ;
			if ( sendingShifted )
				shifty.setKeyUp ( DOWN ) ;
			else
				shifty.setKeyUp ( UP ) ;
			shifty.send ( ) ;
			_delay.send ( ) ;
			while ( pt < _text.size ( ) &&
					needShift ( _text [ pt ] ) == sendingShifted )
			{
				key.setKey ( keyFromChar ( _text [ pt ] ) ) ;
				if ( ! sendWithDelay ( & key ) )
					return false ;
				++pt ;
			}
		}

		return true ;
	}
}
