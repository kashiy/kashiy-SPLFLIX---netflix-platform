#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "../include/Action.h"
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
using namespace std;

/*enum ActionStatus{ //delete this
	PENDING, COMPLETED, ERROR
}*/

// **** BaseAction() implementation ****
BaseAction::BaseAction():  errorMsg("error"),  status(PENDING){}//constructor
    ActionStatus BaseAction::getStatus() const{
        return status;
    };
    void BaseAction::complete(){
        status=COMPLETED;
    }
    void BaseAction::error(const std::string& errorMsg){
      status=ERROR;
      BaseAction::errorMsg=errorMsg;
    }

    std::string BaseAction::getErrorMsg() const{
        return errorMsg;
    }

std::string BaseAction::getStatusTo_String() const {
    std::string statusString;
    if (BaseAction::getStatus() == PENDING) {
        statusString = "PENDING";
    } else if (BaseAction::getStatus() == COMPLETED) {
        statusString = "COMPLETED";
    } else if (BaseAction::getStatus() == ERROR) {
        statusString = "ERROR: "+ BaseAction::getErrorMsg();
    }
    return statusString;
}

BaseAction::~BaseAction() {

}

// **** CreateUser implementation ****
CreateUser::CreateUser(std::string username, std::string tlc): BaseAction(), user_name(username), three_letter_code(tlc) {}//constructor

    void CreateUser::act(Session &sess) {
        std::unordered_map<std::string, User *> *userMap = sess.getMap();
        std::unordered_map<std::string,User*>::const_iterator it=(*userMap).find(user_name);//check if username already exists
        if (it != (*userMap).end()){//if the user name is new, find function returns iterator to map.end
            error("This user name already exists");
            std::cout<< BaseAction::getStatusTo_String() << std::endl;
        }
        else {//check recommendation algorithm for new username
            if (three_letter_code == "len") {
                (*userMap)[user_name] = new LengthRecommenderUser(user_name);//insert into userMap
                BaseAction::complete();//change status
            } else if (three_letter_code == "rer") {
                (*userMap)[user_name] = new RerunRecommenderUser(user_name);
                BaseAction::complete();
            } else if (three_letter_code == "gen") {
                (*userMap)[user_name] = new GenreRecommenderUser(user_name);
                BaseAction::complete();
            } else {//if 3-letter-code invalid
                BaseAction::error("recommendation algorithm is invalid");
                std::cout<< BaseAction::getStatusTo_String() << std::endl;
            }
        }
        sess.getActionsLogVector()->push_back(this->clone());
    }


    std::string CreateUser::toString() const {
        return "Create User " + BaseAction::getStatusTo_String();
    }

    BaseAction* CreateUser::clone(){
        return new CreateUser(*this);
    }


// **** ChangeActiveUser implementation ****
ChangeActiveUser::ChangeActiveUser(std::string changeToUser): BaseAction(), change_To_User(changeToUser) { //constructor
}

void ChangeActiveUser::act(Session &sess) {
    std::unordered_map<std::string,User*>::const_iterator itOnUsers= sess.getMap()->find(change_To_User); //search for change user in map
    if(itOnUsers==sess.getMap()->end()){
        BaseAction::error("This user name does not exist");
        std::cout<< BaseAction::getStatusTo_String() << std::endl;
    }
    else{
        sess.setActiveUser(itOnUsers->second);// calls finction of Session and gives the wanted user
        BaseAction::complete();
    }
    sess.getActionsLogVector()->push_back(this->clone());
}

std::string ChangeActiveUser::toString() const {
        return "Change Active User " + BaseAction::getStatusTo_String();
}

BaseAction* ChangeActiveUser::clone(){
        return new ChangeActiveUser(*this);
    }


// **** DeleteUser implementation ****
DeleteUser::DeleteUser(std::string username):BaseAction(), user_name(username) {}//constructor

void DeleteUser:: act(Session & sess) {
    std::unordered_map<std::string, User *> *userMap = sess.getMap();
    std::unordered_map<std::string, User *>::const_iterator it = (*userMap).find(user_name);//check if username already exists
    if (it == (*userMap).end()) {
        BaseAction::error("this user name doesn't exists");
        std::cout<< BaseAction::getStatusTo_String() << std::endl;
    }
    else{
        delete it->second;//delete from heap - the user has a distructor
        (*userMap).erase(user_name);//delete from userMap
        BaseAction::complete();
    }
    sess.getActionsLogVector()->push_back(this->clone());
}
std::string DeleteUser::toString() const{
        return "Delete User " + BaseAction::getStatusTo_String();
    }

BaseAction* DeleteUser::clone(){
        return new DeleteUser(*this);
    }

// **** DuplicateUser implementation ****

DuplicateUser::DuplicateUser(std::string original_user_name, std::string new_user_name):BaseAction(), original_user_name(original_user_name), new_user_name(new_user_name) {}//constructor

