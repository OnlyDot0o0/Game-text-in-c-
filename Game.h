#ifndef GAME_H
#define GAME_H

#include "json.hpp"
#include <string>
#include <vector>
using namespace std;
#include <unordered_map>

enum MapType {
    Map1,
    Map2,
    Map3,
    Map4,

};

class Room {
public:
    std::string id;
    std::string desc;
    std::unordered_map<std::string, std::string> exits;
};

class Object {
public:
    std::string id;
    std::string desc;
    std::string initialRoom;
};

class Enemy {
public:
    std::string id;
    std::string desc;
    int aggressiveness;
    std::string initialRoom;
    std::vector<std::string> killedBy;
    bool isKilled;
    Enemy(): isKilled(false){}
};

class Player {
public:
    std::string initialRoom;
};

class Objective {
public:
    std::string type;
    std::vector<std::string> what;
};

class MapData {
public:
    MapType mapType;  
    std::vector<Room> rooms;
    std::vector<Object> objects;
    std::vector<Enemy> enemies;
    Player player;
    Objective objective;
};

class Game {
public:
    Game(const string& mapFileName);
    void startGame();
    void processCommand(const string& command);
    void lookAround();
    bool isObjectiveComplete();
    void go(const string& direction);
    void pick(const string& objectId);
    void kill (const string& enemyId);

private:
    bool hasGun;
    bool hasSilverBullet;
    bool enemyDefeated(const :: string &enemyId);
    MapData loadMapData(const string& mapFileName);
    void printRoomDescription(const Room& room);
    void printObjectDescription(const Object& object);
    void printEnemyDescription(const Enemy& enemy);
    
    
    MapData mapData;
    Room currentRoom;
    vector<string> collectedGems;
};

#endif // GAME_H
