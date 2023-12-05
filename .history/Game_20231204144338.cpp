#include "Game.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <algorithm> 
using namespace std;

using json = nlohmann::json;

Game::Game(const string& mapFileName): hasGun(false),hasSilverBullet(false) {
    mapData = loadMapData(mapFileName);
    currentRoom = mapData.rooms.front(); 
}

void Game::startGame() {
    printRoomDescription(currentRoom);
}

void Game::processCommand(const string& command) {
    // Convert the command to lowercase for case-insensitivity
    string lowercaseCommand = command;
    transform(lowercaseCommand.begin(), lowercaseCommand.end(), lowercaseCommand.begin(), ::tolower);

    if (lowercaseCommand.find("look around") != string::npos) {
        lookAround();
    } else if (lowercaseCommand.substr(0, 2) == "go") {
        std::string direction = lowercaseCommand.substr(3);
        go(direction);
    } else if (lowercaseCommand.find("pick") != string::npos) {
        string objectId = lowercaseCommand.substr(5); 
        pick(objectId);
    } else if (lowercaseCommand.find("kill") != string::npos) {
        string enemyId = lowercaseCommand.substr(5); 
        kill(enemyId);
    } else {
        cout << "Invalid command. Type 'look around', 'go xxx', 'pick xxx', or 'kill xxx', or quit." << endl;
    }
}

void Game::pick(const string& objectId) {
    auto object = find_if(
        mapData.objects.begin(), mapData.objects.end(),
        [this, &objectId](const Object& obj) { return obj.id == objectId && obj.initialRoom == currentRoom.id; });

    if (object != mapData.objects.end()) {
        if (mapData.mapType == MapType::Map1 && object->id == "gun") {
            hasGun = true;
            cout << "You picked up a " << object->id << "." << endl;
        }

        // Map2
        if (mapData.mapType == MapType::Map2 && object->id == "silver bullet") {
            hasSilverBullet = true;
            cout << "You picked up a " << object->id << "." << endl;
        }

        // Map3
        if (mapData.mapType == MapType::Map3 &&
            (object->id == "red gem" || object->id == "green gem" || object->id == "blue gem")) {
            collectedGems.push_back(object->id);
            cout << "You picked up a " << object->id << "." << endl;
        }

    } else {
        cout << "Error: Object not found or not in this room." << endl;
    }
}




void Game::kill(const string& enemyId) {
    auto enemy = find_if(
        mapData.enemies.begin(), mapData.enemies.end(),
        [this, &enemyId](const Enemy& e) { return e.id == enemyId && e.initialRoom == currentRoom.id; });

    if (enemy != mapData.enemies.end()) {
        if(hasGun || hasSilverBullet){
            cout << "You killed the " << enemyId << "." << endl;
        }
        else{
            cout<<"You cant kill the " << enemyId <<  "because you are weak hehe"<<endl;
        }


    } else {
        cout << "Error: Enemy not found or not in this room." << endl;
    }
}

void Game::lookAround() {
    printRoomDescription(currentRoom);
}

