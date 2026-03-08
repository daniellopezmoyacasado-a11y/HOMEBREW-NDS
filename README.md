# HOMEBREW-NDS-GAME
This is a Custom Made NintendoDS game.

Currently it's more a proof of concept than a real game, since I have spent most of the time in the last couple of weeks trying to get the game to run on emulators and my own Nintendo DS.

## Running on an Emulator
I discovered the Emulator I was using was giving me issues, so I changed to melonDS 1.1 (I recommend using that one if you plan on playing since I couldn't get any other emulator to run my custom roms).

## Running on a Nintendo DS
The project uses devkitpro & devkitARM to compile C++ code into .nds files, which can be directly uploaded to a hacked nintendo ds cartridge (the ones that had a microSD). I had a M3 DS REAL Cartridge from the early 2000s that runs on M3 Sakura. M3 Sakura, just like the DeSmuMe emulator, doesn't run these custom roms. It gets stuck and shows a blank white screen.

I switched to YSMenu (there's a specific build for M3 cartridges), with it you can kind of overwrite the m3 sakura system, so the DS runs YSmenu instead. From there you can load any roms, included homebrewed ones like the one from this progect. 


