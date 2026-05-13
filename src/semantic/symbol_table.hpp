#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <vector>

enum class ObjClass {
    CONSTANT = 0,
    VARIABLE = 1,
    TYPE = 2,
    PROCEDURE = 3,
    FUNCTION = 4,
    PROGRAM = 5, // identifier program utama
    FIELD = 6
};

enum class TypeCode {
    NOTYPE = 0, // belum diketahui atau void
    INTEGER = 1,
    REAL = 2,
    CHAR = 3,
    BOOLEAN = 4,
    STRING = 5,
    ARRAY = 6, // detail ada di atab[ref]
    RECORD = 7, // detail ada di btab[ref]
    SUBRANGE = 8, // detail ada di atab[ref]
    ENUMERATED = 9 // detail ada di btab[ref]
};

struct TabEntry {
    std::string identifier;
    int link; // indeks ke identifier sebelumnya di blok yang sama
    ObjClass obj;
    TypeCode type;
    int ref; // indeks ke atab (array/subrange) atau btab (record/procedure block)
    int nrm; // 1 = normal, 0 = pass by reference
    int lev; // lexical level, kalau 0 = global
    int adr; // offset variabel/nilai konstanta/ukuran lain
    TabEntry();
};

struct AtabEntry {
    int xtyp; // tipe indeks
    int etyp; // tipe elemen
    int eref; // ref untuk tipe elemen komposit
    int low;  // batas bawah indeks
    int high; // batas atas indeks
    int elsz; // ukuran satu elemen (byte/unit memori)
    int size; // total ukuran array = (high-low+1)*elsz
    AtabEntry();
};

struct BtabEntry {
    int last; // indeks tab ke identifier terakhir yang dideklarasikan di blok ini
    int lpar; // indeks tab ke parameter terakhir (0 jika bukan prosedur/fungsi)
    int psze; // total ukuran parameter block
    int vsze; // total ukuran variabel lokal block
    BtabEntry();
};

class SymbolTable {
public:
    std::vector<TabEntry> tab; // tab[0..31] = reserved words, tab[32+] = identifier user
    std::vector<AtabEntry> atab; // diakses via ref pada TabEntry
    std::vector<BtabEntry> btab; // btab[0] = global block
private:
    std::vector<int> display;
    int currentLevel; // lexical level saat ini
    int tabPtr; // indeks tab berikutnya yang bisa diisi
    int atabPtr; // indeks atab berikutnya
    int btabPtr; // indeks btab berikutnya
public:
    SymbolTable();
    int enter_block();
    void exit_block();
    int currentLev() const;
    int currentBlock() const;
    int insert(const std::string& id, ObjClass obj, TypeCode type, int ref = 0, int nrm = 1, int adr = 0);
    int lookup(const std::string& id) const;
    int lookupCurrentBlock(const std::string& id) const;
    int insertArray(int xtyp, int etyp, int eref, int low, int high, int elsz);
    void printTab() const;
    void printAtab() const;
    void printBtab() const;
private:
    void initReservedWords();
    void initPredefined();
    int sizeOf(TypeCode t) const;
};

#endif