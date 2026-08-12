# Agent Notes for HOMEBREW-NDS

This repository is a homebrewed Nintendo DS game written in C++ with libnds/devkitARM. Treat it as an early game prototype: preserve working NDS hardware/emulator behavior, keep changes small, and avoid desktop-game assumptions.

## Project Shape

- `source/` contains game code. `main.cpp` initializes libnds video/VRAM, loads the active room, initializes OAM, then runs the VBlank-paced game loop.
- `include/` contains game headers and shared constants. `Constants.hpp` currently defines the 16x12 tile map using 16px tiles for a 256x192 NDS screen.
- `gfx/` contains grit-generated C/H assets used by the Makefile build.
- `assets/` keeps source/reference art plus duplicated generated outputs. When changing art, keep `assets/` and `gfx/` intent clear.
- `.old_code/old_main.cpp` is historical Pong prototype code. Do not build new features on it unless the user asks.
- `build/`, `*.elf`, `*.map`, and `*.ds.gba` are generated. `GAMEBUILD1.nds` is intentionally tracked because the toolchain can be hard to reproduce.

## Build and Runtime

- Build with `make` from the repo root.
- The Makefile requires devkitPro/devkitARM environment variables, especially `DEVKITARM` and `LIBNDS`.
- The output ROM is `GAMEBUILD1.nds`.
- README notes melonDS 1.1 as the recommended emulator. DeSmuME and M3 Sakura have shown white-screen issues for this project.
- On hardware, the README notes YSMenu as the working path for an M3 DS Real cartridge.

## NDS Constraints

- The game targets the DS screen size: 256x192 pixels.
- Current rooms use 16x16 logical tiles, so map arrays are `MAP_WIDTH` 16 by `MAP_HEIGHT` 12.
- Keep the main loop VBlank-paced with `swiWaitForVBlank()`, `oamUpdate()`, and `bgUpdate()` in the expected order unless there is a clear reason.
- Respect libnds VRAM bank usage. Current setup uses main BG on VRAM A, main sprites on VRAM B, and sub BG/console on VRAM C.
- Sprites currently use OAM main, 1D sprite mapping, extended 8bpp OBJ palettes, and 16x32 sprites copied frame-by-frame from 64x128 sheets.
- Avoid dynamic allocation-heavy or exception/RTTI-dependent code. The Makefile uses `-fno-rtti -fno-exceptions`.

## Current Gameplay Architecture

- `Room gRoom` is global through `Game.hpp` and owns static and dynamic collision maps.
- `Room::load_room(1)` loads `TestFloor` into BG layer 0 and copies `testRoomCollision`.
- `Player` owns input, tile-locked slide movement, feet-tile collision checks, animation timing, frame upload, and OAM drawing.
- `GameState` tracks gold, day, map position, and progression flags, but `checkProgressUnlocks()` is declared and not currently implemented in tracked code.
- `NPC` is currently a fixed-position idle character using the generated `NPC` sprite sheet. It cycles through `IDLE_DIRECTION_SEQUENCE` in `source/npc.cpp` while staying on the first animation column and registers only its bottom/feet tile into dynamic room collision.

## Coding Guidance

- Prefer simple C++ compatible with devkitARM/libnds over modern desktop-only conveniences.
- Keep new shared constants in `include/Constants.hpp` when they are truly global.
- Keep generated grit asset names consistent with their headers, C symbols, and Makefile discovery in `gfx/`.
- Use 16px tile coordinates consistently when adding collision, rooms, or movement logic. Player movement is tile-locked: input starts a slide to the next tile, and new movement is ignored until the slide completes.
- Keep the player walking animation synced to tile slides: one 16px tile step should advance through all four walk poses. Current movement advances 1px every 2 frames, with animation changing every 8 frames.
- Character collision is currently based on the bottom/feet tile so sprites can visually overlap vertically. For 16x32 character sprites, `x` is the left edge of the feet tile and `y + 16` is the top of the feet tile; keep both aligned to the 16px grid.
- Sprite draw depth is based on feet Y, with the lower character drawn on top.
- Do not rewrite tracked generated graphics files unless the task is specifically about assets.
- Do not remove or replace `GAMEBUILD1.nds` unless explicitly asked; it is tracked intentionally.
- Before changing build behavior, confirm it still works with the devkitPro Makefile pattern already in use.

## Verification

- If devkitARM is installed, run `make` after code changes.
- If the build environment is missing, state that clearly and avoid pretending the ROM was validated.
- For gameplay changes, prefer testing the generated `.nds` in melonDS 1.1 when available.
