#include <grpc++/grpc++.h>
#include "jp4cli.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using jp4cli::CmdHandler;
using jp4cli::CmdReply;
using jp4cli::CmdRequest;

class CmdHandlerSvcImpl final : public CmdHandler::Service
{
    Status SendCmd(ServerContext *context, const CmdRequest *req,
                   CmdReply *reply) override;
};

class CLIServer
{
 private:
    const std::string cli_serv_addr;

 public:
    explicit CLIServer(const std::string &serv_addr) : cli_serv_addr(serv_addr)
    {
    }
    void RunServer();
};
