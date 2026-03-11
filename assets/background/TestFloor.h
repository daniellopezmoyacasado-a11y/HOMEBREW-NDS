
//{{BLOCK(TestFloor)

//======================================================================
//
//	TestFloor, 256x192@8, 
//	+ palette 256 entries, not compressed
//	+ 43 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 2752 + 1536 = 4800
//
//	Time-stamp: 2026-03-11, 21:34:46
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TESTFLOOR_H
#define GRIT_TESTFLOOR_H

#define TestFloorTilesLen 2752
extern const unsigned int TestFloorTiles[688];

#define TestFloorMapLen 1536
extern const unsigned short TestFloorMap[768];

#define TestFloorPalLen 512
extern const unsigned short TestFloorPal[256];

#endif // GRIT_TESTFLOOR_H

//}}BLOCK(TestFloor)
