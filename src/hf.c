/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the humanfriendly project.
 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <pwd.h>
#include "hf.h"

#define TIME_UNITS_MAX 6

char *hf_get_username(char *buf_p, size_t size, const char *default_p)
{
    char *res_p;
    struct passwd *passwd_p;

    res_p = buf_p;
    passwd_p = getpwuid(geteuid());

    if (passwd_p == NULL) {
        if (default_p == NULL) {
            res_p = NULL;
        } else {
            strncpy(buf_p, default_p, size);
        }
    } else {
        strncpy(buf_p, passwd_p->pw_name, size);

        if (size > 0) {
            buf_p[size - 1] = '\0';
        }
    }

    buf_p[size - 1] = '\0';

    return (res_p);
}

char *hf_get_hostname(char *buf_p, size_t size, const char *default_p)
{
    int res;
    char *res_p;

    res_p = buf_p;
    res = gethostname(buf_p, size);

    if (res != 0) {
        if (default_p == NULL) {
            res_p = NULL;
        } else {
            strncpy(buf_p, default_p, size);
        }
    }

    buf_p[size - 1] = '\0';

    return (res_p);
}

/* Common time units, used for formatting of time spans. */
struct time_unit_t {
    unsigned long divider;
    const char *singular_p;
    const char *plural_p;
};

static struct time_unit_t time_units[TIME_UNITS_MAX] = {
    {
        .divider = 60 * 60 * 24 * 7 * 52 * 1000ul,
        .singular_p = "year",
        .plural_p = "years"
    },
    {
        .divider = 60 * 60 * 24 * 7 * 1000ul,
        .singular_p = "week",
        .plural_p = "weeks"
    },
    {
        .divider = 60 * 60 * 24 * 1000ul,
        .singular_p = "day",
        .plural_p = "days"
    },
    {
        .divider = 60 * 60 * 1000ul,
        .singular_p = "hour",
        .plural_p = "hours"
    },
    {
        .divider = 60 * 1000ul,
        .singular_p = "min",
        .plural_p = "mins"
    },
    {
        .divider = 1000ul,
        .singular_p = "sec",
        .plural_p = "secs"
    }
};

static const char *get_time_unit(struct time_unit_t *self_p,
                                 unsigned long value)
{
    if (value == 1) {
        return (self_p->singular_p);
    } else {
        return (self_p->plural_p);
    }
}

const char *get_delimiter(bool is_first, bool is_last)
{
    if (is_first) {
        return ("");
    } else if (is_last) {
        return (" and ");
    } else {
        return (", ");
    }
}

int get_width(unsigned long long value)
{
    if (value < 10) {
        return (3);
    } else if (value < 100) {
        return (2);
    } else {
        return (1);
    }
}

char *hf_format_timespan(char *buf_p,
                         size_t size,
                         unsigned long long timespan_ms)
{
    int i;
    unsigned long long count;
    char buf[64];
    char fraction[32];

    strncpy(buf_p, "", size);

    for (i = 0; i < (TIME_UNITS_MAX - 1); i++) {
        count = (timespan_ms / time_units[i].divider);
        timespan_ms -= (count * time_units[i].divider);

        if (count == 0) {
            continue;
        }

        snprintf(&buf[0],
                 sizeof(buf),
                 "%s%llu %s",
                 get_delimiter(strlen(buf_p) == 0, timespan_ms == 0),
                 count,
                 get_time_unit(&time_units[i], count));
        strncat(buf_p, &buf[0], size);
    }

    if (timespan_ms > 0) {
        count = (timespan_ms / time_units[i].divider);
        timespan_ms -= (count * time_units[i].divider);

        if (timespan_ms == 0) {
            strcpy(&fraction[0], "");
        } else if (timespan_ms < 10) {
            sprintf(&fraction[0], ".%03llu", timespan_ms);
        } else if (timespan_ms < 100) {
            sprintf(&fraction[0], ".%02llu", timespan_ms / 10);
        } else {
            sprintf(&fraction[0], ".%01llu", timespan_ms / 100);
        }

        snprintf(&buf[0],
                 sizeof(buf),
                 "%s%llu%s %s",
                 get_delimiter(strlen(buf_p) == 0, true),
                 count,
                 &fraction[0],
                 get_time_unit(&time_units[i], count + 2 * timespan_ms));
        strncat(buf_p, &buf[0], size);
    }

    if (strlen(buf_p) == 0) {
        strncpy(buf_p, "0 secs", size);
    }

    buf_p[size - 1] = '\0';

    return (buf_p);
}
