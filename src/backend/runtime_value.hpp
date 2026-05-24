#ifndef RUNTIME_VALUE_HPP
#define RUNTIME_VALUE_HPP

#include <string>

/**
 * @file runtime_value.hpp
 * @brief Representasi nilai yang hidup di stack saat runtime.
 *
 * Bahasa Arion adalah strongly-typed sehingga setiap RuntimeValue
 * menyimpan tipe eksplisit. Ini digunakan untuk:
 * - Operasi aritmatika dan logika di Interpreter
 * - Output program (WRT/WRTLN)
 * - Validasi tipe saat runtime (meski Arion strongly-typed,
 *   interpreter tetap perlu tahu tipe untuk operasi yang benar)
 *
 * Catatan untuk Interpreter (Orang 4):
 * - Variabel yang belum di-assign memiliki type == ValueType::NIL.
 *   Selalu periksa isNil() sebelum menggunakan nilai variabel untuk
 *   mendeteksi UNDEFINED_VARIABLE error.
 * - Untuk operasi perbandingan (EQL, NEQ, LSS, dll), gunakan operator
 *   == dan < yang sudah disediakan atau akses field ival/dval/bval/cval/sval
 *   sesuai tipe yang diketahui dari Decorated AST.
 * - Untuk JPC, gunakan toBool() yang sudah menangani INTEGER dan BOOLEAN.
 */

/**
 * @brief Tipe nilai yang bisa hidup di stack runtime.
 *
 * NIL     : nilai belum diinisialisasi / kosong
 * INTEGER : bilangan bulat (int 32-bit)
 * REAL    : bilangan real (double)
 * BOOLEAN : nilai boolean (true/false)
 * CHAR    : karakter tunggal
 * STRING  : string teks
 */
enum class ValueType {
    NIL,
    INTEGER,
    REAL,
    BOOLEAN,
    CHAR,
    STRING
};

/**
 * @brief Satu nilai di stack runtime.
 *
 * - Tidak menggunakan union C karena string tidak bisa masuk union.
 * - Semua field selalu ada, hanya field yang sesuai `type` yang valid.
 * - Factory methods digunakan agar pembuatan nilai selalu konsisten.
 */
struct RuntimeValue {
    ValueType   type  = ValueType::NIL;
    int         ival  = 0;
    double      dval  = 0.0;
    bool        bval  = false;
    char        cval  = '\0';
    std::string sval  = "";

    RuntimeValue() = default;

    /** @brief Buat RuntimeValue bertipe INTEGER. */
    static RuntimeValue fromInt(int v);

    /** @brief Buat RuntimeValue bertipe REAL. */
    static RuntimeValue fromReal(double v);

    /** @brief Buat RuntimeValue bertipe BOOLEAN. */
    static RuntimeValue fromBool(bool v);

    /** @brief Buat RuntimeValue bertipe CHAR. */
    static RuntimeValue fromChar(char v);

    /** @brief Buat RuntimeValue bertipe STRING. */
    static RuntimeValue fromString(const std::string& v);

    /**
     * @brief Mengubah nilai menjadi string untuk output program (WRT/WRTLN).
     *
     * - INTEGER : "42"
     * - REAL    : "3.14" (tanpa trailing zero berlebih, selalu ada titik desimal)
     * - BOOLEAN : "TRUE" / "FALSE"
     * - CHAR    : satu karakter
     * - STRING  : isi string apa adanya
     * - NIL     : "(nil)"
     */
    std::string toString() const;

    /**
     * @brief Mengubah nilai menjadi boolean untuk keperluan JPC.
     *
     * - BOOLEAN : nilai apa adanya
     * - INTEGER : 0 → false, selainnya → true
     * - Tipe lain: selalu true (tidak seharusnya terjadi di Arion)
     */
    bool toBool() const;

    /**
     * @brief Mengubah nilai menjadi integer.
     * Valid untuk INTEGER, BOOLEAN, CHAR, dan REAL (truncate).
     * Throws std::runtime_error untuk NIL dan STRING.
     */
    int toInt() const;

    /**
     * @brief Mengubah nilai menjadi double.
     * Valid untuk INTEGER dan REAL.
     * Throws std::runtime_error untuk tipe lain.
     */
    double toReal() const;

    /**
     * @brief Operator perbandingan kesamaan antar dua RuntimeValue.
     *
     * Aturan:
     * - Tipe sama     : bandingkan field yang relevan.
     * - INTEGER vs REAL (atau sebaliknya) : bandingkan sebagai double.
     * - Tipe tidak kompatibel : selalu false.
     */
    bool operator==(const RuntimeValue& other) const;
    bool operator!=(const RuntimeValue& other) const;

    /**
     * @brief Operator perbandingan urutan antar dua RuntimeValue.
     *
     * Aturan:
     * - INTEGER, REAL, CHAR : bandingkan nilai numerik/ordinal.
     * - STRING              : bandingkan lexicographic.
     * - BOOLEAN, NIL        : tidak didukung, selalu false.
     * - INTEGER vs REAL     : bandingkan sebagai double.
     */
    bool operator<(const RuntimeValue& other) const;
    bool operator<=(const RuntimeValue& other) const;
    bool operator>(const RuntimeValue& other) const;
    bool operator>=(const RuntimeValue& other) const;

    bool isNil()     const { return type == ValueType::NIL; }
    bool isInt()     const { return type == ValueType::INTEGER; }
    bool isReal()    const { return type == ValueType::REAL; }
    bool isBool()    const { return type == ValueType::BOOLEAN; }
    bool isChar()    const { return type == ValueType::CHAR; }
    bool isString()  const { return type == ValueType::STRING; }
    bool isNumeric() const { return type == ValueType::INTEGER || type == ValueType::REAL; }

    std::string typeName() const;
};

/**
 * @brief Mengubah ValueType menjadi string.
 * Contoh: ValueType::INTEGER → "INTEGER"
 */
std::string valueTypeToString(ValueType vt);

#endif