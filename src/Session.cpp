#include "../include/Session.h"
#include "../include/json.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include "../include/Action.h"
#include "../include/Watchable.h"
#include "../include/User.h"


using json = nlohmann::json;
using namespace std;

Session::Session(const std::string &configFilePath): content(), actionsLog() , userMap(), activeUser(), work(true){
    //constructor + MIL
    //actionLog is initialized as an empty vector cause no actions have occurred in a new session
    std::ifstream i(configFilePath);//reding json files
    json j;
    i>>j;
    //std::string st=j.dump(1);
    i.close();
    long id=1;//field of watchable objects


    for (auto &item : j["movies"].items()) {
        string name = item.value()["name"];
        int minutes = item.value()["length"];

        vector<string> tags_;

        for (auto &tag: item.value()["tags"].items()){//iterator of every movie tags
            tags_.push_back(tag.value());//insert each tag into tag field
        }

        Movie *anotherMovie= new Movie(id,name,minutes,tags_);//create new movie on heap

        content.push_back(anotherMovie);//insert into content vector field

        id++;
    }//iterator of the movies
    //id=1;
    for (auto &item : j["tv_series"].items()) {
        string nameE = item.value()["name"];
        int minutesE = item.value()["episode_length"];

        vector<int> seasons_;//contains amount of episodes of each season

        for (auto &season: item.value()["seasons"].items()) {//iterator of seasons of series
            seasons_.push_back(season.value());//insert into season vector
        }

        vector<string> tags_;

        for (auto &tag: item.value()["tags"].items()) {//iterator of every tv tags
            tags_.push_back(tag.value());//insert each tag into tag field
        }

        int seasonNUM=1;//counter of seasons

        vector<int>::iterator seasonsIt;
        for(seasonsIt= seasons_.begin(); seasonsIt<seasons_.end(); seasonsIt++)
        {
            for(int episode=1; episode<=(*seasonsIt) ; episode++) {
                Episode *anotherEpisode = new Episode(id, nameE, minutesE,seasonNUM,episode,tags_);
                if((seasonsIt+1)==seasons_.end() && episode==(*seasonsIt)){ //if this is the last episode in series Make NextEpisodeId negative
                    anotherEpisode->setNextEpisodeId(-1);
                }
                else { //setNextEpisodeId for the nextEpisode
                    anotherEpisode->setNextEpisodeId(id);
                }

                content.push_back(anotherEpisode);//insert into content vector field
                id++;
            }//iterator of episodes of current season
            seasonNUM++;
        }//iterator of seasons vector


    }//iterator of the series

    userMap["default"]= new LengthRecommenderUser("default");//initialize with default user
    activeUser=userMap["default"];//at initialization time, default user is only user

}

Session::~Session(){//destructor
    vector<Watchable *>::iterator it1;
    for (auto &it1: content){
        if(it1){
            delete it1;
            (it1)=nullptr;
        }
    }
    content.clear();
    vector<BaseAction*>::iterator it2;
    for (auto &it2: actionsLog){
        if(it2){
            delete it2;
            (it2)=nullptr;
        }
    }
    actionsLog.clear();
    unordered_map<std::string,User*>::iterator it3;
    for (auto &it3: userMap){
        if(it3.second){
            delete it3.second;
            (it3.second)=nullptr;
        }
    }
    userMap.clear();
    activeUser= nullptr;//already deleted from heap through the deletion of userMap
}//destructor

void Session::copy(const Session& another) {
    for(auto &somecontent:  another.content){
        content.push_back(somecontent->clone());
    }

    for(auto &somelogs:  another.actionsLog){
        actionsLog.push_back(somelogs->clone());
    }

    for(auto &someusers:  another.userMap){
        userMap[someusers.first]=(someusers.second->clone());
        for(std::size_t i=0; i<someusers.second->get_history().size(); ++i){

            (*userMap[someusers.first]->historyPointer()).at(i)= content.at(someusers.second->get_history().at(i)->getId()-1);
            //take the history in index i, get the id, search place in content vector (at id-1), copy new pointer to history
        }
    }

    std::unordered_map<std::string,User*>::const_iterator it=userMap.find(another.activeUser->getName()); //we get the corrent pointer to the cloned active user
    activeUser= it->second; //we now give the correct place of the cloned active user
}

Session::Session(const Session &another): content(), actionsLog() , userMap(), activeUser(), work(true) { //copy constructor
    copy(another); //here we actually copy each object with the visitor pattern
}

