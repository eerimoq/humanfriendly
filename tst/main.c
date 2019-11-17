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
