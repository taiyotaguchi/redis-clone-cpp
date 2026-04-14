#include "../../../include/resp2.h"
#include <gtest/gtest.h>
#include <memory>

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
  std::string integer_encoded = encode_integer(67);
  EXPECT_EQ(integer_encoded, ":67\r\n");
}
TEST(RESP2Encoding, Array) {
  RespValue elem_1;
  elem_1.type = simple_string;
  elem_1.value = "Hello";
  RespValue elem_2;
  elem_2.type = bulk_string;
  elem_2.value = "Hello, World!";
  RespValue elem_3;
  elem_3.type = integer;
  elem_3.value = 42;
  RespValue elem_4;
  elem_4.type = error;
  elem_4.value = "Error has occured";
  RespValue elem_5;
  elem_5.type = null;
  elem_5.value = "";
  RespValue elem_6;
  elem_6.type = array;
  RespValue elem_6_1;
  elem_6_1.type = simple_string;
  elem_6_1.value = "Yes";
  RespValue elem_6_2;
  elem_6_2.type = integer;
  elem_6_2.value = 12;
  std::vector<std::shared_ptr<RespValue>> elem_6_vec = {
      std::make_shared<RespValue>(elem_6_1),
      std::make_shared<RespValue>(elem_6_2)};
  elem_6.value = elem_6_vec;

  std::vector<std::shared_ptr<RespValue>> input_vec = {
      std::make_shared<RespValue>(elem_1), std::make_shared<RespValue>(elem_2),
      std::make_shared<RespValue>(elem_3), std::make_shared<RespValue>(elem_4),
      std::make_shared<RespValue>(elem_5), std::make_shared<RespValue>(elem_6),
  };

  std::string array_encoded = encode_array(input_vec);
  EXPECT_EQ(array_encoded,
            "*6\r\n+Hello\r\n$13\r\nHello, World!\r\n:42\r\n-Error has "
            "occured\r\n$-1\r\n*2\r\n+Yes\r\n:12\r\n");
}
TEST(RESP2Encoding, Null) {
  std::string null_encoded = encode_null();
  EXPECT_EQ(null_encoded, "$-1\r\n");
}

