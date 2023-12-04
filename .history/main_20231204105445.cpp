#include "Game.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
    Game game("map1.json");
    game.startGame();

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
            string direction = userInput.substr(3);
            game.go(direction);
        } else if (userInput.find("pick") != string::npos) {
            string objectId = userInput.substr(5); 
            game.pick(objectId);
        } else if (userInput.find("kill") != string::npos) {
            string enemyId = userInput.substr(5); 
            game.kill(enemyId);
        } else {
            game.processCommand(userInput);
        }
    
       if (game.isObjectiveComplete()) {
            cout << "Congratulations! You have completed the objective. You win!" << endl;
            break; 
        
       }
    }

    return 0;
}