void Game::go(const string& direction) {
    auto exit = currentRoom.exits.find(direction);
    if (exit != currentRoom.exits.end()) {
        auto nextRoom = find_if(
            mapData.rooms.begin(), mapData.rooms.end(),
            [&exit](const Room& room) { return room.id == exit->second; });

        if (nextRoom != mapData.rooms.end()) {
            currentRoom = *nextRoom;
            printRoomDescription(currentRoom);
        } else {
            cout << "Error: Destination room not found." << endl;
        }
    } else {
        cout << "Error: Invalid direction." << endl;
    }
}
MapData Game::loadMapData(const std::string& mapFileName) {
    std::ifstream file(mapFileName);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open map file '" << mapFileName << "'. Please make sure the file exists and try again." << std::endl;
    }
    json jsonData;
    file >> jsonData;

    MapData mapData;

    // Extract data from jsonData and populate mapData

    // // Set the default map type to MapType::Map1
    // mapData.mapType = MapType::Map1;

    // if (jsonData.contains("mapType") && jsonData["mapType"].is_string()) {
    //     const std::string mapTypeStr = jsonData["mapType"];
    //     if (mapTypeStr == "Map1") {
    //         mapData.mapType = MapType::Map1;
    //     } else if (mapTypeStr == "Map2") {
    //         mapData.mapType = MapType::Map2;
    //     } else if (mapTypeStr == "Map3") {
    //         mapData.mapType = MapType::Map3;
    //     } else if (mapTypeStr == "Map4") {
    //         mapData.mapType = MapType::Map4;
    //     } 
    // }

    // Extract data from jsonData and populate mapData
    if (jsonData.contains("rooms") && jsonData["rooms"].is_array()) {
        for (const auto& roomJson : jsonData["rooms"]) {
            Room room;
            room.id = roomJson["id"];
            room.desc = roomJson["desc"];
            
            for (const auto& exit : roomJson["exits"].items()) {
                room.exits[exit.key()] = exit.value();
            }

            mapData.rooms.push_back(room);
        }
    }

    if (jsonData.contains("objects") && jsonData["objects"].is_array()) {
        for (const auto& objectJson : jsonData["objects"]) {
            Object object;
            object.id = objectJson["id"];
            object.desc = objectJson["desc"];
            object.initialRoom = objectJson["initialroom"];
            mapData.objects.push_back(object);
        }
    }

    if (jsonData.contains("enemies") && jsonData["enemies"].is_array()) {
        for (const auto& enemyJson : jsonData["enemies"]) {
            Enemy enemy;
            enemy.id = enemyJson["id"];
            enemy.desc = enemyJson["desc"];
            enemy.aggressiveness = enemyJson["aggressiveness"];
            enemy.initialRoom = enemyJson["initialroom"];

            for (const auto& killedBy : enemyJson["killedby"]) {
                enemy.killedBy.push_back(killedBy);
            }

            mapData.enemies.push_back(enemy);
        }
    }

    if (jsonData.contains("player") && jsonData["player"].is_object()) {
        mapData.player.initialRoom = jsonData["player"]["initialroom"];
    }

    if (jsonData.contains("objective") && jsonData["objective"].is_object()) {
        mapData.objective.type = jsonData["objective"]["type"];

        for (const auto& what : jsonData["objective"]["what"]) {
            mapData.objective.what.push_back(what);
        }
    }

    return mapData;
}

void Game::printRoomDescription(const Room& room) {
    cout << room.desc << endl;

//objects
    for (const auto& object : mapData.objects) {
        if (object.initialRoom == room.id) {
            printObjectDescription(object);
        }
    }

    //enemies
    for (const auto& enemy : mapData.enemies) {
        if (enemy.initialRoom == room.id) {
            printEnemyDescription(enemy);
        }
    }
}

void Game::printObjectDescription(const Object& object) {
    cout << "Interesting. Is that a " << object.id <<"? "<< object.desc << "." << endl;
}

void Game::printEnemyDescription(const Enemy& enemy) {
    cout << "Oh no.. Thats a " << enemy.id <<". "<<enemy.desc<< "." << endl;
}


bool Game::isObjectiveComplete() {
    if (mapData.objective.type == "kill") {
        // Check if all specified enemies are killed
        for (const auto& enemyId : mapData.objective.what) {
            auto killedEnemy = find_if(
                mapData.enemies.begin(), mapData.enemies.end(),
                [&enemyId](const Enemy& enemy) {
                    return enemy.id == enemyId && enemy.isKilled;
                });

            if (killedEnemy == mapData.enemies.end()) {
                return false; // Objective not complete
            }
        }
        return true; // All specified enemies are killed
    } else if (mapData.objective.type == "collect") {
        // Check if all specified objects are collected
        for (const auto& objectId : mapData.objective.what) {
            auto collectedObject = find(
                collectedGems.begin(), collectedGems.end(), objectId);

            if (collectedObject == collectedGems.end()) {
                return false; // Objective not complete
            }
        }
        return true; // All specified objects are collected
    } else if (mapData.objective.type == "room") {
        // Check if the player is in the specified room
        return currentRoom.id == mapData.objective.what.front();
    } else {
        // Handle other types of objectives as needed
        return false; // Placeholder
    }
}
