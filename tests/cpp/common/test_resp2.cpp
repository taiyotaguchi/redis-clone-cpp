#include "../../../include/resp2.h"
#include <gtest/gtest.h>

TEST(RESP2Encoding, BulkString) {
  std::string bulk_string_encoded = encode_bulk_string("Foo Bar");
  EXPECT_EQ(bulk_string_encoded, "$7\r\nFoo Bar\r\n");
}
TEST(RESP2Encoding, Error) {
  std::string error_encoded = encode_error("My error");
  EXPECT_EQ(error_encoded, "-My error\r\n");
}
TEST(RESP2Encoding, SimpleString) {
  std::string simple_string_encoded = encode_simple_string("My ss");
  EXPECT_EQ(simple_string_encoded, "+My ss\r\n");
}
TEST(RESP2Encoding, Integer) {
  std::string integer_encoded = encode_integer("67");
  EXPECT_EQ(integer_encoded, ":67\r\n");
}
TEST(RESP2Encoding, Array) {
  std::vector<std::string> v = {"I", "Like", "Food"};
  std::string array_encoded = encode_array(v);
  EXPECT_EQ(array_encoded, "*3\r\n$1\r\nI\r\n$4\r\nLike\r\n$4\r\nFood\r\n");
}
TEST(RESP2Encoding, Null) {
  std::string null_encoded = encode_null();
  EXPECT_EQ(null_encoded, "$-1\r\n");
}

TEST(RESP2Parsing, BulkString) {
  std::string bulk_string_encoded = "$7\r\nFoo Bar\r\n";
  RespValue bulk_string_parsed = parse_bulk_string(bulk_string_encoded);
  EXPECT_EQ(std::get<std::string>(bulk_string_parsed.value), "Foo Bar");
  EXPECT_EQ(bulk_string_parsed.type, bulk_string);
}
TEST(RESP2Parsing, SimpleString) {
  std::string simple_string_encoded = "+My ss\r\n";
  RespValue simple_string_parsed = parse_simple_string(simple_string_encoded);
  EXPECT_EQ(std::get<std::string>(simple_string_parsed.value), "My ss");
  EXPECT_EQ(simple_string_parsed.type, simple_string);
}
TEST(RESP2Parsing, Error) {
  std::string error_encoded = "-My error\r\n";
  RespValue error_parsed = parse_error(error_encoded);
  EXPECT_EQ(std::get<std::string>(error_parsed.value), "My error");
  EXPECT_EQ(error_parsed.type, error);
}
TEST(RESP2Parsing, Integer) {
  std::string integer_encoded = ":67\r\n";
  RespValue integer_parsed = parse_integer(integer_encoded);
  EXPECT_EQ(std::get<std::string>(integer_parsed.value), "67");
  EXPECT_EQ(integer_parsed.type, integer);
}
TEST(RESP2Parsing, Array) {
  std::string array_encoded = "*3\r\n$1\r\nI\r\n$4\r\nLike\r\n$4\r\nFood\r\n";
  RespValue array_parsed = parse_array(array_encoded);
  std::vector<std::shared_ptr<RespValue>> values_vec =
      std::get<std::vector<std::shared_ptr<RespValue>>>(array_parsed.value);

  EXPECT_EQ(std::get<std::string>(values_vec[0]->value), "I");
  EXPECT_EQ(std::get<std::string>(values_vec[1]->value), "Like");
  EXPECT_EQ(std::get<std::string>(values_vec[2]->value), "Food");

  EXPECT_EQ(values_vec[0]->type, bulk_string);
  EXPECT_EQ(values_vec[1]->type, bulk_string);
  EXPECT_EQ(values_vec[2]->type, bulk_string);

  EXPECT_EQ(array_parsed.type, array);
}
TEST(RESP2Parsing, Null) {
  std::string null_encoded = "$-1\r\n";
  RespValue null_parsed = parse_null();
  EXPECT_EQ(std::get<std::string>(null_parsed.value), "");
  EXPECT_EQ(null_parsed.type, null);
}
TEST(RESP2Parsing, Failed) {
  RespValue fail_parsed = parse_failed();
  EXPECT_EQ(std::get<std::string>(fail_parsed.value), "");
  EXPECT_EQ(fail_parsed.type, failed);
}
TEST(RESP2Parsing, Parse) {
  std::string bulk_string_encoded = "$7\r\nFoo Bar\r\n";
  std::string simple_string_encoded = "+My ss\r\n";
  std::string error_encoded = "-My error\r\n";
  std::string integer_encoded = ":67\r\n";
  std::string array_encoded = "*3\r\n$1\r\nI\r\n$4\r\nLike\r\n$4\r\nFood\r\n";
  std::string null_encoded = "$-1\r\n";

  RespValue bulk_string_parsed = parse(bulk_string_encoded);
  RespValue simple_string_parsed = parse(simple_string_encoded);
  RespValue error_parsed = parse(error_encoded);
  RespValue integer_parsed = parse(integer_encoded);
  RespValue array_parsed = parse(array_encoded);
  RespValue null_parsed = parse(null_encoded);
  RespValue fail_parsed = parse("wronginput");

  EXPECT_EQ(std::get<std::string>(bulk_string_parsed.value), "Foo Bar");
  EXPECT_EQ(bulk_string_parsed.type, bulk_string);
  EXPECT_EQ(std::get<std::string>(simple_string_parsed.value), "My ss");
  EXPECT_EQ(simple_string_parsed.type, simple_string);
  EXPECT_EQ(std::get<std::string>(error_parsed.value), "My error");
  EXPECT_EQ(error_parsed.type, error);
  EXPECT_EQ(std::get<std::string>(integer_parsed.value), "67");
  EXPECT_EQ(integer_parsed.type, integer);

  std::vector<std::shared_ptr<RespValue>> values_vec =
      std::get<std::vector<std::shared_ptr<RespValue>>>(array_parsed.value);
  EXPECT_EQ(std::get<std::string>(values_vec[0]->value), "I");
  EXPECT_EQ(std::get<std::string>(values_vec[1]->value), "Like");
  EXPECT_EQ(std::get<std::string>(values_vec[2]->value), "Food");
  EXPECT_EQ(values_vec[0]->type, bulk_string);
  EXPECT_EQ(values_vec[1]->type, bulk_string);
  EXPECT_EQ(values_vec[2]->type, bulk_string);
  EXPECT_EQ(array_parsed.type, array);

  EXPECT_EQ(std::get<std::string>(null_parsed.value), "");
  EXPECT_EQ(null_parsed.type, null);
  EXPECT_EQ(std::get<std::string>(fail_parsed.value), "");
  EXPECT_EQ(fail_parsed.type, failed);
}
