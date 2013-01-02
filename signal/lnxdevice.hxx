
# include "signal/device.h"
# include "signal/noop.h"
# include "signal/mods.h"

using namespace std ;

namespace funlibrary
{
	mutex Device::_enableLock ;
	const input_event Device::_syncer = initSync ( ) ;

	// initialize syncer
	input_event Device::initSync( )
	{
		input_event val ;

		memset ( & val, 0, sizeof ( val ) ) ;
		val.type = EV_SYN ;
		val.code = SYN_REPORT ;
		val.value = 0 ;

		return val ;
	}

	Device::Device ( const std::map<int, std::vector<int> > & type_value_map,
									 const uinput_user_dev * device, const bool enable )
		: _type_value_map ( type_value_map ), _device ( ), _enable ( false )
	{
		setDev ( device ) ;
		this->enable ( enable ) ;
	}

	Device::~Device ( )
	{
		if ( _fs.fd != 0 )
		{
			if ( ioctl ( _fs.fd, UI_DEV_DESTROY ) < 0 )
			{
				fprintf ( stderr, "Destroying device id %d: %s", _fs.fd, strerror ( errno ) ) ;
			}
		}
	}

	void Device::usage ( )
	{
		cout << "Device automates creating uinput devices." << endl << endl ;
		cout << "<code>" << endl ;
		cout << "Device obj ;" << endl ;
		cout << "map < int, vector < int > > typeVal = {" << endl ;
		cout << "{{UI_SET_EVBIT}, {EV_KEY, EV_SYN}}, {{UI_SET_KEYBIT}, {KEY_A, KEY_B, KEY_C, KEY_1, KEY_2, KEY_3}}}" << endl ;
		cout << "obj.setInputBits ( & obj ) ;" << endl ;
		cout << "obj.enable ( true ) ;" << endl ;
		cout << "</code>" << endl << endl ;
	}

	uinput_user_dev Device::getDev ( ) const
	{
		return _device ;
	}

	void Device::setDev ( const uinput_user_dev * device )
	{
		if ( device != NULL )
		{
			memcpy ( & _device, device, sizeof ( _device ) ) ;
			return ;
		}

		memset ( & _device, 0, sizeof ( _device ) ) ;
		
		snprintf ( _device.name, UINPUT_MAX_NAME_SIZE, "funlibrary-Generic" ) ;

		_device.id.bustype = BUS_USB ;
		_device.id.vendor = 1 ;
		_device.id.product = 1 ;
		_device.id.version = 1 ;
	}

	map < int, vector < int > > Device::getInputBits ( ) const
	{
		return _type_value_map ;
	}

	void Device::setInputBits ( const int bitType, const int * bits, const unsigned int bit_len )
	{
		if ( bits == NULL || bit_len == 0 )
		{
			_type_value_map.erase ( bitType ) ;
			return ;
		}

		vector < int > inVals ;
		for ( unsigned int i = 0 ; i < bit_len ; i++ )
		{
			inVals.push_back ( bits [ i ] ) ;
		}

		// insert will not work if key exists
		_type_value_map.erase ( bitType ) ;

		_type_value_map.insert ( pair < int, vector < int > > ( bitType, inVals ) ) ;
	}

	void Device::setInputBits ( const std::map<int, std::vector<int> > & type_value_map )
	{
		_type_value_map = type_value_map ;
	}

	bool Device::isEnabled ( ) const
	{
		return _enable ;
	}

	bool Device::enable ( const bool enable )
	{
		// If already enabled, need to close and restart.
		_fs.close ( ) ;
		_enable = false ;

		if ( ! enable )		// disable complete
			return true ;
		// Cannot have device with no bits to set.
		// Must have at least 1 UI_SET_EVBIT.
		map < int, vector < int > >::iterator evIt = _type_value_map.find ( UI_SET_EVBIT ) ;
		if ( evIt == _type_value_map.end ( ) || evIt->second.empty ( ) )
		{
			cerr << "No UI_SET_EVBIT available." << endl;
			_fs.close ( ) ;
			return false ;
		}

		// Start by opening.
        _enableLock.lock ( ) ;
		bool valid = open ( ) ;
		if ( !valid )
        {
            _enableLock.unlock ( ) ;
			return false ;
        }

		// Force evbit satisfaction.
		valid = setBits ( evIt->first, evIt->second ) ;
		if ( !valid )
        {
            _enableLock.unlock ( ) ;
			return false ;
        }

		// Then set bits. Ensured at least UI_SET_EVBIT exists.
		map < int, vector < int > >::iterator it ;
		for ( it = _type_value_map.begin ( ) ; it != _type_value_map.end ( ) ; it++ )
		{
			if ( it->first != evIt->first )
			{
				valid = ( setBits ( it->first, it->second ) ) ? valid : false ;
			}
		}

		if ( !create ( ) )
        {
            _enableLock.unlock ( ) ;
			return false ;
        }

		// Created and ready. valid true is all good, false means some non-UI_SET_EVBIT did not work.
		_enable = true ;
        setEventFile ( ) ;
        _enableLock.unlock ( ) ;
		return valid ;
	}

