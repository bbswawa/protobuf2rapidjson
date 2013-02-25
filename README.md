Protobuf 2 Rapidjson - Conversion of protobuf and rapidjson
===========================================================

This is a very simple library for converting protobuf messages
to rapidjson Value objects, and back.

No compilation is required, as the library is formed by a single
header `include/protobuf2rapidjson.h`.

### Usage

Usage is simple, via the two functions `decode_from_json_value` and
`encode_to_json_value`:

```c++
#include <protobuf2rapidjson.h>
using namespace Protobuf2Rapidjson;

/* ... some code ... */

// encode the message proto_buf_message into a json document
rapidjson::Document json_doc;
encode_to_json_value(proto_buf_message, &json_doc, &json_doc.GetAllocator());

/* ... more code ... */

// decode the message proto_buf_message from a json value json_value
if(!decode_from_json_value(json_value, &proto_buf_message)) {
		cout << "Decoding error!" << endl;
		return -1;
}
```

### Example and test

A very simple example can be found in the `test/` directory, and
can be compiled with

    $ make PBDIR=<PRotoBuf install prefix> RJDIR=<RapidJson install prefix>
    $ ./test