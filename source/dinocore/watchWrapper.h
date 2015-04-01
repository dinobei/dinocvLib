#ifndef __WATCH_WRAPPER_H__
#define __WATCH_WRAPPER_H__
#include "../common/def_os_selector_header.h"
#include "cwatch.h"

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	DWatch *dinocv_watch_create();

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_watch_release(DWatch *watch);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_watch_start(DWatch *watch);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_watch_stop(DWatch *watch);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	const float dinocv_watch_get_duration_second(DWatch *watch);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	const float dinocv_watch_get_duration_milisecond(DWatch *watch);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	const char *dinocv_watch_get_time_text(DWatch *watch);






#endif
