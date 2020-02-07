#include <iostream>
//#include <memory>
//#include <thread>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"

#include "tsc.grpc.pb.h"
#include "tsc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using tsc::TscService;
using tsc::User;
using tsc::FollowRequest;
using tsc::UnfollowRequest;
using tsc::FollowReply;
using tsc::ListReply;
using tsc::ConnectRequest;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}
        

    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        std::unique_ptr<TscService::Stub> stub_;
        // You can have an instance of the client stub
        // as a member variable.
        // std::unique_ptr<NameOfYourStubClass::Stub> stub_;
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "default";
    std::string port = "50051";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();




    return 0;
}

int Client::connectTo()
{
    // ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
    // ------------------------------------------------------------
    // create a channel
    std::string channelName = hostname + ":" + port;
    std::cout<<"connecting to : "<<channelName;
    stub_ = TscService::NewStub(grpc::CreateChannel(channelName,
                            grpc::InsecureChannelCredentials()));
    User u1;
    u1.set_name("default");
    
    ConnectRequest c1;
    c1.set_user1(username);
    // f1.set_allocated_user2(&u2);

    ClientContext context;
    FollowReply r1;
    Status status = stub_->AddNewUser(&context, c1, &r1);
    if (!status.ok())
        {
            std::cout<<"connection failed."<<std::endl;
            //return false;
        }
        else
        { 
            std::cout << "Connection worked! "<<std::endl;
        //return true;
        }
    std::cout<<r1.message()<<std::endl;
    std::cout<<"Finished connect method!"<<std::endl;
    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
    // ------------------------------------------------------------
    // GUIDE 1:
    // In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
    //
    // FOLLOW <username>
    // UNFOLLOW <username>
    // LIST
    // TIMELINE
    //
    // - JOIN/LEAVE and "<username>" are separated by one space.
    // ------------------------------------------------------------
    FollowReply r1;
    IReply ire;
    ClientContext context;
    if(input.substr(0,6).compare("FOLLOW")==0){
        std::string user2 = input.substr(7,input.length());
        // std::cout<<user2<<std::endl;
        std::cout<<"Follow request!"<<std::endl;
        User u1, u2;
        // std::string user1 = "User2";
        // std::string user2 = "User3";
        u1.set_name(username);
        u2.set_name(user2);
        FollowRequest f1;
        f1.set_user1(username);
        f1.set_user2(user2);

        Status status = stub_->AddToUsersDB(&context, f1, &r1);

        if(!status.ok()){
            std::cout<<"Something went wrong!"<<std::endl;
        }
        std::cout<<"Finished Follow!"<<std::endl;
        ire.grpc_status = status;
        if (status.ok()) {
            ire.comm_status = SUCCESS;
        } else {
            ire.comm_status = FAILURE_NOT_EXISTS;
        }

    }


    if(input.substr(0,8).compare("UNFOLLOW")==0){
        std::string user2 = input.substr(9,input.length());
        // std::cout<<user2<<std::endl;
        std::cout<<"Unfollow request!"<<std::endl;
        User u1, u2;
        // std::string user1 = "User2";
        // std::string user2 = "User3";
        u1.set_name(username);
        u2.set_name(user2);
        UnfollowRequest uf1;
        uf1.set_user1(username);
        uf1.set_user2(user2);

        Status status = stub_->RemoveFromUsersDB(&context, uf1, &r1);

        if(!status.ok()){
            std::cout<<"Something went wrong!"<<std::endl;
        }
        std::cout<<"Finished Unfollow!"<<std::endl;
        ire.grpc_status = status;
        if (status.ok()) {
            ire.comm_status = SUCCESS;
        } else {
            ire.comm_status = FAILURE_NOT_EXISTS;
        }

    }

    if(input.substr(0,4).compare("LIST")==0){
        // std::string user2 = input.substr(9,input.length());
        // std::cout<<user2<<std::endl;
        ListReply l1;
        std::cout<<"List request!"<<std::endl;
        User u1;
        // std::string user1 = "User2";
        // std::string user2 = "User3";
        u1.set_name(username);
        // u2.set_name(user2);
        ConnectRequest c1;
        c1.set_user1(username);
        // uf1.set_user2(user2);

        Status status = stub_->GetAllFollowers(&context, c1, &l1);
        // for(int i = 0; l1->users_size(); i++){
        //     std::cout<<l1->users(i)<<std::end;
        // }
        std::vector<std::string> following(l1.users().begin(), l1.users().end());
        std::vector<std::string> allUsers(l1.allUsers().begin(), l1.allUsers().end());
        ire.following_users = following;
        ire.all_users = allUsers;
        if(!status.ok()){
            std::cout<<"Something went wrong!"<<std::endl;
        }
        std::cout<<"Finished List!"<<std::endl;
        ire.grpc_status = status;
        if (status.ok()) {
            ire.comm_status = SUCCESS;
        } else {
            ire.comm_status = FAILURE_NOT_EXISTS;
        }

    }
    // ------------------------------------------------------------
    // GUIDE 2:
    // Then, you should create a variable of IReply structure
    // provided by the client.h and initialize it according to
    // the result. Finally you can finish this function by returning
    // the IReply.
    // ------------------------------------------------------------
    
    // ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Join" service method for JOIN command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Join(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
    // ------------------------------------------------------------
    

    
    return ire;
}

void Client::processTimeline()
{
    // ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
    // ------------------------------------------------------------
}
