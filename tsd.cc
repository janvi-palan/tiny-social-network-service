#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
//#include <memory>
//#include <thread>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "tsc.grpc.pb.h"
#include <grpc/grpc.h>
// #include <iostream>
#include <fstream>
#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/jsoncpp.cpp"
#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/json.h"



using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using tsc::TscService;
using tsc::User;
using tsc::FollowRequest;
using tsc::UnfollowRequest;
using tsc::FollowReply;
using tsc::ListReply;
using tsc::ConnectRequest;

class TscImpl final : public TscService::Service {
	public:
	// explicit TscImpl() {
	//     // tsc::ParseDb(db, &feature_list_);
	// }
//create user function

	Status AddNewUser(ServerContext* context, const ConnectRequest* cRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Success");
		std::cout<<cRequest->user1().name()<<std::endl;
		Json::Value user; 
		// user["Name"] = 
		user["Followers"] = Json::Value(Json::arrayValue);
		user["Following"] = Json::Value(Json::arrayValue);
		std::cout<<"outside!"<<std::endl;
		std::fstream fs;
		std::string filename = "db.json";
		fs.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
		if(fs){
			std::cout<<"Creating a new json file"<<std::endl;
			fs.open(filename,  std::fstream::in | std::fstream::out | std::fstream::trunc);
			std::cout<<"Creating db as first time used. "<<std::endl;
			Json::Value users;
			users[cRequest->user1().name()] = user;
			Json::StyledWriter styledWriter;
			fs<<styledWriter.write(users);
			fs.close();
			

		}
	return Status::OK;

	}

	Status AddToUsersDB(ServerContext* context, const FollowRequest* fRequest,
	                  FollowReply* fReply) override {
	  fReply->set_message("Success");

	  return Status::OK;		
	}

	Status RemoveFromUsersDB(ServerContext* context, const UnfollowRequest* uRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Success");
		return Status::OK;
	}
	Status GetAllFollowers(ServerContext* context, const User* user,
	                  ListReply* listReply) override {
		// listReply->
		return Status::OK;
	}

};

void RunServer() {
  std::string server_address("0.0.0.0:50051");

  TscImpl service;


  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();

}

int main(int argc, char** argv) {
  std::fstream fs;
  std::string filename = "db.json";

  // fs.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
  // if(!fs){
  // 	std::cout<<"Creating db as first time used. ";
  // 	fs.open(filename,  fstream::in | fstream::out | fstream::trunc);
  // }
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  // std::string db = tsc::GetDbFileContent(argc, argv);
  RunServer();
  return 0;
}
