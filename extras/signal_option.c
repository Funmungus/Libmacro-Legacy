/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#include "mcr/extras/extras.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MCR_API const char *mcr_signalOptionName = "signal";
MCR_API const char *mcr_sendOptionName = "send";
static struct mcr_Map _sigOpts;
static struct mcr_Array _sigMods;

typedef int (*data_modify_fnc) (void *data, char *value);

static int signal_modify_loop(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut, const char **fncNames,
	const int *fncNameLengths, const data_modify_fnc * fncs,
	const int fncSetLen);

static int alarm_set_sec(void *data, char *value);
static int alarm_set_min(void *data, char *value);
static int alarm_set_hour(void *data, char *value);
static int alarm_set_mday(void *data, char *value);
static int alarm_set_mon(void *data, char *value);
static int alarm_set_year(void *data, char *value);
static int alarm_set_wday(void *data, char *value);
static int alarm_set_yday(void *data, char *value);
static int alarm_set_isdst(void *data, char *value);

/* Echo special case */

static int key_set_key(void *data, char *value);
static int key_set_scan(void *data, char *value);
static int key_set_up(void *data, char *value);

static int mc_set_x(void *data, char *value);
static int mc_set_y(void *data, char *value);
static int mc_set_z(void *data, char *value);
static int mc_set_justify(void *data, char *value);

static int noop_set_sec(void *data, char *value);
static int noop_set_nsec(void *data, char *value);

static int scroll_set_x(void *data, char *value);
static int scroll_set_y(void *data, char *value);
static int scroll_set_z(void *data, char *value);

/* Mods special case */
/* Script special case */

static int command_set_file(void *data, char *value);
static int command_add_arg(void *data, char *value);
static int command_add_env(void *data, char *value);

static int stringkey_set_text(void *data, char *value);
static int stringkey_set_delay(void *data, char *value);

int mcr_signal_option(int argc, char **argv, int index)
{
	dassert(argv);
	dassert(index < argc);

	/* Need at least one additional argument for signal functions. */
	MCR_OPTION_MIN_OPTS(argc, argv, index, 1);

	mcr_option_fnc *fPt = mcr_StringMap_value(&_sigOpts, argv[index]);
	if (fPt)
		index = (*fPt) (argc, argv, index);
	return index;
}

void mcr_signal_option_set(char *key, mcr_option_fnc optFnc)
{
	dassert(key);
	if (!optFnc) {
		mcr_StringMap_unmap(&_sigOpts, key);
	} else if (!mcr_StringMap_map(&_sigOpts, key, &optFnc))
		dmsg;
}

int mcr_signal_send_option(int argc, char **argv, int index)
{
	dassert(argv);
	dassert(index < argc);

	MCR_OPTION_MIN_OPTS(argc, argv, index, 1);
	struct mcr_Signal sendSig = {.isig = NULL };
	/* Will be at one after "send" */
	index = mcr_signal_modify(argc, argv, index, &sendSig);
	mcr_send(NULL, &sendSig);
	mcr_Signal_free(&sendSig);
	return index;
}

int mcr_signal_add_name(int argc, char **argv, int index)
{
	dassert(argv);
	dassert(index < argc);

	/* At least 2 args, signal to add name, and name to add. */
	MCR_OPTION_MIN_OPTS(argc, argv, index, 2);
	struct mcr_ISignal *iSigPt = mcr_ISignal_from_name(NULL, argv[index++]);
	mcr_reg_add_name(mcr_ISignal_reg(NULL), iSigPt, argv[index++]);
	return index;
}