	// TODO dynamic path
	const char * uinputPath = "/dev/uinput";

	bool Device::open ( )
	{
		// Close in case previously opened.
		_fs.close ( ) ;
		_fs.fd = ::open ( uinputPath, O_RDWR ) ;

		if ( _fs.fd == -1 )
		{
			fprintf ( stderr, "error opening uinput, fd %d\n", _fs.fd ) ;
			_fs.fd = -1 ;
			return false ;
		}

		if ( write ( _fs.fd, &_device, sizeof ( _device ) ) != sizeof ( _device ) )
		{
			fprintf ( stderr, "error writing user device, fd %d, device %d\n", _fs.fd, (int)(long)&_device ) ;
			_fs.close ( ) ;
			return false ;
		}

		return true ;
	}

	bool Device::setBits ( const int bitType, const std::vector < int > & bits )
	{
		bool success = true ;
//		if ( bits == NULL )
//			return true ;

		unsigned int len = bits.size ( ) ;
		for ( unsigned int i = 0 ; i < len ; i++ )
		{
			if ( ioctl ( _fs.fd, bitType, bits.at ( i ) ) < 0 )
			{
				fprintf ( stderr, "error set bit, type %d, value %d\n", bitType, bits.at ( i ) ) ;
				success = false;
			}
		}

		return success ;
	}

	bool Device::create ( )
	{
		if ( ioctl ( _fs.fd, UI_DEV_CREATE ) < 0)
		{
			fprintf ( stderr, "error UI_DEV_CREATE, fd %d\n", _fs.fd ) ;
 			return false ;
		}

		// Device is not recognized for i/o until time has passed.
		NoOp delay ( 10 ) ;
        delay.send ( ) ;	// NoOp return has no effect on creation.
		return true ;
	}

	/// <summary>
	/// Writes the given eventObj to device gived by fd
	/// </summary>
	bool Device::send ( const input_event * eventObj ) const
	{
		return send ( eventObj, sizeof ( * eventObj ) ) ;
	}

	bool Device::send ( const input_event * eventObjs, const unsigned int size ) const
	{
		if ( _fs.fd == -1 )
		{
			fprintf ( stderr, "Error sending event to fd -1.\n" ) ;
			return false ;
		}
		if ( eventObjs == NULL )
			return true ;

		bool valid = write ( _fs.fd, eventObjs, size ) >= size ;

		if ( ! valid )
		{
			fprintf( stderr, "Device send : %s.\n", strerror ( errno ) ) ;
			fprintf( stderr, "fd %d, struct %d, size %d\n", _fs.fd, (int) (long) &* eventObjs, size ) ;
			return false ;
		}

		unsigned int count = size / sizeof ( *eventObjs ) ;

		for ( unsigned int i = 0 ; i < count ; i++ )
		{
			if ( eventObjs [ i ].type == EV_KEY )
			{
				if ( eventObjs [ i ].value == 0 )
				{
					InternalMods.remove ( Mods::modFromKey ( eventObjs [ i ].code ) ) ;
				}
				else
				{
					InternalMods.add ( Mods::modFromKey ( eventObjs [ i ].code ) ) ;
				}
			}
		}

		return true ;
	}

	int Device::getFd ( ) const
	{
		return _fs.fd ;
	}

	int Device::getEventFd() const
	{
			return _eventFs.fd ;
	}

	void Device::setEventFile ( )
	{
			_eventFs.close ( ) ;
			string basePath = "/dev/input/" ;
			string curPath ;
			int ret ;
			char mName [ UINPUT_MAX_NAME_SIZE ] ;

			DIR * dir_point = opendir ( basePath.c_str ( ) ) ;
			if ( dir_point == NULL )
			{
					fprintf ( stderr, "Error opening %s.\n", basePath.c_str ( ) ) ;
					return ;
			}

			for ( dirent * entry = readdir ( dir_point ) ;
						entry != NULL ; entry = readdir ( dir_point ) )
			{	// if !entry then end of directory
					if ( entry->d_type == DT_CHR )
					{	// if entry is a char device
							std::string fname = entry->d_name;	// filename
							curPath = basePath + fname ;
							_eventFs.fd = ::open ( curPath.c_str ( ), O_RDONLY ) ;
							if ( _eventFs.fd != -1 )
							{
									ret = ioctl ( _eventFs.fd, EVIOCGNAME ( sizeof mName ), mName ) ;
									if ( ret == -1 )
									{
											_eventFs.close ( ) ;
											continue ;
									}
									ret = strncmp ( mName, _device.name, UINPUT_MAX_NAME_SIZE ) ;
									// Found correct fd for this uinput device
									if ( ret == 0 )
									{
											break ;
									}
									_eventFs.close ( ) ;
							}
					}
			}

			closedir ( dir_point ) ;
	}

