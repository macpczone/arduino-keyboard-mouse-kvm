typedef struct {
    unsigned char x11keynum;
    unsigned char keysim[20];
    unsigned char keyout;
} keystruct;

keystruct keysims[] = {
		{9, "Escape", 177},
		{10, "1", 49},
		{11, "2", 50},
		{12, "3", 51},
		{13, "4", 52},
		{14, "5", 53},
		{15, "6", 54},
		{16, "7", 55},
		{17, "8", 56},
		{18, "9", 57},
		{19, "0", 48},
		{20, "minus", 45},
		{21, "equal", 61},
		{22, "BackSpace", 178},
		{23, "Tab", 179},
		{24, "q", 113},
		{25, "w", 119},
		{26, "e", 101},
		{27, "r", 114},
		{28, "t", 116},
		{29, "y", 121},
		{30, "u", 117},
		{31, "i", 105},
		{32, "o", 111},
		{33, "p", 112},
		{34, "bracketleft", 91},
		{35, "bracketright", 93},
		{36, "Return", 176},
		{37, "Control_L", 128},
		{38, "a", 97},
		{39, "s", 115},
		{40, "d", 100},
		{41, "f", 102},
		{42, "g", 103},
		{43, "h", 104},
		{44, "j", 106},
		{45, "k", 107},
		{46, "l", 108},
		{47, "semicolon", 59},
		{48, "apostrophe", 39},
		{49, "grave", 95},
		{50, "Shift_L", 129},
		{51, "backslash", 92},
		{52, "z", 122},
		{53, "x", 120},
		{54, "c", 99},
		{55, "v", 118},
		{56, "b", 98},
		{57, "n", 110},
		{58, "m", 109},
		{59, "comma", 44},
		{60, "full_stop", 46},
		{61, "slash", 47},
		{62, "Shift_R", 133},
		//{63, "KP_Multiply", },
		{64, "Alt_L", 130},
		{65, "space", 32},
		{66, "Caps_Lock", 193},
		{67, "F1", 194},
		{68, "F2", 195},
		{69, "F3", 196},
		{70, "F4", 197},
		{71, "F5", 198},
		{72, "F6", 199},
		{73, "F7", 200},
		{74, "F8", 201},
		{75, "F9", 202},
		{76, "F10", 203},
		//{77, "Num_Lock", },
		//{78, "Scroll_Lock", },
		//{79, "KP_Home", },
		//{80, "KP_Up", },
		//{81, "KP_Prior", },
		//{82, "KP_Subtract", },
		//{83, "KP_Left", },
		//{84, "KP_Begin", },
		//{85, "KP_Right", },
		//{86, "KP_Add", },
		//{87, "KP_End", },
		//{88, "KP_Down", },
		//{89, "KP_Next", },
		//{90, "KP_Insert", },
		//{91, "KP_Delete", },
		//{92, "Print", },
		//{93, "Mode_switch", },
		//{94, "less", },
		{95, "F11", 204},
		{96, "F12", 205},
		{97, "Home", 210},
		{98, "Up", 218},
		{99, "Prior", 211},
		{100, "Left", 216},
		{102, "Right", 215},
		{103, "End", 213},
		{104, "Down", 217},
		{105, "Next", 214},
		{106, "Insert", 73},
		{107, "Delete", 212},
		//{108, "KP_Enter", },
		{109, "Control_R", 132},
		{110, "Pause", 72},
		//{111, "Print", },
		//{112, "KP_Divide", },
		{113, "Alt_R", 134},
		//{114, "Pause", },
		{115, "Super_L", 131},
		{116, "Super_R", 135},
		//{117, "Menu", },
		//{124, "ISO_Level3_Shift", },
		//{126, "KP_Equal, }
};
