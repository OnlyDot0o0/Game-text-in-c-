#ifndef GAME_H
#define GAME_H

#include "json.hpp"
#include <string>
#include <vector>
using namespace std;
#include <unordered_map>



class Room {
public:
    string id;
    string desc;
    unordered_map<string, string> exits;
};

class Object {
public:
    string id;
    string desc;
    string initialRoom;
    bool isPickedUp; //track whether the object is picked up.

    Object() : isPickedUp(false) {}
};


class Enemy {
public:
    string id;
    string desc;
    int aggressiveness;
    string initialRoom;
    vector<string> killedBy;
    bool isKilled;
    string currentRoom;
    Enemy(): isKilled(false){}
};

class Player {
public:
    string initialRoom;
    vector<string> inventory;
    int lives; 
    Player() : lives(1) {};
};

class Objective {
public:
    string type;
    vector<string> what;
};

class MapData {
public:
    vector<Room> rooms;
    vector<Object> objects;
    vector<Enemy> enemies;
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
    void eat(const string& objectId);
    bool hasRequiredItems(const Enemy& enemy);
    void handleEnemyAttack(const string& command);
    void removeEnemy(const string& enemyId);
    bool isObjectInCurrentRoom(const string& objectId) const;
    void removeObjectFromRoom(const string& objectId, const string& roomId);
    MapData loadMapData(const string& mapFileName);
    void printRoomDescription(const Room& room);
    void printObjectDescription(const Object& object);
    void printEnemyDescription(const Enemy& enemy);
    void displayInventory() const;
    void moveEnemyToRandomRoom(Enemy& enemy);
    void simulateEnemyMovement();
    MapData mapData;
    Room currentRoom;
    vector<string> collectedGems;
};

#endif // GAME_H