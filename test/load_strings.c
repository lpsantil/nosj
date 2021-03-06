/***************************************************************************//**

  @file         load_strings.c

  @author       Stephen Brennan

  @date         Created Wednesday,  9 December 2015

  @brief        Tests for comparing strings.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <wchar.h>

#include "libstephen/ut.h"
#include "nosj.h"

static int test_normal_string(void)
{
  wchar_t input[] = L"\"hello\"";
  wchar_t string[] = L"hello";
  wchar_t buffer[6];
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 6);
  TEST_ASSERT(tokens[0].length == 5);
  json_string_load(input, tokens, 0, buffer);
  TEST_ASSERT(0 == wcscmp(buffer, string));
  return 0;
}

static int test_escape_quote(void)
{
  wchar_t input[] = L"\"he\\\"llo\"";
  wchar_t string[] = L"he\"llo";
  wchar_t buffer[7];
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 8);
  TEST_ASSERT(tokens[0].length == 6);
  json_string_load(input, tokens, 0, buffer);
  TEST_ASSERT(0 == wcscmp(buffer, string));
  return 0;
}

static int test_escape_backslash(void)
{
  wchar_t input[] = L"\"he\\\\llo\"";
  wchar_t string[] = L"he\\llo";
  wchar_t buffer[7];
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 8);
  TEST_ASSERT(tokens[0].length == 6);
  json_string_load(input, tokens, 0, buffer);
  TEST_ASSERT(0 == wcscmp(buffer, string));
  return 0;
}

static int test_unicode_escape(void)
{
  wchar_t input[] = L"\"he\\u006Clo\"";
  wchar_t string[] = L"hello";
  wchar_t buffer[6];
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 11);
  TEST_ASSERT(tokens[0].length == 5);
  json_string_load(input, tokens, 0, buffer);
  TEST_ASSERT(0 == wcscmp(buffer, string));
  return 0;
}

static int test_surrogate_pair(void)
{
  wchar_t input[] = L"\"\\uD83D\\uDCA9\"";
  wchar_t string[] = L"💩"; // directly included poop :)
  wchar_t buffer[2];
  struct json_token tokens[1];
  struct json_parser p = json_parse(input, tokens, 1);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 1);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 13);
  TEST_ASSERT(tokens[0].length == 1);
  json_string_load(input, tokens, 0, buffer);
  TEST_ASSERT(0 == wcscmp(buffer, string));
  return 0;
}

void test_load_strings(void)
{
  smb_ut_group *group = su_create_test_group("test/load_strings.c");

  smb_ut_test *normal_string = su_create_test("normal_string", test_normal_string);
  su_add_test(group, normal_string);

  smb_ut_test *escape_quote = su_create_test("escape_quote", test_escape_quote);
  su_add_test(group, escape_quote);

  smb_ut_test *escape_backslash = su_create_test("escape_backslash", test_escape_backslash);
  su_add_test(group, escape_backslash);

  smb_ut_test *unicode_escape = su_create_test("unicode_escape", test_unicode_escape);
  su_add_test(group, unicode_escape);

  smb_ut_test *surrogate_pair = su_create_test("surrogate_pair", test_surrogate_pair);
  su_add_test(group, surrogate_pair);

  su_run_group(group);
  su_delete_group(group);
}
