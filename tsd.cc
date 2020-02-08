#include <iostream>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
//#include <memory>
#include <thread>
#include <unordered_map>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "tsc.grpc.pb.h"
#include <grpc/grpc.h>
// #include <iostream>
#include <fstream>
#include <grpc++/support/string_ref.h>

#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/jsoncpp.cpp"
#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/json.h"



using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using grpc::StatusCode;
using tsc::TscService;
using tsc::User;
using tsc::FollowRequest;
using tsc::UnfollowRequest;
using tsc::FollowReply;
using tsc::ListReply;
using tsc::ConnectRequest;
using tsc::Post;

// bool isUserExists(std::string curr_user, Json::Value users){
// 	if(users::isMember(curr_user)){
// 		std::cout<<"This user already exists. Connection done!"<<std::endl;
// 		return true;
// 	} 
// 	return false;

// }
class TscImpl final : public TscService::Service {
	public:
		std::unordered_map<std::string, ServerReaderWriter<Post, Post>* > name_streams;
	// explicit TscImpl() {
	//     // tsc::ParseDb(db, &feature_list_);
	// }
//create user function

	Status AddNewUser(ServerContext* context, const ConnectRequest* cRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Connection to server is successful.");
		std::string filename = "db.json";
		std::string timeline_name = "timeline.json";
		std::cout<<cRequest->user1()<<std::endl;
		std::string curr_user = cRequest->user1();
		//read from users db and check if present
		Json::Value users, posts;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		std::ifstream ip_posts(timeline_name);
		ip_users >> users;
		ip_posts >> posts;
		// user["Name"] = 
		if(users.isMember(curr_user)){
			std::cout<<"This user already exists. Connection done!"<<std::endl;
			return Status(StatusCode::ALREADY_EXISTS, "User exists!");		
		} 
		//finished checking for user
		Json::Value user;
		Json::Value post_user; 
		user["Followers"] = Json::Value(Json::arrayValue);
		user["Following"] = Json::Value(Json::arrayValue);
		post_user["posts"] = Json::Value(Json::arrayValue);
		std::cout<<"Adding new user to the database."<<std::endl;
		user["Following"].append(curr_user);
		user["Followers"].append(curr_user);
		users[curr_user] = user;
		posts[curr_user] = post_user;
		std::ofstream of_obj(filename);
		std::ofstream of_post(timeline_name);

		of_obj<<std::setw(4)<<users<<std::endl;
		of_post<<std::setw(4)<<posts<<std::endl;
		return Status::OK;
	}

	Status AddToUsersDB(ServerContext* context, const FollowRequest* fRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Success");
		std::string filename = "db.json";
		std::string user1 = fRequest->user1();
		std::string user2 = fRequest->user2();
		std::cout<<user1<<" Following "<<user2<<std::endl;

		Json::Value users;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		ip_users >> users;


		if(users.isMember(user1) && users.isMember(user2)){
			users[user1]["Following"].append(user2);
			users[user2]["Followers"].append(user1);
			std::cout<<"Finished appending to users db."<<std::endl;
			std::ofstream of_obj(filename);
			of_obj<<std::setw(4)<<users<<std::endl;
			return Status::OK;		
		} else{
			std::cout<<"Both the users don't exist."<<std::endl;
		}

		

		return Status::OK;		
	}

