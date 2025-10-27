//=============================================================================
// INCLUDES
//=============================================================================
#include "msxgl.h"
//#include "debug.h"
#include "vdp_inl.h"
#include "vdp.h"
#include "memory.h"
#include "string.h"
#include "compress/zx0.h"
#include "vgm/lvgm_player.h"
#include "psg.h"
#if (VGM_USE_SCC)
#include "scc.h"
#endif
#if (VGM_USE_MSXMUSIC)
#include "msx-music.h"
#endif
#if (VGM_USE_MSXAUDIO)
#include "msx-audio.h"
#endif

#include "s_vgm_rawdef.h"
#include "ayfx/ayfx_player.h"

// SFX bank (sound pack)
#include "content/ayfx/ayfx_bank.h"

// Fonts
//#include "gfx/vram.h"

// sprites
#include "nave.h"

// Defines
#define CHAR_FRAMES     8
#define BACK_BUFFER     768


#define TILEW           16
#define TILEH           16

#define MAX_SP_LOG      52
#define MAX_SP_PHYS     32
#define MAX_NOFLICK     12
#define MAX_SHOOTS      MAX_SP_LOG-MAX_NOFLICK

#define SWIDTH          256
#define SHEIGHT         212

#define SCORE_HEIGHT    20
#define CHAR_OFFY       SCORE_HEIGHT+2
#define TILE_OFFY       CHAR_OFFY+16
#define SPLIT_SCORE     (SCORE_HEIGHT-6)
#define SPLIT_TELA     (SHEIGHT-6)

// VGM music entry
struct MusicEntry
{
	const c8* Name;
	const u8* Data;
	u8        Segment;
};

typedef struct SoundEntry{
    u8  bank;
    u8  play;
}SoundEntry;

extern const unsigned char vram1[];
extern const unsigned char vram2[];
extern const unsigned char vram3[];
extern const unsigned int vram1_len;
extern const unsigned int vram2_len;
extern const unsigned int vram3_len;
extern const unsigned char vram_pal;

volatile u8 g_SegIdx;
volatile u8 g_SegLoop;

volatile u8 g_CurrentMusic = 0;
volatile u8 isMusicSet=FALSE, isMusicChangeBank=FALSE, isMusicLoop=FALSE;
volatile u8 soundPlay = FALSE;

const u8 map[1024] =
{
    1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,3,0,0,0,
    0,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,3,3,3,3,0,0,0,3,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,3,3,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,0,3,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,0,3,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,0,3,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,0,3,0,0,0,0,0,
    0,0,0,3,3,3,3,0,0,3,3,3,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0,
    0,0,0,3,0,0,3,0,0,0,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,0,0,0,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,3,3,3,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,0,0,0,0,
    0,0,0,3,0,0,3,0,0,3,0,0,0,0,0,0,
    0,0,0,3,3,3,3,0,0,3,3,3,3,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,
    4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,
    0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,2,0,0,0,0,0,0,0,2,2,0,0,0,
    0,0,0,2,0,0,0,0,0,0,0,2,0,2,0,0,
    0,0,0,2,0,0,0,2,2,2,0,2,0,2,0,0,
    0,0,0,2,0,0,0,2,0,2,0,2,0,2,0,0,
    0,0,0,2,0,0,0,2,0,2,0,2,0,2,0,0,   //48
    0,0,0,2,2,2,0,2,2,2,0,2,2,0,0,0,   //49
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //50
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //51
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,   //52
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   //53
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,   //54
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //55
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,   //56
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //57
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,   //58
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,   //59
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,   //60
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //61
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,   //62
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0    //63
};

const u8    mapHeight=64;           // How many lines the map has
volatile u8 mapLine=64-16;   // Where the map starts drawing
volatile u8 offsetScroll=240;       // Line to start drawing the next 16 pixels for scrolling
volatile u8 scrollCount=0;          // How many pixels were elapsed, should draw one tile each frame, from lef to right, at offsetScroll
/*{
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};*/

//=============================================================================
// MEMORY DATA
//=============================================================================

