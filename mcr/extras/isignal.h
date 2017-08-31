/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

/*!
 * \file
 * \brief \ref ISignal - Abstraction to \ref mcr_register a new ISignal in
 * C++.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_ISIGNAL_H
#define MCR_EXTRAS_ISIGNAL_H

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"

namespace mcr
{
/*!
 * \brief \ref mcr_ISignal C++ indirection for \ref ISignalData classes
 *
 * Usage: \n
 * \code
 * try {
 *   // optional template typedef
 *   typedef ISignal<ISignalDataType> MyCppISignal;
 *   ISignal<ISignalDataType> myISignal(Libmacro::instance());
 *   // optional, if data size of ISignalDataType is not correct
 *   isignal.instance.data_size = sizeof(ISignalDataType_subclass);
 *   isignal.registerType();
 * } catch (int err) {
 * // Show error message
 * }
 * \endcode
 */
template<typename T>
class ISignal : public CtxISignal
{
public:
	ISignal(Libmacro *context = NULL, mcr_Dispatcher *dispatcher = NULL)
		: CtxISignal(context, ISignal<T>::send, dispatcher,
			     mcr_Interface_new(sizeof(T),
					       ISignal<T>::init, ISignal<T>::deinit, ISignal<T>::compare,
					       ISignal<T>::copy))
	{
	}

	/*! \brief \ref mcr_Interface.init */
	static int init(void *dataPt)
	{
		if (dataPt) {
			try {
				new ((T *) dataPt) T();
			} catch(int err) {
				return err;
			}
		}
		return 0;
	}
	/*! \brief \ref mcr_Interface.deinit */
	static int deinit(void *dataPt)
	{
		if (dataPt) {
			try {
				((T *)dataPt)->~T();
			} catch(int err) {
				return err;
			}
		}
		return 0;
	}
	/*! \brief \ref mcr_Interface.compare */
	static int compare(const void *lhsPt, const void *rhsPt)
	{
		if (rhsPt) {
			if (lhsPt) {
				return ((const T *) lhsPt)->compare(*(const T *)rhsPt);
			}
			return -1;
		}
		return !!lhsPt;
	}
	/*! \brief \ref mcr_Interface.copy */
	static int copy(void *dstPt, const void *srcPt)
	{
		if (!dstPt)
			return EFAULT;
		try {
			((T *) dstPt)->copy((T *) srcPt);
		} catch (int err) {
			return err;
		}
		return 0;
	}

	/*! \brief \ref mcr_ISignal.send */
	static int send(mcr_Signal * signalPt)
	{
		dassert(signalPt->isignal);
		T *dataPt = (T *)mcr_Instance_data(signalPt);
		if (dataPt) {
			try {
				dataPt->send();
			} catch (int err) {
				return err;
			}
		}
		return 0;
	}

	/*! \brief \ref mcr_register */
	void registerType()
	{
		T inst;
		size_t count = inst.addNamesCount();
		const char **addN = count ? new const char *[count] : NULL;
		inst.addNames(addN,
			      count); /* If not implemented this will do nothing \ref ISignalData.addNames */
		ISignalRef(ptr()).registerType(inst.name(), addN, count);
		if (addN)
			delete []addN;
	}
};
}

#endif