	Status RemoveFromUsersDB(ServerContext* context, const UnfollowRequest* uRequest,
	                  FollowReply* fReply) override {
		fReply->set_message("Success");
		std::string filename = "db.json";
		std::string user1 = uRequest->user1();
		std::string user2 = uRequest->user2();
		std::cout<<user1<<" unfollowing "<<user2<<std::endl;

		Json::Value users;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		ip_users >> users;
		// users.removeMember("default", &user2);
		if(users.isMember(user1) && users.isMember(user2)){
			Json::Value new_items = Json::arrayValue;
			// Json::arrayValue new_items;
			Json::Value new_followers = Json::arrayValue;
			int c = 0;
			for(int i = 0; i<users[user1]["Following"].size(); i++){
				if(users[user1]["Following"][i].compare(user2) != 0){
					new_items[c] = users[user1]["Following"][i];
					c++;
					
				}

			}
			users[user1]["Following"] = new_items;

			int d = 0;
			for(int i = 0; i<users[user2]["Followers"].size(); i++){
				if(users[user2]["Followers"][i].compare(user1) != 0){
					new_followers[d] = users[user2]["Followers"][i];
					d++;
					
				}

			}
			users[user1]["Followers"] = new_followers;

			// users[user1]["Following"].removeIndex()
			
			std::cout<<"Finished unfollowing users db."<<std::endl;
			std::ofstream of_obj(filename);
			of_obj<<std::setw(4)<<users<<std::endl;
			return Status::OK;		
		} else{
			std::cout<<"Either of the users don't exist."<<std::endl;
		}

		

		return Status::OK;		
	}
	Status GetAllFollowers(ServerContext* context, const ConnectRequest* c1,
	                  ListReply* listReply) override {
		// listReply->
		// listReply->set_message("Success");
		std::string filename = "db.json";
		std::string user = c1->user1();
		Json::Value users;
		Json::Reader reader;
		std::ifstream ip_users(filename);
		ip_users >> users;
		if(users.isMember(user)){
			for(int i = 0 ; i < users[user]["Followers"].size();i++){
				listReply->add_users(users[user]["Followers"][i].asString());
			}
			
		}else{
			std::cout<<"Member does not exist in the database"<<std::endl;
		}

		// Json::Value::members allUsers = users.getMemberNames();
		for(auto const& id : users.getMemberNames()){
			listReply->add_allusers(id);
		}
		return Status::OK;
	}

	Status TimeLine(ServerContext* context, ServerReaderWriter<Post, Post>* stream) override {
		std::string filename = "timeline.json";
		std::string db_filename = "db.json";
		Json::Value posts;
		Json::Value users;
		Json::Reader reader1;
		Json::Reader reader2;
		std::ifstream ip_posts(filename);
		std::ifstream ip_users(db_filename);
		
		Post p;
		grpc::string_ref curr_ref = context->client_metadata().find("user_name")->second;
   		std::string user(curr_ref.begin(), curr_ref.end());
		name_streams[user] = stream;

		// std::cout<<name_streams[user]<<std::endl;
        while(stream->Read(&p)) {
        	ip_users>>users;
        	ip_posts>>posts;
            std::string msg = p.content();
            Post new_post;
            new_post.set_content(msg);
            std::cout << "got a message from client: " << msg << std::endl;
            for(int i =0; i< users[user]["Followers"].size(); i++){
            	std::string curr_follower = users[user]["Followers"][i].asString();
            	Json::Value newTL = Json::arrayValue;
            	//check if timeline for user already exists and add this post to the top of the timeline
            	if(posts.isMember(curr_follower)){
            		newTL.append(msg);
            		for(int j =1;j<posts[curr_follower].size(); j++){
            			newTL.append(posts[curr_follower][j].asString());
            		}
            	}

            	if(name_streams.find(curr_follower) == name_streams.end()){
            		std::cout<<"No stream for follower yet."<<std::endl;
            	} else{

            		std::cout << "Returning messages to follower: " << curr_follower << std::endl;
            		for(int j = 0; j<newTL.size() ; j++){
            			Post new_post;
            			new_post.set_content(newTL[j].asString());
            			name_streams[curr_follower]->Write(new_post);
            			if(j == 20) break;
            		}
            		posts[curr_follower]["posts"] = newTL;
            		
            		
            	}
            	
            }
            std::ofstream of_obj(filename);
			of_obj<<std::setw(4)<<posts<<std::endl;
            
            // std::cout << "returning a message to client: " << new_post.content() << std::endl;
            
            
        }

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
