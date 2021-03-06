/***************************************************************************//**

  @file         parse_arrays.c

  @author       Stephen Brennan

  @date         Created Tuesday, 24 November 2015

  @brief        Test array parsing.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"

#include "nosj.h"

static int test_empty_array(void)
{
  wchar_t input[] = L"[]";
  size_t ntok = 1;
  struct json_token tokens[ntok];
  struct json_parser p = json_parse(input, tokens, ntok);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == ntok);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t)-1);
  TEST_ASSERT(tokens[0].type == JSON_ARRAY);
  TEST_ASSERT(tokens[0].start == 0);
  TEST_ASSERT(tokens[0].end == 1);
  TEST_ASSERT(tokens[0].length == 0);
  TEST_ASSERT(tokens[0].child == 0);
  TEST_ASSERT(tokens[0].next == 0);
  return 0;
}

static int test_single_element(void)
{
  wchar_t input[] = L"[1]";
  size_t ntok = 2, i;
  struct json_token tokens[ntok];
  struct json_token expected[] = {
    {.type = JSON_ARRAY, .start = 0, .end = 2, .length=1, .child = 1, .next = 0},
    {.type = JSON_NUMBER, .start = 1, .end = 1, .length=0, .child = 0, .next = 0},
  };
  struct json_parser p = json_parse(input, tokens, ntok);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == ntok);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  for (i = 0; i < ntok; i++) {
    TEST_ASSERT(tokens[i].type == expected[i].type);
    TEST_ASSERT(tokens[i].start == expected[i].start);
    TEST_ASSERT(tokens[i].end == expected[i].end);
    TEST_ASSERT(tokens[i].child == expected[i].child);
    TEST_ASSERT(tokens[i].next == expected[i].next);
  }
  return 0;
}

static int test_multiple_elements(void)
{
  size_t ntok = 3, i;
  wchar_t input[] = L"[1, 2]";
  struct json_token tokens[ntok];
  struct json_token expected[] = {
    {.type = JSON_ARRAY, .start = 0, .end = 5, .length=2, .child = 1, .next = 0},
    {.type = JSON_NUMBER, .start = 1, .end = 1, .length=0, .child = 0, .next = 2},
    {.type = JSON_NUMBER, .start = 4, .end = 4, .length=0, .child = 0, .next = 0},
  };
  struct json_parser p = json_parse(input, tokens, ntok);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == ntok);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  for (i = 0; i < ntok; i++) {
    TEST_ASSERT(tokens[i].type == expected[i].type);
    TEST_ASSERT(tokens[i].start == expected[i].start);
    TEST_ASSERT(tokens[i].end == expected[i].end);
    TEST_ASSERT(tokens[i].child == expected[i].child);
    TEST_ASSERT(tokens[i].next == expected[i].next);
  }
  return 0;
}

static int test_extra_comma(void)
{
  wchar_t input[] = L"[1,]";
  size_t ntok = 2, i;
  struct json_token tokens[ntok];
  struct json_token expected[] = {
    {.type = JSON_ARRAY, .start = 0, .end = 3, .length=1, .child = 1, .next = 0},
    {.type = JSON_NUMBER, .start = 1, .end = 1, .length=0, .child = 0, .next = 0},
  };
  struct json_parser p = json_parse(input, tokens, ntok);
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == ntok);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);
  for (i = 0; i < ntok; i++) {
    TEST_ASSERT(tokens[i].type == expected[i].type);
    TEST_ASSERT(tokens[i].start == expected[i].start);
    TEST_ASSERT(tokens[i].end == expected[i].end);
    TEST_ASSERT(tokens[i].child == expected[i].child);
    TEST_ASSERT(tokens[i].next == expected[i].next);
  }
  return 0;
}

static int test_no_end(void)
{
  wchar_t input[] = L"[1,";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_PREMATURE_EOF);
  return 0;
}

static int test_error_within_list(void)
{
  wchar_t input[] = L"[1e,";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_INVALID_NUMBER);
  return 0;
}

static int test_no_comma(void)
{
  wchar_t input[] = L"[1 2 3]";
  struct json_parser p = json_parse(input, NULL, 0);
  TEST_ASSERT(p.error == JSONERR_EXPECTED_TOKEN);
  TEST_ASSERT(p.errorarg = L',');
  return 0;
}

static int test_get(void)
{
  wchar_t input[] = L"[1, null, true, \"hi\", {}]";
  struct json_token tokens[7];
  struct json_parser p = json_parse(input, tokens, 7);
  size_t res;

  // assertions for parsing correctly
  TEST_ASSERT(p.error == JSONERR_NO_ERROR);
  TEST_ASSERT(p.tokenidx == 6);
  TEST_ASSERT(p.textidx == sizeof(input)/sizeof(wchar_t) - 1);

  // assertions for the results of json_array_get()
  res = json_array_get(input, tokens, 0, 0);
  TEST_ASSERT(res == 1);
  TEST_ASSERT(tokens[res].type == JSON_NUMBER);

  res = json_array_get(input, tokens, 0, 1);
  TEST_ASSERT(res == 2);
  TEST_ASSERT(tokens[res].type == JSON_NULL);

  res = json_array_get(input, tokens, 0, 2);
  TEST_ASSERT(res == 3);
  TEST_ASSERT(tokens[res].type == JSON_TRUE);

  res = json_array_get(input, tokens, 0, 3);
  TEST_ASSERT(res == 4);
  TEST_ASSERT(tokens[res].type == JSON_STRING);

  res = json_array_get(input, tokens, 0, 4);
  TEST_ASSERT(res == 5);
  TEST_ASSERT(tokens[res].type == JSON_OBJECT);

  res = json_array_get(input, tokens, 0, 5);
  TEST_ASSERT(res == 0);

  return 0;
}

static int test_get_empty(void)
{
  wchar_t input[] = L"[]";
  struct json_token tokens[1];
  json_parse(input, tokens, 1);

  TEST_ASSERT(0 == json_array_get(input, tokens, 0, 0));
  return 0;
}

void test_parse_arrays(void)
{
  smb_ut_group *group = su_create_test_group("test/parse_arrays.c");

  smb_ut_test *empty_array = su_create_test("empty_array", test_empty_array);
  su_add_test(group, empty_array);

  smb_ut_test *single_element = su_create_test("single_element", test_single_element);
  su_add_test(group, single_element);

  smb_ut_test *multiple_elements = su_create_test("multiple_elements", test_multiple_elements);
  su_add_test(group, multiple_elements);

  smb_ut_test *extra_comma = su_create_test("extra_comma", test_extra_comma);
  su_add_test(group, extra_comma);

  smb_ut_test *no_end = su_create_test("no_end", test_no_end);
  su_add_test(group, no_end);

  smb_ut_test *error_within_list = su_create_test("error_within_list", test_error_within_list);
  su_add_test(group, error_within_list);

  smb_ut_test *no_comma = su_create_test("no_comma", test_no_comma);
  su_add_test(group, no_comma);

  smb_ut_test *get = su_create_test("get", test_get);
  su_add_test(group, get);

  smb_ut_test *get_empty = su_create_test("get_empty", test_get_empty);
  su_add_test(group, get_empty);

  su_run_group(group);
  su_delete_group(group);
}
