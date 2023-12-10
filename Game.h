#ifndef GAME_H
#define GAME_H

#include "json.hpp"
#include <string>
#include <vector>
using namespace std;
#include <unordered_map>

class Room
{
public:
    string id;
    string desc;
    unordered_map<string, string> exits;
};

class Object
{
public:
    std::string id;
    std::string desc;
    std::string initialRoom;
    bool isPickedUp;

    // Constructor
    Object() : isPickedUp(false) {}

    // Copy constructor
    Object(const Object &other)
        : id(other.id), desc(other.desc), initialRoom(other.initialRoom), isPickedUp(other.isPickedUp) {}

    // Destructor
    ~Object()
    {
        // Add any necessary cleanup code here
    }

    bool isMatch(const std::string &objectId) const
    {
        return id == objectId;
    }
};

class Enemy
{
public:
    string id;
    string desc;
    int aggressiveness;
    string initialRoom;
    vector<string> killedBy;
    bool isKilled;
    string currentRoom;
    Enemy() : isKilled(false) {}
    string intro_msg;
    string successful_kill_msg;
    string unsuccessful_kill_msg;
    string unsuccessful_escape_msg;
};

class Player
{
public:
    string initialRoom;
    vector<Object *> inventory;
    int lives;
    Player() : lives(1){};
};

class Objective
{
public:
    string type;
    vector<string> what;
};

class MapData
{
public:
    vector<Room> rooms;
    vector<Object> objects;
    vector<Enemy> enemies;
    Player player;
    Objective objective;
};

class Game
{
public:
    Game(const string &mapFileName);
    void startGame();
    void processCommand(const string &command);

    void look(const string &id);
    void lookAround();
    void go(const string &direction);
    void pick(const string &objectId);
    void kill(const string &enemyId);
    void eat(const string &objectId);

    bool isObjectiveComplete() const;
    bool hasRequiredItems(const Enemy &enemy);
    void handleEnemyAttack(const string &command);
    void removeEnemy(const string &enemyId);
    bool isObjectInCurrentRoom(const string &objectId) const;
    void removeObjectFromRoom(const string &objectId, const string &roomId);
    // bool isItemInInventory(const std::string &itemId, const std::vector<Object *> &inventory);

    void printRoomDescription(const Room &room);
    void printObjectDescription(const Object &object);
    void printObjectID(const Object &object);
    void printEnemyDescription(const Enemy &enemy);
    void printEnemyID(const Enemy &enemy);
    void displayInventory() const;
    void moveEnemyToRandomRoom(Enemy &enemy);
    void simulateEnemyMovement();

    MapData loadMapData(const string &mapFileName);
    MapData mapData;
    Room currentRoom;
    vector<string> collectedGems;
    Player player;
};

#endif // GAME_H
