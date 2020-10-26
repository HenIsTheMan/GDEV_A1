************
* Controls *
************

- MENU -
Use mouse to select

- GAME (General) -
P - Save game
Y - Toggle debug info
U - Toggle coin spin animation

- GAME (Player) -
VK_LEFT or A - Move left
VK_RIGHT or D - Move right
VK_UP or W or X - Jump (press time proportional to jump height), Double Jump
VK_SHIFT or Z - Sprint
1 - Use item in 1st slot of inventory
2 - Use item in 2nd slot of inventory
3 - Use item in 3rd slot of inventory
4 - Use item in 4th slot of inventory


****************************************
* In-game stuff (objects, items, etc.) *
****************************************

- MAIN -
Dirt0 		(key: 100)
Dirt1 		(key: 101)
Ice 		(key: 102)
Lock 		(key: 103)
Mystery 	(key: 104)
Bomb 		(key: 105)
PrincessLeft 	(key: 106)
PrincessRight 	(key: 107)

Coin0 		(key: 200)
HealthHeart 	(key: 201)
LifeHeart 	(key: 202)
Acid 		(key: 203)
Lava 		(key: 204)
Checkpoint1 	(key: 205)
Checkpoint2 	(key: 206)
Medkit 		(key: 207)
BlackPotion 	(key: 208)
BluePotion 	(key: 209)
GreenPotion 	(key: 210)
OrangePotion 	(key: 211)
PurplePotion 	(key: 212)
RedPotion 	(key: 213)
Key 		(key: 214)

- MISC -
Player (with animation)
Enemies (with animation)
Background
Inventory (2D)


**************
* Stuff Done *
**************
- Made player inherit from Entity
- Create function templates for common functions between player class and enemy class
- Revamped Entity Factory for better management of entities
- Allowed for multiple enemies to be in a 2D map
- Ensured pixel-perfect collisions with all in-game stuff
- Cleared all memory leaks :)
- Added debug info (FPS and lvl no., can be toggled)
- Added animations to enemies and coins (can be toggled)
- Allowed for multiple text renderers with multiple fonts
- Added in-game music
- Improved Finite State Machine (FSM) of enemy class (made it more consistent)
- Improved the save system (save multiple levels, save player and enemies on top of other in-game objs, saving at checkpoints, pressing button to save)
- Added real-time loop control to limit left and right movement speed of player
- Added limits to jump vel and fall vel of player
- Organised in-game stuff into certain ranges of keys