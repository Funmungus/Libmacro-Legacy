/*! \file xml.h
 * \brief Xml functions for funlibrary.
 */

# ifndef FUN_XML_H
# define FUN_XML_H

# include "util/funstrings.h"
# include "util/object.h"
# include "rapidxml/rapidxml.hpp"

namespace funlibrary
{

/*! \brief Implements void call ( char *, bool ) ;
	 *
	 * Used to receive C-string named boolean values.
	 */
	typedef ICall < void, char *, bool > ICallNamedBool ;
	/*! \brief Implements void call ( char *, int ) ;
	 *
	 * Used to receive C-string named int values.
	 */
	typedef ICall < void, char *, int > ICallNamedInt ;
/*! \brief Implements void call ( char *, long ) ;
	 *
	 * Used to receive C-string named long values.
	 */
	typedef ICall < void, char *, long > ICallNamedLong ;
	/*! \brief Implements void call ( char *, char * ) ;
	 *
	 * Used to receive C-string named C-string values.
	 */
	typedef ICall < void, char *, char * > ICallNamedCString ;
/*! \brief Implements void call ( char *, unsigned int ) ;
	 *
	 * Used to receive C-string named unsigned int values.
	 */
	typedef ICall < void, char *, unsigned int > ICallNamedUnsigned ;
	/*! \brief Implements void call ( rapidxml::xml_node < char > * ) ;
	 *
	 * Used to receive xml_node < char > * from the rapidxml namespace.
	 */
	typedef ICall < void, rapidxml::xml_node < char > * > ICallXmlNode ;




	//! \brief Map a value type name to a function which receives a name-value pair in strings, and sets the value in TemplateBase.
	//! This is intended to be used with \ref ICall < void, std::string, <NewType> >#call ( std::string name, <NewType> value )
	class NameValueReceiver
	{
	public :
		typedef void (* ReceiveNameValue) (TemplateBase * receivingObj, std:string valueType, std::string valueName, std::string value ) ;
		static void addMap ( std::string valueType, const ReceiveNameValue ) ;
		static void overrideMap ( std::string valueType, const ReceiveNameValue ) ;
		static void removeMap ( std::string valueType ) ;
		static void clear ( ) ;

		static void receive ( TemplateBase * receivingObj, std:string valueType, std::string valueName, std::string value ) ;
	private :
		static NameValueReceiver _instance ;
		NameValueReceiver () ;
		~NameValueReceiver () ;
	} ;

//	class TemplateBase ;

	/*! \brief Given an xml string, set the name-value pairs in an ICall object.
	 *
	 * \param receivingObj This is a TemplateBase, but it is assumed here to inherit the correct
     * ICall subtypes to receive a C-string and value type for any named fefvalue it may receive in xml.
	 * For unknown types, an ICall < void, rapidxml::xml_node < char > * > is called for the
	 * class to handle the unknown type.
	 * \param node This represents receivingObj as xml.  Any values
	 */
	UTIL_API void receiveXml ( TemplateBase * receivingObj, rapidxml::xml_node < char > * node ) ;

	/*! \brief Return true if node's name is typeName.*/
	UTIL_API bool verifyNodeName ( rapidxml::xml_node < char > * node,
		const char * typeName ) ;

	/*! \brief Return true if node has an attribute named attrName, with value attrVal. */
	UTIL_API bool verifyNodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen, const char * attrVal ) ;

	/*! \brief Get a node attribute of given name.
	 * 
	 * \param node The xml node to search for an attribute in.
	 * \param attrName The name of the attribute to search for.
	 * \param attrLen Length of attrName.
	 */
	UTIL_API char * nodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen ) ;

	/*! \brief Return type="typeName". */
	UTIL_API std::string makeType ( std::string typeName ) ;

	/*! \brief Return true if node name is nodeName,
	 * and has a "type" attribute with a value of nodeType. */
	UTIL_API bool verifyNodeAsType ( rapidxml::xml_node < char > * node,
		const char * nodeName, const char * nodeType ) ;

	/*! \brief Convert a string to an xml-compatible string.
	 * 
	 * The characters ', ", <, >, and space cannot be used in xml.  This function
	 * converts a string to conform to this requirement.  These characters
	 * are changed and given new values with the escape character '&'.
	 */
	UTIL_API std::string toXml ( std::string nonXml ) ;
}

# endif

