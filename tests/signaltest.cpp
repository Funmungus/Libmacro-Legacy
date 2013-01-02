
# ifdef WIN
# include "stdafx.h"
# include "Header.h"
# endif
# include "signal/signal.h"
# include <ctime>
# include <iostream>
# include <cstdlib>
# include <cstdio>
# include <sstream>

// TODO origin and new both be vector of unique_ptr of ISignal.
//static const char * logdir = "C:¥¥Users¥¥Jonathan¥¥Documents¥¥log" ;
//static const char * logfile = "test.log" ;

void randSig ( std::vector < std::unique_ptr < funlibrary::ISignal > > * randosigs ) ;
void randMod ( funlibrary::ISignal * randosig ) ;
void randKey ( funlibrary::ISignal * randosig ) ;
void randSpace ( funlibrary::ISignal * randosig ) ;
void randCom ( funlibrary::ISignal * randosig ) ;
void randScript ( funlibrary::ISignal * randosig ) ;
void randNoOp ( funlibrary::ISignal * randosig ) ;
std::string randString ( ) ;
void setFrom ( std::vector < std::unique_ptr < funlibrary::ISignal > > * orig,
	std::vector < std::unique_ptr < funlibrary::ISignal > > * news ) ; // , size_t len ) ;
void printErr ( funlibrary::ISignal * err ) ;
void validate ( std::vector < std::unique_ptr < funlibrary::ISignal > > * orig,
	std::vector < std::unique_ptr < funlibrary::ISignal > > * news ) ; // , size_t len ) ;

# ifdef WIN
int _tmain ( int argc, _TCHAR* argv [ ] )
# endif
# ifdef LNX
int main ( )
# endif
{
std::cout<<"echo"<<std::endl;
	std::srand ( ( unsigned int ) time ( 0 ) ) ;
	system ( "sleep 2" ) ;
	const static size_t size = 10 ;
	const static size_t repeat = 10 ;
	for ( size_t j = 0 ; j < repeat ; j++ )
	{
		std::vector < std::unique_ptr < funlibrary::ISignal > > orig, sets ;

		for ( size_t i = 0 ; i < size ; i++ )
		{
			randSig ( & orig ) ;
			//orig.at ( i ).get ( )->send ( ) ;
		}
		
		setFrom ( & orig, & sets ) ;

		validate ( & orig, & sets ) ; // , size ) ;

		//system ( "timeout 1" );

	}
	system ( "sleep 2" ) ;

	return 0;
}


typedef void ( *Randomizer ) ( funlibrary::ISignal * ) ;

void randSig ( std::vector < std::unique_ptr < funlibrary::ISignal > > * randosigs )
{
	static const std::string names [ ] = { funlibrary::Mods::name,
		funlibrary::KeySignal::name, funlibrary::SpaceSignal::name, 
		funlibrary::ComSignal::name, funlibrary::ScriptSignal::name,
		funlibrary::NoOp::name } ;
	static const Randomizer funcs [ ] = { randMod, randKey, randSpace,
		randCom, randScript, randNoOp } ;
	static const size_t len = 6 ;
	funlibrary::ISignal * newSig ;
	int typeI = ( rand ( ) % len ) ;
	//int typeI = 1 ;

	newSig = funlibrary::SignalFactory::get ( names [ typeI ] ) ;
	funcs [ typeI ] ( newSig ) ;

	randosigs->push_back ( std::unique_ptr < funlibrary::ISignal > ( newSig ) ) ;
}

void randMod ( funlibrary::ISignal * randosig )
{
	funlibrary::Mods * modsig = dynamic_cast < funlibrary::Mods * > ( randosig ) ;
	if ( modsig != NULL )
	{
		static int val ;
		val = ( rand ( ) % 0x10 ) + 1 ;
		modsig->set ( ( const unsigned int ) val ) ;
	}
}
void randKey ( funlibrary::ISignal * randosig ) 
{
	funlibrary::KeySignal * keysig = dynamic_cast < funlibrary::KeySignal * > ( randosig ) ;
	static funlibrary::KeyUpType ups [ 3 ] = { funlibrary::KeyUpType::DOWN, funlibrary::KeyUpType::UP, funlibrary::KeyUpType::BOTH } ;
	if ( keysig != NULL )
	{
		static int val ;
		val = ( rand ( ) % 0x05 ) + 'A' ;
		keysig->setKey ( val ) ;
		val = ( rand ( ) % 0x05 ) + 'A' ;
		keysig->setScan ( val ) ;
		val = ( rand ( ) % 3 ) ;
		keysig->setKeyUp ( ups [ val ] ) ;
		/*keysig->setKey ( 'A' ) ;
		keysig->setMods ( 0 ) ;
		keysig->setEnableKeyUp ( true ) ;
		keysig->setEnableScan ( false ) ;*/
	}
}
void randSpace ( funlibrary::ISignal * randosig )
{
	funlibrary::SpaceSignal * spacesig = dynamic_cast < funlibrary::SpaceSignal * > ( randosig ) ;
	if ( spacesig != NULL )
	{
		static int val ;
		val = ( rand ( ) % 2 ) ;
		spacesig->setEnableCursorJustify ( val != 0 ) ;
		val = ( rand ( ) % 0x20 ) ;
		spacesig->setEvent ( val ) ;
		val = ( rand ( ) % 0xfe ) ;
		spacesig->setEventData ( val ) ;
		val = ( rand ( ) % 0xff ) ;
		spacesig->setX ( val ) ;
		val = ( rand ( ) % 0xff ) ;
		spacesig->setY ( val ) ;
		val = ( rand ( ) % 0xff ) ;
		spacesig->setZ ( val ) ;
	}
}
void randCom ( funlibrary::ISignal * randosig )
{
	funlibrary::ComSignal * comsig = dynamic_cast < funlibrary::ComSignal * > ( randosig ) ;
	if ( comsig != NULL )
	{
		comsig->setMessage ( randString ( ) ) ;
	}
}
void randScript ( funlibrary::ISignal * randosig )
{
	funlibrary::ScriptSignal * scriptsig = dynamic_cast < funlibrary::ScriptSignal * > ( randosig ) ;
	if ( scriptsig != NULL )
	{
		scriptsig->setScript ( randString ( ) ) ;
	}
}
void randNoOp( funlibrary::ISignal * randosig )
{
	funlibrary::NoOp * scriptsig = dynamic_cast < funlibrary::NoOp * > ( randosig ) ;
	if ( scriptsig != NULL )
	{
		scriptsig->setMillis ( ( unsigned int ) rand ( ) ) ;
	}
}