// V-synch variables
volatile u8 g_VBlank = 1;
volatile u16 g_Frame = 0;
volatile u16 vblanks = 0;
volatile u8 hphase=0;

// The index to pages used - it's the VDP line index, not the page number
u8  animate = TRUE;
bool keyPressed=FALSE;

typedef struct Character{
    // Logical values
    u8  index;      // The sprite index
    u8  color;      // The sprite color index
    i16 x;          // Current x position (logical)
    i16 y;          // current y position (logical)
    i16 dx;         // Pixel displacement on x axis
    i16 dy;         // Pixel displacement on y axis
    u8  frame;      // The current frame
    u8  counter;    // generic counter (animation loop, etc)
}Character;

typedef struct SprAttr{
    u8  y;
    u8  x;
    u8  pattern;
    u8 unused;
}SprAttr;

/*typedef struct Movement{
    u8  size;
    u8  *x;
    u8  *y;
}Movement;*/

//const u8 move1a[] = {0,6,12,18,24,30,36,42,48,54,60,66,60,54,48,42,36,30,24,18,12,6};
//const u8 move1b[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*Movement movesList[]={
    { 22, move1a, move1b }
};*/

typedef struct SprAttrTable{
    u8          colors[512];
    SprAttr     attributes[32];
}SprAttrTable;

volatile SprAttrTable   tableSprites[2];

// Sprite atributes at line 212: b110101000000000 (27136)
// Sprite atributes at line 220: b110111000000000 (28160)
// Sprite atributes at line 228: b111001000000000 (29184)
// Sprite atributes at line 236: b111011000000000 (30208)

// Lower bits for the addresses of the color and attributes tables. The high bits are always 0
const u16   spriteAttributeLow[4]={ 27136, 28160, 29184, 30208 };
// Initially the sprite pbackbuffer starts at index 2
volatile u8 spriteAttributeLowDisplay=0, spriteAttributeLowBack=2, spriteAttributeLowTemp;

// Sprite colors at line 208: 26624
// Sprite colors at line 216: 27648
// Sprite colors at line 224: 28672
// Sprite colors at line 232: 29696

// Lower bits for the addresses of the color and attributes tables. The high bits are always 0
const u16   spriteColorsLow[4]={ 26624, 27648, 28672, 29696 };

// The value for register 5 for the four different addresses of the sprites attributes + colors. R11 is always 0
const u8    attributesTableR5[4]={0b11010111, 0b11011111, 0b11100111, 0b11101111 };
volatile u8    *attributesTableDisplay=attributesTableR5, *attributesTableBack=&attributesTableR5[2], *attributesTableTemp;

volatile u8     framecount=0, swapSprites=FALSE;
// Uses the same structure as before, even if we don't really use blobs, but sprites
volatile Character noflick[MAX_NOFLICK];
volatile Character shoots[MAX_SHOOTS];

