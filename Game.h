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
    bool isPickedUp; 
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
    string intro_msg;
    string successful_kill_msg;
    string unsuccessful_kill_msg;
    string unsuccessful_escape_msg;

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
    void go(const string& direction);
    void pick(const string& objectId);
    void kill (const string& enemyId);
    void eat(const string& objectId);

    bool isObjectiveComplete();
    bool hasRequiredItems(const Enemy& enemy);
    void handleEnemyAttack(const string& command);
    void removeEnemy(const string& enemyId);
    bool isObjectInCurrentRoom(const string& objectId) const;
    void removeObjectFromRoom(const string& objectId, const string& roomId);


    void printRoomDescription(const Room& room);
    void printObjectDescription(const Object& object);
    void printEnemyDescription(const Enemy& enemy);
    void displayInventory() const;
    void moveEnemyToRandomRoom(Enemy& enemy);
    void simulateEnemyMovement();

    MapData loadMapData(const string& mapFileName);
    MapData mapData;
    Room currentRoom;
    vector<string> collectedGems;
};

#endif // GAME_H
