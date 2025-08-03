#include "helpers.h"

#include <stdio.h>

void sprint_friendly_time(double_t total_seconds, char buffer[256])
{
    uint32_t days = (uint32_t)(total_seconds / (60 * 60 * 24));
    total_seconds -= days * 60.0 * 60.0 * 24.0;
    uint32_t hours = (uint32_t)(total_seconds / (60 * 60));
    total_seconds -= hours * 60.0 * 60.0;
    uint32_t minutes = (uint32_t)(total_seconds / 60);
    total_seconds -= minutes * 60.0;
    uint32_t seconds = (uint32_t)total_seconds;

    if (days > 0)
    {
        snprintf(buffer, 256, "%d day%s, %d hour%s, %d minute%s, %d second%s",
            days, days > 1 ? "s" : "",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (hours > 0)
    {
        snprintf(buffer, 256, "%d hour%s, %d minute%s, %d second%s",
            hours, hours > 1 ? "s" : "",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else if (minutes > 0)
    {
        snprintf(buffer, 256, "%d minute%s, %d second%s",
            minutes, minutes > 1 ? "s" : "",
            seconds, seconds > 1 ? "s": "");
    }
    else
    {
        snprintf(buffer, 256, "%d second%s",
            seconds, seconds > 1 ? "s": "");
    }
}