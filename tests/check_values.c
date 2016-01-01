/* vi:set ts=4 sw=4 expandtab:
 *
 * Copyright 2016, Chris Leishman (http://github.com/cleishm)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../config.h"
#include "../src/lib/values.h"
#include <check.h>
#include <errno.h>
#include <unistd.h>


START_TEST (null_value)
{
    char buf[256];

    neo4j_value_t value = neo4j_null;
    ck_assert(neo4j_type(value) == NEO4J_NULL);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(buf, "null");

    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 4);
    ck_assert_str_eq(buf, "n");
    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 4);
}
END_TEST


START_TEST (null_eq)
{
    ck_assert(neo4j_eq(neo4j_null, neo4j_null));
    ck_assert(!neo4j_eq(neo4j_null, neo4j_bool(true)));
}
END_TEST


START_TEST (bool_value)
{
    char buf[256];

    neo4j_value_t value = neo4j_bool(true);
    ck_assert(neo4j_type(value) == NEO4J_BOOL);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(buf, "true");

    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 4);
    ck_assert_str_eq(buf, "t");

    value = neo4j_bool(false);
    ck_assert_str_eq(neo4j_tostring(value, buf, sizeof(buf)), "false");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 5);
}
END_TEST


START_TEST (bool_eq)
{
    ck_assert(neo4j_eq(neo4j_bool(true), neo4j_bool(true)));
    ck_assert(neo4j_eq(neo4j_bool(false), neo4j_bool(false)));
    ck_assert(!neo4j_eq(neo4j_bool(true), neo4j_bool(false)));
    ck_assert(!neo4j_eq(neo4j_bool(false), neo4j_bool(true)));
    ck_assert(!neo4j_eq(neo4j_bool(true), neo4j_int(1)));
}
END_TEST


START_TEST (int_value)
{
    char buf[256];

    neo4j_value_t value = neo4j_int(42);
    ck_assert(neo4j_type(value) == NEO4J_INT);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(buf, "42");

    value = neo4j_int(-53);
    ck_assert_str_eq(neo4j_tostring(value, buf, sizeof(buf)), "-53");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 3);
    ck_assert_str_eq(buf, "-");
    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 3);
}
END_TEST


START_TEST (int_eq)
{
    ck_assert(neo4j_eq(neo4j_int(0), neo4j_int(0)));
    ck_assert(neo4j_eq(neo4j_int(42), neo4j_int(42)));
    ck_assert(neo4j_eq(neo4j_int(-127), neo4j_int(-127)));
    ck_assert(!neo4j_eq(neo4j_int(-127), neo4j_int(0)));
    ck_assert(!neo4j_eq(neo4j_int(0), neo4j_int(42)));
    ck_assert(!neo4j_eq(neo4j_int(127), neo4j_int(0)));
    ck_assert(!neo4j_eq(neo4j_int(42), neo4j_int(0)));
    ck_assert(!neo4j_eq(neo4j_int(1), neo4j_float(1.0)));
}
END_TEST


START_TEST (float_value)
{
    char buf[256];

    neo4j_value_t value = neo4j_float(4.2);
    ck_assert(neo4j_type(value) == NEO4J_FLOAT);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "4.200000");

    value = neo4j_float(-89.83423);
    neo4j_tostring(value, buf, sizeof(buf));
    ck_assert_str_eq(buf, "-89.834230");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 4), 10);
    ck_assert_str_eq(buf, "-89");
    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 10);
}
END_TEST


START_TEST (float_eq)
{
    ck_assert(neo4j_eq(neo4j_float(0), neo4j_float(0)));
    ck_assert(neo4j_eq(neo4j_float(42), neo4j_float(42)));
    ck_assert(neo4j_eq(neo4j_float(-1.27), neo4j_float(-1.27)));
    ck_assert(!neo4j_eq(neo4j_float(-127), neo4j_float(0)));
    ck_assert(!neo4j_eq(neo4j_float(0), neo4j_float(42)));
    ck_assert(!neo4j_eq(neo4j_float(127), neo4j_float(0)));
    ck_assert(!neo4j_eq(neo4j_float(42), neo4j_float(0)));
    ck_assert(!neo4j_eq(neo4j_float(1), neo4j_string("bernie")));
}
END_TEST


START_TEST (string_value)
{
    char buf[256];

    neo4j_value_t value = neo4j_string("the \"rum diary\"");
    ck_assert(neo4j_type(value) == NEO4J_STRING);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "\"the \\\"rum diary\\\"\"");

    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 19);
    ck_assert_str_eq(buf, "\"the \\\"rum diary\\\"\"");

    value = neo4j_ustring("the \"rum diary\"", 8);
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 11);
    ck_assert_str_eq(buf, "\"the \\\"rum\"");

    value = neo4j_string("the \"rum\"");
    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 13);
    ck_assert_int_eq(neo4j_ntostring(value, buf, 1), 13);
    ck_assert_str_eq(buf, "");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 13);
    ck_assert_str_eq(buf, "\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 3), 13);
    ck_assert_str_eq(buf, "\"t");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 4), 13);
    ck_assert_str_eq(buf, "\"th");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 5), 13);
    ck_assert_str_eq(buf, "\"the");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 6), 13);
    ck_assert_str_eq(buf, "\"the ");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 7), 13);
    ck_assert_str_eq(buf, "\"the ");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 8), 13);
    ck_assert_str_eq(buf, "\"the \\\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 9), 13);
    ck_assert_str_eq(buf, "\"the \\\"r");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 10), 13);
    ck_assert_str_eq(buf, "\"the \\\"ru");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 11), 13);
    ck_assert_str_eq(buf, "\"the \\\"rum");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 12), 13);
    ck_assert_str_eq(buf, "\"the \\\"rum");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 13), 13);
    ck_assert_str_eq(buf, "\"the \\\"rum\\\"");

    value = neo4j_string("black\\white");
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 14);
    ck_assert_str_eq(buf, "\"black\\\\white\"");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 14);
    ck_assert_int_eq(neo4j_ntostring(value, buf, 7), 14);
    ck_assert_str_eq(buf, "\"black");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 8), 14);
    ck_assert_str_eq(buf, "\"black");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 9), 14);
    ck_assert_str_eq(buf, "\"black\\\\");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 10), 14);
    ck_assert_str_eq(buf, "\"black\\\\w");
}
END_TEST


START_TEST (string_eq)
{
    neo4j_value_t value = neo4j_string("the rum diary");

    ck_assert(neo4j_eq(value, neo4j_string("the rum diary")));
    ck_assert(!neo4j_eq(value, neo4j_string("the rum")));
    ck_assert(!neo4j_eq(value, neo4j_string("the rum journal")));
    ck_assert(!neo4j_eq(value, neo4j_string("the rum diary 2")));
}
END_TEST


START_TEST (list_value)
{
    char buf[256];

    neo4j_value_t list_values[] = { neo4j_int(1), neo4j_string("the \"rum\"") };
    neo4j_value_t value = neo4j_list(list_values, 2);
    ck_assert(neo4j_type(value) == NEO4J_LIST);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "[1,\"the \\\"rum\\\"\"]");

    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 17);
    ck_assert_str_eq(str, "[1,\"the \\\"rum\\\"\"]");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 17);
    ck_assert_int_eq(neo4j_ntostring(value, buf, 1), 17);
    ck_assert_str_eq(buf, "");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 17);
    ck_assert_str_eq(buf, "[");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 3), 17);
    ck_assert_str_eq(buf, "[1");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 4), 17);
    ck_assert_str_eq(buf, "[1,");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 5), 17);
    ck_assert_str_eq(buf, "[1,\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 6), 17);
    ck_assert_str_eq(buf, "[1,\"t");

    ck_assert_int_eq(neo4j_ntostring(value, buf, 9), 17);
    ck_assert_str_eq(buf, "[1,\"the ");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 10), 17);
    ck_assert_str_eq(buf, "[1,\"the ");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 11), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"");

    ck_assert_int_eq(neo4j_ntostring(value, buf, 14), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"rum");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 15), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"rum");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 16), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"rum\\\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 17), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"rum\\\"\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 18), 17);
    ck_assert_str_eq(buf, "[1,\"the \\\"rum\\\"\"]");

    value = neo4j_list(list_values, 0);
    str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert_str_eq(str, "[]");
}
END_TEST


START_TEST (list_eq)
{
    neo4j_value_t list_values1[] = { neo4j_int(1), neo4j_int(2) };
    neo4j_value_t value1 = neo4j_list(list_values1, 2);
    neo4j_value_t list_values2[] = { neo4j_int(1), neo4j_int(2) };
    neo4j_value_t value2 = neo4j_list(list_values2, 2);
    neo4j_value_t list_values3[] = { neo4j_int(1), neo4j_int(3) };
    neo4j_value_t value3 = neo4j_list(list_values3, 2);
    neo4j_value_t list_values4[] = { neo4j_int(1) };
    neo4j_value_t value4 = neo4j_list(list_values4, 1);
    neo4j_value_t list_values5[] = { neo4j_int(1), neo4j_int(2), neo4j_int(3) };
    neo4j_value_t value5 = neo4j_list(list_values5, 3);

    ck_assert(neo4j_eq(value1, value2));
    ck_assert(!neo4j_eq(value1, value3));
    ck_assert(!neo4j_eq(value3, value1));
    ck_assert(!neo4j_eq(value1, value4));
    ck_assert(!neo4j_eq(value4, value1));
    ck_assert(!neo4j_eq(value1, value5));
    ck_assert(!neo4j_eq(value5, value1));
}
END_TEST


START_TEST (map_value)
{
    char buf[256];

    neo4j_map_entry_t map_entries[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_string("sanders") },
          { .key = neo4j_string("b. sanders"), .value = neo4j_int(2) } };
    neo4j_value_t value = neo4j_map(map_entries, 2);
    ck_assert(neo4j_type(value) == NEO4J_MAP);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "{bernie:\"sanders\",`b. sanders`:2}");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 33);
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 33);
    ck_assert_str_eq(buf, "{bernie:\"sanders\",`b. sanders`:2}");

    ck_assert_int_eq(neo4j_ntostring(value, buf, 1), 33);
    ck_assert_str_eq(buf, "");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 2), 33);
    ck_assert_str_eq(buf, "{");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 3), 33);
    ck_assert_str_eq(buf, "{b");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 9), 33);
    ck_assert_str_eq(buf, "{bernie:");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 10), 33);
    ck_assert_str_eq(buf, "{bernie:\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 11), 33);
    ck_assert_str_eq(buf, "{bernie:\"s");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 19), 33);
    ck_assert_str_eq(buf, "{bernie:\"sanders\",");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 20), 33);
    ck_assert_str_eq(buf, "{bernie:\"sanders\",`");

    value = neo4j_map(map_entries, 0);
    str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert_str_eq(buf, "{}");
}
END_TEST


START_TEST (invalid_map_value)
{
    neo4j_map_entry_t map_entries[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_int(1), .value = neo4j_int(2) } };
    neo4j_value_t value = neo4j_map(map_entries, 2);
    ck_assert(neo4j_type(value) == NEO4J_MAP);

    char *str = neo4j_tostring(value, NULL, 0);
    ck_assert_ptr_eq(str, NULL);
    ck_assert_int_eq(errno, NEO4J_INVALID_MAP_KEY_TYPE);
}
END_TEST


START_TEST (map_eq)
{
    neo4j_map_entry_t map_entries1[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(2) } };
    neo4j_value_t value1 = neo4j_map(map_entries1, 2);
    neo4j_map_entry_t map_entries2[] =
        { { .key = neo4j_string("sanders"), .value = neo4j_int(2) },
          { .key = neo4j_string("bernie"), .value = neo4j_int(1) } };
    neo4j_value_t value2 = neo4j_map(map_entries2, 2);
    neo4j_map_entry_t map_entries3[] =
        { { .key = neo4j_string("sanders"), .value = neo4j_int(2) } };
    neo4j_value_t value3 = neo4j_map(map_entries3, 1);
    neo4j_map_entry_t map_entries4[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(2) },
          { .key = neo4j_string("president"), .value = neo4j_int(3) } };
    neo4j_value_t value4 = neo4j_map(map_entries4, 1);
    neo4j_map_entry_t map_entries5[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(3) } };
    neo4j_value_t value5 = neo4j_map(map_entries5, 2);

    ck_assert(neo4j_eq(value1, value2));
    ck_assert(!neo4j_eq(value1, value3));
    ck_assert(!neo4j_eq(value3, value1));
    ck_assert(!neo4j_eq(value1, value4));
    ck_assert(!neo4j_eq(value4, value1));
    ck_assert(!neo4j_eq(value1, value5));
    ck_assert(!neo4j_eq(value5, value1));
}
END_TEST


START_TEST (map_get)
{
    neo4j_map_entry_t map_entries[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(2) } };
    neo4j_value_t value = neo4j_map(map_entries, 2);

    const neo4j_value_t v = neo4j_map_get(value, neo4j_string("bernie"));
    ck_assert(neo4j_type(v) == NEO4J_INT);
    ck_assert(neo4j_eq(v, neo4j_int(1)));
}
END_TEST


START_TEST (node_value)
{
    char buf[256];

    neo4j_value_t labels[] =
        { neo4j_string("Person"), neo4j_string("Human Being") };
    neo4j_map_entry_t props[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(2) } };

    neo4j_value_t field_values[] =
        { neo4j_int(1), neo4j_list(labels, 2), neo4j_map(props, 2) };
    neo4j_value_t value = neo4j_node(field_values);
    ck_assert(neo4j_type(value) == NEO4J_NODE);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "(:Person:`Human Being`{bernie:1,sanders:2})");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 43);
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 43);
    ck_assert_str_eq(buf, "(:Person:`Human Being`{bernie:1,sanders:2})");
}
END_TEST


START_TEST (invalid_node_value)
{
    neo4j_value_t labels[] =
        { neo4j_string("Person"), neo4j_int(1) };
    neo4j_map_entry_t props[] =
        { { .key = neo4j_string("bernie"), .value = neo4j_int(1) },
          { .key = neo4j_string("sanders"), .value = neo4j_int(2) } };

    neo4j_value_t field_values[] =
        { neo4j_int(1), neo4j_list(labels, 2), neo4j_map(props, 2) };
    neo4j_value_t value = neo4j_node(field_values);
    ck_assert(neo4j_type(value) == NEO4J_NODE);

    char *str = neo4j_tostring(value, NULL, 0);
    ck_assert_ptr_eq(str, NULL);
    ck_assert_int_eq(errno, NEO4J_INVALID_LABEL_TYPE);
}
END_TEST


START_TEST (relationship_value)
{
    char buf[256];

    neo4j_value_t type = neo4j_string("Candidate");
    neo4j_map_entry_t props[] =
        { { .key = neo4j_string("year"), .value = neo4j_int(2016) } };

    neo4j_value_t field_values[] =
        { neo4j_int(1), neo4j_int(1), neo4j_int(2), type, neo4j_map(props, 1) };
    neo4j_value_t value = neo4j_relationship(field_values);
    ck_assert(neo4j_type(value) == NEO4J_RELATIONSHIP);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "[:Candidate{year:2016}]");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 23);
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 23);
    ck_assert_str_eq(str, "[:Candidate{year:2016}]");
}
END_TEST


START_TEST (struct_value)
{
    char buf[256];

    neo4j_value_t field_values[] = { neo4j_int(1), neo4j_string("bernie") };
    neo4j_value_t value = neo4j_struct(0x78, field_values, 2);
    ck_assert(neo4j_type(value) == NEO4J_STRUCT);

    char *str = neo4j_tostring(value, buf, sizeof(buf));
    ck_assert(str == buf);
    ck_assert_str_eq(str, "struct<0x78>(1,\"bernie\")");

    ck_assert_int_eq(neo4j_ntostring(value, NULL, 0), 24);
    ck_assert_int_eq(neo4j_ntostring(value, buf, sizeof(buf)), 24);
    ck_assert_str_eq(buf, "struct<0x78>(1,\"bernie\")");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 24), 24);
    ck_assert_str_eq(buf, "struct<0x78>(1,\"bernie\"");
    ck_assert_int_eq(neo4j_ntostring(value, buf, 23), 24);
    ck_assert_str_eq(buf, "struct<0x78>(1,\"bernie");
}
END_TEST


START_TEST (struct_eq)
{
    neo4j_value_t field_values1[] = { neo4j_int(1), neo4j_int(2) };
    neo4j_value_t value1 = neo4j_struct(0x78, field_values1, 2);
    neo4j_value_t field_values2[] = { neo4j_int(1), neo4j_int(2) };
    neo4j_value_t value2 = neo4j_struct(0x78, field_values2, 2);
    neo4j_value_t field_values3[] = { neo4j_int(1), neo4j_int(2) };
    neo4j_value_t value3 = neo4j_struct(0x79, field_values3, 2);
    neo4j_value_t field_values4[] = { neo4j_int(1), neo4j_bool(false) };
    neo4j_value_t value4 = neo4j_struct(0x78, field_values4, 2);
    neo4j_value_t field_values5[] = { neo4j_int(1) };
    neo4j_value_t value5 = neo4j_struct(0x78, field_values5, 1);

    ck_assert(neo4j_eq(value1, value2));
    ck_assert(neo4j_eq(value2, value1));
    ck_assert(!neo4j_eq(value1, value3));
    ck_assert(!neo4j_eq(value3, value1));
    ck_assert(!neo4j_eq(value1, value4));
    ck_assert(!neo4j_eq(value4, value1));
    ck_assert(!neo4j_eq(value1, value5));
    ck_assert(!neo4j_eq(value5, value1));
}
END_TEST


TCase* values_tcase(void)
{
    TCase *tc = tcase_create("values");
    tcase_add_test(tc, null_value);
    tcase_add_test(tc, null_eq);
    tcase_add_test(tc, bool_value);
    tcase_add_test(tc, bool_eq);
    tcase_add_test(tc, int_value);
    tcase_add_test(tc, int_eq);
    tcase_add_test(tc, float_value);
    tcase_add_test(tc, float_eq);
    tcase_add_test(tc, string_value);
    tcase_add_test(tc, string_eq);
    tcase_add_test(tc, list_value);
    tcase_add_test(tc, list_eq);
    tcase_add_test(tc, map_value);
    tcase_add_test(tc, invalid_map_value);
    tcase_add_test(tc, map_eq);
    tcase_add_test(tc, map_get);
    tcase_add_test(tc, node_value);
    tcase_add_test(tc, invalid_node_value);
    tcase_add_test(tc, relationship_value);
    tcase_add_test(tc, struct_value);
    tcase_add_test(tc, struct_eq);
    return tc;
}