TEST(RESP2Parsing, BulkString) {
  std::string bulk_string_encoded = "$7\r\nFoo Bar\r\n";
  ParseResult bulk_string_parsed = parse_bulk_string(bulk_string_encoded);
  EXPECT_EQ(std::get<std::string>(bulk_string_parsed.resp_value.value),
            "Foo Bar");
  EXPECT_EQ(bulk_string_parsed.resp_value.type, bulk_string);
}
TEST(RESP2Parsing, SimpleString) {
  std::string simple_string_encoded = "+My ss\r\n";
  ParseResult simple_string_parsed = parse_simple_string(simple_string_encoded);
  EXPECT_EQ(std::get<std::string>(simple_string_parsed.resp_value.value),
            "My ss");
  EXPECT_EQ(simple_string_parsed.resp_value.type, simple_string);
}
TEST(RESP2Parsing, Error) {
  std::string error_encoded = "-My error\r\n";
  ParseResult error_parsed = parse_error(error_encoded);
  EXPECT_EQ(std::get<std::string>(error_parsed.resp_value.value), "My error");
  EXPECT_EQ(error_parsed.resp_value.type, error);
}
TEST(RESP2Parsing, Integer) {
  std::string integer_encoded = ":67\r\n";
  ParseResult integer_parsed = parse_integer(integer_encoded);
  EXPECT_EQ(std::get<int>(integer_parsed.resp_value.value), 67);
  EXPECT_EQ(integer_parsed.resp_value.type, integer);
}
TEST(RESP2Parsing, Array) {
  std::string array_encoded = "*3\r\n$1\r\nI\r\n$4\r\nLike\r\n$4\r\nFood\r\n";
  ParseResult array_parsed = parse_array(array_encoded);
  std::vector<std::shared_ptr<RespValue>> values_vec =
      std::get<std::vector<std::shared_ptr<RespValue>>>(
          array_parsed.resp_value.value);

  EXPECT_EQ(std::get<std::string>(values_vec[0]->value), "I");
  EXPECT_EQ(std::get<std::string>(values_vec[1]->value), "Like");
  EXPECT_EQ(std::get<std::string>(values_vec[2]->value), "Food");

  EXPECT_EQ(values_vec[0]->type, bulk_string);
  EXPECT_EQ(values_vec[1]->type, bulk_string);
  EXPECT_EQ(values_vec[2]->type, bulk_string);

  EXPECT_EQ(array_parsed.resp_value.type, array);
}
TEST(RESP2Parsing, Null) {
  std::string null_encoded = "$-1\r\n";
  ParseResult null_parsed = parse_null(null_encoded);
  EXPECT_EQ(std::get<std::string>(null_parsed.resp_value.value), "");
  EXPECT_EQ(null_parsed.resp_value.type, null);
}
TEST(RESP2Parsing, Failed) {
  ParseResult fail_parsed = parse_failed();
  EXPECT_EQ(std::get<std::string>(fail_parsed.resp_value.value), "");
  EXPECT_EQ(fail_parsed.resp_value.type, failed);
}
TEST(RESP2Parsing, Parse) {
  std::string bulk_string_encoded = "$7\r\nFoo Bar\r\n";
  std::string simple_string_encoded = "+My ss\r\n";
  std::string error_encoded = "-My error\r\n";
  std::string integer_encoded = ":67\r\n";
  std::string array_encoded = "*3\r\n$1\r\nI\r\n$4\r\nLike\r\n$4\r\nFood\r\n";
  std::string null_encoded = "$-1\r\n";
  std::string partial_bulk_string_encoded = "$7\r\nFoo B";

  ParseResult bulk_string_parsed = parse(bulk_string_encoded);
  ParseResult simple_string_parsed = parse(simple_string_encoded);
  ParseResult error_parsed = parse(error_encoded);
  ParseResult integer_parsed = parse(integer_encoded);
  ParseResult array_parsed = parse(array_encoded);
  ParseResult null_parsed = parse(null_encoded);
  ParseResult fail_parsed = parse("wronginput");
  ParseResult partial_bulk_string_parsed = parse(partial_bulk_string_encoded);

  EXPECT_EQ(std::get<std::string>(bulk_string_parsed.resp_value.value),
            "Foo Bar");
  EXPECT_EQ(bulk_string_parsed.resp_value.type, bulk_string);
  EXPECT_EQ(std::get<std::string>(simple_string_parsed.resp_value.value),
            "My ss");
  EXPECT_EQ(simple_string_parsed.resp_value.type, simple_string);
  EXPECT_EQ(std::get<std::string>(error_parsed.resp_value.value), "My error");
  EXPECT_EQ(error_parsed.resp_value.type, error);
  EXPECT_EQ(std::get<int>(integer_parsed.resp_value.value), 67);
  EXPECT_EQ(integer_parsed.resp_value.type, integer);
  EXPECT_EQ(std::get<std::string>(partial_bulk_string_parsed.resp_value.value),
            "");
  EXPECT_EQ(partial_bulk_string_parsed.resp_value.type, incomplete);

  std::vector<std::shared_ptr<RespValue>> values_vec =
      std::get<std::vector<std::shared_ptr<RespValue>>>(
          array_parsed.resp_value.value);
  EXPECT_EQ(std::get<std::string>(values_vec[0]->value), "I");
  EXPECT_EQ(std::get<std::string>(values_vec[1]->value), "Like");
  EXPECT_EQ(std::get<std::string>(values_vec[2]->value), "Food");
  EXPECT_EQ(values_vec[0]->type, bulk_string);
  EXPECT_EQ(values_vec[1]->type, bulk_string);
  EXPECT_EQ(values_vec[2]->type, bulk_string);
  EXPECT_EQ(array_parsed.resp_value.type, array);

  EXPECT_EQ(std::get<std::string>(null_parsed.resp_value.value), "");
  EXPECT_EQ(null_parsed.resp_value.type, null);
  EXPECT_EQ(std::get<std::string>(fail_parsed.resp_value.value), "");
  EXPECT_EQ(fail_parsed.resp_value.type, failed);

  EXPECT_EQ(bulk_string_parsed.consumed, 13);
  EXPECT_EQ(simple_string_parsed.consumed, 8);
  EXPECT_EQ(error_parsed.consumed, 11);
  EXPECT_EQ(integer_parsed.consumed, 5);
  EXPECT_EQ(array_parsed.consumed, 31);
  EXPECT_EQ(null_parsed.consumed, 5);
  EXPECT_EQ(fail_parsed.consumed, -1);
  EXPECT_EQ(partial_bulk_string_parsed.consumed, 0);
}

