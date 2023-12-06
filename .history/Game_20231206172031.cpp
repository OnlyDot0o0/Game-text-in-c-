#include "Game.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

using json = nlohmann::json;

Game::Game(const string& mapFileName) {
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
        string direction = lowercaseCommand.substr(3);
        go(direction);
    } else if (lowercaseCommand.find("pick") != string::npos) {
        string objectId = lowercaseCommand.substr(5); 
        pick(objectId);
    } else if (lowercaseCommand.find("kill") != string::npos) {
        string enemyId = lowercaseCommand.substr(5); 
        kill(enemyId);
    } else {
        cout << "Invalid command. Type 'look around', 'go xxx', 'pick xxx', or 'kill xxx', or 'quit' or 'inventory'" << endl;
    }
}
void Game::displayInventory() const {
    // Check if the player's inventory is empty
    if (mapData.player.inventory.empty()) {
        std::cout << "Your inventory is empty." << std::endl;
    } else {
        // Display the contents of the player's inventory
        std::cout << "Inventory: ";
        for (const auto& item : mapData.player.inventory) {
            std::cout << "'"<< item << "' ";
        }
        std::cout << std::endl;
    }
}

void Game::pick(const string& objectId) {
    // Check if the object is already in the player's inventory
    auto inventoryIter = find(mapData.player.inventory.begin(), mapData.player.inventory.end(), objectId);
    if (inventoryIter != mapData.player.inventory.end()) {
        cerr << "You have already picked up the " << objectId << "." << endl;
        return;
    }

    if(isObjectInCurrentRoom(objectId)){
        // Check if the specified object is in the current room
        Object* objectToPick = nullptr;
        for (auto& object : mapData.objects) {
            if (object.id == objectId && object.initialRoom == currentRoom.id) {
                objectToPick = &object;
                break;
            }
        }

        if (objectToPick != nullptr) {
            if (!objectToPick->isPickedUp) {
                // Add the object to the player's inventory
                mapData.player.inventory.push_back(objectId);
                cout << "You picked up the " << objectId << "." << endl;

                // Set the isPickedUp flag to true
                objectToPick->isPickedUp = true;
                removeObjectFromRoom(objectId, currentRoom.id);

            }
        } else {
            // Error: The specified object is not in this room
            cerr << "The " << objectId << " is not in this room." << endl;
        }
    } else {
        cerr << "You can't pick up " << objectId << "." << endl;
    }
}

bool Game::isObjectInCurrentRoom(const string& objectId) const {
    // Check if the specified object is in the current room
    for (const auto& object : mapData.objects) {
        if (object.id == objectId && object.initialRoom == currentRoom.id && !object.isPickedUp) {
            // Debug Print: Print whether the object is found for debugging
            // std::cout << "Object " << objectId << " found in room." << std::endl;
            return true;
        }
    }
    // Debug Print: Print whether the object is found for debugging
    //std::cout << "Object " << objectId << " not found in room." << std::endl;
    return false;
}


void Game::removeObjectFromRoom(const string& objectId, const string& roomId) {
    // Iterate through rooms to find the specified room
    for (auto& room : mapData.rooms) {
        // Check if the current room matches the specified room ID
        if (room.id == roomId) {
            // Iterate through objects to find the specified object
            for (auto it = mapData.objects.begin(); it != mapData.objects.end(); ) {
                // Check if the current object matches the specified object ID and is in the specified room
                if (it->id == objectId && it->initialRoom == roomId) {
                    // Erase the specified object from the objects list
                    it = mapData.objects.erase(it);
                    return; // Exit the function once the object is removed
                } else {
                    ++it;
                }
            }
        }
    }

    // Error: Specified room not found
    cerr << "Error: Room not found!" << endl;
}