	KeyDevice::KeyDevice ( const char * name , const bool enable )
	{
		uinput_user_dev dev ;
		memset ( & dev, 0, sizeof ( dev ) ) ;
		
		if ( name == NULL )
			snprintf ( dev.name, UINPUT_MAX_NAME_SIZE, "funlibrary-key" ) ;
		else
			strncpy ( dev.name, name, UINPUT_MAX_NAME_SIZE ) ;

		dev.id.bustype = BUS_USB ;
		dev.id.vendor = 1 ;
		dev.id.product = 1 ;
		dev.id.version = 1 ;

		setDev ( & dev ) ;
		
		prepBits ( ) ;

		this->enable ( enable ) ;
	}
		
	AbsDevice::AbsDevice ( const char * name , const bool enable )
	{
		uinput_user_dev dev ;
		memset ( & dev, 0, sizeof ( dev ) ) ;

		if ( name == NULL )		
			snprintf ( dev.name, UINPUT_MAX_NAME_SIZE, "funlibrary-abs" ) ;
		else
			strncpy ( dev.name, name, UINPUT_MAX_NAME_SIZE ) ;

		dev.id.bustype = BUS_USB ;
		dev.id.vendor = 1 ;
		dev.id.product = 1 ;
		dev.id.version = 1 ;

		for ( int i = 0 ; i < ABS_MAX ; i++ )
		{
			dev.absmax[i] = 0x7FFFFFFF;
			dev.absmin[i] = 0;
			dev.absfuzz[i] = 0;
			dev.absflat[i] = 0;
		}

		setDev ( & dev ) ;
		
		prepBits ( ) ;

		this->enable ( enable ) ;
	}

	RelDevice::RelDevice (const char * name , const bool enable )
	{
		uinput_user_dev dev ;
		memset ( & dev, 0, sizeof ( dev ) ) ;

		if ( name == NULL )	
			snprintf ( dev.name, UINPUT_MAX_NAME_SIZE, "funlibrary-rel" ) ;
		else
			strncpy ( dev.name, name, UINPUT_MAX_NAME_SIZE ) ;

		dev.id.bustype = BUS_USB ;
		dev.id.vendor = 1 ;
		dev.id.product = 1 ;
		dev.id.version = 1 ;
		setDev ( & dev ) ;

		prepBits ( ) ;

		this->enable ( enable ) ;
	}

	void KeyDevice::prepBits ( )
	{
		map < int, vector < int > > bitmap =
			{ { UI_SET_EVBIT, { EV_KEY, EV_MSC, EV_SYN } }, { UI_SET_MSCBIT, { MSC_SCAN } } } ;
		vector < int > veccy ;
		for ( int i = 0 ; i < KEY_CNT ; i++ )
		{
			veccy.push_back ( i ) ;
		}

		bitmap.insert ( pair < int, vector < int > > ( UI_SET_KEYBIT, veccy ) ) ;
		
		setInputBits ( bitmap ) ;
	} 

	void AbsDevice::prepBits ( )
	{
		// Setting a UI_SET_KEYBIT seems to be required.
		map < int, vector < int > > bitmap =
			{ {UI_SET_EVBIT, {EV_ABS, EV_KEY, EV_SYN}}, {UI_SET_KEYBIT, {BTN_LEFT}} } ;
		vector < int > veccy ;
		// For some reason we cannot set more than this many ABS values
		for ( int i = ABS_X ; i < ABS_CNT / 3 ; i++ )
		{
			veccy.push_back ( i ) ;
		}

		bitmap.insert ( pair < int, vector < int > > ( UI_SET_ABSBIT, veccy ) ) ;
		
		setInputBits ( bitmap ) ;
	}

	void RelDevice::prepBits ( )
	{
		// Setting a UI_SET_KEYBIT seems to be required.
		map < int, vector < int > > bitmap =
			{ {UI_SET_EVBIT, {EV_REL, EV_KEY, EV_SYN}}, {UI_SET_KEYBIT, {BTN_LEFT}} } ;
		vector < int > veccy ;
		for ( int i = REL_X ; i < REL_CNT ; i++ )
		{
			veccy.push_back ( i ) ;
		}

		bitmap.insert ( pair < int, vector < int > > ( UI_SET_RELBIT, veccy ) ) ;
		setInputBits ( bitmap ) ;
	}
}