void DuplicateUser::act(Session &sess) { //Duplicate the user
    std::unordered_map<std::string,User*>::const_iterator iterOriginalUser= sess.getMap()->find(original_user_name); //search for original user in map
    std::unordered_map<std::string,User*>::const_iterator iterNewUser= sess.getMap()->find(new_user_name); //search if new_user already in map
    if(iterOriginalUser==sess.getMap()->end() ){ //is the original exist?
        BaseAction::error("The original user does not exist");
        std::cout<< BaseAction::getStatusTo_String() << std::endl;
    }
    else if(iterNewUser != sess.getMap()->end()){ //new_user_name already taken?
        BaseAction::error("The new user already exists");
        std::cout<< BaseAction::getStatusTo_String() << std::endl;
    }
    else{
        User* copy_of_new_user= iterOriginalUser->second->clone(); //clone all the content of the user with the visitor design pattern
        copy_of_new_user->setName(new_user_name); //set the name to be new
        std::unordered_map<std::string, User *> *userMapInsertTheDup = sess.getMap(); // get the pointer to the UsersMap
        (*userMapInsertTheDup)[new_user_name]=copy_of_new_user; //insert the cloned user to the map
        BaseAction::complete();
    }
    sess.getActionsLogVector()->push_back(this->clone());
}

std::string DuplicateUser::toString() const{
    return "Duplicate Active User " + BaseAction::getStatusTo_String();
}

BaseAction* DuplicateUser::clone(){
    return new DuplicateUser(*this);
}

// **** PrintContentList implementation ****
PrintContentList::PrintContentList(): BaseAction() {}//constructor
void PrintContentList::act (Session& sess){
    for (auto &itContVec: (*sess.getContentVector())){
       long idp = itContVec->getId();
       std::string namep = itContVec->toString();
       long lengthp = itContVec->getLength();
       std::string tagp = itContVec->getTagsString();
       cout<<idp<<". "<<namep<<" "<<lengthp<<" minutes "<<tagp<<endl;

    }//iterator of all watchable content
    BaseAction::complete();
    sess.getActionsLogVector()->push_back(this->clone());
}

std::string PrintContentList::toString() const{
        return "Print content list " + BaseAction::getStatusTo_String();
    }

BaseAction* PrintContentList::clone(){
    return new PrintContentList(*this);
    }

// **** PrintWatchHistory implementation ****
PrintWatchHistory::PrintWatchHistory():BaseAction(){}//constructor

void PrintWatchHistory::act (Session& sess){
        std::string username = sess.getActiveUser()->getName();
        cout<<"Watch history for "<<username<<endl;//first line to be printed
        int count=1;

      for(auto &iterHisrory: sess.getActiveUser()->get_history()){
        cout<<count<<". "<<iterHisrory->toString()<<endl;
        count++;
    } //print every watchable in activeUser's history
    BaseAction::complete();
    sess.getActionsLogVector()->push_back(this->clone());
}
std::string PrintWatchHistory::toString() const{
        return "Print watch history " + BaseAction::getStatusTo_String();
}

BaseAction*  PrintWatchHistory::clone(){
        return new PrintWatchHistory(*this);
    }

// **** Watch implementation ****
Watch::Watch(long content_id):id(content_id){}//constructor

void Watch::act(Session &sess) {

        std::string contentName =sess.getContentVector()->at(id-1)->toString(); //we ask for content in id.. but vector starts in zero.. so we need to take less one in id
        //The session have the content vector,
        //By using 'at' func of vectors,
        //we get a pointer at the watchable content who's id we received,
        //Now we can use the 'toString()' func of watchable
        //and receive the content's name
        cout<<"Watching "<<contentName<<endl;

        sess.getActiveUser()->addToWatchHistory(sess.getContentVector()->at(id-1));//adding this content to the current active user's watch history
        //add to history
        id=id+1;
        BaseAction::complete();
    sess.getActionsLogVector()->push_back(this->clone());

    Watchable *next_content = sess.getActiveUser()->getRecommendation(sess);
    if (next_content != nullptr){//If no content fits the recommendation criteria – a null pointer should be returned.
        char ans;
        cout<<"We recommend watching "<<(*next_content).toString()<<", continue watching? [y/n]"<<endl;
        cin>>ans;
        if (ans == 'y'){//user wants to watch the recommended content
            id=(*next_content).getId(); //if the user wanted to watch the recommendation we need to update the id of it correctly here
            Watch::act(sess);
        }
        else{
            cin.ignore();
            cin.clear();
        }

    }
    }

std::string Watch::toString() const{
        return "Watch " + BaseAction::getStatusTo_String();
    }

BaseAction* Watch::clone(){
    return new Watch(*this);
    }

// **** PrintActionsLog implementation ****
PrintActionsLog::PrintActionsLog(): BaseAction(){}//constructor

void PrintActionsLog::act(Session& sess){

      for (vector<BaseAction*>::reverse_iterator iterActVec=sess.getActionsLogVector()->rbegin(); iterActVec!=sess.getActionsLogVector()->rend(); ++iterActVec){
     cout<<(*iterActVec)->toString()<<endl;

    }//print all actions

    BaseAction::complete();
    sess.getActionsLogVector()->push_back(this->clone());
}
std::string PrintActionsLog::toString() const{
        return "Print Actions Log " + BaseAction::getStatusTo_String();
    }

BaseAction*  PrintActionsLog::clone(){
    return new PrintActionsLog(*this);
}


// **** Exit implementation ****
Exit::Exit():BaseAction() {}//constructor

void Exit::act(Session &sess) {
        bool*temp = sess.getWork();
    (*temp)=false;
    BaseAction::complete();
    sess.getActionsLogVector()->push_back(this->clone());
}

std::string Exit::toString() const{
        return "Exit " + BaseAction::getStatusTo_String();
}
    BaseAction*  Exit::clone(){
        return new Exit(*this);
    }

