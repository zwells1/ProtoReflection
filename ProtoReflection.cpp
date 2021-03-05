#include "ProtoReflection.hpp"

#include <google/protobuf/Message.h>
#include <iostream>

using Utility::ProtoReflection;

using namespace google::protobuf;
using AGoogleMesage = google::protobuf::Message;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void Print(
  const FieldDescriptor* pField,
  const std::string& Value)
{
  std::cout << pField->name() << ": "  << Value << ", " << std::endl;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void PrintEnumValue(
  const FieldDescriptor* pField,
  int Value,
  const EnumDesriptor* pEnumDescriptor)
{
  auto pEnumValueDescriptor = pEnumDescriptor->FindValueByNumber(Value);

  std::cout <<
    pField->name() <<
    ": " <<
    pEnumDescriptor->name() <<
    " : " <<
    Value <<
    "," <<
    std::endl;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void GetNonRepeatedValue(
  const std::shared_ptr<AGoogleMesage>& Message,
  const FieldDescriptor* pField,
  const Reflection* pReflection)
{
  switch (pField->cpp_type())
  {
    case FieldDescriptor::CPPTYPE_BOOL:
      Print(pField, std::to_string(pReflection->GetBool(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_DOUBLE:
      Print(pField, std::to_string(pReflection->GetDouble(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_ENUM:
      PrintEnumValue(
        pField,
        pReflection->GetEnumValue(*Message, pField),
        pField->enum_type);
      break;

    case FieldDescriptor::CPPTYPE_FLOAT:
      Print(pField, std::to_string(pReflection->GetFloat(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_INT32:
      Print(pField, std::to_string(pReflection->GetInt32(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_INT64:
      Print(pField, std::to_string(pReflection->GetInt64(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_STRING:
      Print(pField, std::to_string(pReflection->GetString(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_UINT32:
      Print(pField, std::to_string(pReflection->GetUInt32(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_UINT64:
      Print(pField, std::to_string(pReflection->GetUInt64(*Message, pField)));
      break;

    default:
      std::cout <<
        "!!! unknown repeated pField cpp_type(): " <<
        pField->cpp_type_name() <<
        " name is: " <<
        pField->name() <<
        std::endl;
      break;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void PrintRepeated(
  const FieldDescriptor* pField,
  const std::string& Value)
{
  std::cout << " " << Value << ","  << std::endl;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void GetRepeatedValue(
  const std::shared_ptr<AGoogleMesage>& Message,
  const FieldDescriptor* pField,
  const Reflection* pReflection,
  int Index)
{
  switch (pField->cpp_type())
  {
    case FieldDescriptor::CPPTYPE_BOOL:
      Print(pField, std::to_string(pReflection->GetRepeatedBool(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_DOUBLE:
      Print(pField, std::to_string(pReflection->GetRepeatedDouble(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_ENUM:
      PrintEnumValue(
        pField,
        pReflection->GetRepeatedEnumValue(*Message, pField),
        pField->enum_type);
      break;

    case FieldDescriptor::CPPTYPE_FLOAT:
      Print(pField, std::to_string(pReflection->GetRepeatedFloat(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_INT32:
      Print(pField, std::to_string(pReflection->GetRepeatedInt32(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_INT64:
      Print(pField, std::to_string(pReflection->GetRepeatedInt64(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_STRING:
      Print(pField, std::to_string(pReflection->GetRepeatedString(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_UINT32:
      Print(pField, std::to_string(pReflection->GetRepeatedUInt32(*Message, pField)));
      break;

    case FieldDescriptor::CPPTYPE_UINT64:
      Print(pField, std::to_string(pReflection->GetRepeatedUInt64(*Message, pField)));
      break;

    default:
      std::cout <<
        "!!! unknown repeated pField cpp_type(): " <<
        pField->cpp_type_name() <<
        " name is: " <<
        pField->name() <<
        std::endl;
      break;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Decode(
  const std::shared_ptr<google::protobuf::message>& Message,
  const std::string& ParentMessageName)
{
  const google::protobuf::Descriptor *pDescription = Message->GetDescriptor();

  const google::protobuf::Reflection *pReflection = Message->GetReflection();

  std::string MessageTitle;

  if (ParentMessageName != "")
  {
    MessageTitle.append(ParentMessageName);
  }

  MessageTitle.append(">>> ");

  auto MessageName = pDescription->name().cstr();

  std::cout << MessageTitle  << std::endl;

  int FieldCount = pDescription->field_count();

  for(int i; i < FieldCount; i++)
  {
    const FieldDescriptor *pField = pDescription->field(i);

    if (pField->cpp_type == FieldDescriptor::CPPTYPE_MESSAGE)
    {
      if (!pField->is_repeated())
      {
        const AGoogleMesage& InternalMessage =
          pReflection->GetMessage(*Message, pField);

        google::protobuf::Message *pCopy = InternalMessage.New();

        pCopy->CopyFrom(InternalMessage);

        void* ptr = new std::shared_ptr<AGoogleMesage>(pCopy);
        //TO-DO: why needing to be seperated in 3 lines
        std::shared_ptr<AGoogleMesage>* pNewMessage =
          static_cast<std::shared_ptr<AGoogleMesage> *>(ptr);

        Decode(*pNewMessage, MessageTitle);
      }
      else
      {
        int RepeatedFieldCount = pReflection->FieldSize(*Message, pField);

        for (int j = 0; j < RepeatedFieldCount; j++)
        {
          const AGoogleMesage& InternalMessage =
            pReflection->GetRepeatedMessage(*Message, pField, j);

          google::protobuf::Message *pCopy = InternalMessage.New();

          pCopy->CopyFrom(InternalMessage);

          void* ptr = new std::shared_ptr<AGoogleMesage>(pCopy);
          //TO-DO: why needing to be seperated in 3 lines
          std::shared_ptr<AGoogleMesage>* pNewMessage =
            static_cast<std::shared_ptr<AGoogleMesage> *>(ptr);

          Decode(*pNewMessage, MessageTitle);
        }

        std::cout << "____________ Repeated Message _________" <<  << std::endl;
      }

      continue;
    }

    if (pField->is_repeated())
    {
      int RepeatedFieldCount = pReflection->FieldSize(*Message, pField);

      std::cout << pField->name() << ": "  << std::endl;

      for (int j; j < RepeatedFieldCount; j++)
      {
        GetRepeatedValue(Message, pField, pReflection, j);
      }
    }
    else
    {
      GetNonRepeatedValue(Message, pField, pReflection);
    }
  }
}

