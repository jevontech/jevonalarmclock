/* 
 * @file  Util.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */


#ifndef UTIL_H
#define	UTIL_H

struct HexCharStruct {
    unsigned char c;

    HexCharStruct(unsigned char _c) : c(_c) {
    }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << (int) hs.c);
}

inline HexCharStruct hex(unsigned char _c) {
    return HexCharStruct(_c);
}

static vector<unsigned char> IntegerToBytes(int integer) {
    vector<unsigned char> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[i] = (integer >> (i * 8));
    return arrayOfByte;
}

static vector<unsigned char> ShortToBytes(short shortinteger) {
    vector<unsigned char> arrayOfByte(2);
    for (int i = 0; i < 2; i++)
        arrayOfByte[i] = (shortinteger >> (i * 8));
    return arrayOfByte;
}

static string const BoolToString(bool b) {
    return b ? "true" : "false";
}


 


#endif	/* UTIL_H */

