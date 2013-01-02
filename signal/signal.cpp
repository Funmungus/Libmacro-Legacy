// signal.cpp : Defines the exported functions for the DLL application.
//

# ifdef WIN
//# include "stdafx.h"
# endif

# include "signal/signal.h"

using namespace std ;

namespace funlibrary
{
	void usage_xmlSignalSet ( )
	{
		cout << "To create a set of signals..." << endl
			<< "Firstly, one must have an xml C-string (char *), "
			<< "and a vector of DPointers to contain ISignals." << endl
			<< "Both objects are referenced as pointers.  If either "
			<< "object is NULL, gimmeSiggy will do nothing." << endl
			<< "Both references will be modified, and cannot be constant." << endl
			<< "Items added to siggySet will be dynamically allocated ISignal objects. "
			<< "DPointers delete automatically on destruction, so items are added "
			<< "with the \"new\" operator." << endl ;
	}

	/// <summary>
	/// May not use rapidxml in the future.  Please use function with 
	/// char * all_signal_xml parameter.
	/// </summary>
	void gimmeSiggy_internal ( rapidxml::xml_document < char > * all_signal_doc,
		vector < unique_ptr < ISignal > > * siggySet ) ;

	/// <summary>
	/// Get next ISignal::name node.
	/// </summary>
	rapidxml::xml_node < char > * nextNode_internal ( rapidxml::xml_node < char > * curNode ) ;

	/// <summary>
	/// Get a signal from this node.
	/// </summary>
	ISignal * sigFromNode_internal ( rapidxml::xml_node < char > * node ) ;

	void xmlToSiggy ( char * all_signal_xml,
		vector < unique_ptr < ISignal > > * siggySet )
	{
		if ( all_signal_xml == NULL || siggySet == NULL )
			return ;

		rapidxml::xml_document < char > doc ;
		doc.parse < 0 > ( all_signal_xml ) ;    // 0 means default parse
		gimmeSiggy_internal ( &doc, siggySet ) ;
	}

	void gimmeSiggy_internal ( rapidxml::xml_document < char > * all_signal_doc,
		vector < unique_ptr < ISignal > > * siggySet )
	{
		if ( all_signal_doc == NULL || siggySet == NULL )
		{
			return ;
		}

		ISignal * newSig = NULL ;

		const size_t signame_len = strlen ( ISignal::name.c_str ( ) ) ;
		rapidxml::xml_node < char > * curNode =
			all_signal_doc->first_node ( ISignal::name.c_str ( ), signame_len, false ) ;

		while ( curNode != 0 )
		{
			newSig = sigFromNode_internal ( curNode ) ;
			if ( newSig != NULL )
				siggySet->push_back ( unique_ptr < ISignal > ( newSig ) ) ;
			curNode = nextNode_internal ( curNode ) ;
		}
	}

	rapidxml::xml_node < char > * nextNode_internal
		( rapidxml::xml_node < char > * curNode )
	{
		const size_t signame_len = strlen ( ISignal::name.c_str ( ) ) ;
		rapidxml::xml_node < char > * nextNode =
			curNode->next_sibling ( ISignal::name.c_str ( ), signame_len, false ) ;
		return nextNode ;
	}

	ISignal * sigFromNode_internal ( rapidxml::xml_node < char > * node )
	{
		const size_t type_len = strlen ( "type" ) ;
		ISignal * modori = NULL ;
		rapidxml::xml_attribute < char > * attr =
			node->first_attribute ( "type", type_len, false ) ;
		if ( attr != 0 )
		{
			char * typeVal = attr->value ( ) ;
			if ( typeVal != 0 )
			{
				modori = SignalFactory::get ( typeVal ) ;
				if ( modori != NULL )
				{
					receiveXml ( modori, node ) ;
				}
			}
		}
		return modori ;
	}

	void serialToSiggy ( std::string all_signal_serial,
		std::vector < std::unique_ptr < ISignal > > * siggySet )
	{
		size_t len, sPt = 0, pt = 0 ;
		string signame ;
		ISignal * newSig = NULL ;
		string whitespace = "\a\b\f\n\r\t\v " ;
		whitespace += SerialSeparator ;
		len = all_signal_serial.length ( ) ;
		while ( pt < len && sPt < len )
		{
			// Case 1: pt=0, skip first whitespace.
			// Case 2: pt++, skip whitespace after previous pt.
			pt = all_signal_serial.find_first_not_of ( whitespace, pt ) ;
			sPt = all_signal_serial.find_first_of ( whitespace, pt ) ;
			while ( newSig == NULL && pt < sPt && sPt < len ) // pt < sPt < all_signal_serial.length ( ) ) ;
			{
				signame = all_signal_serial.substr ( pt, sPt - pt ) ;

				newSig = SignalFactory::get ( signame ) ;
				if ( newSig == NULL )
				{
					++ pt ;
				}
				else
				{
					all_signal_serial = newSig->deserialize ( all_signal_serial, pt ) ;
					if ( all_signal_serial.length ( ) < len )
					{
						siggySet->push_back ( std::unique_ptr < ISignal > ( newSig ) ) ;
						pt = sPt = 0 ;
						len = all_signal_serial.length ( ) ;
					}
					else
					{
						++ pt ;
					}
				}
				// This is assuming sPt will be <= length of the full string.
			}
			newSig = NULL ;
		}
	}

}