int mcr_signal_add_names(int argc, char **argv, int index)
{
	dassert(argv);
	dassert(index < argc);

	/* At least 2 args, signal to add name, and name ( s ) to add. */
	MCR_OPTION_MIN_OPTS(argc, argv, index, 2);
	struct mcr_ISignal *iSigPt = mcr_ISignal_from_name(NULL, argv[index++]);
	int isSetNotation = argv[index][0] == '{';
	if (isSetNotation)
		++index;
	int first = index;
	int end = first;
	while (index < argc) {
		if (isSetNotation && argv[index][0] == '}') {
			end = index;
			++index;
			break;
		}

		if (++index >= argc)
			end = index;
	}
	mcr_reg_add_names(mcr_ISignal_reg(NULL), iSigPt,
		(const char **)argv + first, (size_t) (end - first));
	return index;
}

#define suredata( sigPt ) \
if ( !( sigPt )->inst.data.data ) \
	( sigPt )->inst.data = mcr_imkdata (( sigPt )->iface );

#define error_return( expression, errIndex ) \
if ( expression ) \
{ \
	dmsg; \
	mcr_Script_parse_error (); \
	return errIndex; \
}

int mcr_signal_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);

	/* Should be at signal type name. */
	struct mcr_ISignal *isigPt = mcr_ISignal_from_name(NULL, argv[index]);
	if (!isigPt || _sigMods.used < isigPt->iface.id)
		return index;	/* First index is not processed. */
	if (sigPtOut->isig != isigPt) {
		mcr_Signal_free(sigPtOut);
		sigPtOut->isig = isigPt;
	}
	/* One past signal type name. Might have no arguments to modify. */
	++index;
	if (index >= argc)
		return index;
	suredata(sigPtOut);
	mcr_signal_modify_fnc *fPt = MCR_ARR_ELEMENT(_sigMods,
		isigPt->iface.id);
	dassert(fPt);
	if (*fPt)
		return (*fPt) (argc, argv, index, sigPtOut);
	return index;
}

void mcr_signal_modify_set(size_t sigId, mcr_signal_modify_fnc setModifier)
{
	dassert(sigId != (size_t) ~ 0);
	if (sigId >= _sigMods.used) {
		mcr_Array_insert_filled(&_sigMods, sigId, &setModifier, 1,
			NULL);
	} else {
		mcr_Array_set(&_sigMods, sigId, &setModifier);
	}
}

int mcr_alarm_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iAlarm(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"sec", "min", "hr", "hour", "mday", "monthday", "mon", "yr",
		"year", "wday", "weekday", "yday", "yearday", "dst", "isdst"
	};
	const int lens[] = {
		3, 3, 2, 4, 4, 8, 3, 2,
		4, 4, 7, 4, 7, 3, 5
	};
	const data_modify_fnc datafncs[] = {
		alarm_set_sec, alarm_set_min, alarm_set_hour, alarm_set_hour,
		alarm_set_mday, alarm_set_mday, alarm_set_mon, alarm_set_year,
		alarm_set_year, alarm_set_wday, alarm_set_wday, alarm_set_yday,
		alarm_set_yday, alarm_set_isdst, alarm_set_isdst
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_echo_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iHidEcho(NULL));
	suredata(sigPtOut);

	int setNotation = argv[index][0] == '{';
	if (setNotation)
		++index;
	if (index >= argc)
		return index;
	if (argv[index][0] == '}')
		return index + 1;
	if (index + 1 >= argc)	/* NoVal */
		return index;

	int echo;
	char *afterval = NULL;
	if (isdigit(argv[index][0]))
		echo = strtol(argv[index], &afterval, 0);
	else
		echo = mcr_HidEcho_name_echo(NULL, argv[index]);
	/* Already checked for digit on the strtol. */
	mcr_Echo_set_echo(sigPtOut->inst.data.data, echo);
	++index;
	return setNotation && argv[index][0] == '}' ? index + 1 : index;
}

