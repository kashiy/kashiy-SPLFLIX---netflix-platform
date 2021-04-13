#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "../include/Action.h"



class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    ~Session();//destructor
    Session(const Session &another);//copy constructor
    Session(Session &&another);//move constructor
    void clean();
    void copy(const Session& another);
    Session& operator=(const Session &another);//copy assignment operator
    Session& operator=(Session &&another);//move assignment operator
    void start();
    std::unordered_map<std::string, User *> * getMap();
    std::vector<Watchable *>* getContentVector();
    std::vector<BaseAction*>* getActionsLogVector();
    void setActiveUser(User * newUser) ;
    User* getActiveUser() const;
    bool* getWork() ;
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    User* activeUser;
    bool work;
};
#endif
