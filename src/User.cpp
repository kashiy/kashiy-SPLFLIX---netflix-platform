//
// Created by kashiy on 24/11/2019.
//

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include "../include/User.h"
#include "../include/Watchable.h"
#include "../include/Session.h"
#include <algorithm>

User::User(const std::string &name):history(), name(name) {

}//constructor

std::string User::getName() const {
    return name;
}

void User::setName(const std::string &newName) {
    name=newName;
}

std::vector<Watchable *> User::get_history() const { //get_history - it will copy all history by value
    return history;
}

void User::addToWatchHistory(Watchable * content) {
    history.push_back(content);
}


void User::clean() { //we delete only the pointers themselves
    if(!history.empty()){
            history.clear();
    }
    name="";
}

User::~User() {//destructor
    User::clean();
}

void User::copy(const User &other) { //deep copy
    this->name=other.getName();
    for(auto &contentWatchedPointer: other.get_history()){
        addToWatchHistory(contentWatchedPointer);
    }
}

User::User(const User &copiedUser): history(), name(){ // copy constructor
    copy(copiedUser);
}

User &User::operator=(const User &copiedUser) { // copy assignment constructor
    if (&copiedUser != this){
        clean();
        copy(copiedUser);
    }
    return *this;
}

bool User::alreadyInHistory(Watchable *content) {
    for(auto &watched : get_history()){
        if(watched->getId()==content->getId()){
            return true;
        }
    }

    return false;
}

std::vector<Watchable *>* User::historyPointer() {
    return &history;
}


LengthRecommenderUser::LengthRecommenderUser(const std::string &name) : User(name) ,historysAverageLength(0) {} //constructor

User *LengthRecommenderUser::clone() { // clone visitor pattern
    return new LengthRecommenderUser(*this);
}

void LengthRecommenderUser::addToWatchHistory(Watchable * content) { //add To Watch History of LengthRecommenderUser
    long contentLength=(*content).getLength();
    if(!history.empty()){ //computing the average
        long numContents=history.size();
        historysAverageLength= (historysAverageLength*numContents+contentLength)/(numContents+1);
    }
    else{ //this is the first content in history so this is the average
        historysAverageLength=contentLength;
    }
    User::addToWatchHistory(content);
}

Watchable *LengthRecommenderUser::getRecommendation(Session &s) {
    Watchable *lastwatched=get_history().back();
    Watchable *nextToBeWatched=lastwatched->getNextWatchable(s);
    if(nextToBeWatched==nullptr){ //means no next to watch, so need to use the ALGORITHM of LengthRecommender
        bool firstPositiveGap = true;
        long closestGap = std::abs((*s.getContentVector()).front()->getLength()-historysAverageLength); // give the first gap as the first value

        for(auto &content : (*s.getContentVector())){ //check every content for the closest gap
            long gap = std::abs(content->getLength()-historysAverageLength); //we need the abs because we want always the positive gap
            if(firstPositiveGap && !User::alreadyInHistory(content) ){
                    firstPositiveGap=false;
                    closestGap=gap;
                    nextToBeWatched=content;
            }
            else if(gap<closestGap && !User::alreadyInHistory(content)){
                closestGap=gap;
                nextToBeWatched=content;
            }
        }
        return nextToBeWatched;
    }
    else{ //means the next already determined and there is next episode
        return nextToBeWatched;
    }
}




RerunRecommenderUser::RerunRecommenderUser(const std::string &name) : User(name), index(0){}//constructor

Watchable* RerunRecommenderUser::getRecommendation(Session& s){

    Watchable *nextContent = get_history().back();
    nextContent = nextContent->getNextWatchable(s);

    if (nextContent == nullptr) {
        //last watched content was a movie or the last episode
        nextContent = get_history().at(index);
        index++;

    }
    return nextContent;
}

void  RerunRecommenderUser::addToWatchHistory(Watchable* content){
        User::addToWatchHistory(content);

}

User *RerunRecommenderUser::clone() {
    int temp =  index;
    index=0;
    User *newone= new RerunRecommenderUser(*this);
    index= temp;
    return newone;
}


GenreRecommenderUser::GenreRecommenderUser(const std::string &name) : User(name) , tagsPopularity() {}//constructor


User *GenreRecommenderUser::clone() {
    return new GenreRecommenderUser(*this);
}

void GenreRecommenderUser::addToWatchHistory(Watchable *content) {
    std::vector<std::string> tagsVectorOfWatched= content->getTagsVectorRef();
    for(auto &tag: tagsVectorOfWatched){
        if(exists(tag)){//if the pair for the tag exists so add 1 to popularity
            std::pair<std::string,long>* pa=wherePairExists(tag);
            (*pa).second=(*pa).second+1;
        }
        else{ //pair for the tags doesnt exist so create with 1 popularity
            std::pair <std::string,long> newWatched(tag ,1);
            tagsPopularity.push_back(newWatched);
        }
    }
    //sort tags
    std::sort( tagsPopularity.begin(), tagsPopularity.end(), [](const std::pair<std::string,long>& right, const std::pair<std::string,long>& left){
        return right.first.compare(left.first);
    });
    //after tags need to sort popularity
    std::sort( tagsPopularity.begin(), tagsPopularity.end(), [](const std::pair<std::string,long>& right, const std::pair<std::string,long>& left){
        return right.second-left.second;
    });

    User::addToWatchHistory(content);
}

std::pair<std::string,long>* GenreRecommenderUser::wherePairExists(const std::string &tag) { //returns thr cell of the pair in the vector
    std::vector<std::pair<std::string,long>>::iterator it;
    for(auto &it: tagsPopularity){
        if(it.first==tag){
            return &it;
        }
    }
    return nullptr;
}

bool GenreRecommenderUser::exists(const std::string &tag) { //search in the vector of pairs if the pair of this tag already exists
    std::pair<std::string,long>* pa=wherePairExists(tag);
    if(pa != nullptr){
        return true;
    }
    return false;
}

Watchable *GenreRecommenderUser::getRecommendation(Session &s) {
    Watchable *lastwatched=get_history().back();
    Watchable *nextToBeWatched=lastwatched->getNextWatchable(s);

    if(nextToBeWatched==nullptr) { //means no next to watch, so need to use the Genre ALGORITHM
        std::vector<std::pair<std::string,long>>::iterator pairsIt;
        for(pairsIt= tagsPopularity.begin(); pairsIt<tagsPopularity.end(); pairsIt++) //i need to start from the first tag with highest popularity
        {
            for(auto &content : (*s.getContentVector())) { //check every content for the tags
                if(!User::alreadyInHistory(content) && contentHasTag(*content, pairsIt.base()->first)){ //if haven't watched content and find the tag in this content this new content
                 nextToBeWatched = content;
                 return nextToBeWatched;//return only if found
                }
            }
        }
    }
    return nextToBeWatched; //return the nullptr if until now we didn't return any recommend
}

bool GenreRecommenderUser::contentHasTag(Watchable &content, std::string tag) {
    for(auto &currentTagInContentVector: content.getTagsVectorRef()){
        if(currentTagInContentVector==tag){
            return true;
        }
    }
    return false;
}


