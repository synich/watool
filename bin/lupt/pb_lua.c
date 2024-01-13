//#include "lua/lauxlib.h"

static void luafn_pb(lua_State* L) {
  const unsigned char B1[]={
    27,76,117,97,81,0,1,4,4,4,8,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,2,28,156,0,0,0,
    36,0,0,0,7,0,0,0,36,64,0,0,7,64,0,0,
    5,128,0,0,100,128,0,0,9,64,128,129,5,0,1,0,
    100,192,0,0,9,64,128,130,36,0,1,0,7,128,1,0,
    36,64,1,0,7,192,1,0,36,128,1,0,7,0,2,0,
    36,192,1,0,7,64,2,0,10,64,0,0,9,0,195,133,
    7,128,2,0,3,0,0,0,74,192,1,0,73,64,195,134,
    73,128,67,135,73,0,196,135,73,128,196,136,73,0,197,137,
    73,128,197,138,73,0,198,139,138,64,0,0,137,64,198,140,
    197,128,6,0,0,1,128,0,220,0,1,1,22,0,0,128,
    137,128,129,3,225,128,0,0,22,0,255,127,228,0,2,0,
    0,0,128,0,36,65,2,0,100,129,2,0,0,0,0,0,
    164,193,2,0,0,0,128,1,228,1,3,0,10,66,1,0,
    9,2,129,141,9,66,1,142,9,130,1,129,9,194,129,142,
    69,194,7,0,9,66,2,143,36,64,3,0,0,0,0,4,
    69,130,2,0,164,130,3,0,0,0,0,0,73,130,2,144,
    67,2,128,4,164,194,3,0,192,2,0,5,1,67,8,0,
    65,195,5,0,129,67,5,0,193,195,4,0,220,130,128,2,
    0,3,0,5,65,67,8,0,129,195,5,0,193,67,5,0,
    1,196,4,0,65,132,8,0,129,196,8,0,193,4,9,0,
    28,131,0,4,64,3,0,5,129,67,3,0,193,67,6,0,
    1,132,3,0,65,4,4,0,129,132,4,0,193,4,5,0,
    1,133,5,0,65,5,6,0,129,69,9,0,92,131,0,5,
    128,3,0,5,193,131,9,0,1,196,9,0,65,4,10,0,
    156,131,0,2,202,195,0,0,201,67,74,147,201,131,202,147,
    201,195,74,148,36,4,4,0,100,68,4,0,164,132,4,0,
    228,196,4,0,0,0,0,9,36,5,5,0,0,0,128,8,
    0,0,128,9,0,0,128,6,0,0,0,1,100,69,5,0,
    0,0,0,8,0,0,0,6,0,0,128,8,164,133,5,0,
    0,0,0,8,0,0,0,6,0,0,0,7,0,0,128,8,
    0,0,128,7,228,197,5,0,0,0,0,8,0,0,128,5,
    0,0,128,4,0,0,128,8,36,6,6,0,0,0,0,8,
    0,0,128,5,0,0,128,8,0,0,128,4,74,70,4,0,
    73,6,5,135,73,70,5,150,73,70,133,150,73,70,5,151,
    73,70,133,151,73,70,5,152,73,70,133,152,73,70,5,153,
    73,70,133,153,73,70,5,154,73,70,133,154,73,70,5,155,
    73,134,5,140,73,134,5,137,73,134,5,138,73,198,133,155,
    73,6,6,156,100,66,6,0,0,0,128,12,0,0,128,8,
    133,134,2,0,228,134,6,0,0,0,128,4,0,0,0,8,
    0,0,128,5,0,0,128,8,137,198,134,156,30,0,128,0,
    58,0,0,0,4,4,0,0,0,102,109,116,0,4,9,0,
    0,0,118,97,114,95,100,117,109,112,0,4,7,0,0,0,
    115,116,114,105,110,103,0,4,6,0,0,0,115,112,108,105,
    116,0,4,3,0,0,0,111,115,0,4,6,0,0,0,112,
    111,112,101,110,0,4,4,0,0,0,109,97,112,0,4,7,
    0,0,0,102,105,108,116,101,114,0,4,7,0,0,0,114,
    101,100,117,99,101,0,4,6,0,0,0,114,97,110,103,101,
    0,4,5,0,0,0,106,115,111,110,0,4,9,0,0,0,
    95,118,101,114,115,105,111,110,0,4,6,0,0,0,48,46,
    49,46,50,0,4,2,0,0,0,92,0,4,2,0,0,0,
    34,0,4,2,0,0,0,8,0,4,2,0,0,0,98,0,
    4,2,0,0,0,12,0,4,2,0,0,0,102,0,4,2,
    0,0,0,10,0,4,2,0,0,0,110,0,4,2,0,0,
    0,13,0,4,2,0,0,0,114,0,4,2,0,0,0,9,
    0,4,2,0,0,0,116,0,4,2,0,0,0,47,0,4,
    6,0,0,0,112,97,105,114,115,0,4,4,0,0,0,110,
    105,108,0,4,6,0,0,0,116,97,98,108,101,0,4,7,
    0,0,0,110,117,109,98,101,114,0,4,8,0,0,0,98,
    111,111,108,101,97,110,0,4,9,0,0,0,116,111,115,116,
    114,105,110,103,0,4,7,0,0,0,101,110,99,111,100,101,
    0,4,2,0,0,0,32,0,4,2,0,0,0,93,0,4,
    2,0,0,0,125,0,4,2,0,0,0,44,0,4,2,0,
    0,0,117,0,4,5,0,0,0,116,114,117,101,0,4,6,
    0,0,0,102,97,108,115,101,0,4,5,0,0,0,110,117,
    108,108,0,1,1,1,0,0,4,2,0,0,0,48,0,4,
    2,0,0,0,49,0,4,2,0,0,0,50,0,4,2,0,
    0,0,51,0,4,2,0,0,0,52,0,4,2,0,0,0,
    53,0,4,2,0,0,0,54,0,4,2,0,0,0,55,0,
    4,2,0,0,0,56,0,4,2,0,0,0,57,0,4,2,
    0,0,0,45,0,4,2,0,0,0,91,0,4,2,0,0,
    0,123,0,4,7,0,0,0,100,101,99,111,100,101,0,27,
    0,0,0,0,0,0,0,1,0,0,0,8,0,0,0,0,
    1,3,12,18,0,0,0,138,0,0,0,229,0,0,0,162,
    64,0,0,193,64,0,0,199,0,0,0,193,128,0,0,20,
    1,0,1,65,129,0,0,224,64,1,128,203,193,64,0,69,
    2,0,0,134,130,1,1,193,130,0,0,220,129,128,2,0,
    0,128,3,223,0,254,127,30,0,0,1,30,0,128,0,4,
    0,0,0,4,5,0,0,0,104,111,108,101,0,4,3,0,
    0,0,123,125,0,3,0,0,0,0,0,0,240,63,4,5,
    0,0,0,103,115,117,98,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,
    14,0,0,0,0,1,0,11,21,0,0,0,69,64,0,0,
    128,0,0,0,92,128,0,1,23,64,0,128,22,192,2,128,
    67,0,0,1,197,128,0,0,0,1,0,0,220,0,1,1,
    22,192,0,128,5,194,0,0,64,2,0,3,128,2,128,3,
    28,66,128,1,225,128,0,0,22,64,254,127,22,128,0,128,
    69,192,0,0,128,0,0,0,92,64,0,1,30,0,128,0,
    4,0,0,0,4,6,0,0,0,116,97,98,108,101,0,4,
    5,0,0,0,116,121,112,101,0,4,6,0,0,0,112,97,
    105,114,115,0,4,6,0,0,0,112,114,105,110,116,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,16,0,0,0,31,0,0,0,0,2,0,12,33,
    0,0,0,138,0,0,0,193,0,0,0,1,1,0,0,65,
    65,0,0,129,1,0,0,203,129,64,0,64,2,128,0,128,
    2,128,1,194,2,128,0,220,129,128,2,0,1,128,3,26,
    1,0,0,22,128,1,128,203,193,64,0,64,2,128,1,141,
    2,64,2,220,129,0,2,64,1,128,3,204,0,64,2,22,
    192,0,128,203,193,64,0,64,2,128,1,220,129,128,1,64,
    1,128,3,197,1,1,0,198,65,193,3,0,2,0,1,64,
    2,128,2,220,65,128,1,26,65,0,0,22,64,249,127,158,
    0,0,1,30,0,128,0,6,0,0,0,3,0,0,0,0,
    0,0,240,63,4,1,0,0,0,0,4,5,0,0,0,102,
    105,110,100,0,4,4,0,0,0,115,117,98,0,4,6,0,
    0,0,116,97,98,108,101,0,4,7,0,0,0,105,110,115,
    101,114,116,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,33,0,0,0,39,0,0,0,
    0,1,0,7,20,0,0,0,75,0,64,0,193,64,0,0,
    1,129,0,0,92,128,0,2,0,0,128,0,69,192,0,0,
    70,0,193,0,128,0,0,0,92,128,0,1,139,64,193,0,
    1,129,1,0,156,128,128,1,203,192,193,0,220,64,0,1,
    203,0,66,1,65,65,2,0,129,129,2,0,221,0,0,2,
    222,0,0,0,30,0,128,0,11,0,0,0,4,5,0,0,
    0,103,115,117,98,0,4,2,0,0,0,96,0,4,3,0,
    0,0,92,96,0,4,3,0,0,0,105,111,0,4,6,0,
    0,0,112,111,112,101,110,0,4,5,0,0,0,114,101,97,
    100,0,4,3,0,0,0,42,97,0,4,6,0,0,0,99,
    108,111,115,101,0,4,4,0,0,0,115,117,98,0,3,0,
    0,0,0,0,0,240,63,3,0,0,0,0,0,0,0,192,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,41,0,0,0,50,0,0,0,0,2,0,11,
    15,0,0,0,138,0,0,0,193,0,0,0,1,1,0,0,
    84,1,0,0,129,1,0,0,32,65,1,128,6,194,1,0,
    64,2,128,0,128,2,0,4,92,130,0,1,137,64,130,1,
    204,0,192,1,31,1,254,127,158,0,0,1,30,0,128,0,
    1,0,0,0,3,0,0,0,0,0,0,240,63,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,52,0,0,0,63,0,0,0,0,2,0,11,17,0,0,
    0,138,0,0,0,193,0,0,0,1,1,0,0,84,1,0,
    0,129,1,0,0,32,193,1,128,6,194,1,0,64,2,128,
    0,128,2,0,4,92,130,0,1,90,2,0,0,22,64,0,
    128,137,0,130,1,204,0,192,1,31,129,253,127,158,0,0,
    1,30,0,128,0,1,0,0,0,3,0,0,0,0,0,0,
    240,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,65,0,0,0,72,0,0,0,0,3,
    0,12,14,0,0,0,192,0,128,0,1,1,0,0,84,1,
    0,0,129,1,0,0,32,65,1,128,6,194,1,0,64,2,
    0,1,128,2,128,1,192,2,0,4,92,130,128,1,192,0,
    128,4,31,1,254,127,222,0,0,1,30,0,128,0,1,0,
    0,0,3,0,0,0,0,0,0,240,63,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,74,
    0,0,0,85,0,0,0,0,2,0,8,16,0,0,0,90,
    64,0,0,22,128,0,128,128,0,0,0,1,0,0,0,64,
    0,0,1,138,0,0,0,193,64,0,0,0,1,0,0,77,
    65,192,0,129,65,0,0,32,65,0,128,137,192,129,1,204,
    64,192,1,31,1,255,127,158,0,0,1,30,0,128,0,2,
    0,0,0,3,0,0,0,0,0,0,0,0,3,0,0,0,
    0,0,0,240,63,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,135,0,0,0,137,0,0,
    0,1,1,0,6,14,0,0,0,65,0,0,0,132,0,0,
    0,134,0,0,1,154,64,0,0,22,64,1,128,133,64,0,
    0,134,128,64,1,193,192,0,0,11,1,65,0,28,1,0,
    1,156,128,0,0,85,128,128,0,94,0,0,1,30,0,128,
    0,5,0,0,0,4,2,0,0,0,92,0,4,7,0,0,
    0,115,116,114,105,110,103,0,4,7,0,0,0,102,111,114,
    109,97,116,0,4,6,0,0,0,117,37,48,52,120,0,4,
    5,0,0,0,98,121,116,101,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,140,0,0,
    0,142,0,0,0,0,1,0,2,3,0,0,0,65,0,0,
    0,94,0,0,1,30,0,128,0,1,0,0,0,4,5,0,
    0,0,110,117,108,108,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,145,0,0,0,184,
    0,0,0,1,2,0,15,109,0,0,0,138,0,0,0,90,
    64,0,0,22,64,0,128,202,0,0,0,64,0,128,1,198,
    0,128,0,218,0,0,0,22,128,0,128,197,0,0,0,1,
    65,0,0,220,64,0,1,73,128,64,0,197,192,0,0,0,
    1,0,0,65,1,1,0,220,128,128,1,23,64,193,1,22,
    0,1,128,197,128,1,0,0,1,0,0,220,128,0,1,23,
    64,193,1,22,128,11,128,193,192,1,0,5,1,2,0,64,
    1,0,0,28,1,1,1,22,0,2,128,5,66,2,0,64,
    2,128,3,28,130,0,1,87,128,66,4,22,128,0,128,5,
    2,0,0,65,194,2,0,28,66,0,1,204,0,193,1,33,
    65,0,0,22,0,253,127,20,1,0,0,87,0,129,1,22,
    128,0,128,5,1,0,0,65,1,3,0,28,65,0,1,5,
    65,3,0,64,1,0,0,28,1,1,1,22,192,1,128,69,
    130,3,0,70,194,195,4,128,2,0,1,196,2,0,0,0,
    3,0,4,64,3,128,0,220,2,128,1,92,66,0,0,33,
    129,0,0,22,64,253,127,73,64,65,0,1,1,4,0,69,
    129,3,0,70,65,196,2,128,1,0,1,193,129,4,0,92,
    129,128,1,129,193,4,0,21,129,1,2,30,1,0,1,22,
    64,9,128,197,0,2,0,0,1,0,0,220,0,1,1,22,
    64,5,128,5,66,2,0,64,2,0,3,28,130,0,1,87,
    0,69,4,22,128,0,128,5,2,0,0,65,194,2,0,28,
    66,0,1,5,130,3,0,6,194,67,4,64,2,0,1,132,
    2,0,0,192,2,0,3,0,3,128,0,156,130,128,1,193,
    66,5,0,4,3,0,0,64,3,128,3,128,3,128,0,28,
    131,128,1,149,2,3,5,28,66,128,1,225,128,0,0,22,
    192,249,127,73,64,65,0,193,128,5,0,5,129,3,0,6,
    65,68,2,64,1,0,1,129,129,4,0,28,129,128,1,65,
    193,5,0,213,64,129,1,222,0,0,1,30,0,128,0,24,
    0,0,0,4,6,0,0,0,101,114,114,111,114,0,4,19,
    0,0,0,99,105,114,99,117,108,97,114,32,114,101,102,101,
    114,101,110,99,101,0,1,1,4,7,0,0,0,114,97,119,
    103,101,116,0,3,0,0,0,0,0,0,240,63,0,4,5,
    0,0,0,110,101,120,116,0,3,0,0,0,0,0,0,0,
    0,4,6,0,0,0,112,97,105,114,115,0,4,5,0,0,
    0,116,121,112,101,0,4,7,0,0,0,110,117,109,98,101,
    114,0,4,42,0,0,0,105,110,118,97,108,105,100,32,116,
    97,98,108,101,58,32,109,105,120,101,100,32,111,114,32,105,
    110,118,97,108,105,100,32,107,101,121,32,116,121,112,101,115,
    0,4,28,0,0,0,105,110,118,97,108,105,100,32,116,97,
    98,108,101,58,32,115,112,97,114,115,101,32,97,114,114,97,
    121,0,4,7,0,0,0,105,112,97,105,114,115,0,4,6,
    0,0,0,116,97,98,108,101,0,4,7,0,0,0,105,110,
    115,101,114,116,0,4,2,0,0,0,91,0,4,7,0,0,
    0,99,111,110,99,97,116,0,4,2,0,0,0,44,0,4,
    2,0,0,0,93,0,4,7,0,0,0,115,116,114,105,110,
    103,0,4,2,0,0,0,58,0,4,2,0,0,0,123,0,
    4,2,0,0,0,125,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,187,0,0,0,189,
    0,0,0,1,1,0,6,9,0,0,0,65,0,0,0,139,
    64,64,0,1,129,0,0,68,1,0,0,156,128,0,2,193,
    0,0,0,85,192,128,0,94,0,0,1,30,0,128,0,3,
    0,0,0,4,2,0,0,0,34,0,4,5,0,0,0,103,
    115,117,98,0,4,10,0,0,0,91,37,122,1,45,31,92,
    34,93,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,192,0,0,0,198,0,0,0,0,
    1,0,5,26,0,0,0,23,0,0,0,22,0,2,128,69,
    0,0,0,70,64,192,0,82,0,128,0,89,64,0,0,22,
    192,0,128,69,0,0,0,70,64,192,0,25,0,128,0,22,
    192,1,128,69,128,0,0,129,192,0,0,197,0,1,0,0,
    1,0,0,220,128,0,1,1,65,1,0,149,0,1,1,92,
    64,0,1,69,128,1,0,70,192,193,0,129,0,2,0,192,
    0,0,0,93,0,128,1,94,0,0,0,30,0,128,0,9,
    0,0,0,4,5,0,0,0,109,97,116,104,0,4,5,0,
    0,0,104,117,103,101,0,4,6,0,0,0,101,114,114,111,
    114,0,4,26,0,0,0,117,110,101,120,112,101,99,116,101,
    100,32,110,117,109,98,101,114,32,118,97,108,117,101,32,39,
    0,4,9,0,0,0,116,111,115,116,114,105,110,103,0,4,
    2,0,0,0,39,0,4,7,0,0,0,115,116,114,105,110,
    103,0,4,7,0,0,0,102,111,114,109,97,116,0,4,6,
    0,0,0,37,46,49,52,103,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,210,0,0,
    0,217,0,0,0,1,2,0,8,19,0,0,0,133,0,0,
    0,192,0,0,0,156,128,0,1,196,0,0,0,198,128,128,
    1,218,0,0,0,22,0,1,128,0,1,128,1,64,1,0,
    0,128,1,128,0,29,1,128,1,30,1,0,0,5,65,0,
    0,65,129,0,0,128,1,0,1,193,193,0,0,85,193,129,
    2,28,65,0,1,30,0,128,0,4,0,0,0,4,5,0,
    0,0,116,121,112,101,0,4,6,0,0,0,101,114,114,111,
    114,0,4,18,0,0,0,117,110,101,120,112,101,99,116,101,
    100,32,116,121,112,101,32,39,0,4,2,0,0,0,39,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,220,0,0,0,222,0,0,0,1,1,0,3,
    5,0,0,0,68,0,0,0,128,0,0,0,92,128,0,1,
    94,0,0,1,30,0,128,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    231,0,0,0,237,0,0,0,0,0,3,9,16,0,0,0,
    74,0,0,0,129,0,0,0,197,64,0,0,1,129,0,0,
    101,1,0,0,220,128,0,0,1,1,0,0,160,0,1,128,
    133,65,0,0,192,1,128,2,37,2,0,0,156,129,0,0,
    73,192,64,3,159,64,254,127,94,0,0,1,30,0,128,0,
    4,0,0,0,3,0,0,0,0,0,0,240,63,4,7,0,
    0,0,115,101,108,101,99,116,0,4,2,0,0,0,35,0,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,251,0,0,0,2,1,0,0,0,4,
    0,12,17,0,0,0,0,1,128,0,84,1,0,0,129,1,
    0,0,32,193,1,128,11,66,64,0,128,2,128,3,192,2,
    128,3,28,130,0,2,6,2,2,1,87,192,0,4,22,0,
    0,128,222,1,0,1,31,129,253,127,20,1,0,0,12,1,
    64,2,30,1,0,1,30,0,128,0,2,0,0,0,3,0,
    0,0,0,0,0,240,63,4,4,0,0,0,115,117,98,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,5,1,0,0,16,1,0,0,0,3,0,13,
    26,0,0,0,193,0,0,0,1,1,0,0,65,1,0,0,
    141,1,192,0,193,1,0,0,96,1,2,128,12,1,64,2,
    75,66,64,0,192,2,0,4,0,3,0,4,92,130,0,2,
    23,128,192,4,22,64,0,128,204,0,192,1,1,1,0,0,
    95,65,253,127,69,193,0,0,133,1,1,0,134,65,65,3,
    193,129,1,0,0,2,0,1,64,2,128,1,128,2,0,2,
    156,1,128,2,92,65,0,0,30,0,128,0,7,0,0,0,
    3,0,0,0,0,0,0,240,63,4,4,0,0,0,115,117,
    98,0,4,2,0,0,0,10,0,4,6,0,0,0,101,114,
    114,111,114,0,4,7,0,0,0,115,116,114,105,110,103,0,
    4,7,0,0,0,102,111,114,109,97,116,0,4,21,0,0,
    0,37,115,32,97,116,32,108,105,110,101,32,37,100,32,99,
    111,108,32,37,100,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,19,1,0,0,33,1,
    0,0,0,1,0,7,71,0,0,0,69,0,0,0,70,64,
    192,0,25,128,64,0,22,64,1,128,133,192,0,0,134,0,
    65,1,192,0,0,0,157,0,0,1,158,0,0,0,22,0,
    13,128,25,64,65,0,22,128,2,128,133,192,0,0,134,0,
    65,1,192,0,128,0,15,129,65,0,220,128,0,1,204,192,
    193,1,16,129,65,0,12,1,66,2,157,0,128,1,158,0,
    0,0,22,192,9,128,25,64,66,0,22,192,3,128,133,192,
    0,0,134,0,65,1,192,0,128,0,15,129,66,0,220,128,
    0,1,204,192,194,1,0,1,128,0,80,129,66,0,79,129,
    193,2,28,129,0,1,12,1,66,2,80,129,65,0,76,1,
    194,2,157,0,0,2,158,0,0,0,22,64,5,128,25,0,
    67,0,22,192,4,128,133,192,0,0,134,0,65,1,192,0,
    128,0,15,65,67,0,220,128,0,1,204,128,195,1,0,1,
    128,0,80,65,67,0,79,129,194,2,28,129,0,1,12,1,
    66,2,64,1,128,0,144,129,66,0,143,129,65,3,92,129,
    0,1,76,1,194,2,144,129,65,0,140,1,66,3,157,0,
    128,2,158,0,0,0,133,192,3,0,197,192,0,0,198,0,
    196,1,1,65,4,0,64,1,0,0,220,0,128,1,156,64,
    0,0,30,0,128,0,18,0,0,0,4,5,0,0,0,109,
    97,116,104,0,4,6,0,0,0,102,108,111,111,114,0,3,
    0,0,0,0,0,192,95,64,4,7,0,0,0,115,116,114,
    105,110,103,0,4,5,0,0,0,99,104,97,114,0,3,0,
    0,0,0,0,252,159,64,3,0,0,0,0,0,0,80,64,
    3,0,0,0,0,0,0,104,64,3,0,0,0,0,0,0,
    96,64,3,0,0,0,0,224,255,239,64,3,0,0,0,0,
    0,0,176,64,3,0,0,0,0,0,0,108,64,3,0,0,
    0,0,255,255,48,65,3,0,0,0,0,0,0,16,65,3,
    0,0,0,0,0,0,110,64,4,6,0,0,0,101,114,114,
    111,114,0,4,7,0,0,0,102,111,114,109,97,116,0,4,
    31,0,0,0,105,110,118,97,108,105,100,32,117,110,105,99,
    111,100,101,32,99,111,100,101,112,111,105,110,116,32,39,37,
    120,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,36,1,0,0,45,1,0,0,1,
    1,0,7,30,0,0,0,69,0,0,0,139,64,64,0,1,
    129,0,0,65,193,0,0,156,128,0,2,193,0,1,0,92,
    128,128,1,133,0,0,0,203,64,64,0,65,65,1,0,129,
    129,1,0,220,128,0,2,1,1,1,0,156,128,128,1,154,
    0,0,0,22,0,2,128,196,0,0,0,13,193,193,0,14,
    1,66,2,77,65,66,1,12,65,1,2,12,129,66,2,221,
    0,0,1,222,0,0,0,22,192,0,128,196,0,0,0,0,
    1,128,0,221,0,0,1,222,0,0,0,30,0,128,0,11,
    0,0,0,4,9,0,0,0,116,111,110,117,109,98,101,114,
    0,4,4,0,0,0,115,117,98,0,3,0,0,0,0,0,
    0,240,63,3,0,0,0,0,0,0,16,64,3,0,0,0,
    0,0,0,48,64,3,0,0,0,0,0,0,28,64,3,0,
    0,0,0,0,0,36,64,3,0,0,0,0,0,0,235,64,
    3,0,0,0,0,0,0,144,64,3,0,0,0,0,0,128,
    235,64,3,0,0,0,0,0,0,240,64,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,48,
    1,0,0,86,1,0,0,4,2,0,13,94,0,0,0,129,
    0,0,0,204,64,192,0,0,1,128,1,84,1,0,0,25,
    64,129,1,22,64,20,128,75,129,64,0,192,1,128,1,92,
    129,128,1,24,192,192,2,22,64,1,128,132,1,0,0,192,
    1,0,0,0,2,128,1,65,2,1,0,156,65,0,2,22,
    0,17,128,23,64,193,2,22,192,13,128,128,1,0,1,203,
    129,65,0,64,2,0,2,141,66,192,1,220,129,0,2,149,
    192,1,3,204,64,192,1,139,129,65,0,0,2,128,1,64,
    2,128,1,156,129,0,2,23,192,65,3,22,0,6,128,203,
    1,66,0,65,66,2,0,140,66,192,1,220,129,0,2,218,
    65,0,0,22,128,2,128,203,1,66,0,65,130,2,0,140,
    66,192,1,220,129,0,2,218,65,0,0,22,0,1,128,196,
    1,0,0,0,2,0,0,77,66,192,1,129,194,2,0,220,
    129,0,2,0,2,0,1,68,2,128,0,128,2,128,3,92,
    130,0,1,149,64,2,4,20,2,128,3,204,0,130,1,22,
    192,3,128,196,1,0,1,198,129,129,3,218,65,0,0,22,
    192,1,128,196,1,0,0,0,2,0,0,77,66,192,1,129,
    2,3,0,192,2,0,3,1,67,3,0,149,2,3,5,220,
    65,0,2,192,1,0,1,4,2,128,1,6,130,1,4,149,
    0,130,3,12,65,192,1,22,128,2,128,23,128,195,2,22,
    0,2,128,128,1,0,1,203,129,65,0,64,2,0,2,141,
    66,192,1,220,129,0,2,149,192,1,3,128,1,0,1,204,
    65,192,1,158,1,128,1,204,64,192,1,22,128,234,127,68,
    1,0,0,128,1,0,0,192,1,128,0,1,194,3,0,92,
    65,0,2,30,0,128,0,16,0,0,0,4,1,0,0,0,
    0,3,0,0,0,0,0,0,240,63,4,5,0,0,0,98,
    121,116,101,0,3,0,0,0,0,0,0,64,64,4,28,0,
    0,0,99,111,110,116,114,111,108,32,99,104,97,114,97,99,
    116,101,114,32,105,110,32,115,116,114,105,110,103,0,3,0,
    0,0,0,0,0,87,64,4,4,0,0,0,115,117,98,0,
    4,2,0,0,0,117,0,4,6,0,0,0,109,97,116,99,
    104,0,4,28,0,0,0,94,91,100,68,93,91,56,57,97,
    65,98,66,93,37,120,37,120,92,117,37,120,37,120,37,120,
    37,120,0,4,10,0,0,0,94,37,120,37,120,37,120,37,
    120,0,4,33,0,0,0,105,110,118,97,108,105,100,32,117,
    110,105,99,111,100,101,32,101,115,99,97,112,101,32,105,110,
    32,115,116,114,105,110,103,0,4,22,0,0,0,105,110,118,
    97,108,105,100,32,101,115,99,97,112,101,32,99,104,97,114,
    32,39,0,4,12,0,0,0,39,32,105,110,32,115,116,114,
    105,110,103,0,3,0,0,0,0,0,0,65,64,4,34,0,
    0,0,101,120,112,101,99,116,101,100,32,99,108,111,115,105,
    110,103,32,113,117,111,116,101,32,102,111,114,32,115,116,114,
    105,110,103,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,89,1,0,0,97,1,0,0,
    3,2,0,11,26,0,0,0,132,0,0,0,192,0,0,0,
    0,1,128,0,68,1,128,0,156,128,0,2,203,0,64,0,
    64,1,128,0,141,65,64,1,220,128,0,2,5,129,0,0,
    64,1,128,1,28,129,0,1,26,65,0,0,22,192,1,128,
    68,1,0,1,128,1,0,0,192,1,128,0,1,194,0,0,
    64,2,128,1,129,2,1,0,21,130,2,4,92,65,0,2,
    64,1,0,2,128,1,0,1,94,1,128,1,30,0,128,0,
    5,0,0,0,4,4,0,0,0,115,117,98,0,3,0,0,
    0,0,0,0,240,63,4,9,0,0,0,116,111,110,117,109,
    98,101,114,0,4,17,0,0,0,105,110,118,97,108,105,100,
    32,110,117,109,98,101,114,32,39,0,4,2,0,0,0,39,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,100,1,0,0,107,1,0,0,5,2,0,
    10,26,0,0,0,132,0,0,0,192,0,0,0,0,1,128,
    0,68,1,128,0,156,128,0,2,203,0,64,0,64,1,128,
    0,141,65,64,1,220,128,0,2,4,1,0,1,6,193,0,
    2,26,65,0,0,22,192,1,128,4,1,128,1,64,1,0,
    0,128,1,128,0,193,129,0,0,0,2,128,1,65,194,0,
    0,213,65,130,3,28,65,0,2,4,1,0,2,6,193,0,
    2,64,1,0,1,30,1,128,1,30,0,128,0,4,0,0,
    0,4,4,0,0,0,115,117,98,0,3,0,0,0,0,0,
    0,240,63,4,18,0,0,0,105,110,118,97,108,105,100,32,
    108,105,116,101,114,97,108,32,39,0,4,2,0,0,0,39,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,110,1,0,0,134,1,0,0,4,2,0,
    10,54,0,0,0,138,0,0,0,193,0,0,0,76,0,192,
    0,3,1,0,2,68,1,0,0,128,1,0,0,192,1,128,
    0,4,2,128,0,66,2,128,0,92,129,128,2,64,0,128,
    2,75,65,64,0,192,1,128,0,0,2,128,0,92,129,0,
    2,23,128,192,2,22,64,0,128,76,0,192,0,22,128,7,
    128,68,1,0,1,128,1,0,0,192,1,128,0,92,193,128,
    1,64,0,0,3,0,1,128,2,137,0,129,1,204,0,192,
    1,68,1,0,0,128,1,0,0,192,1,128,0,4,2,128,
    0,66,2,128,0,92,129,128,2,64,0,128,2,75,65,64,
    0,192,1,128,0,0,2,128,0,92,129,0,2,76,0,192,
    0,23,128,192,2,22,0,0,128,22,192,1,128,87,192,192,
    2,22,128,245,127,132,1,128,1,192,1,0,0,0,2,128,
    0,65,2,1,0,156,65,0,2,22,0,244,127,0,1,0,
    1,64,1,128,0,30,1,128,1,30,0,128,0,5,0,0,
    0,3,0,0,0,0,0,0,240,63,4,4,0,0,0,115,
    117,98,0,4,2,0,0,0,93,0,4,2,0,0,0,44,
    0,4,20,0,0,0,101,120,112,101,99,116,101,100,32,39,
    93,39,32,111,114,32,39,44,39,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,137,1,
    0,0,171,1,0,0,4,2,0,10,94,0,0,0,138,0,
    0,0,76,0,192,0,195,0,0,2,68,1,0,0,128,1,
    0,0,192,1,128,0,4,2,128,0,66,2,128,0,92,129,
    128,2,64,0,128,2,75,65,64,0,192,1,128,0,0,2,
    128,0,92,129,0,2,23,128,192,2,22,64,0,128,76,0,
    192,0,22,192,17,128,75,65,64,0,192,1,128,0,0,2,
    128,0,92,129,0,2,87,192,192,2,22,0,1,128,68,1,
    0,1,128,1,0,0,192,1,128,0,1,2,1,0,92,65,
    0,2,68,1,128,1,128,1,0,0,192,1,128,0,92,193,
    128,1,64,0,0,3,192,0,128,2,68,1,0,0,128,1,
    0,0,192,1,128,0,4,2,128,0,66,2,128,0,92,129,
    128,2,64,0,128,2,75,65,64,0,192,1,128,0,0,2,
    128,0,92,129,0,2,87,64,193,2,22,0,1,128,68,1,
    0,1,128,1,0,0,192,1,128,0,1,130,1,0,92,65,
    0,2,68,1,0,0,128,1,0,0,204,1,192,0,4,2,
    128,0,66,2,128,0,92,129,128,2,64,0,128,2,68,1,
    128,1,128,1,0,0,192,1,128,0,92,193,128,1,64,0,
    0,3,0,1,128,2,137,0,129,1,68,1,0,0,128,1,
    0,0,192,1,128,0,4,2,128,0,66,2,128,0,92,129,
    128,2,64,0,128,2,75,65,64,0,192,1,128,0,0,2,
    128,0,92,129,0,2,76,0,192,0,23,128,192,2,22,0,
    0,128,22,192,1,128,87,192,193,2,22,64,235,127,132,1,
    0,1,192,1,0,0,0,2,128,0,65,2,2,0,156,65,
    0,2,22,192,233,127,192,0,0,1,0,1,128,0,222,0,
    128,1,30,0,128,0,9,0,0,0,3,0,0,0,0,0,
    0,240,63,4,4,0,0,0,115,117,98,0,4,2,0,0,
    0,125,0,4,2,0,0,0,34,0,4,24,0,0,0,101,
    120,112,101,99,116,101,100,32,115,116,114,105,110,103,32,102,
    111,114,32,107,101,121,0,4,2,0,0,0,58,0,4,23,
    0,0,0,101,120,112,101,99,116,101,100,32,39,58,39,32,
    97,102,116,101,114,32,107,101,121,0,4,2,0,0,0,44,
    0,4,20,0,0,0,101,120,112,101,99,116,101,100,32,39,
    125,39,32,111,114,32,39,44,39,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,195,1,
    0,0,202,1,0,0,2,2,0,10,22,0,0,0,139,0,
    64,0,0,1,128,0,64,1,128,0,156,128,0,2,196,0,
    0,0,198,128,128,1,218,0,0,0,22,0,1,128,0,1,
    128,1,64,1,0,0,128,1,128,0,29,1,128,1,30,1,
    0,0,4,1,128,0,64,1,0,0,128,1,128,0,193,65,
    0,0,0,2,0,1,65,130,0,0,213,65,130,3,28,65,
    0,2,30,0,128,0,3,0,0,0,4,4,0,0,0,115,
    117,98,0,4,23,0,0,0,117,110,101,120,112,101,99,116,
    101,100,32,99,104,97,114,97,99,116,101,114,32,39,0,4,
    2,0,0,0,39,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,205,1,0,0,215,1,
    0,0,4,1,0,8,38,0,0,0,69,0,0,0,128,0,
    0,0,92,128,0,1,87,64,192,0,22,128,1,128,69,128,
    0,0,129,192,0,0,197,0,0,0,0,1,0,0,220,128,
    0,1,149,192,0,1,92,64,0,1,68,0,0,0,128,0,
    0,0,196,0,128,0,0,1,0,0,65,1,1,0,132,1,
    0,1,194,1,128,0,220,0,128,2,92,192,0,0,196,0,
    128,0,0,1,0,0,64,1,0,1,132,1,0,1,194,1,
    128,0,220,128,128,2,128,0,128,1,212,0,0,0,25,192,
    0,1,22,0,1,128,196,0,128,1,0,1,0,0,64,1,
    0,1,129,65,1,0,220,64,0,2,94,0,0,1,30,0,
    128,0,6,0,0,0,4,5,0,0,0,116,121,112,101,0,
    4,7,0,0,0,115,116,114,105,110,103,0,4,6,0,0,
    0,101,114,114,111,114,0,4,39,0,0,0,101,120,112,101,
    99,116,101,100,32,97,114,103,117,109,101,110,116,32,111,102,
    32,116,121,112,101,32,115,116,114,105,110,103,44,32,103,111,
    116,32,0,3,0,0,0,0,0,0,240,63,4,17,0,0,
    0,116,114,97,105,108,105,110,103,32,103,97,114,98,97,103,
    101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  };

  if (luaL_loadbuffer(L,(const char*)B1,sizeof(B1),"buf_chunk_pb")==0)
    lua_call(L, 0, 0);
  //lua_setglobal(L, "pb");
}