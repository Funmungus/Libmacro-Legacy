/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
	}
	void ScreenCrop::ScreenSafety::reset ( )
	{
		if ( obj )
			ReleaseDC ( NULL, obj ) ;
		obj = GetDC ( NULL ) ;
	}

	ScreenCrop::ScreenSafety ScreenCrop::_hdc ;
	int ScreenCrop::_scrHeight = 0 ;
	int ScreenCrop::_scrWidth = 0 ;

	SpacePosition ScreenCrop::convertFromSpatial ( const SpacePosition & spatial )
	{
		SpacePosition ret ;
		ret.x = ( spatial.x * GetSystemMetrics ( SM_CXVIRTUALSCREEN ) ) / 65536 ;
		ret.y = ( spatial.y * GetSystemMetrics ( SM_CYVIRTUALSCREEN ) ) / 65536 ;
		ret.z = ( spatial.z ) ;
		return ret ;
	}

	void ScreenCrop::resetBmp ( )
	{
		if ( _mDesk )
			DeleteDC ( _mDesk ) ;
		if ( _mBmp )
			DeleteObject ( _mBmp ) ;

		_hdc.reset ( ) ; 	// recover main desktop screen
		_mDesk = CreateCompatibleDC ( _hdc.obj ) ;

		_scrHeight = GetSystemMetrics ( SM_CYVIRTUALSCREEN ) ;
		_scrWidth = GetSystemMetrics ( SM_CXVIRTUALSCREEN ) ;
		if ( _pos.x > _scrWidth ) _pos.x = _scrWidth ;
		if ( _pos.y > _scrHeight ) _pos.y = _scrHeight ;

		// TODO decide if x and y can be negative
		if ( _pos.x < 0 ) _pos.x = 0 ;
		if ( _pos.y < 0 ) _pos.y = 0 ;

		_actualWidth = ( int ) _pos.x + _dm.x < _scrWidth ?
			( int ) _dm.x : _scrWidth - ( int ) _pos.x ;
		_actualHeight = ( int ) _pos.y + _dm.y < _scrHeight ?
			( int ) _dm.y : _scrHeight - ( int ) _pos.y ;

		if ( _actualWidth == 0 ) _actualWidth = 1 ;
		if ( _actualHeight == 0 ) _actualHeight = 1 ;

		_mBmp = CreateCompatibleBitmap ( _hdc.obj, _actualWidth, _actualHeight ) ;
		// use the previously created device context with the bitmap
		SelectObject ( _mDesk, _mBmp ) ;
	}

	bool ScreenCrop::send ( )
	{
		FILE * fp ;
		FileSafety fs ;
		if ( _filename == "stdout" )
			fp = stdout ;
		else if ( _filename == "stderr" || _filename.empty ( ) )
			fp = stderr ;
		else
		{
			fopen_s ( & fp, _filename.c_str ( ), "wb" ) ;
			if ( fp == NULL )
				fp = stderr ;
			else
				fs.fp = fp ;
		}

		// copy from the desktop device context to the bitmap device context
		// call this once per 'frame'
		BitBlt ( _mDesk, 0, 0, _actualWidth, _actualHeight,
			_hdc.obj, ( int ) _pos.x, ( int ) _pos.y, SRCCOPY ) ;

		LPVOID pBuf = NULL ;
		BITMAPINFO bmpInfo ;
		BITMAPFILEHEADER bmpFileHeader ;

		do
		{
			ZeroMemory ( & bmpInfo, sizeof ( BITMAPINFO ) ) ;
			bmpInfo.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER ) ;
			int scansRead = GetDIBits ( _mDesk, _mBmp, 0, 0, NULL, & bmpInfo, DIB_RGB_COLORS ) ;
			if ( scansRead == NULL )
			{
				//std::wcerr << L"Unable to get bitmap info." << std::endl ;
				fprintf ( stderr, "Unable to get bitmap info.\n" ) ;
				break ;
			}
			if ( bmpInfo.bmiHeader.biSizeImage <= 0 )
				bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * abs ( bmpInfo.bmiHeader.biHeight ) * ( bmpInfo.bmiHeader.biBitCount + 7 ) / 8 ;
			if ( ( pBuf = malloc ( bmpInfo.bmiHeader.biSizeImage ) ) == NULL )
			{
				//std::wcerr << L"Unable to Allocate Bitmap Memory" << std::endl ;
				fprintf ( stderr, "Unable to Allocate Bitmap Memory.\n" ) ;
				break ;
			}
			bmpInfo.bmiHeader.biCompression = BI_RGB ;
			scansRead = GetDIBits ( _mDesk, _mBmp, 0, bmpInfo.bmiHeader.biHeight, pBuf, & bmpInfo, DIB_RGB_COLORS ) ;
// File should already be opened at this point.
//			if ( ! fileName.empty ( ) )
//			{
//				errno_t err = _wfopen_s ( & fp, L"tmp.bmp", L"wb" ) ;
//				if ( fp == NULL )
//				{
//					std::wcerr << L"Could not open file tmp.bmp." << std::endl ;
//					break ;
//				}
//			}
			bmpFileHeader.bfReserved1 = 0 ;
			bmpFileHeader.bfReserved2 = 0 ;
			bmpFileHeader.bfSize = sizeof ( BITMAPFILEHEADER ) +sizeof ( BITMAPINFOHEADER ) +bmpInfo.bmiHeader.biSizeImage ;
			bmpFileHeader.bfType = 'MB' ;
			bmpFileHeader.bfOffBits = sizeof ( BITMAPFILEHEADER ) +sizeof ( BITMAPINFOHEADER ) ;
			//BitBlt ( ) // This crops image.
			if ( fp != NULL )
			{
				fwrite ( & bmpFileHeader, sizeof ( BITMAPFILEHEADER ), 1, fp ) ;
				fwrite ( & bmpInfo.bmiHeader, sizeof ( BITMAPINFOHEADER ), 1, fp ) ;
				fwrite ( pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp ) ;
			}
		} while ( false ) ;

		if ( pBuf ) free ( pBuf ) ;

		return true ;
	}
}
