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

// bool isUserExists(std::string curr_user, Json::Value users){
// 	if(users::isMember(curr_user)){
// 		std::cout<<"This user already exists. Connection done!"<<std::endl;
// 		return true;
// 	} 
// 	return false;

// }
class TscImpl final : public TscService::Service {
	public:
	// explicit TscImpl() {
	//     // tsc::ParseDb(db, &feature_list_);
	// }
//create user function

	Status AddNewUser(ServerContext* context, const ConnectRequest* cRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Connection to server is successful.");
		std::string filename = "db.json";
		std::cout<<cRequest->user1().name()<<std::endl;
		std::string curr_user = cRequest->user1().name();
		//read from users db and check if present
		Json::Value users;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		ip_users >> users;

		if(!reader.parse(ip_users, users, true)){
		        //for some reason it always fails to parse
			std::cout  << "Failed to parse configuration\n"
		               << reader.getFormattedErrorMessages();
		}
		// user["Name"] = 
		if(users.isMember(curr_user)){
			std::cout<<"This user already exists. Connection done!"<<std::endl;
			return Status::OK;		
		} 
		//finished checking for user
		Json::Value user; 
		user["Followers"] = Json::Value(Json::arrayValue);
		user["Following"] = Json::Value(Json::arrayValue);

		std::cout<<"Adding new user to the database."<<std::endl;
		
		users[curr_user] = user;
		std::ofstream of_obj(filename);
		of_obj<<std::setw(4)<<users<<std::endl;
		return Status::OK;
	}

	Status AddToUsersDB(ServerContext* context, const FollowRequest* fRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Success");
		std::string filename = "db.json";
		std::string user1 = fRequest->user1().name();
		std::string user2 = fRequest->user2().name();
		std::cout<<user1<<" Following "<<user2<<std::endl;

		Json::Value users;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		ip_users >> users;

		if(!reader.parse(ip_users, users, true)){
		        //for some reason it always fails to parse
			std::cout  << "Failed to parse configuration\n"
		               << reader.getFormattedErrorMessages();
		}
		// user["Name"] = 

		if(users.isMember(user1) && users.isMember(user2)){
			users[user1]["Following"].append(user2);
			users[user2]["Followers"].append(user1);
			std::cout<<"Finished appending to users db.";
			return Status::OK;		
		} else{
			std::cout<<"Both the users don't exist."<<std::endl;
		}

		

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
