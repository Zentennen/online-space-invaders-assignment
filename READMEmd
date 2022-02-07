#online-space-invaders-assignment

This is an online multiplayer space invaders made for a university assignment.
The following files were provided as part of the assignment and have not been written by me:
charle.hpp
charlie.cc
charlie_application.h
charlie_application.cpp
charlie_gameplay.hpp
charlie_gameplay.cpp
charlie_network.hpp
charlie_gameplay.cc
charlie_network.hpp
charlie_network.cc
charlie_protocol.hpp
charlie_gameplay.cc

The following is the README delivered as part of the university assignment:
PROJECT MEMBERS:
Olof Wikmark

All code is either provided by the teacher or written by me.

USAGE DETAILS:
The MasterServer program needs to be running for the application
to work as intended. If it isn't running the server will keep trying
to register itself at the master ip address and the client will keep
trying to acquire servers from the same address.

Upon running, the server will register at the master server and then
enter a lobby state where it waits until the maximum number of players
have joined. After all the players join, there is a delay before the
game starts of 0.5 seconds (30 ticks). Max players is set in Config.h
using the constexpr MAX_PLAYERS.

The client starts in a server discovery state when ran. Here it will
fetch servers from the master server and display a list of servers
(ip addresses) acquired at the last fetch. When joining a server, the 
client will then enter a lobby state, and proceed to the game state 
when the server sends an EventGameStart message.

CONTROLS:
Space - Connect to the selected server in the servery discovery state
W - Move the server selection up in the server discovery state
S - Move the server selection down in the server discovery state
A - Move left in game
D - Move right in game
S - Shoot in game
