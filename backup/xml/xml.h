/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/xml.h
 * \brief Xml functions for macro.
 */

# ifndef MCR_XML_H
# define MCR_XML_H

# include "util/mcrstrings.h"
# include "rapidxml/rapidxml.hpp"

namespace macro
{
	/*! \brief Used in \ref receiveXml ( XmlReceiver, T *, rapidxml::xml_node < char > * )
	 * as the fuction to set every node into the receiving object.
	 *
	 * The node passed in from \ref receiveXml will be all first children of that class
	 * type's node.
	 * <br><br>
	 * The template parameter is the base class of the object, but this should be
	 * implemented for every child object which receives values from xml.
	 */
	template < class T >
	using XmlReceiver = MCR_API void ( * ) ( T *, rapidxml::xml_node < char > * ) ;

	/*! \brief Set \ref XmlReceiver for subclass name before using
	 * \ref receiveXml. XmlReceiver will retreive all values from
	 * an xml node, and set them into the receiving object.
	 **/
	template < class T >
	using XmlReceiverMap = MCR_API std::map < std::string, XmlReceiver < T > > ;

	/*! \brief Replace the mapping of a name to XmlReceiver in an XmlReceiverMap.
	 *
	 * \param name The name to replace, given the current character case,
	 * lower case, and upper case.
	 * \param value The XmlReceiver to use for given name.
	 * \param map The map to contain name-value pair of string to XmlReceiver.
	 **/
	template < class T >
	void replaceXmlMapping ( const std::string & name, XmlReceiver < T > value,
							XmlReceiverMap < T > * map )
	{
		if ( name.empty ( ) || value == NULL || map == NULL ) return ;
		// reg
		std::string upDown = name ;
		map->erase ( upDown ) ;
		map->insert ( std::pair < std::string, XmlReceiver < T > >
			( upDown, value ) ) ;
		// lower
		transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::tolower ) ;
		map->erase ( upDown ) ;
		map->insert ( std::pair < std::string, XmlReceiver < T > >
			( upDown, value ) ) ;
		// upper
		transform ( upDown.begin ( ), upDown.end ( ), upDown.begin ( ), ::toupper ) ;
		map->erase ( upDown ) ;
		map->insert ( std::pair < std::string, XmlReceiver < T > >
			( upDown, value ) ) ;
	}

	/*! \brief Convert a string to an xml-compatible string.
	 *
	 * The characters ', ", <, >, and space cannot be used in xml. This function
	 * converts a string to conform to this requirement. These characters
	 * are changed and given new values with the escape character ' & '.
	 */
	MCR_API std::string toXml ( const std::string & nonXml ) ;

	/*! \brief Given an xml string, set the name-value pairs in an object.
	 * \ref XmlReceiverMap must already be set with the helper function
	 * for all node names.
	 *
	 * \param receivingObj receiver will be used to set values into this object
	 * for node and all first-children of node.
	 * \param node This represents receivingObj as xml.
	 * \param receiver An \ref XmlReceiver specific to the subclass that is
	 * the xml node's name. If NULL, no action is taken. This will be used to
	 * set values into receivingObj.
	 */
	template < class T >
	MCR_API void receiveXml ( T * receivingObj, rapidxml::xml_node < char > * node,
							XmlReceiver < T > receiver ) ;

	/*! \brief Given an xml string, output the parsed Document to docPtr.
	 * \param all_xml String of all xml objects.
	 * \param docPtr The parsed document gets set into this object.
	 * \param nodePtr Optionally set the first xml node into this object pointer.
	 */
	MCR_API void parseXml ( const std::string & all_xml, rapidxml::xml_document < char > * docPtr, rapidxml::xml_node < char > ** nodePtr = NULL ) ;
	// ! \brief \ref parseXml ( const std::string &, rapidxml::xml_document<char> *, rapidxml::xml_node<char> ** )
	MCR_API void parseXml ( char * all_xml, rapidxml::xml_document < char > * docPtr, rapidxml::xml_node < char > ** nodePtr = NULL ) ;

	//
	// Template implementations.
	//

	template < class T >
	void childXR ( T * receivingObj, rapidxml::xml_node < char > * node,
		XmlReceiver < T > receiver )
	{
		receiver ( receivingObj, node ) ;
		for ( rapidxml::xml_node < char > * child = node->first_node ( ) ;
			child != 0 ; child = child->next_sibling ( ) )
		{
			receiver ( receivingObj, child ) ;
		}	// end while
	}

	template < class T >
	void receiveXml ( T * receivingObj, rapidxml::xml_node < char > * node,
		XmlReceiver < T > receiver )
	{
		// Assume parsing is already done. Just check for null.
		if ( node == NULL || receivingObj == NULL || receiver == NULL )
			return ;
		// If node's name is null, or cannot be found in the function map, return with no action.
		if ( node->name ( ) == NULL ) return ;

		// Receive parent node, which may contain attributes, and all children.
		childXR ( receivingObj, node, receiver ) ;

		return ; //true ;
	}


	//
	// Depricated
	//

	/*! \brief Get a node attribute of given name.
	 *
	 * \param node The xml node to search for an attribute in.
	 * \param attrName The name of the attribute to search for.
	 * \param attrLen Length of attrName.
	 */
	MCR_API char * nodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen ) ;

	/*! \brief Return type="typeName". */
	MCR_API std::string makeType ( std::string typeName ) ;

	/*! \brief Return true if node name is nodeName,
	 * and has a "type" attribute with a value of nodeType. */
	MCR_API bool verifyNodeAsType ( rapidxml::xml_node < char > * node,
		const char * nodeName, const char * nodeType ) ;

	/*! \brief Return true if node's name is typeName. */
	MCR_API bool verifyNodeName ( rapidxml::xml_node < char > * node,
		const char * typeName ) ;

	/*! \brief Return true if node has an attribute named attrName, with value attrVal. */
	MCR_API bool verifyNodeAttribute ( rapidxml::xml_node < char > * node,
		const char * attrName, unsigned int attrLen, const char * attrVal ) ;

}

# endif