void Game::kill(const string& enemyId) {
    vector<string> nonExhaustibleItems = {"gun"};
    auto enemyIter = find_if(
        mapData.enemies.begin(), mapData.enemies.end(),
        [&enemyId](const Enemy& enemy) {
            return enemy.id == enemyId && !enemy.isKilled;
        });

    if (enemyIter != mapData.enemies.end()) {
        // Check if the player has the required items
        if (hasRequiredItems(*enemyIter)) {
            // Mark the enemy as killed
            enemyIter->isKilled = true;
            cout << "You killed the " << enemyId << "." << endl;

            // Remove required items from the player's inventory
            for (const auto& item : enemyIter->killedBy) {
                // Only remove the item if it's not in the nonExhaustibleItems list
                if (find(nonExhaustibleItems.begin(), nonExhaustibleItems.end(), item) == nonExhaustibleItems.end()) {
                    auto itemIter = find(mapData.player.inventory.begin(), mapData.player.inventory.end(), item);
                    if (itemIter != mapData.player.inventory.end()) {
                        mapData.player.inventory.erase(itemIter);
                    }
                }
            }

            removeEnemy(enemyId);
        }
    } else {
        cerr << "Error: Enemy not found or already killed." << endl;
    }
}

bool Game::hasRequiredItems(const Enemy& enemy) {
    vector<string> missingItems;  // Keep track of missing items for later print

    for (const auto& item : enemy.killedBy) {
        auto itemIter = find(mapData.player.inventory.begin(), mapData.player.inventory.end(), item);
        if (itemIter == mapData.player.inventory.end()) {
            missingItems.push_back(item);
        }
    }

    if (!missingItems.empty()) {
        cout << "You don't have all the required items (" << enemy.id << " needs: ";
        for (const auto& item : missingItems) {
            cout << item << " ";
        }
        cout << ")." << endl;
        return false;
    }

    return true;
}

void Game::removeEnemy(const string& enemyId) {
    // Iterate through enemies to find the specified enemy
    for (auto it = mapData.enemies.begin(); it != mapData.enemies.end(); ) {
        // Check if the current enemy matches the specified enemy ID and is killed
        if (it->id == enemyId && it->isKilled) {
            // Erase the specified enemy from the enemies list
            it = mapData.enemies.erase(it);
            return; // Exit the function once the enemy is removed
        } else {
            ++it;
        }
    }

    // Error: Specified enemy not found or not killed
    cerr << "Error: Enemy not found or not killed." << endl;
}

void Game::lookAround() {
    printRoomDescription(currentRoom);
}

void Game::go(const string& direction) {
    auto exit = currentRoom.exits.find(direction);
    if (exit != currentRoom.exits.end()) {
        handleEnemyAttack(direction);
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

void Game::handleEnemyAttack(const string& command) {
    cout << "Command received: " << command << endl;

    // Check if the command is an attempt to exit the current room
    auto exitIter = currentRoom.exits.find(command);
    if (exitIter != currentRoom.exits.end()) {
        cout << "Command is an attempt to exit the current room." << endl;

        // Check if there's an enemy in the current room
        for (auto& enemy : mapData.enemies) {
            if (enemy.initialRoom == currentRoom.id && !enemy.isKilled) {
                cout << "Enemy detected in the room." << endl;

                // Check if the player has the required items to kill the enemy
                if (!hasRequiredItems(enemy)) {
                    cout << "You don't have the required items to kill the enemy!" << endl;
                    cout << "The " << enemy.id << " attacks you as you try to leave the room and you die." << endl;
                    cout << "Game over!" << endl;
                    exit(0);  // Exit the program
                }

                // Generate a random number between 0 and 100
                int randomNum = rand() % 101;
                cout << "Random number generated: " << randomNum << endl;

                // If the random number is less than or equal to the enemy's aggressiveness, the enemy attacks
                if (randomNum <= enemy.aggressiveness) {
                    cout << "The enemy attacks!" << endl;
                    cout << "The " << enemy.id << " attacks you as you try to leave the room and you die." << endl;
                    cout << "Game over!" << endl;
                    exit(0);  // Exit the program
                }
            }
        }
    } else {
        cout << "Command is not an attempt to exit the current room." << endl;
    }
}



MapData Game::loadMapData(const string& mapFileName) {
    ifstream file(mapFileName);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        cerr << "Error: Could not open map file '" << mapFileName << "'. Please make sure the file exists and try again." << endl;
    }
    json jsonData;
    file >> jsonData;

    MapData mapData;

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

