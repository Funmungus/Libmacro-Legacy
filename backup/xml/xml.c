
# include "factory/factory.h"

namespace macro
{
	XmlReceiverMap < ISignal > defaultSignalXRMap ( )
	{
		XmlReceiverMap < ISignal > ret ;
		replaceXmlMapping < ISignal > ( ComSignal::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( KeySignal::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( KeyString::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( Mods::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( NoOp::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( ScreenCrop::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( ScriptSignal::name, signalBaseReceiver, & ret ) ;
		replaceXmlMapping < ISignal > ( SpaceSignal::name, spaceReceiver, & ret ) ;
		return ret ;
	}

	XmlReceiverMap < ISignal > SignalXRMap = defaultSignalXRMap ( ) ;

	//
	// Util receiveXml uses the SignalXR for the object's node, including all children.
	//

	void gimmeXml ( std::vector < std::unique_ptr < ISignal > > * container,
		const std::string & allSiggy )
	{
		if ( container == NULL || allSiggy.empty ( ) ) return ;
# include "util/xml.h"

namespace macro
{
	//std::map < std::string, XmlHelperFunction > receivXmlHelperMap ;
	/* std::map < const std::string, const NameValueReceiver::ReceiveTypeNameValue > NameValueReceiver::_receiverMap ;

	bool receiveNamedString ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedString * receiver = dynamic_cast < ICallNamedString * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedString : No receiver provided.\n\n" ) ;
			return false ;
		}

		receiver->call ( valueName, value ) ;
		return true ;
	}
	bool receiveNamedBool ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedBool * receiver = dynamic_cast < ICallNamedBool * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedBool : No receiver provided.\n\n" ) ;
			return false ;
		}

		bool val = false ;
		try
		{
			val = convertBool ( value ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "receiveNamedBool : Incorrect bolean conversion from %s.\n\n", value ) ;
			return false ;
		}
		receiver->call ( valueName, val ) ;
		return true ;
	}
	bool receiveNamedInt ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedInt * receiver = dynamic_cast < ICallNamedInt * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedInt : No receiver provided.\n\n" ) ;
			return false ;
		}

		int val = 0 ;
		try
		{
			val = std::stoi ( value ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "receiveNamedInt : Incorrect integer conversion from %s.\n\n", value ) ;
			return false ;
		}
		receiver->call ( valueName, val ) ;
		return true ;
	}
	bool receiveNamedLongLong ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedLongLong * receiver = dynamic_cast < ICallNamedLongLong * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedLongLong : No receiver provided.\n\n" ) ;
			return false ;
		}

		long long val = 0 ;
		try
		{
			val = std::stoll ( value ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "receiveNamedLongLong : Incorrect long long conversion from %s\n\n", value ) ;
			return false ;
		}
		receiver->call ( valueName, val ) ;
		return true ;
	}
	bool receiveNamedUnsigned ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedUnsigned * receiver = dynamic_cast < ICallNamedUnsigned * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedUnsigned : No receiver provided.\n\n" ) ;
			return false ;
		}

		unsigned long tmpVal = 0 ;
		unsigned int val = 0 ;
		try
		{
			tmpVal = std::stoul ( value ) ;
			val = tmpVal > ( unsigned long ) ( ( unsigned int ) -1 ) ? -1 : ( unsigned int ) tmpVal ;
		}
		catch (... )
		{
			fprintf ( stderr, "receiveNamedUnsigned : Incorrect unsigned int conversion from %s.\n\n", value ) ;
			return false ;
		}
		receiver->call ( valueName, val ) ;
		return true ;
	}
	bool receiveNamedUnsignedLongLong ( FunObject * receivingObj, const std::string & valueName, const std::string & value )
	{
		ICallNamedUnsignedLongLong * receiver = dynamic_cast < ICallNamedUnsignedLongLong * > ( receivingObj ) ;
		if ( receiver == NULL )
		{
			fprintf ( stderr, "receiveNamedUnsignedLongLong : No receiver is provided.\n\n.\n\n" ) ;
			return false ;
		}

		unsigned long long val = 0 ;
		try
		{
			val = std::stoull ( value ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "receiveNamedUnsignedLongLong : Incorrect unsigned long long conversion from %s.\n\n", value ) ;
			return false ;
		}
		receiver->call ( valueName, val ) ;
		return true ;
	}

	void NameValueReceiver::addMap ( const std::string & valueType, const ReceiveTypeNameValue newFunc )
	{
		std::string upDown = valueType ;
		add ( upDown, newFunc ) ;
		std::transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::tolower ) ; ;
		add ( upDown, newFunc ) ;
		std::transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::toupper ) ; ;
		add ( upDown, newFunc ) ;
	}
	void NameValueReceiver::overrideMap ( const std::string & valueType, const ReceiveTypeNameValue newFunc )
	{
		removeMap ( valueType ) ;
		addMap ( valueType, newFunc ) ;
	}
	void NameValueReceiver::removeMap ( const std::string & valueType )
	{
		std::string upDown = valueType ;
		_receiverMap.erase ( upDown ) ;
		transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::tolower ) ;
		_receiverMap.erase ( upDown ) ;
		transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::toupper ) ;
		_receiverMap.erase ( upDown ) ;
	}
	void NameValueReceiver::clear ( )
	{
		_receiverMap.clear ( ) ;
	}

	bool NameValueReceiver::receive ( FunObject * receivingObj, const std::string & valueType, const std::string & valueName, const std::string & value )
	{
		auto callFunc = _receiverMap.find ( valueType ) ;
		if ( callFunc == _receiverMap.end ( ) ) return false ;
		return callFunc->second ( receivingObj, valueName, value ) ;
	}

	NameValueReceiver::NameValueReceiver ( )
	{
		addMap ( "string", receiveNamedString ) ;
		addMap ( "bool", receiveNamedBool ) ;
		addMap ( "int", receiveNamedInt ) ;
		addMap ( "integer", receiveNamedInt ) ;
		addMap ( "unsigned", receiveNamedUnsigned ) ;
		addMap ( "long", receiveNamedLongLong ) ;
		addMap ( "unsigned_long", receiveNamedUnsignedLongLong ) ;
	}
	NameValueReceiver NameValueReceiver::_instance ; */

	bool verifyNodeName ( rapidxml::xml_node < char > * node,
		const char * typeName )
	{
		if ( node == NULL )
			return false ;

		char * mNodeName = node->name ( ) ;
		if ( mNodeName == NULL )
			return false ;

		bool isName = containsAt ( mNodeName, 0, typeName, true ) ;

		return isName ;
	}

	bool verifyNodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen, const char * attrVal )
	{
		char * mAttrVal = nodeAttribute ( node, attrName, attrLen ) ;

		if ( mAttrVal == NULL )
			return false ;

		bool modori = containsAt ( mAttrVal, 0, attrVal, true ) ;

		return modori ;
	}

	char * nodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen )
	{
		if ( node == NULL )
			return NULL ;

		rapidxml::xml_attribute < char > * mAttr =
			node->first_attribute ( attrName, attrLen, false ) ;

		if ( mAttr == NULL )
			return NULL ;

		return mAttr->value ( ) ;
	}

	std::string makeType ( std::string typeName )
	{
		typeName = "type=\"" + typeName ;
		typeName = typeName + "\"" ;
		return typeName ;
	}

	bool verifyNodeAsType ( rapidxml::xml_node < char > * node,
		const char * nodeName, const char * nodeType )
	{
		unsigned int typeLen = strlen ( "type" ) ;

		bool isName = verifyNodeName ( node, nodeName ) ;
		if ( ! isName )
			return false ;

		isName = verifyNodeAttribute ( node, "type", typeLen, nodeType ) ;
		return isName ;
	}

	char * getNodeType ( rapidxml::xml_node < char > * node )
	{
		unsigned int typeLen = strlen ( "type" ) ;
		return nodeAttribute ( node, "type", typeLen ) ;
	}

	char * getNodeName ( rapidxml::xml_node < char > * node )
	{
		unsigned int nameLen = strlen ( "name" ) ;
		return nodeAttribute ( node, "name", nameLen ) ;
	}

	std::string toXml ( const std::string & nonXml )
	{
		//std::stringstream ss
		std::string modori ;
		//const size_t len = nonXml.length ( ) ;
		modori.reserve ( nonXml.length ( ) ) ;
		//char curChar ;
		const char * charPtEnd = nonXml.c_str ( ) + nonXml.length ( ) ;
		for ( const char * charPt = nonXml.c_str ( ) ;
			charPt != charPtEnd ; charPt ++ )
		{
			//curChar = nonXml [ i ] ;
			//switch ( curChar )
			switch ( * charPt )
			{
			case ' & ':
				modori.append ( " & amp ; " ) ;
				break ;
			case '\'':
				modori.append ( " & apos ; " ) ;
				break ;
			case '"':
				modori.append ( " & quot ; " ) ;
				break ;
			case '<':
				modori.append ( " & lt ; " ) ;
				break ;
			case '>':
				modori.append ( " & gt ; " ) ;
				break ;
			case ' ; ':
				modori.append ( " & #59 ; " ) ;
				break ;
			case '\n':
				modori.append ( " & #10 ; " ) ;
				break ;
			case '\r':
				modori.append ( " & #13 ; " ) ;
				break ;
			case ' ':
				modori.append ( " & #32 ; " ) ;
				break ;
			case '\t':
				modori.append ( " & #09 ; " ) ;
				break ;
			default:
				modori += * charPt ;
				break ;
			}
		}
		//std::string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}

	void parseXml ( const std::string & all_xml, rapidxml::xml_document < char > * docPtr, rapidxml::xml_node < char > ** nodePtr )
	{
		char * tmp = new char [ all_xml.size ( ) + 1 ] ;
		tmp [ all_xml.size ( ) ] = '\0' ;
		strncpy ( tmp, all_xml.c_str ( ), all_xml.size ( ) ) ;
		parseXml ( tmp, docPtr, nodePtr ) ;
		delete [ ] tmp ;
	}

	void parseXml ( char * all_xml, rapidxml::xml_document < char > * docPtr, rapidxml::xml_node < char > ** nodePtr )
	{
		docPtr->parse < 0 > ( all_xml ) ;
		if ( nodePtr != NULL )
			 * nodePtr = docPtr->first_node ( ) ;
	}
}


		rapidxml::xml_document < char > doc ;
		rapidxml::xml_node < char > * node ;
		parseXml ( allSiggy, & doc, & node ) ;
		// skip base node, if it exists.
		if ( node != 0 )
		{
			std::string name = node->name ( ) == NULL ? "" : node->name ( ) ;
			ISignal * siggy = SignalFactory::get ( name ) ;
			if ( siggy == NULL )
				node = node->first_node ( ) ;
			else
				delete siggy ;
		}
		gimmeXml ( container, node ) ;
	}
	void gimmeXml ( std::vector < std::unique_ptr < ISignal > > * container,
		rapidxml::xml_node < char > * node )
	{
		if ( container == NULL || node == NULL ) return ;
		for ( ; node != 0 ; node = node->next_sibling ( ) )
		{
			std::string name = node->name ( ) == NULL ? "" : node->name ( ) ;
			ISignal * siggy = SignalFactory::get ( name ) ;
			if ( siggy != NULL )
			{
				container->push_back ( std::unique_ptr < ISignal > ( siggy ) ) ;
				auto it = SignalXRMap.find ( name ) ;
				if ( it != SignalXRMap.end ( ) )
					receiveXml ( siggy, node, it->second ) ;
			}
		}
	}

	void signalBaseReceiver ( ISignal * receiver, rapidxml::xml_node < char > * obj )
	{
		if ( receiver == NULL || obj == NULL ) return ;

		if ( obj->name ( ) != NULL && obj->value ( ) != NULL )
			receiver->receive ( obj->name ( ), obj->value ( ) ) ;
	}
	void spaceReceiver ( ISignal * receiver, rapidxml::xml_node < char > * obj )
	{
		if ( receiver == NULL || obj == NULL ) return ;

		// blip members are same as space, so child XR is fine.
		if ( containsAt ( obj->name ( ), 0, "blip", true )
			|| containsAt ( obj->name ( ), 0, "pos", true )
			|| containsAt ( obj->name ( ), 0, "spacepos", true ) )
		{
			childXR ( receiver, obj, signalBaseReceiver ) ;
		}
		else
			signalBaseReceiver ( receiver, obj ) ;
	}

	//extern bool StaticAddToFactory = initSignalXRMap ( ) ;
}
