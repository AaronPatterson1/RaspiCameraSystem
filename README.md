# Team14Repo - Pi Motion Camera

## Compile Instructions
```
Pi - Server
 g++ -Wno-write-strings -std=c++0x M1Server.cpp PktDef.cpp -lpthread
```

## Running program instructions
```
1. Start the server on the pi
2. Launch client GUI
3. Hit the init button in the bottom right corner to start data com connection
4. Enter a command in the top bar in the client GUI
5. Hit send besides the input buffer to send the packet

Packet options :
0 - Allow you to enter a command or script to be run
1 - Turn off server and close client side connection once sleep packet is recieved
2 - Ack packet (command is used for debugging ack packets are sent by default from either the server or client when appropriate)

Extras :
  - Hitting the save settings will send the currently selected settings values to the pi
  - Packet info will be display under the input buffer
  - Video files will be displayed in the Right display window besides settings
```

## Files
```
 Server
  - M1Server.cpp
  - PktDef.cpp
  - PktDef.h
  - pi_info.py
  - vid_files.py
  - cpu_info.py
  - hello_world.py (test file not used outside debugging)
 Client
  - M1Client.cpp
  - Session.cpp
  - Session.h
  - PktDef.cpp
  - PktDef.h
```

## Goals
 - Constant update of vids on pi
 - Add effects to camera
 - Change Settings
 - Clean up GUI
 - Get motion sensor
 - Display sensitivity in client GUI
