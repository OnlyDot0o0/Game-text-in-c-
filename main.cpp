#include "Game.h"
#include <iostream>
#include <string>
#include <sstream>
// Function to calculate Levenshtein distance between two strings
int levenshteinDistance(const std::string &s1, const std::string &s2)
{
    int m = s1.length();
    int n = s2.length();

    // Create a matrix to store distances
    int dp[m + 1][n + 1];

    // Initialize the matrix
    for (int i = 0; i <= m; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            if (i == 0)
            {
                dp[i][j] = j;
            }
            else if (j == 0)
            {
                dp[i][j] = i;
            }
            else
            {
                dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1)});
            }
        }
    }

    return dp[m][n];
}

// Function to correct only the first word of a command based on a list of valid commands
std::string correctCommand(const std::string &input, const std::vector<std::string> &validCommands)
{
    // Split the input into words
    std::istringstream iss(input);
    std::string firstWord;
    iss >> firstWord;

    // Check if the first word is a valid command
    if (std::find(validCommands.begin(), validCommands.end(), firstWord) != validCommands.end())
    {
        // If it is, return the original command
        return input;
    }

    // Find the closest matching valid command for the first word
    std::string correctedFirstWord = firstWord;
    int minDistance = std::numeric_limits<int>::max();

    for (const auto &validCommand : validCommands)
    {
        int distance = levenshteinDistance(firstWord, validCommand);
        if (distance < minDistance)
        {
            minDistance = distance;
            correctedFirstWord = validCommand;
        }
    }

    // Reconstruct the corrected command with the rest of the input
    std::string correctedCommand = correctedFirstWord + iss.str().substr(firstWord.length());

    return correctedCommand;
}

// Function to initialize the game
std::string initializeGame(int argc, char *argv[])
{
    std::string mapFileName;

    // Check if a map name was provided
    if (argc > 1)
    {
        mapFileName = argv[1];
    }
    else
    {
        std::cerr << "Error: No map name provided. Please run the program with a map name, like this: './main MapName'" << std::endl;
        exit(1);
    }

    return mapFileName;
}

bool shouldConfirm(const std::string &command)
{
    // List of commands that require confirmation
    std::vector<std::string> commandsRequiringConfirmation = {"quit", "destroy", "delete"};

    // Check if the command is in the list of commands requiring confirmation
    if (std::find(commandsRequiringConfirmation.begin(), commandsRequiringConfirmation.end(), command) !=
        commandsRequiringConfirmation.end())
    {
        // Ask for confirmation
        std::string confirmation;
        std::cout << "Do you want to confirm? (yes/no): ";
        std::getline(std::cin, confirmation);

        // Return true only if the user confirms with "yes"
        return (confirmation == "yes");
    }

    // Return false for commands that don't require confirmation
    return false;
}
// Function to run the game loop
void runGameLoop(Game &game, const std::vector<std::string> &validCommands)
{
    std::string userInput;
    while (true)
    {
        std::cout << "\nEnter a command: ";
        std::getline(std::cin, userInput);
        std::cout << std::endl;

        if (userInput == "quit")
        {
            if (shouldConfirm(userInput))
            {
                // Handle confirmation
                std::cout << "Confirmed! Exiting the game." << std::endl;
                break;
            }
            else
            {
                // User canceled the confirmation
                std::cout << "Canceled quitting." << std::endl;
            }
        }
        else
        {
            // Correct only the first word of the command
            std::string correctedCommand = correctCommand(userInput, validCommands);

            // Process the corrected command only if it's different from the original command
            if (correctedCommand != userInput)
            {
                std::cout << "Did you mean: " << correctedCommand << std::endl;
            }

            game.processCommand(correctedCommand);

            if (game.isObjectiveComplete())
            {
                std::cout << "Congratulations! You have completed the objective. You win!" << std::endl;
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    std::string mapFileName = initializeGame(argc, argv);

    Game game(mapFileName);
    game.startGame();

    std::vector<std::string> validCommands = {"look", "go", "take", "kill", "list", "list items", "eat", "quit"};

    // Run the game loop
    runGameLoop(game, validCommands);

    return 0;
}
