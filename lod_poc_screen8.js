//*****************************************************************************
// PROJECT CONFIG OVERWRITE
//*****************************************************************************

ProjName = "lod_poc";

Target = "ROM_ASCII8";

//-- ROM mapper total size in KB (number). Must be a multiple of 8 or 16 depending on the mapper type (from 64 to 4096)
ROMSize = 128;

//-- Number of segments in the main program of a mapped-ROM (number)
ROMMainSegments = 2;

//-- Application ID. Can be 2 character string or 16-bits integer (0~65535)
AppID = "LD";

//-- List of library modules to build (array)
LibModules = ["scc", "msx-audio", "psg", "msx-music", "vgm/lvgm_player", "ayfx/ayfx_player","system", "bios", "vdp", "input", "memory", "compress/zx0", "string" ];
//"pt3/pt3_player",
//-- List of raw data files to be added to final binary (array). Each entry must be in the following format: { offset:0x0000, file:"myfile.bin" }
// Segments 0-2: Main code (banks 2-4)
// Segment 3: Splash and common data (To load in bank 5)
// Segment 4-5: Extra code and memory area + ISR
// Segment 6: music 1
// Segment 7: music 2

RawFiles = [
	{ segment: 3, file:"gfx/xaa.zx0" },
	{ segment: 3, file:"gfx/xab.zx0" },
	{ segment: 3, file:"gfx/xac.zx0" },
	{ segment: 3, file:"gfx/palette.bin" },
	//{ segment: 5, file:"content/vgm/psg_galious_05.vgm"},
//  { segment: 6, file:"content/vgm/mm_psycho_03.vgm" },
	{ segment: 6, file:"content/lvgm/penguin.lvm"},
    { segment: 7, file:"content/lvgm/mm_deva_08.bin"},
    //{ segment: 6, file:"content/lvgm/laydock2.lvm"},
    //{ segment: 6, file:"content/lvgm/mm_psycho_03.lvm" },
    //{ segment: 6, file:"content/vgm/mm_undeadline_03.vgm" },

];

//-- Add SCC extension (boolean)
EmulSCC = true;

//-- Add MSX-Music extension (boolean)
EmulMSXMusic = true;

//-- Add MSX-Audio extension (boolean)
EmulMSXAudio = true;

//-- Add mapped-RAM extension (boolean)
EmulRAM = true;