std::string randString ( )
{
	int len = ( rand ( ) % 0x20 ) + 1 ;
	std::string newString ;
	char curChar ;
	for ( int i = 0 ; i < len ; i++ )
	{
		curChar = ( rand ( ) % 10 ) + '0' ;
		/*while ( curChar == '"' )
		{
			curChar = ( rand ( ) % '‾' ) + 1 ;
		}*/
		newString += curChar ;
	}

	return newString ;
}



void setFrom ( std::vector < std::unique_ptr < funlibrary::ISignal > > * orig,
	std::vector < std::unique_ptr < funlibrary::ISignal > > * news )
{
/*
	std::cout << "beginning values are\n" ;
	for ( auto it = orig->begin ( ) ; it != orig->end ( ) ; it ++ )
	{
		std::cout << (*it)->xml ( ) << std::endl ;
	}
*/

	std::stringstream ss ;
	ss << "" ;
	for ( size_t i = 0 ; i < orig->size ( ) ; i++ )
	{
		//ss << orig->at ( i )->serialize ( ) ;
		ss << orig->at ( i )->xml ( ) ;
	}
/*
	std::string all_serial = ss.str ( ) ; ss.flush ( ) ;
	funlibrary::gimmeSiggy ( all_serial, news ) ;
*/
	std::string all_xml = ss.str ( ) ; ss.flush ( ) ;

	const size_t mLen = all_xml.length ( ) + 1 ;
	char * tmp = new char [ mLen ] ;
	tmp [ mLen - 1 ] = '\0' ;

	strncpy ( tmp, all_xml.c_str ( ), mLen - 1 ) ;
	
	try
	{
		funlibrary::gimmeSiggy ( tmp, news ) ;
	}
	catch ( ... )
	{

		std::cerr << "error\n" << std::endl ;
		for ( auto it = orig->begin ( ) ; it != orig->end ( ) ; it ++ )
		{
			std::cerr << (*it)->xml ( ) << std::endl ;
		}
	}

	delete [ ] tmp ;

	if ( news->size ( ) != orig->size ( ) )
	{
		std::cerr << "error occuring" << std::endl ;
	}
}

void printErr ( funlibrary::ISignal * err )
{
	if ( err == NULL )
	{
		std::cerr << "Null value : " << err << std::endl << std::endl ;
	}
	else
	{
		std::cerr << "Signal : " << err << std::endl
			<< err->xml ( ) << std::endl << std::endl ;
	}
}

void validate ( std::vector < std::unique_ptr < funlibrary::ISignal > > * orig,
	std::vector < std::unique_ptr < funlibrary::ISignal > > * news )
{
	bool equality ;
	size_t len, min ;
	min = orig->size ( ) ;
	len = news->size ( ) ;
	if ( min != len )
	{
		std::cerr << "Origin and new inequal lengths.¥nOrigin = " << min << std::endl
			<< "New = " << len << std::endl ;
		min = ( len < min ) ? len : min ;
	}

	for ( size_t i = 0 ; i < min ; i++ )
	{
		std::string xml1 = orig->at ( i ).get ( )->xml ( ) ;
		std::string xml2 = news->at ( i ).get ( )->xml ( ) ;

		equality = ( orig->at ( i ).get ( )->xml ( ) ==
			news->at ( i ).get ( )->xml ( ) ) ;

		if ( equality )
		{
			std::cout << orig->at ( i ).get ( ) << " == " <<
				news->at ( i ).get ( ) << std::endl ;
			/*cerr << orig->at ( i ).get ( ) << " == " <<
				news->at ( i ).get ( ) << endl ;*/
			/*printErr ( orig->at ( i ).get ( ) ) ;
			printErr ( news->at ( i ).get ( ) ) ;*/
		}
		else
		{
			std::cout << orig->at ( i ).get ( ) << " != "
				<< news->at ( i ).get ( ) << std::endl ;
			std::cerr << orig->at ( i ).get ( ) << " != "
				<< news->at ( i ).get ( ) << std::endl ;
			printErr ( orig->at ( i ).get ( ) ) ;
			printErr ( news->at ( i ).get ( ) ) ;
		}
	}
}

