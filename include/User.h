#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    void setName(const std::string& newName);
    virtual void addToWatchHistory(Watchable* content);

    virtual User* clone()=0;

    void clean(); //clean func for assignments
    virtual ~User(); //destructor
    void copy(const User& other); //copy function for constructors
    User(const User& copiedUser); //copy constructor
    User& operator=(const User& copiedUser); //assignement operator

    virtual bool alreadyInHistory(Watchable* content);
    std::vector<Watchable*>* historyPointer();

protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* clone();
    virtual void addToWatchHistory(Watchable* content);

private:
    long historysAverageLength;
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* clone();
    virtual void addToWatchHistory(Watchable* content);
private:
    int index;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* clone();
    virtual void addToWatchHistory(Watchable* content);
    virtual bool exists(const std::string &tag);
    virtual std::pair<std::string,long>* wherePairExists(const std::string& tag);
    virtual bool contentHasTag(Watchable &content,std::string tag);
private:
    std::vector<std::pair<std::string,long>> tagsPopularity;
};

#endif