void Session::clean() {
    vector<Watchable *>::iterator it1;
    for (auto &it1: content){
        if(it1){
            delete it1;
            (it1)=nullptr;
        }
    }
    content.clear();
    vector<BaseAction*>::iterator it2;
    for (auto &it2: actionsLog){
        if(it2){
            delete it2;
            (it2)=nullptr;
        }
    }
    actionsLog.clear();
    unordered_map<std::string,User*>::iterator it3;
    for (auto &it3: userMap){
        if(it3.second){
            delete it3.second;
            (it3.second)=nullptr;
        }
    }
    userMap.clear();
    activeUser= nullptr;//already deleted from heap through the deletion of userMap
}



Session::Session(Session &&another): content(another.content), actionsLog(another.actionsLog) , userMap(another.userMap), activeUser(another.activeUser), work(true) {//move constructor
    another.content.clear();
    another.actionsLog.clear();
    another.userMap.clear();
    another.activeUser= nullptr;
}//here we only steal the pointers in the vectors.. so no copy() is called here, but we delete the pointers in the vectors.

//copy assignment operator
Session& Session:: operator=(const Session &another) {
    if(&another == this){
        return *this;
    }
    clean();
    copy(another);
    work=true;
    return *this;
}

Session& Session:: operator=(Session &&another){//move assignment operator
    if(&another == this){
        return *this;
    }
    clean();
    copy(another);
    another.clean();
    work=true;
    return *this;
}



// **** start() implementation ****
void Session::start(){
    cout << "SPLFLIX is now on!" << std::endl; //inside the constructor of Session we initialize the default user

    while(work==true){
        std::string name;
        std::string actionInput;
        getline(cin,actionInput);
        BaseAction *action;
        if(actionInput != "") { //fix the problem with entering n and getting error
            if (actionInput.find("createuser") != std::string::npos) { //createuser ACTION
                std::string recAlg = actionInput.substr(actionInput.size() - 3);
                std::string createuser = "createuser";
                name = actionInput.substr(createuser.size() + 1, actionInput.size() - createuser.size() - 5);
                action = new CreateUser(name, recAlg);
                action->act(*this);
                delete action;
            } else if (actionInput.find("changeuser") != std::string::npos) { // changeuser ACTION
                std::string changeuser = "changeuser";
                name = actionInput.substr(changeuser.size() + 1);
                action = new ChangeActiveUser(name);
                action->act(*this);
                delete action;
            } else if (actionInput.find("deleteuser") != std::string::npos) { //DELETEUSER ACTION
                std::string deleteuser = "deleteuser";
                name = actionInput.substr(deleteuser.size() + 1);
                action = new DeleteUser(name);
                action->act(*this);
                delete action;
            } else if (actionInput.find("dupuser") != std::string::npos) { //DUP USER ACTION
                std::string dupuser = "dupuser";
                std::string origianlName;
                std::string newName;
                name = actionInput.substr(dupuser.size() + 1); // get both names;
                origianlName = name.substr(0, name.find(" "));
                newName = name.substr(name.find(" ") + 1);
                action = new DuplicateUser(origianlName, newName);
                action->act(*this);
                delete action;
            } else if (actionInput == "content") { //Print Content List ACTION
                action = new PrintContentList();
                action->act(*this);
                delete action;
            } else if (actionInput == "watchhist") { //Print watchhist ACTION
                action = new PrintWatchHistory();
                action->act(*this);
                delete action;
            } else if (actionInput.find("watch") != std::string::npos) { //watch ACTION
                std::string watch = "watch";
                std::string contentId;
                contentId = actionInput.substr(watch.size() + 1);
                //he C++ std::string class provides string::size_type as an integer datatype
                // large enough to represent any possible string size
                long id = std::stol(contentId);
                //base argument is '10' by default
                action = new Watch(id);
                action->act(*this);
                delete action;
            } else if (actionInput == "log") { //Print Actions Log ACTIONS
                action = new PrintActionsLog();
                action->act(*this);
                delete action;
            } else if (actionInput == "exit") {
                action = new Exit();
                action->act(*this);
                delete action;
            } else if (actionInput == "n") {
                //do nothing, go to next input, empty CIN
            }
            else {
                std::cout << "Error - No such Action is known to us" << std::endl;
            }
        }

    }
}


// **** getMap() implementation ****
std::unordered_map<std::string,User*>* Session::getMap(){
    return &userMap;
}

// **** setActiveUser() implementation ****
void Session::setActiveUser(User * newUser)  {
    activeUser=newUser;
}

// **** getActionsLogVector() implementation ****
std::vector<BaseAction*>* Session::getActionsLogVector(){
    return &actionsLog;
}

// **** getContentVector() implementation ****
std::vector<Watchable *>* Session::getContentVector(){
    return &content;
}
// **** getActiveUserName() implementation ****
User* Session::getActiveUser()const{
    return activeUser;
}
// **** getWork() implementation ****
bool* Session::getWork() {
    return &work;
}


