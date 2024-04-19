
# Text Adventure Game

This is a text adventure game developed as part of the CO3105/4105/7105 Assignment. The game is built according to the instructions provided in the assignment document and includes some additional advanced features.

## Game Description

In this game, players interact with the game using text commands. The game environment consists of a collection of rooms, each with a name and a description. Players navigate through these rooms, interact with various objects, and encounter enemies.

## Advanced Features

### 1. Exhaustible and Non-Exhaustible Items

Objects in the game are categorized into two types:

- **Exhaustible Items**: These items are not removed from the inventory after one use. For example, a gun.
- **Non-Exhaustible Items**: These items disappear from the inventory after one use. For example, the two different types of bullets in map2 and the apple in map2.

### 2. Eating an Apple to Gain a Life

In map2, we have incorporated a function which is "eat". If the player picks up the apple and eats it, they gain an extra life. This can save them if they try to escape the room where the enemy is present without killing it or when they actually get killed by the enemy.

### 3. Autocorrect

The game includes an autocorrect feature. If the player has a typo, the program provides a correct suggestion and then executes the suggested command. This feature also neglects extra trailing spaces. For example, "take gun " would work fine even though there is a space after the gun. The command to take the gun will be executed successfully.

## How to Play

1. **Starting the Game**: Run the game executable (e.g., `./main map1.json` or `./main map_name`) to begin the adventure.

2. **Commands**: Enter text commands to interact with the game. Common commands include:
   - `go [direction]`: Move to a different room (e.g., "go north").
   - `look`: Observe the current room and its surroundings.
   - `take [item]`: Pick up an item.
   - `use [item]`: Use an item from your inventory.
   - `inventory` or `inv`: View your inventory.
   - `help`: Display available commands.

3. **Exploration**: Explore the rooms, collect items, and solve puzzles. Be cautious of enemies!

4. **Winning**: The goal is to achieve a specific objective (e.g., finding a treasure, defeating a boss). Good luck!

