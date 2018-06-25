//
// Arduino library for controlling TI's TLC59711
//
// 21 Feb 2016 by Ulrich Stern
//
// open source (see LICENSE file)
//
// notes:
// * the library's Wiki on GitHub has a comparison with the Adafruit_TLC59711
//  library and various other info
//

#ifndef TLC59711_H_
#define TLC59711_H_

#include <Arduino.h>

class Tlc59711 {
 public:
  // object default state: (matches reset())
  // - all grayscale (PWM) values 0
  // - all brightness values 127 (maximum)
  explicit Tlc59711(uint16_t numTlc, uint8_t clkPin = 13, uint8_t dataPin = 11);

  ~Tlc59711();

  // select data transfer mode
  // * beginFast():
  //  - use hardware support (SPI library)
  //  - default: 10 MHz SPI clock (TLC59711 datasheet value)
  // * beginSlow():
  //  - use software (shiftOut(), which is digitalWrite()-based)
  // * if this library is NOT used on an Arduino Uno or with 10 MHz,
  //  postXferDelayMicros possibly needs to be adjusted; see write()
  //  implementation and the library's GitHub Wiki for details
  void beginFast(bool bufferXfer = true, uint32_t spiClock = 10000000,
    unsigned int postXferDelayMicros = 4);
  void beginSlow(unsigned int postXferDelayMicros = 200,
    bool interrupts = false);
  // control of TMGRST bit
  // * separate from begin() functions to reduce their parameters
  // * side effect: resets all brightness values to maximum (127)
  void setTmgrst(bool val = true);

  // the following functions affect the state of this object only (the data
  // is transferred to the chip when write() is called)
  // * index order follows the datasheet
  //   (note: chip 1 is the one directly connected to Arduino)
  //  - channel: 0:R0 (chip 1), 1:G0, ..., 12:R0 (chip 2), ...
  //  - RGB/LED: 0:R0,G0,B0 (chip 1), ..., 4:R0,G0,B0 (chip 2), ...
  //  - tlcIdx: 0:chip 1, 1:chip 2, ...
  // * functions without index set values for all chips and LEDs
  // * brightness values (bcr, bcg, bcb) must be 0-127
  void setChannel(uint16_t idx, uint16_t val);
  void setRGB(uint16_t idx, uint16_t r, uint16_t g, uint16_t b);
  void setRGB(uint16_t r = 0, uint16_t g = 0, uint16_t b = 0);
  void setLED(uint16_t idx, uint16_t r, uint16_t g, uint16_t b) {
    setRGB(idx, r, g, b);
  }
  uint16_t getChannel(uint16_t idx);
  // setLED() is convenience function to allow faster testing/porting of
  // code using Adafruit_TLC59711; the index order for multiple chips is
  // different here, however, and chip 1 has the smallest indices

  void setBrightness(uint16_t tlcIdx, uint8_t bcr, uint8_t bcg, uint8_t bcb);
  void setBrightness(uint8_t bcr = 127, uint8_t bcg = 127, uint8_t bcb = 127);
  void reset() {
    setRGB();
    setBrightness();
  }

  // transfer data to TLC59711; will work only after a call to beginFast() or
  // beginSlow()
  void write();

  // stop using this library
  void end();

 private:
  // const uint16_t idx_lookup;
  const uint16_t numTlc, bufferSz;
  const uint8_t clkPin, dataPin;
  uint16_t *buffer, *buffer2;
  bool beginCalled;
  uint8_t fc;
  bool useSpi_, bufferXfer_, noInterrupts;
  unsigned int postXferDelayMicros_;

