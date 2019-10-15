/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

/*! \file
 *  \brief \ref ISignal - Abstraction to \ref mcr_register a new ISignal in
 *  C++.
 */

#ifndef MCR_EXTRAS_ISIGNAL_H_
#define MCR_EXTRAS_ISIGNAL_H_

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"

namespace mcr
{
/*! \ref mcr_ISignal C++ indirection for \ref IData classes
 *
 *  Usage: \n
 *  \code
 *  try {
 *    // optional template typedef
 *    typedef ISignal<IDataType> MyCppISignal;
 *    ISignal<IDataType> myISignal(Libmacro::instance());
 *    // optional, if data size of IDataType is not correct
 *    isignal.instance.data_size = sizeof(IDataType_subclass);
 *    isignal.registerType();
 *  } catch (int err) {
 *  // Show error message
 *  }
 *  \endcode
 */
template<typename T>
class ISignal : public CtxISignal
{
public:
	ISignal(Libmacro *context = nullptr, mcr_Dispatcher *dispatcher = nullptr)
		: CtxISignal(context, ISignal<T>::send, dispatcher,
					 mcr_Interface_new(nullptr, sizeof(T),
									   ISignal<T>::init, ISignal<T>::deinit, ISignal<T>::compare,
									   ISignal<T>::copy))
	{
		isignal.interface.context = this->context()->ptr();
	}

	/*! \ref mcr_Interface.init */
	static int init(void *dataPt)
	{
		if (dataPt) {
			try {
				new (static_cast<T *>(dataPt)) T();
			} catch(int err) {
				return err;
			}
		}
		return 0;
	}
	/*! \ref mcr_Interface.deinit */
	static int deinit(void *dataPt)
	{
		if (dataPt) {
			try {
				static_cast<T *>(dataPt)->~T();
			} catch(int err) {
				return err;
			}
		}
		return 0;
	}
	/*! \ref mcr_Interface.compare */
	static int compare(const void *lhsPt, const void *rhsPt)
	{
		if (rhsPt) {
			if (lhsPt) {
				return static_cast<const T *>(lhsPt)->compare(*static_cast<const T *>(rhsPt));
			}
			return -1;
		}
		return !!lhsPt;
	}
	/*! \ref mcr_Interface.copy */
	static int copy(void *dstPt, const void *srcPt)
	{
		if (!dstPt)
			return EFAULT;
		try {
			static_cast<T *>(dstPt)->copy(static_cast<const T *>(srcPt));
		} catch (int err) {
			return err;
		}
		return 0;
	}

	/*! \ref mcr_ISignal.send */
	static int send(mcr_Signal * signalPt)
	{
		dassert(signalPt->isignal);
		T *dataPt = static_cast<T *>(mcr_Instance_data(signalPt));
		if (dataPt) {
			try {
				dataPt->send();
			} catch (int err) {
				return err;
			}
		}
		return 0;
	}

	static ISignal<T> *cast(CtxISignal *pt)
	{
		return dynamic_cast<ISignal<T> *>(pt);
	}
	static const ISignal<T> *cast(const CtxISignal *pt)
	{
		return dynamic_cast<const ISignal<T> *>(pt);
	}

	/*! \ref mcr_register */
	ISignal<T> &registerType()
	{
		T inst;
		size_t count = inst.addNameCount();
		const char **addN = count ? new const char *[count] : nullptr;
		/* If not implemented this will do nothing \ref IData.addNames */
		inst.addNames(addN, count);
		ISignalRef(context(), ptr()).registerType(inst.name(), addN, count);
		if (addN)
			delete []addN;
		return *this;
	}
};
}

#endif
