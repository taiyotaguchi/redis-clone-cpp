def encode_simple_string(input):
    return f"+{input}\r\n"


def encode_bulk_string(input):
    return f"${len(input)}\r\n{input}\r\n"


def encode_integer(input):
    return f":{input}\r\n"


def encode_null():
    return "$-1\r\n"


def encode_error(input):
    return f"-{input}\r\n"


def encode_array(input):
    output = f"*{len(input)}\r\n"
    for elem in input:
        bulk_string_encoded = encode_bulk_string(elem)
        output += bulk_string_encoded
    return output


def find_crlf_index(input):
    return input.find("\r\n")


def parse_failed():
    return (None, -1)


def parse(input):
    if len(input) < 3:
        return parse_failed()

    if input[0] == "+":
        return parse_simple_string(input)
    elif input[0] == "$":
        if input[1] == "-":
            return parse_null(input)
        else:
            return parse_bulk_string(input)
    elif input[0] == ":":
        return parse_integer(input)
    elif input[0] == "-":
        return parse_error(input)
    elif input[0] == "*":
        return parse_array(input)
    else:
        return parse_failed()


def parse_simple_string(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    return (input[1:crlf_index], crlf_index + 2)


def parse_bulk_string(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    crlf_index_2 = find_crlf_index(input[crlf_index + 2 :]) + crlf_index + 2
    if crlf_index_2 == -1:
        print("Parse incomplete")
        return parse_failed()

    return (input[crlf_index + 2 : crlf_index_2], crlf_index_2 + 2)


def parse_error(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    return (input[1:crlf_index], crlf_index + 2)


def parse_integer(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    try:
        return (int(input[1:crlf_index]), crlf_index + 2)
    except:
        return parse_failed()


def parse_null(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    if input[0:5] == "$-1\r\n":
        return (None, 5)

    return parse_failed()


def parse_array(input):
    crlf_index = find_crlf_index(input)
    if crlf_index == -1:
        print("Parse incomplete")
        return parse_failed()

    total_consumed = crlf_index + 2

    array_len = 0

    try:
        array_len = int(input[1:crlf_index])
    except:
        print("Parsing failed")
        return parse_failed()

    output = []

    input = input[crlf_index + 2 :]

    for i in range(array_len):
        parsed = parse(input)
        if parsed[1] == -1:
            return parse_failed()
        output.append(parsed[0])
        total_consumed += parsed[1]
        input = input[parsed[1] :]

    return (output, total_consumed)
