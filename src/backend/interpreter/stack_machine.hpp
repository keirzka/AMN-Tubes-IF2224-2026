#ifndef STACK_MACHINE_HPP
#define STACK_MACHINE_HPP

#include <vector>
#include "backend/runtime_value.hpp"
#include "backend/runtime_error.hpp"

/**
 * @file stack_machine.hpp
 * @brief Stack Machine untuk eksekusi Intermediate Code bahasa Arion.
 *
 * Arsitektur memori mengikuti konvensi ORKOM/PL0:
 *
 * Layout satu Stack Frame (dimulai dari BP):
 *   BP+0 : Static Link  (BP frame scope luar, untuk akses variabel non-lokal)
 *   BP+1 : Dynamic Link (BP frame pemanggil, untuk kembali setelah RET)
 *   BP+2 : Return Address (PC instruksi setelah CAL)
 *   BP+3 : variabel lokal pertama
 *   BP+4 : variabel lokal kedua
 *   ...
 *   SP   : elemen teratas stack (operand sementara / variabel lokal terakhir)
 *
 * Alur pemanggilan prosedur/fungsi:
 *   1. Interpreter memanggil enterFrame(SL, DL, RA) → push 3 header, update BP
 *   2. Instruksi INT m → allocFrame(m) → push m-3 slot variabel kosong
 *   3. Eksekusi badan prosedur/fungsi
 *   4. Instruksi RET → exitFrame() → pulihkan SP dan BP, kembalikan RA
 *
 * Alur program utama:
 *   1. Interpreter memanggil initMainFrame() sebelum eksekusi
 *   2. Instruksi INT m → allocFrame(m) → push m-3 slot variabel kosong
 *   3. Eksekusi badan program
 *   4. Instruksi RET → exitFrame() → selesai
 *
 * Batas-batas (untuk deteksi bonus):
 *   MAX_STACK_SIZE  : total elemen maksimum di stack
 *   MAX_FRAME_DEPTH : kedalaman frame maksimum (untuk deteksi infinite recursion)
 */

static constexpr int FRAME_STATIC_LINK  = 0;
static constexpr int FRAME_DYNAMIC_LINK = 1;
static constexpr int FRAME_RETURN_ADDR  = 2;
static constexpr int FRAME_VAR_START    = 3;

static constexpr int MAX_STACK_SIZE  = 10000;
static constexpr int MAX_FRAME_DEPTH = 1000;

/**
 * @brief Stack Machine: mengelola memori eksekusi program Arion.
 *
 * Digunakan oleh Interpreter untuk:
 * - Menyimpan dan membaca nilai variabel lokal
 * - Melakukan operasi push/pop untuk evaluasi ekspresi
 * - Manajemen frame saat memanggil prosedur/fungsi
 *
 * Catatan untuk Interpreter (Orang 4):
 * - Selalu panggil initMainFrame() sebelum mulai eksekusi.
 * - Instruksi INT m → panggil allocFrame(m).
 * - Instruksi CAL l → panggil enterFrame(SL, BP, PC+1) lalu set PC = l.
 * - Instruksi RET   → int ra = exitFrame(); set PC = ra.
 * - Instruksi LOD level offset → int base = findFrameBase(level);
 *                                 push(readMemAtFrame(base, FRAME_VAR_START + offset))
 * - Instruksi STO level offset → int base = findFrameBase(level);
 *                                 writeMemAtFrame(base, FRAME_VAR_START + offset, pop())
 * - Semua method yang bisa melempar ArionRuntimeException harus di-catch
 *   di loop utama run() untuk menghentikan eksekusi dengan bersih.
 */
class StackMachine {
private:
    std::vector<RuntimeValue> stack;
    int SP;
    int BP;
    int frameDepth;

    RuntimeErrorCollector& errors;

public:

    /**
     * @brief Konstruktor StackMachine.
     * @param errorCollector Referensi ke RuntimeErrorCollector milik Interpreter.
     */
    explicit StackMachine(RuntimeErrorCollector& errorCollector);

    /**
     * @brief Inisialisasi frame program utama.
     *
     * Harus dipanggil SEKALI sebelum eksekusi dimulai, sebelum instruksi pertama.
     * Push SL=0, DL=0, RA=0 sebagai header frame program utama, lalu set BP=0.
     * Setelah ini, instruksi INT m akan memanggil allocFrame(m) untuk
     * mengalokasikan m-3 slot variabel lokal.
     *
     * Tidak melempar exception.
     */
    void initMainFrame();

    /**
     * @brief Push satu nilai ke puncak stack.
     *
     * Melempar ArionRuntimeException(STACK_OVERFLOW) jika SP+1 >= MAX_STACK_SIZE.
     *
     * @param val Nilai yang akan di-push.
     */
    void push(const RuntimeValue& val);

    /**
     * @brief Pop satu nilai dari puncak stack dan kembalikan nilainya.
     *
     * Melempar ArionRuntimeException(STACK_UNDERFLOW) jika stack kosong (SP < 0).
     *
     * @return Nilai yang di-pop.
     */
    RuntimeValue pop();

    /**
     * @brief Lihat nilai di puncak stack tanpa mengambilnya.
     *
     * Melempar ArionRuntimeException(STACK_UNDERFLOW) jika stack kosong.
     *
     * @return Referensi ke nilai di puncak stack.
     */
    RuntimeValue& peek();

    /**
     * @brief Baca nilai dari address memori absolut dalam stack.
     *
     * Digunakan oleh instruksi LOD ketika level == 0 (akses langsung ke frame aktif
     * dengan address absolut). Untuk akses multi-level gunakan readMemAtFrame.
     *
     * Melempar ArionRuntimeException(OOB_ACCESS) jika address di luar [0..SP].
     * Melempar ArionRuntimeException(UNDEFINED_VARIABLE) jika nilai adalah NIL.
     *
     * @param address Index absolut dalam array stack.
     * @return Nilai di address tersebut.
     */
    RuntimeValue readMem(int address);

