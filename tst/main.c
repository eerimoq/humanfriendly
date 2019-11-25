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

#include <pwd.h>
#include "hf.h"
#include "nala.h"
#include "nala_mocks.h"

#define membersof(a) (sizeof(a) / sizeof((a)[0]))

#define SECOND (1000ul)
#define MINUTE (60ul * SECOND)
#define HOUR   (60ul * MINUTE)
#define DAY    (24ul * HOUR)
#define WEEK   (7ul * DAY)
#define YEAR   (52ul * WEEK)

TEST(test_get_username_ok)
{
    char buf[32];
    struct passwd passwd;

    geteuid_mock_once(5);
    passwd.pw_name = "foobar";
    getpwuid_mock_once(5, &passwd);

    ASSERT_EQ(hf_get_username(&buf[0], sizeof(buf), ""), &buf[0]);
    ASSERT_EQ(&buf[0], "foobar");
}

TEST(test_get_username_default_null)
{
    char buf[32];

    geteuid_mock_once(4);
    getpwuid_mock_once(4, NULL);

    ASSERT_EQ((void *)hf_get_username(&buf[0], sizeof(buf), NULL), NULL);
}

TEST(test_get_username_default)
{
    char buf[32];

    geteuid_mock_once(4);
    getpwuid_mock_once(4, NULL);

    ASSERT_EQ(hf_get_username(&buf[0], sizeof(buf), "unknown"), &buf[0]);
    ASSERT_EQ(&buf[0], "unknown");
}

TEST(test_get_username_default_longer_than_buffer)
{
    char buf[32];

    geteuid_mock_once(4);
    getpwuid_mock_once(4, NULL);

    ASSERT_EQ(hf_get_username(&buf[0],
                              sizeof(buf),
                              "12345678901234567890123456789012"),
              &buf[0]);
    ASSERT_EQ(&buf[0], "1234567890123456789012345678901");
}

TEST(test_get_hostname_ok)
{
    char buf[32];

    gethostname_mock_once(NULL, 32, 0);
    gethostname_mock_ignore___name_in();
    gethostname_mock_set___name_out("foobar", 7);

    ASSERT_EQ(hf_get_hostname(&buf[0], sizeof(buf), ""), &buf[0]);
    ASSERT_EQ(&buf[0], "foobar");
}

TEST(test_get_hostname_default_null)
{
    char buf[32];

    gethostname_mock_once(NULL, 32, -1);
    gethostname_mock_ignore___name_in();

    ASSERT_EQ((void *)hf_get_hostname(&buf[0], sizeof(buf), NULL), NULL);
}

TEST(test_get_hostname_default)
{
    char buf[32];

    gethostname_mock_once(NULL, 32, -1);
    gethostname_mock_ignore___name_in();

    ASSERT_EQ(hf_get_hostname(&buf[0], sizeof(buf), "unknown"), &buf[0]);
    ASSERT_EQ(&buf[0], "unknown");
}

TEST(test_get_hostname_default_longer_than_buffer)
{
    char buf[32];

    gethostname_mock_once(NULL, 32, -1);
    gethostname_mock_ignore___name_in();

    ASSERT_EQ(hf_get_hostname(&buf[0],
                              sizeof(buf),
                              "12345678901234567890123456789012"),
              &buf[0]);
    ASSERT_EQ(&buf[0], "1234567890123456789012345678901");
}

TEST(test_format_timespan)
{
    char buf[128];
    int i;
    struct data_t {
        unsigned long long timespan_ms;
        const char *expected_p;
    } datas[] = {
         {
             .timespan_ms = 0,
             .expected_p = "0s"
         },
         {
             .timespan_ms = 1,
             .expected_p = "1ms"
         },
         {
             .timespan_ms = 10,
             .expected_p = "10ms"
         },
         {
             .timespan_ms = 100,
             .expected_p = "100ms"
         },
         {
             .timespan_ms = 1000,
             .expected_p = "1s"
         },
         {
             .timespan_ms = 1001,
             .expected_p = "1s and 1ms"
         },
         {
             .timespan_ms = 2 * SECOND,
             .expected_p = "2s"
         },
         {
             .timespan_ms = 2 * MINUTE,
             .expected_p = "2m"
         },
         {
             .timespan_ms = WEEK + 1,
             .expected_p = "1w and 1ms"
         },
         {
             .timespan_ms = (1 * YEAR + 2 * DAY + 3 * HOUR),
             .expected_p = "1y, 2d and 3h"
         }
    };

    for (i = 0; i < membersof(datas); i++) {
        ASSERT_EQ(hf_format_timespan(&buf[0], sizeof(buf), datas[i].timespan_ms),
                  &buf[0]);
        ASSERT_EQ(&buf[0], datas[i].expected_p);
    }
}

TEST(test_format_timespan_short_buffer)
{
    char buf[2];

    ASSERT_EQ(hf_format_timespan(&buf[0], sizeof(buf), 0), &buf[0]);
    ASSERT_EQ(&buf[0], "0");
}

TEST(test_format_timespan_very_short_buffer)
{
    char buf[1];

    ASSERT_EQ(hf_format_timespan(&buf[0], sizeof(buf), 0), &buf[0]);
    ASSERT_EQ(&buf[0], "");
}

TEST(test_string_to_long)
{
    ASSERT_EQ(hf_string_to_long("", -10, 10, -1, 0), -1);
    ASSERT_EQ(hf_string_to_long("0", -10, 10, -1, 0), 0);
    ASSERT_EQ(hf_string_to_long("1", -10, 10, -1, 0), 1);
    ASSERT_EQ(hf_string_to_long("-11", -10, 10, -1, 0), -10);
    ASSERT_EQ(hf_string_to_long("11", -10, 10, -1, 0), 10);
    ASSERT_EQ(hf_string_to_long("1a", -10, 10, -1, 10), -1);
    ASSERT_EQ(hf_string_to_long("11111111111111111111111111111111111111",
                                -10,
                                10,
                                -1,
                                10),
              10);
    ASSERT_EQ(hf_string_to_long("-11111111111111111111111111111111111111",
                                -10,
                                10,
                                -1,
                                10),
              -10);
}

TEST(test_buffer_to_string)
{
    char buf[8];

    ASSERT_EQ(hf_buffer_to_string(&buf[0], sizeof(buf), "", 0), &buf[0]);
    ASSERT_MEMORY(&buf[0], "", 1);

    ASSERT_EQ(hf_buffer_to_string(&buf[0], sizeof(buf), "1", 1), &buf[0]);
    ASSERT_MEMORY(&buf[0], "1", 2);

    ASSERT_EQ(hf_buffer_to_string(&buf[0], sizeof(buf), "12345678", 8), &buf[0]);
    ASSERT_MEMORY(&buf[0], "1234567", 8);
}