// The colors for the sprites
const u8        colors[3][16] ={
        { 9, 9, 9, 9,14,14,14,14,14,14,14,14, 9, 9, 9, 9},
        {13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
        {11,10,10,13,13,14,14,14,14,14,14,13,13,10,10,11}
};
// Input variables
volatile u8  offset = 0, offsetDisplay= 0;
u16 posx = 0;
volatile u8 xtemp, ytemp, pattemp;

// Temporary variables
u16 utemp16;
i16 itemp16, spCount;
u8  utemp8;
i8  itemp8;
i16 heighta, heightb;
u16 vectemp[8];
u8  ctext;
char buffer[32];
u8 row8, prevRow8=0xFF;

// The buffer for unpacking data
volatile u8 decompress[5120];

// Characters
i8 numSprites=0, numNoFlick=0, numShoots=0, numSpritesDisplay=0;

// Controls the frame delay to adjust FPS
u8  dframe=0;
i8  dmode=0,cdindex=0;
const u8  delays[6][3]={{0,0,60}, {1,1,30}, {1,2,25}, {2,2,20}, {3,2,15}, {3,3,10}};

// For PAL 30: 2,2,1
// For NTSC 30: 2,2,2

const struct MusicEntry g_MusicEntry[] =
{
	{ "Galious             ", 0xA000 + HONOTORI_LVM_REL , 6}, //HONOTORI_LVM_SEG },
	{ "Undeadline          ", 0xA000 + UNDEADLINE_LVM_REL  , 7}//UNDEADLINE_LVM_SEG }
};

SoundEntry  sound = { 0, FALSE};

//=============================================================================
// FUNCTIONS
//=============================================================================

inline void SoundPlay(u8 bank){
    sound.bank=bank;
    sound.play=TRUE;
}

//-----------------------------------------------------------------------NOFLICK_SP------
// VBlank interrupt
void VDP_InterruptHandler()
{
    VDP_SetPage(0);
    VDP_SetVerticalOffset(0);
    VDP_SetHBlankLine(SPLIT_SCORE);
    VDP_DisableSprite();
    hphase=0;
}

void VDP_HBlankHandler()
{
    if(hphase==0){
        VDP_SetPage(1);
        VDP_SetVerticalOffset(offsetDisplay);
        VDP_EnableSprite(TRUE);
        VDP_SetHBlankLine(SPLIT_TELA+offsetDisplay);
        if(swapSprites){
            attributesTableTemp=attributesTableDisplay;
            attributesTableDisplay=attributesTableBack;
            attributesTableBack=attributesTableTemp;
            swapSprites=FALSE;
        }
        VDP_RegWrite(5, attributesTableDisplay[framecount%2]);
        //VDP_RegWrite(11, 0);
        ++framecount;
        hphase=1;
        if(sound.play){
            ayFX_PlayBank(sound.bank, 0);
            sound.play=FALSE;
        }
        if(isMusicSet)
            LVGM_Stop();
        LVGM_Decode();

        ayFX_Update();
        PSG_Apply();
        if(isMusicSet){
            SET_BANK_SEGMENT(3, g_MusicEntry[g_CurrentMusic].Segment);
            LVGM_Play((const void*)0xA000, TRUE);
            isMusicSet=FALSE;
        }
    }else if(hphase==1){
        hphase=2;
        ++dframe;
        ++vblanks;
        if(dframe>delays[dmode][cdindex]){
            g_VBlank = 1;
            dframe=0;
            cdindex=(cdindex+1)%2;
        }
    }
}

void soundEnd(void){
    soundPlay=FALSE;
}

//-----------------------------------------------------------------------------
// Wait for V-Blank period
void WaitVBlank()
{

	while(g_VBlank == 0) {
	}
	g_VBlank = 0;
	g_Frame++;
}

bool MusicNotification(u8 id)
{
	switch(id)
	{
	case 0: // End of data segment
		g_SegIdx++;
		SET_BANK_SEGMENT(3, g_SegIdx);
		LVGM_SetPointer((const u8*)0xA000); // Set playback address to the start of the next segment
		//isMusicChangeBank=TRUE;
		//VDP_SetColor(11);  // Yellow
		return FALSE;

	case 0xFE: // Reach loop marker
		g_SegLoop = g_SegIdx; // Backup the segment that contain the loop marker
		//VDP_SetColor(12); // Green
		break;

	case 0xFF: // Jump to loop marker
        g_SegIdx = g_SegLoop; // Restore the segment that contain the loop marker
		SET_BANK_SEGMENT(3, g_SegIdx);
		//VDP_SetColor(4);
	}

	return TRUE;
}


inline void drawText(const char *text, u16 x, u16 y){
    for(ctext=0;ctext<String_Length(text);ctext++){
        VDP_CommandHMMM((text[ctext]-32)%32*8,((text[ctext]-32)/32)*8+CHAR_OFFY, x+ctext*8,y, 8,8);
    }
}

//-----------------------------------------------------------------------------
// H_KEYI interrupt hook
/*void InterruptHook()
{
	__asm
		// Get S#1
		ld		a, #1
		out		(P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(P_VDP_ADDR), a
		in		a, (P_VDP_STAT)
		//  Call H-Blank if bit #0 of S#1 is set
		rrca
		jp		nc, _no_hblank
		call	_VDP_HBlankHandler // call to C function HBlankHook()
	_no_hblank:
		// Reset R#15 to S#0
		xor		a
		out		(P_VDP_ADDR), a
		ld		a, #(0x80 + 15)
		out		(P_VDP_ADDR),a
	__endasm;

	// Call((u16)HookBackup_KEYI);
}*/

/*void SetMusic(u8 idx)
{
    g_CurrentMusic = idx;
	VGM_Decode();
	#if (PSG_ACCESS == PSG_INDIRECT)
        PSG_Apply();
	#endif
	SET_BANK_SEGMENT(3, g_MusicEntry[idx].Segment);
	VGM_Play(g_MusicEntry[idx].Data, TRUE);
}*/

void SetMusic(u8 idx)
{
    /*LVGM_Stop();
    LVGM_Decode();
    #if (PSG_ACCESS == PSG_INDIRECT)
        PSG_Apply();
    #endif*/
	// Init mapper
    g_CurrentMusic = idx;
	g_SegIdx = g_MusicEntry[idx].Segment;
	g_SegLoop = g_SegIdx;
    isMusicSet=TRUE;
	//SET_BANK_SEGMENT(3, g_MusicEntry[idx].Segment);
	//bool ok = LVGM_Play((const void*)0xA000, TRUE);
}


//-----------------------------------------------------------------------------
// Programme entry point
void main()
{
/*	DEBUG_INIT();
	DEBUG_LOG("Start debug session!");
	DEBUG_PRINT("Print: %c %s %i %d %u %f %x %X %%", '*', "Example game", -1234, -1234, -1, -0.123, -1, 0xABCD);
	DEBUG_PRINT("Align: [%04i] [% 4d] [%-4u] [%+i] [%04X] [%016b]\n\n", 123, 123, 123, 123, 123, 1234);*/

	// Initialize VDP
	VDP_Initialize();
	VDP_SetMode(VDP_MODE_SCREEN5);
	VDP_SetColor(0);
	VDP_ClearVRAM();
	VDP_SetSpriteFlag(VDP_SPRITE_SIZE_16);

    SET_BANK_SEGMENT(3, 3);
    VDP_SetPalette((const u8*)(0xA000+GFX_PALETTE_BIN_REL));
    //Mem_Copy(vram1, compressed, vram1_len);
    ZX0_UnpackToRAM((const u8*)(0xA000+GFX_XAA_ZX0_REL), decompress);
    VDP_CommandHMMC(decompress,0, CHAR_OFFY, 256, 32);
    //Mem_Copy(vram2, compressed, vram2_len);
    ZX0_UnpackToRAM((const u8*)(0xA000+GFX_XAB_ZX0_REL), decompress);
    VDP_CommandHMMC(decompress,0, CHAR_OFFY+32, 256, 32);
    //Mem_Copy(vram3, compressed, vram3_len);
    ZX0_UnpackToRAM((const u8*)(0xA000+GFX_XAC_ZX0_REL), decompress);
    VDP_CommandHMMC(decompress,0, CHAR_OFFY+64, 256, 16);

    //VDP_CommandHMMC(vram,0, CHAR_OFFY, 256, 80);
    //VDP_CommandWait();
    // Sets sprites attriobutes + colors starting at line 240
    VDP_RegWrite(5, attributesTableDisplay[0]);
    VDP_RegWrite(11, 0);

    // Sets sprites pattern tables at line 240 (111100000000000)
    VDP_RegWrite(6, 0b00001111);
    VDP_EnableSprite(TRUE);
    VDP_LoadSpritePattern(nave,0, 96);
    // Draws the first display buffer based on map(page 1)
    VDP_SetPage(1);
    for(u16 l = 0; l < 16; l++)
        for(u16 c = 0; c < 16; c++){
            VDP_CommandHMMM((map[(l+mapLine)*16+c]%16)*16,(map[(l+mapLine)*16+c]/16)*16+TILE_OFFY,
                            c*16,l*16+256,
                            16,16);
        }
    // Draws the next tileset line
    --mapLine;
    for(u16 c = 0; c < 16; c++){
        VDP_CommandHMMM((map[mapLine*16+c]%16)*16,(map[mapLine*16+c]/16)*16+TILE_OFFY,
                            c*16,offsetScroll+256,
                            16,16);
    }
    //Prepares for the next line in the map
    --mapLine;
    offsetScroll-=16;
    // Draw a palette view on page 0

    // Load hardware sprites
/*    DEBUG_PRINT("Sprite attribute table: %X, %X\n", g_SpriteAttributeLow, g_SpriteAttributeHigh);
    DEBUG_PRINT("Sprite pattern table: %X, %X\n",g_SpritePatternLow, g_SpritePatternHigh);
    DEBUG_PRINT("Sprite color table: %X, %X\n", g_SpriteColorLow, g_SpriteColorHigh);
    DEBUG_PRINT("VDP Sprite variable: %X\n", sizeof(g_VDP_Sprite));*/


    Mem_Set(0, noflick, sizeof(Character)*MAX_NOFLICK);
    for(utemp8=0;utemp8<MAX_NOFLICK;utemp8++){
        noflick[utemp8].frame=0;
        noflick[utemp8].counter=0;
        noflick[utemp8].color=0;
        noflick[utemp8].x=(utemp8%6)*42;
        noflick[utemp8].y=(utemp8/6)*20 ;
        noflick[utemp8].index = 0;
        switch(utemp8%4){
            case    0:{
                noflick[utemp8].dx=1;
                noflick[utemp8].dy=1;
                break;
                }
            case    1:{
                noflick[utemp8].dx=1;
                noflick[utemp8].dy=-1;
                break;
                }
            case    2:{
                noflick[utemp8].dx=1;
                noflick[utemp8].dy=-1;
                break;
                }
            case    3:{
                noflick[utemp8].dx=-1;
                noflick[utemp8].dy=-1;
                break;
                }
        }
    }
    Mem_Set(0, shoots, sizeof(Character)*MAX_SHOOTS);
    for(utemp8=0;utemp8<MAX_SHOOTS;utemp8++){
        shoots[utemp8].frame=0;
        shoots[utemp8].counter=0;
        shoots[utemp8].color=1;
        shoots[utemp8].x=(utemp8%8)*32;
        shoots[utemp8].y=40+(utemp8/5)*30 ;
        shoots[utemp8].index = 32;
        switch(utemp8%4){
            case    0:{
                shoots[utemp8].dx=1;
                shoots[utemp8].dy=1;
                break;
                }
            case    1:{
                shoots[utemp8].dx=1;
                shoots[utemp8].dy=-1;
                break;
                }
            case    2:{
                shoots[utemp8].dx=1;
                shoots[utemp8].dy=-1;
                break;
                }
            case    3:{
                shoots[utemp8].dx=-1;
                shoots[utemp8].dy=-1;
                break;
                }
        }
    }
    // Resets the sprite table
    Mem_Set(0, tableSprites, sizeof(SprAttrTable)*2);
    for(utemp8=0;utemp8<MAX_SP_PHYS*2;utemp8++)
        tableSprites[utemp8/MAX_SP_PHYS].attributes[utemp8%MAX_SP_PHYS].y=213;
    for(utemp8=0;utemp8<MAX_NOFLICK;utemp8++)
        Mem_Copy(&colors[0], &tableSprites[0].colors[utemp8*16], 16);
    for(utemp8=0;utemp8<MAX_SHOOTS/2;utemp8++)
        Mem_Copy(&colors[1], &tableSprites[0].colors[(utemp8+MAX_NOFLICK)*16], 16);
    Mem_Copy(&tableSprites[0].colors, &tableSprites[1].colors, 512);
    // There are four sprites colors+attributes tables in trhe VRAM, initializes all of them

    for(utemp8=0;utemp8<4;++utemp8)
        VDP_WriteVRAM((u8*)&tableSprites[0], spriteColorsLow[utemp8], 0, 640);

	// Initialize audio chip
	LVGM_SetNotifyCallback(MusicNotification);
	#if (LVGM_USE_SCC)
	SCC_Initialize();
	#endif
	#if (LVGM_USE_MSXMUSIC)
	MSXMusic_Initialize();
	#endif
	#if (LVGM_USE_MSXAUDIO)
	MSXAudio_Initialize();
	#endif
    //PT3_Init();
	ayFX_InitBank(g_ayfx_bank);
	ayFX_SetChannel(PSG_CHANNEL_C);
	ayFX_SetMode(AYFX_MODE_FIXED);
	//ayFX_SetFinishCB(soundEnd);

	bool bContinue = TRUE;
	offset=0;
	String_Format(buffer, "SP: %02d(%02d,%02d)", numSprites, numNoFlick, numShoots);
	drawText(buffer,0,1);
	String_Format(buffer, "FPS: --/%02d", delays[dmode][2]);
	drawText(buffer,128,1);

	// Decode VGM header
	g_CurrentMusic=0;
	SetMusic(g_CurrentMusic);

	Bios_SetKeyClick(FALSE);
	VDP_EnableHBlank(TRUE);
	VDP_SetHBlankLine(SPLIT_SCORE);
//	Bios_SetHookCallback(H_KEYI, InterruptHook);
	VDP_EnableVBlank(TRUE);
//	Bios_SetHookCallback(H_TIMI, VDP_InterruptHandler);
	drawText("LINE 2",8,11);

    VDP_SetColor(0);
	while(bContinue)
	{

        WaitVBlank();
        if(vblanks>=60){
            vblanks=0;
            String_Format(buffer, "FPS: %02d/%02d", g_Frame, delays[dmode][2]);
            g_Frame=0;
            drawText(buffer,128,1);
        }
        row8 = Keyboard_Read(8);
		if(IS_KEY_PRESSED(row8, KEY_UP) && !IS_KEY_PRESSED(prevRow8, KEY_UP)){
            numSprites++;
            if(numSprites>MAX_SP_LOG)
                numSprites=MAX_SP_LOG;
            else{
                if(numSprites<=MAX_NOFLICK){
                    numNoFlick=numSprites;
                    numShoots=0;
                }else if(numSprites>MAX_NOFLICK){
                    numNoFlick=MAX_NOFLICK;
                    numShoots=numSprites-MAX_NOFLICK;
                }
                String_Format(buffer, "SP: %02d(%02d,%02d)", numSprites, numNoFlick, numShoots);
                drawText(buffer,0,1);
            }
            SoundPlay(0);
            //soundPlay=TRUE;
        }
		else if(IS_KEY_PRESSED(row8, KEY_DOWN) && !IS_KEY_PRESSED(prevRow8, KEY_DOWN)){
            numSprites--;
            if(numSprites<0)
                numSprites=0;
            else{
                if(numSprites<=MAX_NOFLICK){
                    numNoFlick=numSprites;
                    numShoots=0;
                }else if(numSprites>MAX_NOFLICK){
                    numNoFlick=MAX_NOFLICK;
                    numShoots=numSprites-MAX_NOFLICK;
                }
                String_Format(buffer, "SP: %02d(%02d,%02d)", numSprites, numNoFlick, numShoots);
                drawText(buffer,0,1);
            }
            SoundPlay(1);
            //soundPlay=TRUE;
        }
		else if(IS_KEY_PRESSED(row8, KEY_LEFT) && !IS_KEY_PRESSED(prevRow8, KEY_LEFT)){
            dmode--;
            if(dmode<0)
                dmode=0;
            String_Format(buffer, "FPS: --/%02d", delays[dmode][2]);
            drawText(buffer,128,1);
            vblanks=0;
            g_Frame=0;
            SoundPlay(2);
        }
		else if(IS_KEY_PRESSED(row8, KEY_RIGHT) && !IS_KEY_PRESSED(prevRow8, KEY_RIGHT)){
            dmode++;
            if(dmode>=6)
                dmode=5;
            String_Format(buffer, "FPS: --/%02d", delays[dmode][2]);
            drawText(buffer,128,1);
            vblanks=0;
            g_Frame=0;
            SoundPlay(3);
        }else if(IS_KEY_PRESSED(row8, KEY_SPACE) && !IS_KEY_PRESSED(prevRow8, KEY_SPACE)){
            g_CurrentMusic=(g_CurrentMusic+1)%2;
            SetMusic(g_CurrentMusic);
            SoundPlay(4);
        }
        // Updates the non flickering sprites (characters)
        for(utemp8=0;utemp8<numNoFlick;utemp8++){
            noflick[utemp8].x+=noflick[utemp8].dx;
            if(noflick[utemp8].x<0){
                noflick[utemp8].x=0;
                noflick[utemp8].dx=1;
            }else if(noflick[utemp8].x>SWIDTH-16){
                noflick[utemp8].x=SWIDTH-16;
                noflick[utemp8].dx=-1;
            }
            tableSprites[0].attributes[utemp8].x=noflick[utemp8].x;
            noflick[utemp8].y+=noflick[utemp8].dy;
            if(noflick[utemp8].y<16){
                noflick[utemp8].y=16;
                noflick[utemp8].dy=1;
            }else if(noflick[utemp8].y>SHEIGHT-16){
                noflick[utemp8].y=SHEIGHT-16;
                noflick[utemp8].dy=-1;
            }
            if(noflick[utemp8].y+offset!=216)
                tableSprites[0].attributes[utemp8].y=noflick[utemp8].y+offset;
            else
                tableSprites[0].attributes[utemp8].y=215;
            noflick[utemp8].counter++;
            if(noflick[utemp8].counter==8){
               noflick[utemp8].counter=0;
               noflick[utemp8].frame=(noflick[utemp8].frame+1)%8;
            }
            tableSprites[0].attributes[utemp8].pattern=noflick[utemp8].index+noflick[utemp8].frame*4;
            //Mem_Copy(&colors[noflick[utemp8].color], &backSp[0].colors[utemp8*16], 16);
        }
        for(utemp8=numNoFlick;utemp8<MAX_NOFLICK;utemp8++){
            tableSprites[0].attributes[utemp8].y=213+offset;
        }
        // Copies the sprites to the second table
        Mem_Copy(tableSprites[0].attributes, tableSprites[1].attributes, sizeof(SprAttr)*MAX_NOFLICK);
        // Updates the no shoots (may flick)
        spCount=(numShoots>20) ? MAX_SP_PHYS-MAX_NOFLICK : numShoots;
        for(utemp8=0;utemp8<spCount;utemp8++){
            shoots[utemp8].x+=shoots[utemp8].dx;
            if(shoots[utemp8].x<0){
                shoots[utemp8].x=0;
                shoots[utemp8].dx=1;
            }else if(shoots[utemp8].x>SWIDTH-16){
                shoots[utemp8].x=SWIDTH-16;
                shoots[utemp8].dx=-1;
            }
            tableSprites[0].attributes[utemp8+MAX_NOFLICK].x=shoots[utemp8].x;
            shoots[utemp8].y+=shoots[utemp8].dy;
            if(shoots[utemp8].y<16){
                shoots[utemp8].y=16;
                shoots[utemp8].dy=1;
            }else if(shoots[utemp8].y>SHEIGHT-16){
                shoots[utemp8].y=SHEIGHT-16;
                shoots[utemp8].dy=-1;
            }
            if(shoots[utemp8].y+offset!=216)
                tableSprites[0].attributes[utemp8+MAX_NOFLICK].y=shoots[utemp8].y+offset;
            else
                tableSprites[0].attributes[utemp8+MAX_NOFLICK].y=215;
            shoots[utemp8].counter++;
            if(shoots[utemp8].counter==8){
               shoots[utemp8].counter=0;
               shoots[utemp8].frame=(shoots[utemp8].frame+1)%8;
            }
            tableSprites[0].attributes[utemp8+MAX_NOFLICK].pattern=shoots[utemp8].index+shoots[utemp8].frame*4;
        }
        for(utemp8=numShoots+MAX_NOFLICK;utemp8<MAX_SP_PHYS;utemp8++)
            tableSprites[0].attributes[utemp8].y=213+offset;
        // If we have more shoots than we can display, add them to the second frame buffer
        if(numSprites>MAX_SP_PHYS)
        {
            for(utemp8=20;utemp8<numShoots;utemp8++){
                shoots[utemp8].x+=shoots[utemp8].dx;
                if(shoots[utemp8].x<0){
                    shoots[utemp8].x=0;
                    shoots[utemp8].dx=1;
                }else if(shoots[utemp8].x>SWIDTH-16){
                    shoots[utemp8].x=SWIDTH-16;
                    shoots[utemp8].dx=-1;
                }
                tableSprites[1].attributes[utemp8-20+MAX_NOFLICK].x=shoots[utemp8].x;
                shoots[utemp8].y+=shoots[utemp8].dy;
                if(shoots[utemp8].y<16){
                    shoots[utemp8].y=16;
                    shoots[utemp8].dy=1;
                }else if(shoots[utemp8].y>SHEIGHT-16){
                    shoots[utemp8].y=SHEIGHT-16;
                    shoots[utemp8].dy=-1;
                }
                tableSprites[1].attributes[utemp8-20+MAX_NOFLICK].y=shoots[utemp8].y+offset;
                shoots[utemp8].counter++;
                if(shoots[utemp8].counter==8){
                   shoots[utemp8].counter=0;
                   shoots[utemp8].frame=(shoots[utemp8].frame+1)%8;
                }
                tableSprites[1].attributes[utemp8-20+MAX_NOFLICK].pattern=shoots[utemp8].index+shoots[utemp8].frame*4;
            }
            for(utemp8=MAX_NOFLICK+(numShoots-20);utemp8<MAX_SP_PHYS;utemp8++)
                tableSprites[1].attributes[utemp8].y=213+offset;
        }
        // Otherwise, we copy the first frame into the second
        else
        {
            Mem_Copy(&tableSprites[0].attributes[MAX_NOFLICK], &tableSprites[1].attributes[MAX_NOFLICK], sizeof(SprAttr)*(MAX_SP_PHYS-MAX_NOFLICK));
        }
        // Ensures the interrupt has swapped the sprites tables in VRAM
        // Updates the map position
        VDP_CommandHMMM((map[mapLine*16+scrollCount]%16)*16,(map[mapLine*16+scrollCount]/16)*16+TILE_OFFY,
                                scrollCount*16,offsetScroll+256,
                                16,16);
        //Prepares for the next line in the map
        if(++scrollCount==16){
            mapLine=(--mapLine)%mapHeight;
            offsetScroll-=16;
            scrollCount=0;
        }
        while(swapSprites){}
        // Writes the sprites RAM copy to the VRAM
        VDP_WriteVRAM((u8*)tableSprites[0].attributes, spriteAttributeLow[spriteAttributeLowBack], 0, 128);
        VDP_WriteVRAM((u8*)tableSprites[1].attributes, spriteAttributeLow[spriteAttributeLowBack+1], 0, 128);
        // Swap the indexes for front and back indexes in spriteAttributeLow (maybe just one index variable is needed)
        spriteAttributeLowTemp=spriteAttributeLowDisplay;
        spriteAttributeLowDisplay=spriteAttributeLowBack;
        spriteAttributeLowBack=spriteAttributeLowTemp;
        // Flags that the interrupt should swap the sprites
        offsetDisplay=offset;
        swapSprites=TRUE;
        prevRow8=row8;
        // Scrolls the screen
        offset--;
        //DEBUG_PRINT("Frames, vblanks: %d %d", g_Frame, vblanks);*/
	}

	//DEBUG_LOG("End debug session!");
	Bios_Exit(0);
}