    /**
     * @brief Tulis nilai ke address memori absolut dalam stack.
     *
     * Digunakan oleh instruksi STO ketika level == 0 (akses langsung ke frame aktif
     * dengan address absolut). Untuk akses multi-level gunakan writeMemAtFrame.
     *
     * Melempar ArionRuntimeException(OOB_ACCESS) jika address di luar [0..SP].
     * Melempar ArionRuntimeException(STACK_SMASHING) jika penulisan menimpa
     * header frame pemanggil (SL/DL/RA milik frame lain).
     *
     * @param address Index absolut dalam array stack.
     * @param val     Nilai yang akan ditulis.
     */
    void writeMem(int address, const RuntimeValue& val);

    /**
     * @brief Baca nilai dari frame tertentu dengan offset relatif dari frameBase.
     *
     * Digunakan oleh LOD dengan level > 0 (akses variabel di scope luar).
     * offset harus >= FRAME_VAR_START untuk akses variabel lokal.
     *
     * Melempar ArionRuntimeException(OOB_ACCESS) jika address hasil = frameBase+offset
     * di luar [0..SP].
     * Melempar ArionRuntimeException(UNDEFINED_VARIABLE) jika nilai adalah NIL.
     *
     * @param frameBase Index BP frame target (hasil findFrameBase).
     * @param offset    Offset dari frameBase (gunakan FRAME_VAR_START + varOffset).
     * @return Nilai di posisi tersebut.
     */
    RuntimeValue readMemAtFrame(int frameBase, int offset);

    /**
     * @brief Tulis nilai ke frame tertentu dengan offset relatif dari frameBase.
     *
     * Digunakan oleh STO dengan level > 0 (akses variabel di scope luar).
     * offset harus >= FRAME_VAR_START untuk akses variabel lokal.
     *
     * Melempar ArionRuntimeException(OOB_ACCESS) jika address hasil = frameBase+offset
     * di luar [0..SP].
     *
     * @param frameBase Index BP frame target (hasil findFrameBase).
     * @param offset    Offset dari frameBase (gunakan FRAME_VAR_START + varOffset).
     * @param val       Nilai yang akan ditulis.
     */
    void writeMemAtFrame(int frameBase, int offset, const RuntimeValue& val);

    /**
     * @brief Masuk ke frame baru saat memanggil prosedur/fungsi (instruksi CAL).
     *
     * Push SL, DL, RA ke stack dan update BP ke frame baru.
     * Setelah enterFrame, instruksi INT m harus memanggil allocFrame(m)
     * untuk mengalokasikan slot variabel.
     *
     * Melempar ArionRuntimeException(STACK_OVERFLOW) jika frameDepth >= MAX_FRAME_DEPTH.
     *
     * @param staticLink  BP frame scope luar (untuk akses variabel non-lokal).
     * @param dynamicLink BP frame pemanggil (untuk kembali setelah RET).
     * @param returnAddr  PC instruksi setelah CAL (untuk kembali setelah RET).
     */
    void enterFrame(int staticLink, int dynamicLink, int returnAddr);

    /**
     * @brief Keluar dari frame aktif saat RET.
     *
     * Memulihkan SP ke BP-1 dan BP ke Dynamic Link frame aktif.
     * Melempar ArionRuntimeException(STACK_CORRUPTION) jika BP tidak valid.
     *
     * @return Return Address (PC yang harus dilanjutkan setelah RET).
     */
    int exitFrame();

    /**
     * @brief Alokasi slot memori untuk variabel lokal dalam frame aktif (instruksi INT).
     *
     * Instruksi INT m mengalokasikan m slot total dalam frame.
     * Karena enterFrame sudah push 3 header (SL+DL+RA), allocFrame(m)
     * hanya perlu push m-3 slot tambahan untuk variabel lokal.
     * Untuk program utama (setelah initMainFrame), perilaku sama.
     *
     * Melempar ArionRuntimeException(STACK_OVERFLOW) jika alokasi melebihi MAX_STACK_SIZE.
     *
     * @param frameSize Ukuran frame total termasuk 3 header (nilai operand instruksi INT).
     */
    void allocFrame(int frameSize);

    /**
     * @brief Temukan BP frame pada lexical level tertentu di atas frame aktif.
     *
     * Mengikuti rantai Static Link sebanyak levelDiff kali dari BP saat ini.
     * levelDiff == 0 berarti frame aktif sendiri (kembalikan BP saat ini).
     *
     * Melempar ArionRuntimeException(STACK_CORRUPTION) jika rantai SL rusak.
     *
     * @param levelDiff Perbedaan lexical level (0 = frame aktif).
     * @return BP dari frame target.
     */
    int findFrameBase(int levelDiff);

    /** @brief Kembalikan nilai SP saat ini. */
    int getSP() const;

    /** @brief Kembalikan nilai BP saat ini. */
    int getBP() const;

    /** @brief Kembalikan kedalaman frame saat ini. */
    int getFrameDepth() const;

    /** @brief Kembalikan jumlah elemen di stack saat ini (SP+1). */
    int getStackSize() const;

    /**
     * @brief Cek apakah struktur frame stack masih konsisten.
     *
     * Menelusuri rantai Dynamic Link dari BP aktif hingga frame pertama.
     * Digunakan untuk deteksi Stack Corruption (bonus).
     *
     * @return true jika stack valid, false jika ada korupsi.
     */
    bool checkCorruption();
};

#endif