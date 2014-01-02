/*! \file signal/screencrop.h
 * \brief ScreenCrop, Take a screen shot and crop it to size. Bitmap is saved to file..
 */

# ifndef MCR_SCREEN_CROP_H
# define MCR_SCREEN_CROP_H

//# include "signal/spaceposition.h"
# include "signal/isignal.h"
# include "signal/native/screencrop.h"

namespace macro
{
	/*! \brief Take a screen shot and crop it to size. Bitmap is saved to file.
	 * If filename is stdout or stderr, the bitmap is printed in binary to those file pointers.
	 */
	class ScreenCrop : public ISignal
	{
	public:
		/*! \brief ScreenCrop
		 *
		 * \ref ISignal::name
		 */
		static MCR_API const std::string name ;
		// ! \brief \ref ISignal#dispatch ( )
		static MCR_API bool ( * dispatcher ) ( ScreenCrop & ) ;
		/*! ctor
		 */
		inline ScreenCrop ( const SpacePosition pos = {0},
			const Dimensions dimensions = {0},
			const std::string & fileName = "macro.bmp" )
			: _nCrop ( pos, dimensions, fileName )
		{	}
		/*! ctor
		 */
		inline ScreenCrop ( const std::string filename )
			: _nCrop ( {0}, {0}, filename )
		{	}
		// ! \brief ctor
		inline ScreenCrop ( const ScreenCrop & copytron )
			: _nCrop ( copytron.getPosition ( ), copytron.getDimensions ( ),
					copytron.getFilename ( ) )
		{	}
		// ! \brief Values are copied from this copytron.
		inline ScreenCrop & operator = ( const ScreenCrop & copytron )
		{
			if ( & copytron != this )
			{
				setPosition ( copytron.getPosition ( ) ) ;
				setDimensions ( copytron.getDimensions ( ) ) ;
				setFilename ( copytron.getFilename ( ) ) ;
			}
			return * this ;
		}

		// ! \brief Get the coordinate value representing the top-left rectangle corner.
		// ! \param positionType \ref \def MCR_DIMENSION_MAX
		inline long long getPosition ( const unsigned int positionType ) const
		{
			return macro::getPosition ( _nCrop, positionType ) ;
		}
		// ! \brief Set the coordinate value representing the top-left rectangle corner.
		// ! \param positionType \ref \def MCR_DIMENSION_MAX
		inline void setPosition ( const unsigned int positionType, const long long posValue )
		{
			macro::setPosition ( _nCrop, positionType, posValue ) ;
		}
		// ! \brief Get the set of all cubic coordinates ( point of top-left-foreground )
		inline const SpacePosition getPosition ( ) const
		{
			return macro::getPosition ( _nCrop ) ;
		}
		// ! \brief Set the set of all cubic coordinates ( point of top-left-foreground )
		inline void setPosition ( const SpacePosition pos )
		{
			macro::setPosition ( _nCrop, pos ) ;
		}
		// ! \brief Get a rectangular dimension value.
		// ! \param positionType \ref \def MCR_DIMENSION_MAX
		inline long long getDimension ( const unsigned int positionType ) const
		{
			return macro::getDimension ( _nCrop, positionType ) ;
		}
		// ! \brief Set the rectangular dimension value.
		// ! \param positionType \ref \def MCR_DIMENSION_MAX
		inline void setDimension ( const unsigned int positionType, const long long dimenstionValue )
		{
			macro::setDimension ( _nCrop, positionType, dimensionValue ) ;
		}
		// ! \brief Get the set of all dimensions, width, height, depth
		inline const Dimensions getDimensions ( ) const
		{
			return macro::getDimensions ( _nCrop ) ;
		}
		// ! \brief Set the set of all dimensions, width, height, depth
		inline void setDimensions ( const Dimensions dm )
		{
			macro::setDimensions ( _nCrop, dm ) ;
		}
		// ! \brief Get filename this is saving to.
		inline std::string getFilename ( ) const
		{
			return macro::getFileName ( _nCrop ) ;
		}
		// ! \brief Set filename or stdstream to save this to.
		inline void setFilename ( const std::string & filename )
		{
			macro::setFileName ( _nCrop, filename ) ;
		}

		/*! \brief \ref ScreenCrop::name */
		virtual MCR_API std::string type ( ) const ;
		/*! \ref With given dimensions at given point crop the screen
		 * and output to file.
		 */
		virtual inline bool send ( )
		{
			if ( dispatch ( ) ) return true ;
			return macro::send ( _nCrop ) ;
		}
		// ! \brief \ref ISignal#dispatch ( )
		virtual inline bool			dispatch ( )
		{
			if ( dispatcher != NULL )
			{
				if ( dispatcher ( * this ) )
					return true ;
			}
			return ISignal::dispatch ( ) ;
		}

		// Members are native, no operator = needed.
		inline bool operator == ( const ScreenCrop & rhs ) const
		{
			for ( unsigned int i = MCR_X ; i < MCR_DIMENSION_CNT ; i++ )
			{
				if ( getPosition ( i ) != rhs.getPosition ( i ) )
					return false ;
				if ( getDimension ( i ) != rhs.getDimension ( i ) )
					return false ;
			}
			if ( getFilename ( ) != rhs.getFilename ( ) )
				return false ;

			return true ;
		}
		inline bool operator != ( const ScreenCrop & rhs ) const
		{
			bool modori = ! ( * this == rhs ) ;

			return modori ;

		}
		inline bool operator < ( const ScreenCrop & rhs ) const
		{
			for ( unsigned int i = MCR_X ; i < MCR_DIMENSION_CNT ; i++ )
			{
				if ( getPosition ( i ) != rhs.getPosition ( i ) )
					return getPosition ( i ) < rhs.getPosition ( i ) ;
			}
			for ( unsigned int i = MCR_X ; i < MCR_DIMENSION_CNT ; i++ )
			{
				if ( getDimension ( i ) != rhs.getDimension ( i ) )
					return getDimension ( i ) < rhs.getDimension ( i ) ;
			}
			if ( getFilename ( ) != rhs.getFilename ( ) )
				return getFilename ( ) < rhs.getFilename ( ) ;

			return false ;
		}
		inline bool operator > ( const ScreenCrop & rhs ) const
		{
			if ( * this == rhs ) return false ;

			return ! ( * this < rhs ) ;
		}
		inline bool operator <= ( const ScreenCrop & rhs ) const
		{
			bool modori = ! ( * this > rhs ) ;

			return modori ;
		}
		inline bool operator >= ( const ScreenCrop & rhs ) const
		{
			bool modori = ! ( * this < rhs ) ;

			return modori ;
		}
	private:
		NativeScreenCrop _nCrop ;
	} ;
}

# endif
