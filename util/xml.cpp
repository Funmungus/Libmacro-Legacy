 
# ifdef WIN
//# include "stdafx.h"
# endif

# include "util/util.h"

using namespace std ;

namespace funlibrary
{
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
		typeName = typeName + '\"' ;
		return typeName ;
	}

	bool verifyNodeAsType ( rapidxml::xml_node < char > * node,
		const char * nodeName, const char * nodeType )
	{
		static unsigned int typeLen = strlen ( "type" ) ;

		bool isName = verifyNodeName ( node, nodeName ) ;
		if ( ! isName )
			return false ;

		isName = verifyNodeAttribute ( node, "type", typeLen, nodeType ) ;
		return isName ;
	}

	char * getNodeType ( rapidxml::xml_node < char > * node )
	{
		static unsigned int typeLen = strlen ( "type" ) ;
		return nodeAttribute ( node, "type", typeLen ) ;
	}

	string toXml ( std::string nonXml )
	{
		stringstream ss ;
		const size_t len = nonXml.length ( ) ;
		char curChar ;
		for ( size_t i = 0 ; i < len ; i++ )
		{
			curChar = nonXml [ i ] ;
			switch ( curChar )
			{
			case '&':
				ss << "&amp;" ;
				break ;
			case '\'':
				ss << "&apos;" ;
				break ;
			case '"':
				ss << "&quot;" ;
				break ;
			case '<':
				ss << "&lt;" ;
				break ;
			case '>':
				ss << "&gt;" ;
				break ;
			case ';':
				ss << "&#59;" ;
				break ;
			case '\n':
				ss << "&#10;" ;
				break ;
			case '\r':
				ss << "&#13;" ;
				break ;
			case ' ':
				ss << "&#32;" ;
				break ;
			case '\t':
				ss << "&#09;" ;
				break ;
			default:
				ss << curChar ;
				break ;
			}
		}
		string modori = ss.str ( ) ; ss.flush ( ) ;
		return modori ;
	}

	void receiveXml ( TemplateBase * receivingObj, rapidxml::xml_node < char > * node )
	{
// Assume this is already done. Just check for null.
		if ( node == NULL )
			return ;

		ICallNamedBool * toB ;
		ICallNamedInt * toI ;
		ICallNamedCString * toS ;
		ICallNamedLong * toL ;
		ICallNamedUnsigned * toU ;
		ICallXmlNode * toNewObj ;

		rapidxml::xml_node < char > * curNode =
			node->first_node ( ) ;

		char curChar ;
		bool isValid ;
		char * sVal, * curName, * attrName ;
		int val ;
		long valL ;
		unsigned int valU ;
		bool valB ;

		while ( curNode != 0 )
		{
			curName = curNode->name ( ) ;
			
			if ( curName != NULL )
			{
				curChar = curName [ 0 ] ;

				attrName = getNodeType ( curNode ) ;
				sVal = curNode->value ( ) ;

				switch ( curChar )
				{
				case 'b':
				case 'B':
					isValid = verifyNodeName ( curNode, "bool" ) ;
					if ( isValid && sVal != NULL && attrName != NULL )
					{
						try
						{
							valB = convertBool ( sVal ) ;
							toB = dynamic_cast < ICallNamedBool * >
									( receivingObj ) ;
							if ( toB == NULL )
								fprintf ( stderr, "Xml receiver cannot receive bool.\n" ) ;
							else
								toB->call ( attrName, valB ) ;
							toB = NULL ;
							break ;
						}
						catch ( ... )
						{
							fprintf ( stderr, "Invalid boolean value for %s.\n", attrName ) ;
							break ;
						}
					}
					goto XmlNodeFallback ;
				case 'i':
				case 'I':
					isValid = verifyNodeName ( curNode, "int" ) ;
					if ( isValid && sVal != NULL && attrName != NULL )
					{  
						try
						{
							val = stoi ( sVal ) ;
							toI = dynamic_cast < ICallNamedInt * >
								( receivingObj ) ;
							if ( toI == NULL )
								fprintf ( stderr, "Xml receiver cannot receive int.\n" ) ;
							else
								toI->call ( attrName , val ) ;
							toI = NULL ;
							break ;
						}
						catch ( ... )
						{
							fprintf ( stderr, "Invalid int value for %s.\n", attrName ) ;
							break ;
						}
					}
					goto XmlNodeFallback ;
				case 'l':
				case 'L':
					isValid = verifyNodeName ( curNode, "long" ) ;
					if ( isValid && sVal != NULL && attrName != NULL )
					{  
						try
						{
							valL = stol ( sVal ) ;
							toL = dynamic_cast < ICallNamedLong * >
								( receivingObj ) ;
							if ( toL == NULL )
								fprintf ( stderr, "Xml receiver cannot receive long.\n" ) ;
							else
								toL->call ( attrName, valL ) ;
							toL = NULL ;
							break ;
						}
						catch ( ... )
						{
							fprintf ( stderr, "Invalid long value for %s.\n", attrName ) ;
							break ;
						}
					}
					goto XmlNodeFallback ;
				case 's':
				case 'S':
					isValid = verifyNodeName ( curNode, "string" ) ;
					if ( isValid && attrName != NULL )
					{
						toS = dynamic_cast < ICallNamedCString * >
							( receivingObj ) ;
						if ( toS == NULL )
							fprintf ( stderr, "Xml receiver cannot receive a C string.\n" ) ;
						else
							toS->call ( attrName, sVal ) ;
						toS = NULL ;
						break ;
					}
					goto XmlNodeFallback ;
				case 'u':
				case 'U':
					isValid = verifyNodeName ( curNode, "unsigned" ) ;
					if ( isValid && sVal [ 0 ] == '-' )
					{
						fprintf ( stderr, "Unsigned cannot be negative.\n" ) ;
						break ;
					}
					else if ( isValid && sVal != NULL && attrName != NULL )
					{  
						try
						{
							valL = stol ( sVal ) ;
							valU = (unsigned int) valL ;

							toU = dynamic_cast < ICallNamedUnsigned * >
								( receivingObj ) ;
							if ( toU == NULL )
								fprintf ( stderr, "Xml receiver cannot receive unsigned int.\n" ) ;
							else
								toU->call ( attrName, valU ) ;
							toU = NULL ;
							break ;
						}
						catch ( ... )
						{
							fprintf ( stderr, "Invalid unsigned int value for %s.\n", attrName ) ;
							break ;
						}
					}
					goto XmlNodeFallback ;
				default :
					XmlNodeFallback :
					toNewObj = dynamic_cast < ICallXmlNode * >
							( receivingObj ) ;
					if ( toNewObj == NULL )
						fprintf ( stderr, "Xml receiver cannot receive xml node.\n" ) ;
					else
						toNewObj->call ( curNode ) ;
					break ;
				}	// end switch
			}	// end if curName
			curNode = curNode->next_sibling ( ) ;
		}	// end while

		return ; //true ;
	}
}

