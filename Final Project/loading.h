
//{{BLOCK(loading)

//======================================================================
//
//	loading, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 28 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 896 + 2048 = 3456
//
//	Time-stamp: 2022-04-27, 23:51:26
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_LOADING_H
#define GRIT_LOADING_H

#define loadingTilesLen 896
extern const unsigned short loadingTiles[448];

#define loadingMapLen 2048
extern const unsigned short loadingMap[1024];

#define loadingPalLen 512
extern const unsigned short loadingPal[256];

#endif // GRIT_LOADING_H

//}}BLOCK(loading)
