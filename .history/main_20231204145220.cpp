#include "Game.h"
#include <iostream>
#include <string>
using namespace std;

// Function to initialize the game
string initializeGame(int argc, char* argv[]) {
    string mapFileName;

    // Check if a map name was provided
    if (argc > 1) {
        mapFileName = argv[1];
    } else {
        cerr << "Error: No map name provided. Please run the program with a map name, like this: './main MapName'" << endl;
        exit(1);
    }
    mapFileName += ".json";

    return mapFileName;
}

void runGameLoop(Game& game) {
    string userInput;
    while (true) {
        cout << "Enter a command: ";
        getline(cin, userInput);

        if (userInput == "quit" || userInput == "exit") {
            break; // Exit the loop if the user types "quit" or "exit"
        }

        // Convert the user input to lowercase for case-insensitivity
        transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);

        if (userInput.substr(0, 2) == "go") {
            if (userInput.length() > 3) {
                string direction = userInput.substr(3);
                game.go(direction);
            } else {
                cout << "You need to specify a direction to go." << endl;
            }
        } else if (userInput.substr(0, 4) == "pick" != string::npos) {
            if (userInput.length() > 3){
                string objectId = userInput.substr(5); 
                game.pick(objectId);
            } else {
                cout << "You need to specify an item id to pick up." << endl;
            }
        } else if (userInput.find("kill") != string::npos) {
            if (userInput.length() > 3){
                string enemyId = userInput.substr(5); 
                game.kill(enemyId);
            } else {
                cout << "You need to specify an enemy id to kill." << endl;
            }
        } else {
            game.processCommand(userInput);
        }
    
       if (game.isObjectiveComplete()) {
            cout << "Congratulations! You have completed the objective. You win!" << endl;
            break; 
        
       }
    }

}

int main(int argc, char* argv[]) {
    string mapFileName = initializeGame(argc, argv);

    Game game(mapFileName);
    game.startGame();
    runGameLoop(game);

    return 0;
}