// Golden vector tests
TEST(RESP2GoldenVector, SimpleString) {
  std::string input = "Foo";
  std::string encoded = encode_simple_string(input);
  ParseResult parsed = parse_simple_string(encoded);
  EXPECT_EQ(std::get<std::string>(parsed.resp_value.value), "Foo");
  EXPECT_EQ(parsed.resp_value.type, simple_string);
  // +Foo\r\n
  EXPECT_EQ(parsed.consumed, 6);
  EXPECT_EQ(
      encode_simple_string(std::get<std::string>(parsed.resp_value.value)),
      "+Foo\r\n");
}
TEST(RESP2GoldenVector, BulkString) {
  std::string input = "Foo Bar";
  std::string encoded = encode_bulk_string(input);
  ParseResult parsed = parse_bulk_string(encoded);
  EXPECT_EQ(std::get<std::string>(parsed.resp_value.value), "Foo Bar");
  EXPECT_EQ(parsed.resp_value.type, bulk_string);
  // $7\r\nFoo Bar\r\n
  EXPECT_EQ(parsed.consumed, 13);
  EXPECT_EQ(encode_bulk_string(std::get<std::string>(parsed.resp_value.value)),
            "$7\r\nFoo Bar\r\n");
}

TEST(RESP2GoldenVector, Integer) {
  int input = 101;
  std::string encoded = encode_integer(input);
  ParseResult parsed = parse_integer(encoded);
  EXPECT_EQ(std::get<int>(parsed.resp_value.value), 101);
  EXPECT_EQ(parsed.resp_value.type, integer);
  // :101\r\n
  EXPECT_EQ(parsed.consumed, 6);
  EXPECT_EQ(encode_integer(std::get<int>(parsed.resp_value.value)), ":101\r\n");
}
TEST(RESP2GoldenVector, Array) {
  RespValue resp_value_1;
  resp_value_1.value = "Foo";
  resp_value_1.type = bulk_string;
  RespValue resp_value_2;
  resp_value_2.value = 100;
  resp_value_2.type = integer;

  std::vector<std::shared_ptr<RespValue>> input = {
      std::make_shared<RespValue>(resp_value_1),
      std::make_shared<RespValue>(resp_value_2)};
  std::string encoded = encode_array(input);
  ParseResult parsed = parse_array(encoded);
  EXPECT_EQ(std::get<int>(std::get<std::vector<std::shared_ptr<RespValue>>>(
                              parsed.resp_value.value)[1]
                              ->value),
            100);
  EXPECT_EQ(
      std::get<std::string>(std::get<std::vector<std::shared_ptr<RespValue>>>(
                                parsed.resp_value.value)[0]
                                ->value),
      "Foo");
  EXPECT_EQ(parsed.resp_value.type, array);
  // *2\r\n$3\r\nFoo\r\n:100\r\n
  EXPECT_EQ(parsed.consumed, 19);
}
TEST(RESP2GoldenVector, Null) {
  std::string encoded = encode_null();
  ParseResult parsed = parse_null(encoded);
  EXPECT_EQ(std::get<std::string>(parsed.resp_value.value), "");
  EXPECT_EQ(parsed.resp_value.type, null);
  // $-1\r\n
  EXPECT_EQ(parsed.consumed, 5);
}
TEST(RESP2GoldenVector, Error) {
  std::string input = "Error has occured";
  std::string encoded = encode_error(input);
  ParseResult parsed = parse_error(encoded);
  EXPECT_EQ(std::get<std::string>(parsed.resp_value.value),
            "Error has occured");
  EXPECT_EQ(parsed.resp_value.type, error);
  // -Error has occured\r\n
  EXPECT_EQ(parsed.consumed, 20);
  EXPECT_EQ(encode_error(std::get<std::string>(parsed.resp_value.value)),
            "-Error has occured\r\n");
}
TEST(RESP2GoldenVector, SerializeParse) {
  RespValue input;
  input.value = "Foo";
  input.type = bulk_string;
  std::string serialized = serialize(input);
  ParseResult parsed = parse(serialized);
  EXPECT_EQ(std::get<std::string>(parsed.resp_value.value), "Foo");
  EXPECT_EQ(parsed.resp_value.type, bulk_string);
  // $3\r\nFoo\r\n
  EXPECT_EQ(parsed.consumed, 9);
  EXPECT_EQ(serialize(parsed.resp_value), "$3\r\nFoo\r\n");
}
