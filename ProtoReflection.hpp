#pragma once

#include <memory>
#include <string>

namespace google
{
  namespace protobuf
  {
    class message;
  }
}

namespace Utility
{
  class CProtoReflection
  {
    public:

      static void Decode(
        const std::shared_ptr<google::protobuf::message>& Message,
        const std::string& ParentMessageName = "");
  }
}

