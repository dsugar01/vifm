#ifndef __BRACKET_NOTATION_H__
#define __BRACKET_NOTATION_H__

#include "../config.h"

#include <curses.h>

#include <wchar.h>

struct key_pair {
    const char *notation;
    const wchar_t key[8];
    size_t len;
} key_pairs[] = {
    { "<c-a>",      L"\x01"              },
    { "<c-b>",      L"\x02"              },
    { "<c-c>",      L"\x03"              },
    { "<c-d>",      L"\x04"              },
    { "<c-e>",      L"\x05"              },
    { "<c-f>",      L"\x06"              },
    { "<c-g>",      L"\x07"              },
    { "<c-h>",      L"\x08"              },
    { "<c-i>",      L"\x09"              },
    { "<c-j>",      L"\x0a"              },
    { "<c-k>",      L"\x0b"              },
    { "<c-l>",      L"\x0c"              },
    { "<c-m>",      L"\x0d"              },
    { "<c-n>",      L"\x0e"              },
    { "<c-o>",      L"\x0f"              },
    { "<c-p>",      L"\x10"              },
    { "<c-q>",      L"\x11"              },
    { "<c-r>",      L"\x12"              },
    { "<c-s>",      L"\x13"              },
    { "<c-t>",      L"\x14"              },
    { "<c-u>",      L"\x15"              },
    { "<c-v>",      L"\x16"              },
    { "<c-w>",      L"\x17"              },
    { "<c-x>",      L"\x18"              },
    { "<c-y>",      L"\x19"              },
    { "<c-z>",      L"\x1a"              },
    { "<c-[>",      L"\x1b"              },
    { "<c-\\>",     L"\x1c"              },
    { "<c-]>",      L"\x1d"              },
    { "<c-^>",      L"\x1e"              },
    { "<c-_>",      L"\x1f"              },
    { "<cr>",       L"\r"                },
    { "<space>",    L" "                 },
    { "<tab>",      L"\t"                },
#ifdef ENABLE_EXTENDED_KEYS
    { "<s-tab>",    { KEY_BTAB, 0 }      },
#else
    { "<s-tab>",    L"\033"L"[Z"         },
#endif
    { "<a-a>",      L"\033"L"a"          },
    { "<a-b>",      L"\033"L"b"          },
    { "<a-c>",      L"\033"L"c"          },
    { "<a-d>",      L"\033"L"d"          },
    { "<a-e>",      L"\033"L"e"          },
    { "<a-f>",      L"\033"L"f"          },
    { "<a-g>",      L"\033"L"g"          },
    { "<a-h>",      L"\033"L"h"          },
    { "<a-i>",      L"\033"L"i"          },
    { "<a-j>",      L"\033"L"j"          },
    { "<a-k>",      L"\033"L"k"          },
    { "<a-l>",      L"\033"L"l"          },
    { "<a-m>",      L"\033"L"m"          },
    { "<a-n>",      L"\033"L"n"          },
    { "<a-o>",      L"\033"L"o"          },
    { "<a-p>",      L"\033"L"p"          },
    { "<a-q>",      L"\033"L"q"          },
    { "<a-r>",      L"\033"L"r"          },
    { "<a-s>",      L"\033"L"s"          },
    { "<a-t>",      L"\033"L"t"          },
    { "<a-u>",      L"\033"L"u"          },
    { "<a-v>",      L"\033"L"v"          },
    { "<a-w>",      L"\033"L"w"          },
    { "<a-x>",      L"\033"L"x"          },
    { "<a-y>",      L"\033"L"y"          },
    { "<a-z>",      L"\033"L"z"          },
    { "<m-a>",      L"\033"L"a"          },
    { "<m-b>",      L"\033"L"b"          },
    { "<m-c>",      L"\033"L"c"          },
    { "<m-d>",      L"\033"L"d"          },
    { "<m-e>",      L"\033"L"e"          },
    { "<m-f>",      L"\033"L"f"          },
    { "<m-g>",      L"\033"L"g"          },
    { "<m-h>",      L"\033"L"h"          },
    { "<m-i>",      L"\033"L"i"          },
    { "<m-j>",      L"\033"L"j"          },
    { "<m-k>",      L"\033"L"k"          },
    { "<m-l>",      L"\033"L"l"          },
    { "<m-m>",      L"\033"L"m"          },
    { "<m-n>",      L"\033"L"n"          },
    { "<m-o>",      L"\033"L"o"          },
    { "<m-p>",      L"\033"L"p"          },
    { "<m-q>",      L"\033"L"q"          },
    { "<m-r>",      L"\033"L"r"          },
    { "<m-s>",      L"\033"L"s"          },
    { "<m-t>",      L"\033"L"t"          },
    { "<m-u>",      L"\033"L"u"          },
    { "<m-v>",      L"\033"L"v"          },
    { "<m-w>",      L"\033"L"w"          },
    { "<m-x>",      L"\033"L"x"          },
    { "<m-y>",      L"\033"L"y"          },
    { "<m-z>",      L"\033"L"z"          },
    { "<del>",      L"\177"              },
#ifdef ENABLE_EXTENDED_KEYS
    { "<home>",     { KEY_HOME,      0 } },
    { "<end>",      { KEY_END,       0 } },
    { "<left>",     { KEY_LEFT,      0 } },
    { "<right>",    { KEY_RIGHT,     0 } },
    { "<up>",       { KEY_UP,        0 } },
    { "<down>",     { KEY_DOWN,      0 } },
    { "<bs>",       { KEY_BACKSPACE, 0 } },
    { "<delete>",   { KEY_DC,        0 } },
    { "<pageup>",   { KEY_PPAGE,     0 } },
    { "<pagedown>", { KEY_NPAGE,     0 } },
    { "<f0>",       { KEY_F(0),    0 } },
    { "<f1>",       { KEY_F(1),    0 } },
    { "<f2>",       { KEY_F(2),    0 } },
    { "<f3>",       { KEY_F(3),    0 } },
    { "<f4>",       { KEY_F(4),    0 } },
    { "<f5>",       { KEY_F(5),    0 } },
    { "<f6>",       { KEY_F(6),    0 } },
    { "<f7>",       { KEY_F(7),    0 } },
    { "<f8>",       { KEY_F(8),    0 } },
    { "<f9>",       { KEY_F(9),    0 } },
    { "<f10>",      { KEY_F(10),   0 } },
    { "<f11>",      { KEY_F(11),   0 } },
    { "<f12>",      { KEY_F(12),   0 } },
    { "<f13>",      { KEY_F(13),   0 } },
    { "<f14>",      { KEY_F(14),   0 } },
    { "<f15>",      { KEY_F(15),   0 } },
    { "<f16>",      { KEY_F(16),   0 } },
    { "<f17>",      { KEY_F(17),   0 } },
    { "<f18>",      { KEY_F(18),   0 } },
    { "<f19>",      { KEY_F(19),   0 } },
    { "<f20>",      { KEY_F(20),   0 } },
    { "<f21>",      { KEY_F(21),   0 } },
    { "<f22>",      { KEY_F(22),   0 } },
    { "<f23>",      { KEY_F(23),   0 } },
    { "<f24>",      { KEY_F(24),   0 } },
    { "<f25>",      { KEY_F(25),   0 } },
    { "<f26>",      { KEY_F(26),   0 } },
    { "<f27>",      { KEY_F(27),   0 } },
    { "<f28>",      { KEY_F(28),   0 } },
    { "<f29>",      { KEY_F(29),   0 } },
    { "<f30>",      { KEY_F(30),   0 } },
    { "<f31>",      { KEY_F(31),   0 } },
    { "<f32>",      { KEY_F(32),   0 } },
    { "<f33>",      { KEY_F(33),   0 } },
    { "<f34>",      { KEY_F(34),   0 } },
    { "<f35>",      { KEY_F(35),   0 } },
    { "<f36>",      { KEY_F(36),   0 } },
    { "<f37>",      { KEY_F(37),   0 } },
    { "<f38>",      { KEY_F(38),   0 } },
    { "<f39>",      { KEY_F(39),   0 } },
    { "<f40>",      { KEY_F(40),   0 } },
    { "<f41>",      { KEY_F(41),   0 } },
    { "<f42>",      { KEY_F(42),   0 } },
    { "<f43>",      { KEY_F(43),   0 } },
    { "<f44>",      { KEY_F(44),   0 } },
    { "<f45>",      { KEY_F(45),   0 } },
    { "<f46>",      { KEY_F(46),   0 } },
    { "<f47>",      { KEY_F(47),   0 } },
    { "<f48>",      { KEY_F(48),   0 } },
    { "<f49>",      { KEY_F(49),   0 } },
    { "<f50>",      { KEY_F(50),   0 } },
    { "<f51>",      { KEY_F(51),   0 } },
    { "<f52>",      { KEY_F(52),   0 } },
    { "<f53>",      { KEY_F(53),   0 } },
    { "<f54>",      { KEY_F(54),   0 } },
    { "<f55>",      { KEY_F(55),   0 } },
    { "<f56>",      { KEY_F(56),   0 } },
    { "<f57>",      { KEY_F(57),   0 } },
    { "<f58>",      { KEY_F(58),   0 } },
    { "<f59>",      { KEY_F(59),   0 } },
    { "<f60>",      { KEY_F(60),   0 } },
    { "<f61>",      { KEY_F(61),   0 } },
    { "<f62>",      { KEY_F(62),   0 } },
    { "<f63>",      { KEY_F(63),   0 } },
    { "<s-f1>",     { KEY_F(13),   0 } },
    { "<s-f2>",     { KEY_F(14),   0 } },
    { "<s-f3>",     { KEY_F(15),   0 } },
    { "<s-f4>",     { KEY_F(16),   0 } },
    { "<s-f5>",     { KEY_F(17),   0 } },
    { "<s-f6>",     { KEY_F(18),   0 } },
    { "<s-f7>",     { KEY_F(19),   0 } },
    { "<s-f8>",     { KEY_F(20),   0 } },
    { "<s-f9>",     { KEY_F(21),   0 } },
    { "<s-f10>",    { KEY_F(22),   0 } },
    { "<s-f11>",    { KEY_F(23),   0 } },
    { "<s-f12>",    { KEY_F(24),   0 } },
    { "<c-f1>",     { KEY_F(25),   0 } },
    { "<c-f2>",     { KEY_F(26),   0 } },
    { "<c-f3>",     { KEY_F(27),   0 } },
    { "<c-f4>",     { KEY_F(28),   0 } },
    { "<c-f5>",     { KEY_F(29),   0 } },
    { "<c-f6>",     { KEY_F(30),   0 } },
    { "<c-f7>",     { KEY_F(31),   0 } },
    { "<c-f8>",     { KEY_F(32),   0 } },
    { "<c-f9>",     { KEY_F(33),   0 } },
    { "<c-f10>",    { KEY_F(34),   0 } },
    { "<c-f11>",    { KEY_F(35),   0 } },
    { "<c-f12>",    { KEY_F(36),   0 } },
    { "<a-f1>",     { KEY_F(37),   0 } },
    { "<a-f2>",     { KEY_F(38),   0 } },
    { "<a-f3>",     { KEY_F(39),   0 } },
    { "<a-f4>",     { KEY_F(40),   0 } },
    { "<a-f5>",     { KEY_F(41),   0 } },
    { "<a-f6>",     { KEY_F(42),   0 } },
    { "<a-f7>",     { KEY_F(43),   0 } },
    { "<a-f8>",     { KEY_F(44),   0 } },
    { "<a-f9>",     { KEY_F(45),   0 } },
    { "<a-f10>",    { KEY_F(46),   0 } },
    { "<a-f11>",    { KEY_F(47),   0 } },
    { "<a-f12>",    { KEY_F(48),   0 } },
#endif
};

#endif
