#include <NetworkLib/Zmq/SubSocket.hpp>
#include <CUtility/ProtoReflection.hpp>

#include <conditional_variable>
#include <iostream>
#include <stdexcept>

namespace AProtobufLibrary = ProtobufLibrary::Examples;

networklib::zmq::CSubSocket gSubscribeSocket(1);

std::mutex gWaitMutex;

std::conditional_variable gWait;


//-----------------------------------------------------------------------------
void DecodePacket(
  const std::shared_ptr<google::protobuf::Message>& pPacket,
  const std::string& Message);
//-----------------------------------------------------------------------------


std::unordered_map<std::string, std::function<void(const std::string&)>> gDecodeMap
{
  {
    "AnimalDetails",
      [](const std::string& Message){
        DecodePacket(
          std::make_shared<AProtobufLibrary::AnimalDetails>(), Message);}
  },
  {
    "CarDetails",
      [](const std::string& Message){
        DecodePacket(
          std::make_shared<AProtobufLibrary::CarDetails>(), Message);}
  },
  {
    "ComputerDetails",
      [](const std::string& Message){
        DecodePacket(
          std::make_shared<AProtobufLibrary::ComputerDetails>(), Message);}
  }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DecodePacket(
  const std::shared_ptr<google::protobuf::message>& pPacket,
  const std::string& Message)
{
  pPacket->ParseFromString(Message);

  Utility::CProtoReflection::Decode(pPacket);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SetupSubscribeSocket(
  const std::vector<std::string>& Args)
{
  gSubscribeSocket.SetReceiveQueueSize(10);

  gSubscribeSocket.SubscribeToEverything();

  auto ArgMapping =
    Utility::CCommandLineArguments::CreateCommandLineArgumentMap(Args);

  std::string SubscribePort("5001");

  auto IsFound = ArgMapping.find("-sp");

  if (IsFound != ArgMapping.end())
  {
    SubscribePort = IsFound->second;
  }

  std::string SubscribeHostname("localhost");

  IsFound = ArgMapping.find("-sh");

  if (IsFound != ArgMapping.end())
  {
    SubscribeHostname = IsFound->second;
  }

  std::string HostnameAndPort = "tcp://" + SubscribeHostname + ":" + SubscribePort;

  IsFound = ArgMapping.find("-s");

  if (IsFound != ArgMapping.end())
  {
    HostnameAndPort = IsFound->second;
  }

  try
  {
    gSubscribeSocket.Connect(HostnameAndPort);

    std::cout << HostnameAndPort  << std::endl;
  }
  catch (const std::exception& Error)
  {
    std::cerr <<
      "Error: Subscribe Socket Connection " <<
      Error.what() <<
      std::endl;

    exit(1);
  }

  gSubscribeSocket.mSignalMessage.connect(
    [&](const std::string& MessageType, const std::string& Message)
    {
      auto IsFound = gDecodeMap.find(MessageType);

      if (IsFound != gDecodeMap.end())
      {
        IsFound->second(MessageType);

        return;
      }
        //need to handle messages that are MessageType less can be embedded in message
    });
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, const char* argv[])
{
  std::vector<std::string> Args(argv + 1, argv + argc);

  if (argc == 2 && std::string(argv[1] == "-h"))
  {
    std::cout << "ProtoReflection Usage:"  << std::endl;
    std::cout << "-s  Full Subscription Path"  << std::endl;
    std::cout << "-sh  Subscribe hostname(tcp://)"  << std::endl;
    std::cout << "-sp  Subscribe port"  << std::endl;
    std::cout << std::endl;
    std::cout << "defaults: tcp://localhost:5001" << std::endl;
    std::cout << std::endl;
  }

  SetupSubscribeSocket(Args);

  std::unique_lock<std::mutex> LoopLock(gWaitMutex);

  {
    //TO-DO: create escape from this loop
    while (true)
    {
      gWait.wait(LoopLock);
    }
  }

  return 0;
}