int mcr_key_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iKey(NULL));
	suredata(sigPtOut);
	UNUSED(sigPtOut);

	const char *datanames[] = {
		"key", "scan", "up", "keyup", "key up", "key_up"
	};
	const int lens[] = {
		3, 4, 2, 5, 6, 6
	};
	const data_modify_fnc datafncs[] = {
		key_set_key, key_set_scan, key_set_up, key_set_up, key_set_up
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_movecursor_modify(int argc, char **argv,
	int index, struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iMoveCursor(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"x", "y", "z", "justify"
	};
	const int lens[] = {
		1, 1, 1, 7
	};
	const data_modify_fnc datafncs[] = {
		mc_set_x, mc_set_y, mc_set_z, mc_set_justify
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_noop_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{

	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iNoOp(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"sec", "nsec"
	};
	const int lens[] = {
		3, 4
	};
	const data_modify_fnc datafncs[] = {
		noop_set_sec, noop_set_nsec
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_scroll_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iScroll(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"x", "y", "z"
	};
	const int lens[] = {
		1, 1, 1
	};
	const data_modify_fnc datafncs[] = {
		scroll_set_x, scroll_set_y, scroll_set_z
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_mods_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iMods(NULL));
	suredata(sigPtOut);
	return index;
}

int mcr_command_modify(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut)
{

	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iCommand(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"file", "command", "path", "arg", "env"
	};
	const int lens[] = {
		4, 7, 4, 3, 3
	};
	const data_modify_fnc datafncs[] = {
		command_set_file, command_set_file, command_set_file,
		command_add_arg, command_add_env
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

int mcr_stringkey_modify(int argc, char **argv,
	int index, struct mcr_Signal *sigPtOut)
{
	dassert(argv);
	dassert(index < argc);
	dassert(sigPtOut->isig == mcr_iStringKey(NULL));
	suredata(sigPtOut);

	const char *datanames[] = {
		"text", "delay"
	};
	const int lens[] = {
		4, 5
	};
	const data_modify_fnc datafncs[] = {
		stringkey_set_text, stringkey_set_delay
	};
	const int len = arrlen(lens);

	return signal_modify_loop(argc, argv, index, sigPtOut,
		datanames, lens, datafncs, len);
}

static int signal_modify_loop(int argc, char **argv, int index,
	struct mcr_Signal *sigPtOut, const char **fncNames,
	const int *fncNameLengths, const data_modify_fnc * fncs,
	const int fncSetLen)
{
	int setNotation = argv[index][0] == '{';
	if (setNotation)
		++index;
	char *name, *value;
	int i;
	while (index + 1 < argc) {
		name = argv[index];
		value = argv[index + 1];
		if (setNotation) {
			if (*name == '}')
				return index + 1;
			if (*value == '}')
				return index + 2;
		}
		for (i = 0; i < fncSetLen; i++) {
			if (!strncasecmp(name, fncNames[i],
					(unsigned)fncNameLengths[i])) {
				/* Invalid value string. */
				error_return(!fncs[i] (sigPtOut->inst.data.data,
						value), index);
				/* Now at next member name or end. */
				break;
			}
		}
		/* No data name processed, this signal is done. */
		if (i == fncSetLen)
			return index;
		index += 2;
	}
	return index;
}

static int strtol_ok(char *valueIn, long *valOut)
{
	char *afterval = NULL;
	*valOut = strtol(valueIn, &afterval, 0);
	if (afterval == NULL || afterval == valueIn) {
		dmsg;
		mcr_Script_parse_error();
		return 0;
	}
	return 1;
}

static int alarm_set_sec(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_sec = val % 62;
		if (val / 60) {
			((mcr_Alarm *) data)->tm_min = (val / 60) % 60;
		}
		return 1;
	}
	return 0;
}

static int alarm_set_min(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_min = val % 60;
		if (val / 60) {
			((mcr_Alarm *) data)->tm_hour = (val / 60) % 60;
		}
		return 1;
	}
	return 0;
}

static int alarm_set_hour(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_hour = val % 24;
		return 1;
	}
	return 0;
}

static int alarm_set_mday(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_mday = val % 32;
		if (!((mcr_Alarm *) data)->tm_mday)
			((mcr_Alarm *) data)->tm_mday = 1;
		return 1;
	}
	return 0;
}

static int alarm_set_mon(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_mon = val % 12;
		return 1;
	}
	return 0;
}

static int alarm_set_year(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_year = val;
		return 1;
	}
	return 0;
}

