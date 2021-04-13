#include "../include/Session.h"
#include "../include/Watchable.h"
#include <string>
#include <vector>
using namespace std;

// **** watchable implementation ****
    Watchable::Watchable(long id, int length, const std::vector<std::string>& tags):id(id),length(length),tags(tags){}//constructor

    Watchable::~Watchable(){}//destructor



    std::vector<std::string> getTags();
    long Watchable::getId() const{
        return id;
    }
    int Watchable::getLength() const{
        return length;
    }
    string Watchable::getTagsString() const {
        string tagsString= "[";
            for(auto &tag: tags){
                tagsString.append(tag).append(",");
            }
        tagsString= tagsString.substr(0,tagsString.length()-1); // remove the last comma ,
        tagsString.append("]");
        return tagsString;
    }



std::vector<std::string> &Watchable::getTagsVectorRef() {
    return tags;
}


Movie::Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags):Watchable(id,length,tags), name(name){}//constructor

std::string Movie::toString() const{
    return name;
}
Watchable* Movie::getNextWatchable(Session& s) const{
    Watchable *b = nullptr;
    return b;
}

Watchable *Movie::clone() {//shallow copy we need ruleof5
    return new Movie(*this);
}





Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>&    tags): Watchable(id,length,tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(-1){}



std::string Episode::toString() const
{
    std::string addZerotoSeason;
    if (season<=9){
        addZerotoSeason="0"+std::to_string(season);
    }
    else{
        addZerotoSeason=std::to_string(season);
    }

    std::string addZerotoEpisode;
    if (episode<=9){
        addZerotoEpisode="0"+std::to_string(episode);
    }
    else{
        addZerotoEpisode=std::to_string(episode);
    }
return std::string(seriesName + " S" + addZerotoSeason + "E" + addZerotoEpisode);
}

void Episode::setNextEpisodeId(long num){
    nextEpisodeId=num;
}
Watchable* Episode::getNextWatchable(Session& s) const{
        if(getNextEpisodeId() != -1){
            Watchable* nextEpisodeToWatch = s.getContentVector()->at(getNextEpisodeId());
            return nextEpisodeToWatch;
        }
        else{
            return nullptr;
        }

}

long Episode::getNextEpisodeId() const {
    return nextEpisodeId;
}

Watchable *Episode::clone() { //shallow copy we need ruleof5
    return new Episode(*this);
}



