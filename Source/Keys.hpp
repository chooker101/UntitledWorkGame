#pragma once

namespace ASCII
{
	constexpr unsigned char SOH = 0x01;
	constexpr unsigned char STX = 0x02;
	constexpr unsigned char ETX = 0x03;
	constexpr unsigned char EOT = 0x04;
	constexpr unsigned char ENQ = 0x05;
	constexpr unsigned char ACK = 0x06;
	constexpr unsigned char BEL = 0x07;
	constexpr unsigned char BS = 0x08;
}
namespace KEYS
{
	constexpr unsigned char TAB = 0x09;
}
namespace ASCII
{
	constexpr unsigned char LF = 0x0A;
	constexpr unsigned char VT = 0x0B;
	constexpr unsigned char FF = 0x0C;
	constexpr unsigned char CR = 0x0D;
	constexpr unsigned char SO = 0x0E;
	constexpr unsigned char SI = 0x0F;
	constexpr unsigned char DLE = 0x10;
	constexpr unsigned char DC1 = 0x11;
	constexpr unsigned char DC2 = 0x12;
	constexpr unsigned char DC3 = 0x13;
	constexpr unsigned char DC4 = 0x14;
	constexpr unsigned char NAK = 0x15;
	constexpr unsigned char SYN = 0x16;
	constexpr unsigned char ETB = 0x17;
	constexpr unsigned char CAN = 0x18;
	constexpr unsigned char EM = 0x19;
	constexpr unsigned char SUB = 0x1A;
}
namespace KEYS
{
	constexpr unsigned char ESC = 0x1B;
}
namespace ASCII
{
	constexpr unsigned char FS = 0x1C;
	constexpr unsigned char GS = 0x1D;
	constexpr unsigned char RS = 0x1E;
	constexpr unsigned char US = 0x1F;
}
namespace KEYS
{
	constexpr unsigned char Space = 0x20;
}
namespace ASCII
{
	constexpr unsigned char Exclamation = 0x21;
	constexpr unsigned char Quotation = 0x22;
	constexpr unsigned char Hash = 0x23;
	constexpr unsigned char Dollar = 0x24;
	constexpr unsigned char Percent = 0x25;
	constexpr unsigned char And = 0x26;
	constexpr unsigned char Apostrophe = 0x27;
	constexpr unsigned char OpenPar = 0x28;
	constexpr unsigned char ClosePar = 0x29;
	constexpr unsigned char Star = 0x2A;
	constexpr unsigned char Plus = 0x2B;
}
namespace KEYS
{
	constexpr unsigned char Comma = 0x2C;
	constexpr unsigned char Dash = 0x2D;
	constexpr unsigned char Period = 0x2E, Dot = 0x2E;
	constexpr unsigned char ForwardSlash = 0x2F;
	constexpr unsigned char Num0 = 0x30;
	constexpr unsigned char Num1 = 0x31;
	constexpr unsigned char Num2 = 0x32;
	constexpr unsigned char Num3 = 0x33;
	constexpr unsigned char Num4 = 0x34;
	constexpr unsigned char Num5 = 0x35;
	constexpr unsigned char Num6 = 0x36;
	constexpr unsigned char Num7 = 0x37;
	constexpr unsigned char Num8 = 0x38;
	constexpr unsigned char Num9 = 0x39;
}
namespace ASCII
{
	constexpr unsigned char Colon = 0x3A;
}
namespace KEYS
{
	constexpr unsigned char SemiColon = 0x3B;
}
namespace ASCII
{
	constexpr unsigned char LessThan = 0x3C;
}
namespace KEYS
{
	constexpr unsigned char Equals = 0x3D;
}
namespace ASCII
{
	constexpr unsigned char GreaterThan = 0x3E;
	constexpr unsigned char Question = 0x3F;
	constexpr unsigned char At = 0x40;
}
namespace KEYS
{
	constexpr unsigned char A = 0x41;
	constexpr unsigned char B = 0x42;
	constexpr unsigned char C = 0x43;
	constexpr unsigned char D = 0x44;
	constexpr unsigned char E = 0x45;
	constexpr unsigned char F = 0x46;
	constexpr unsigned char G = 0x47;
	constexpr unsigned char H = 0x48;
	constexpr unsigned char I = 0x49;
	constexpr unsigned char J = 0x4A;
	constexpr unsigned char K = 0x4B;
	constexpr unsigned char L = 0x4C;
	constexpr unsigned char M = 0x4D;
	constexpr unsigned char N = 0x4E;
	constexpr unsigned char O = 0x4F;
	constexpr unsigned char P = 0x50;
	constexpr unsigned char Q = 0x51;
	constexpr unsigned char R = 0x52;
	constexpr unsigned char S = 0x53;
	constexpr unsigned char T = 0x54;
	constexpr unsigned char U = 0x55;
	constexpr unsigned char V = 0x56;
	constexpr unsigned char W = 0x57;
	constexpr unsigned char X = 0x58;
	constexpr unsigned char Y = 0x59;
	constexpr unsigned char Z = 0x5A;
	constexpr unsigned char OpenBracket = 0x5B;
	constexpr unsigned char BackSlash = 0x5C;
	constexpr unsigned char CloseBracket = 0x5D;
}
namespace ASCII
{
	constexpr unsigned char Caret = 0x5E;
	constexpr unsigned char UnderScore = 0x5F;
}
namespace KEYS
{
	constexpr unsigned char Grave = 0x60;
}
namespace ASCII
{
	constexpr unsigned char a = 0x61;
	constexpr unsigned char b = 0x62;
	constexpr unsigned char c = 0x63;
	constexpr unsigned char d = 0x64;
	constexpr unsigned char e = 0x65;
	constexpr unsigned char f = 0x66;
	constexpr unsigned char g = 0x67;
	constexpr unsigned char h = 0x68;
	constexpr unsigned char i = 0x69;
	constexpr unsigned char j = 0x6A;
	constexpr unsigned char k = 0x6B;
	constexpr unsigned char l = 0x6C;
	constexpr unsigned char m = 0x6D;
	constexpr unsigned char n = 0x6E;
	constexpr unsigned char o = 0x6F;
	constexpr unsigned char p = 0x70;
	constexpr unsigned char q = 0x71;
	constexpr unsigned char r = 0x72;
	constexpr unsigned char s = 0x73;
	constexpr unsigned char t = 0x74;
	constexpr unsigned char u = 0x75;
	constexpr unsigned char v = 0x76;
	constexpr unsigned char w = 0x77;
	constexpr unsigned char x = 0x78;
	constexpr unsigned char y = 0x79;
	constexpr unsigned char z = 0x7A;
	constexpr unsigned char OpenBrace = 0x7B;
	constexpr unsigned char Or = 0x7C;
	constexpr unsigned char CloseBrace = 0x7D;
	constexpr unsigned char Tildi = 0x7E;
}
namespace KEYS
{
	constexpr unsigned char DEL = 0x7F;
}