  void begin(bool useSpi, unsigned int postXferDelayMicros);
  void xferSpi();
  void xferSpi16();
  void xferShiftOut();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generate lookup table at compile time
// based on stackoverflow questions
// Is it possible to create and initialize an array of values
// using template metaprogramming?
// https://stackoverflow.com/a/2228298/574981
// https://stackoverflow.com/a/37447199/574981

// template <uint16_t idx>
// struct idx_lookup_generator {
//     idx_lookup_generator<idx - 1> rest;
//     static const uint16_t x = (14 * (idx / 12) + idx % 12);
//     // static const uint16_t x = idx * idx;
//     constexpr uint16_t operator[](uint16_t const &i) const {
//         return (i == idx ?  x : rest[i]);
//     }
//     constexpr uint16_t size() const {
//         return idx;
//     }
// };
//
// template <>
// struct idx_lookup_generator<0> {
//     static const uint16_t x = 0;
//     constexpr uint16_t operator[](uint16_t const &i) const { return x; }
//     constexpr uint16_t size() const { return 1; }
// };

// template <int idx>
// struct idx_lookup_generator {
//     idx_lookup_generator<idx - 1> rest;
//     int x;
//     idx_lookup_generator() : x(14 * (idx / 12) + idx % 12) {}
// };
//
// template <>
// struct idx_lookup_generator<1> {
//     int x;
//     idx_lookup_generator() : x(0) {}
// };

// maximum elements is 56173 (as this translates to 65535)
// const idx_lookup_generator<96> idx_lookup_table;

// hardcoded table for up to 1200ch = 100TCL's
const PROGMEM uint16_t idx_lookup_table[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
    84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
    126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
    182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193,
    196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235,
    238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249,
    252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263,
    266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277,
    280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291,
    294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305,
    308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319,
    322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333,
    336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347,
    350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361,
    364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375,
    378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389,
    392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403,
    406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417,
    420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431,
    434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445,
    448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459,
    462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473,
    476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487,
    490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 500, 501,
    504, 505, 506, 507, 508, 509, 510, 511, 512, 513, 514, 515,
    518, 519, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529,
    532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543,
    546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557,
    560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571,
    574, 575, 576, 577, 578, 579, 580, 581, 582, 583, 584, 585,
    588, 589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599,
    602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613,
    616, 617, 618, 619, 620, 621, 622, 623, 624, 625, 626, 627,
    630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641,
    644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655,
    658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669,
    672, 673, 674, 675, 676, 677, 678, 679, 680, 681, 682, 683,
    686, 687, 688, 689, 690, 691, 692, 693, 694, 695, 696, 697,
    700, 701, 702, 703, 704, 705, 706, 707, 708, 709, 710, 711,
    714, 715, 716, 717, 718, 719, 720, 721, 722, 723, 724, 725,
    728, 729, 730, 731, 732, 733, 734, 735, 736, 737, 738, 739,
    742, 743, 744, 745, 746, 747, 748, 749, 750, 751, 752, 753,
    756, 757, 758, 759, 760, 761, 762, 763, 764, 765, 766, 767,
    770, 771, 772, 773, 774, 775, 776, 777, 778, 779, 780, 781,
    784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794, 795,
    798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809,
    812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823,
    826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837,
    840, 841, 842, 843, 844, 845, 846, 847, 848, 849, 850, 851,
    854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865,
    868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879,
    882, 883, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893,
    896, 897, 898, 899, 900, 901, 902, 903, 904, 905, 906, 907,
    910, 911, 912, 913, 914, 915, 916, 917, 918, 919, 920, 921,
    924, 925, 926, 927, 928, 929, 930, 931, 932, 933, 934, 935,
    938, 939, 940, 941, 942, 943, 944, 945, 946, 947, 948, 949,
    952, 953, 954, 955, 956, 957, 958, 959, 960, 961, 962, 963,
    966, 967, 968, 969, 970, 971, 972, 973, 974, 975, 976, 977,
    980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991,
    994, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1005,
    1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019,
    1022, 1023, 1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033,
    1036, 1037, 1038, 1039, 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047,
    1050, 1051, 1052, 1053, 1054, 1055, 1056, 1057, 1058, 1059, 1060, 1061,
    1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071, 1072, 1073, 1074, 1075,
    1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087, 1088, 1089,
    1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103,
    1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117,
    1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131,
    1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145,
    1148, 1149, 1150, 1151, 1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159,
    1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173,
    1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1187,
    1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199, 1200, 1201,
    1204, 1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215,
    1218, 1219, 1220, 1221, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229,
    1232, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243,
    1246, 1247, 1248, 1249, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257,
    1260, 1261, 1262, 1263, 1264, 1265, 1266, 1267, 1268, 1269, 1270, 1271,
    1274, 1275, 1276, 1277, 1278, 1279, 1280, 1281, 1282, 1283, 1284, 1285,
    1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299,
    1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312, 1313,
    1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327,
    1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341,
    1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355,
    1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369,
    1372, 1373, 1374, 1375, 1376, 1377, 1378, 1379, 1380, 1381, 1382, 1383,
    1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397,
};


#endif  // TLC59711_H_