static int alarm_set_wday(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_wday = val;
		((mcr_Alarm *) data)->tm_wday %= 7;
		return 1;
	}
	return 0;
}

static int alarm_set_yday(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_yday = val;
		((mcr_Alarm *) data)->tm_yday %= 366;
		return 1;
	}
	return 0;
}

static int alarm_set_isdst(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_Alarm *) data)->tm_isdst = val;
		((mcr_Alarm *) data)->tm_isdst =
			! !((mcr_Alarm *) data)->tm_isdst;
		return 1;
	}
	return 0;
}

static int key_set_key(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_Key_set_key(data, val);
		return 1;
	}
	return 0;
}

static int key_set_scan(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_Key_set_scan(data, val);
		return 1;
	}
	return 0;
}

static int key_set_up(void *data, char *value)
{
	if (!strncasecmp(value, "down", 4)) {
		mcr_Key_set_up_type(data, MCR_DOWN);
	} else if (!strncasecmp(value, "up", 2)) {
		mcr_Key_set_up_type(data, MCR_UP);
	} else if (!strncasecmp(value, "both", 4)) {
		mcr_Key_set_up_type(data, MCR_BOTH);
	} else if (!strncasecmp(value, "toggle", 6)) {
		mcr_Key_set_up_type(data, MCR_TOGGLE);
	} else
		return 0;
	return 1;
}

static int mc_set_x(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_MC_set_coordinate(data, MCR_X, val);
		return 1;
	}
	return 0;
}

static int mc_set_y(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_MC_set_coordinate(data, MCR_Y, val);
		return 1;
	}
	return 0;
}

static int mc_set_z(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_MC_set_coordinate(data, MCR_Z, val);
		return 1;
	}
	return 0;
}

static int mc_set_justify(void *data, char *value)
{
	bool justify;
	if (mcr_atob(value, &justify)) {
		mcr_MC_set_justify(data, justify);
		return 1;
	}
	return 0;
}

static int noop_set_sec(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_NoOp *) data)->tv_sec = val;
		return 1;
	}
	return 0;
}

static int noop_set_nsec(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		((mcr_NoOp *) data)->tv_nsec = val;
		return 1;
	}
	return 0;
}

static int scroll_set_x(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_Scroll_set_coordinate(data, MCR_X, val);
		return 1;
	}
	return 0;
}

static int scroll_set_y(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_Scroll_set_coordinate(data, MCR_Y, val);
		return 1;
	}
	return 0;
}

static int scroll_set_z(void *data, char *value)
{
	long val;
	if (strtol_ok(value, &val)) {
		mcr_Scroll_set_coordinate(data, MCR_Z, val);
		return 1;
	}
	return 0;
}

static int command_set_file(void *data, char *value)
{
	UNUSED(data);
	UNUSED(value);
	return 0;
}

static int command_add_arg(void *data, char *value)
{
	UNUSED(data);
	UNUSED(value);
	return 0;
}

static int command_add_env(void *data, char *value)
{
	UNUSED(data);
	UNUSED(value);
	return 0;
}

static int stringkey_set_text(void *data, char *value)
{
	UNUSED(data);
	UNUSED(value);
	return 0;
}

static int stringkey_set_delay(void *data, char *value)
{
	UNUSED(data);
	UNUSED(value);
	return 0;
}

void mcr_signaloption_initialize()
{
	mcr_StringMap_init(&_sigOpts);
	mcr_StringMap_set_all(&_sigOpts, sizeof(void *), NULL);
	mcr_Array_init(&_sigMods);
	mcr_Array_set_all(&_sigMods, NULL, sizeof(void *));
}

void mcr_signaloption_cleanup()
{
	mcr_Map_free(&_sigOpts);
	mcr_Array_free(&_sigMods);
}
