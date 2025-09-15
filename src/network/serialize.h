// shared/protocol/serialize.h (JSON via your favorite header-only lib)
#pragma once
#include "messages.h"
#include <string>

namespace proto {
    std::string to_json(MsgId id, const Join&);
    std::string to_json(MsgId id, const Input&);
    // ...
    bool from_json(const std::string& s, MsgId& idOut, Join& out);
    bool from_json(const std::string& s, MsgId& idOut, Input& out);
    // ...
}
