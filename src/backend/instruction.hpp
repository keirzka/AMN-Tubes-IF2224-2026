#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <vector>

/**
 * @file instruction.hpp
 * @brief Definisi instruksi Intermediate Code (IC) untuk bahasa Arion.
 *
 * Format instruksi mengikuti konvensi Stack Machine:
 *   <line> <mnemonic> <level> <operand>
 *
 * Contoh:
 *   0  INT  0  5
 *   1  LIT  0  10
 *   2  STO  0  3
 */
/**
 * @brief Daftar mnemonic instruksi IC.
 *
 * LIT  : Load Literal          — push nilai konstan ke stack
 * LOD  : Load Value            — push nilai dari address memori
 * STO  : Store Value           — pop dari stack, simpan ke address memori
 * CAL  : Call                  — panggil prosedur/fungsi di baris l
 * INT  : Initiate Memory       — alokasi frame memori sebesar m
 * JMP  : Unconditional Jump    — lompat tanpa syarat ke baris l
 * JPC  : Conditional Jump      — lompat ke baris l jika nilai teratas stack == 0 (false)
 * OPR  : Operation             — jalankan operasi aritmatika/logika/IO
 * RET  : Return                — kembali dari prosedur/fungsi
 */
enum class Mnemonic {
    LIT,
    LOD,
    STO,
    CAL,
    INT,
    JMP,
    JPC,
    OPR,
    RET
};

/**
 * @brief Kode operasi untuk instruksi OPR.
 *
 * Nomor operasi mengikuti spesifikasi tugas:
 *   1  NEG   : Negasi (unary minus)
 *   2  ADD   : Penjumlahan
 *   3  SUB   : Pengurangan
 *   4  MUL   : Perkalian
 *   5  DIV   : Pembagian integer
 *   6  MOD   : Modulus
 *   7  EQL   : Sama dengan (==)
 *   8  NEQ   : Tidak sama dengan (<>)
 *   9  LSS   : Kurang dari (<)
 *   10 GEQ   : Lebih dari atau sama dengan (>=)
 *   11 GTR   : Lebih dari (>)
 *   12 LEQ   : Kurang dari atau sama dengan (<=)
 *   13 WRT   : Write (tanpa newline)
 *   14 WRTLN : Writeln (dengan newline)
 */
enum class OprCode {
    NEG   = 1,
    ADD   = 2,
    SUB   = 3,
    MUL   = 4,
    DIV   = 5,
    MOD   = 6,
    EQL   = 7,
    NEQ   = 8,
    LSS   = 9,
    GEQ   = 10,
    GTR   = 11,
    LEQ   = 12,
    WRT   = 13,
    WRTLN = 14
};

/**
 * @brief Satu baris instruksi Intermediate Code.
 *
 * Format output: "<line> <mnemonic> <level> <operand>"
 * Contoh       : "0 INT 0 5"
 *
 * Catatan:
 * - Untuk OPR, field `operand` diisi dengan nilai numerik OprCode (1-14).
 * - Untuk JMP/JPC, field `operand` diisi dengan nomor baris tujuan lompatan.
 * - Untuk LIT, field `operand` diisi dengan nilai literal.
 * - Untuk LOD/STO, field `operand` diisi dengan address memori.
 * - Untuk CAL, field `operand` diisi dengan nomor baris prosedur/fungsi.
 * - Untuk INT, field `operand` diisi dengan ukuran frame.
 * - Untuk RET, field `level` dan `operand` biasanya 0.
 */
struct Instruction {
    int      line;     
    Mnemonic mnemonic; 
    int      level;    
    int      operand;  

    /**
     * @brief Konstruktor instruksi.
     */
    Instruction(int line, Mnemonic mnemonic, int level, int operand);

    /**
     * @brief Mengubah instruksi menjadi string untuk keperluan print/save IC.
     * Format: "<line> <mnemonic> <level> <operand>"
     * Contoh: "0 INT 0 5"
     */
    std::string toString() const;
};

/**
 * @brief Mengubah Mnemonic menjadi string.
 * Contoh: Mnemonic::LIT → "LIT"
 */
std::string mnemonicToString(Mnemonic m);

/**
 * @brief Mengubah OprCode menjadi string nama operasi.
 * Contoh: OprCode::ADD → "ADD"
 */
std::string oprCodeToString(OprCode o);

/**
 * @brief Mengubah integer ke OprCode.
 * Digunakan oleh Interpreter saat membaca operand instruksi OPR.
 * Throws std::invalid_argument jika nilai tidak valid (di luar 1-14).
 */
OprCode intToOprCode(int val);

#endif 