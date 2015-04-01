#include "../common/def_os_selector_export.h"
#include "watchWrapper.h"

DWatch *dinocv_watch_create()
{
	return new DWatch();
}

void dinocv_watch_release(DWatch *watch)
{
	delete watch;
}

void dinocv_watch_start(DWatch *watch)
{
	watch->start();
}

void dinocv_watch_stop(DWatch *watch)
{
	watch->stop();
}

const float dinocv_watch_get_duration_second(DWatch *watch)
{
	return watch->getDurationSecond();
}

const float dinocv_watch_get_duration_milisecond(DWatch *watch)
{
	return watch->getDurationMilliSecond();
}

const char *dinocv_watch_get_time_text(DWatch *watch)
{
	return watch->getTextTime();
}
