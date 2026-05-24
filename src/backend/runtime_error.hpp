#ifndef RUNTIME_ERROR_HPP
#define RUNTIME_ERROR_HPP

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief Definisi dan koleksi error runtime untuk interpreter bahasa Arion.
 *
 * Terdiri dari dua jenis error:
 * 1. Error WAJIB (harus dideteksi sesuai spesifikasi M4)
 * 2. Error BONUS (kerentanan interpreter)
 */

/**
 * @brief Jenis-jenis error runtime.
 *
 * === ERROR WAJIB ===
 * DIVISION_BY_ZERO   : pembagian atau modulus dengan 0
 * TYPE_MISMATCH      : operasi pada tipe yang tidak kompatibel
 * UNDEFINED_VARIABLE : akses variabel yang belum diinisialisasi
 *
 * === ERROR BONUS ===
 * STACK_OVERFLOW     : stack melebihi MAX_STACK_SIZE (biasanya infinite recursion)
 * STACK_UNDERFLOW    : pop dari stack yang sudah kosong
 * STACK_SMASHING     : penulisan melampaui batas variabel lokal (buffer overflow)
 * STACK_CORRUPTION   : jumlah push/pop tidak simetris, frame rusak
 * OOB_ACCESS         : akses indeks array di luar batas (bonus spesifikasi)
 * INVALID_JUMP       : instruksi JMP/JPC melompat ke baris yang tidak ada
 * NUMERIC_OVERFLOW   : hasil operasi aritmatika melampaui batas INTEGER
 * NUMERIC_UNDERFLOW  : hasil operasi aritmatika di bawah batas minimum INTEGER
 */
enum class RuntimeErrorType {
    DIVISION_BY_ZERO,
    TYPE_MISMATCH,
    UNDEFINED_VARIABLE,

    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    STACK_SMASHING,
    STACK_CORRUPTION,
    OOB_ACCESS,
    INVALID_JUMP,
    NUMERIC_OVERFLOW,
    NUMERIC_UNDERFLOW
};

/**
 * @brief Satu kejadian error runtime.
 *
 * @param type    Jenis error
 * @param message Pesan deskriptif untuk ditampilkan ke user
 * @param pc      Program Counter saat error terjadi (-1 jika tidak relevan)
 */
struct RuntimeError {
    RuntimeErrorType type;
    std::string      message;
    int              pc;

    RuntimeError(RuntimeErrorType type, const std::string& message, int pc = -1);

    /**
     * @brief Format error untuk ditampilkan ke terminal.
     * Contoh: "[RUNTIME ERROR] DIVISION_BY_ZERO at PC=5: Pembagian dengan nol"
     */
    std::string toString() const;
};

/**
 * @brief Exception yang dilempar saat terjadi error fatal di runtime.
 *
 * Dilempar oleh StackMachine atau Interpreter ketika menemukan error
 * yang tidak bisa dilanjutkan (misalnya Stack Overflow, Division by Zero).
 * Ditangkap di Interpreter::run() untuk menghentikan eksekusi dengan bersih.
 */
class ArionRuntimeException : public std::runtime_error {
public:
    RuntimeErrorType errorType;
    int              pc;

    ArionRuntimeException(RuntimeErrorType type,
                          const std::string& message,
                          int pc = -1);
};

/**
 * @brief Mengumpulkan dan menampilkan semua error runtime.
 *
 * Digunakan oleh Interpreter untuk mencatat error yang terjadi
 * sebelum menghentikan eksekusi.
 */
class RuntimeErrorCollector {
private:
    std::vector<RuntimeError> errors;

public:
    /**
     * @brief Tambahkan satu error ke koleksi.
     */
    void add(RuntimeErrorType type, const std::string& message, int pc = -1);

    /**
     * @brief Cek apakah ada error yang terkumpul.
     */
    bool hasErrors() const;

    /**
     * @brief Jumlah error yang terkumpul.
     */
    int count() const;

    /**
     * @brief Tampilkan semua error ke stderr.
     */
    void printAll() const;

    /**
     * @brief Lempar ArionRuntimeException dengan error pertama yang terkumpul.
     * Digunakan untuk menghentikan eksekusi secara immediate.
     * Tidak melakukan apa-apa jika tidak ada error.
     */
    void throwFirst() const;

    /**
     * @brief Akses error pertama (untuk keperluan reporting).
     * Precondition: hasErrors() == true
     */
    const RuntimeError& first() const;

    /**
     * @brief Reset koleksi error (untuk testing).
     */
    void clear();
};

/**
 * @brief Mengubah RuntimeErrorType menjadi string nama error.
 * Contoh: RuntimeErrorType::DIVISION_BY_ZERO → "DIVISION_BY_ZERO"
 */
std::string runtimeErrorTypeToString(RuntimeErrorType type);

#endif 