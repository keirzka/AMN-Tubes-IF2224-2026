# Tugas Besar Teori Bahasa Formal dan Automata IF2224
Disusun oleh:
- 13524071 Kalyca Nathania B Manullang
- 13524073 Keisha Rizka Syofyani
- 13524087 Muhammad Fakhry Zaki
- 13524109 Helena Kristela Sarhawa

## Deskripsi Program

Program ini adalah **Lexical Analyzer (Lexer)** untuk bahasa pemrograman mirip Pascal yang mengimplementasikan automata hingga dalam mengenali token-token dari kode sumber. Program membaca file teks berisi kode sumber dan menganalisisnya karakter per karakter, kemudian mengubahnya menjadi daftar token yang sesuai dengan definisi bahasa.

### Fitur Utama:
- **Pembacaan File**: Membaca file input dari folder `test/milestone-1/`
- **Tokenisasi**: Mengidentifikasi dan mengklasifikasikan token termasuk:
  - **Keywords**: `program`, `var`, `const`, `begin`, `end`, `if`, `while`, `for`, dll
  - **Identifiers**: Nama variabel dan fungsi (format: `ident (nama)`)
  - **Konstanta Numerik**: Integer (format: `intcon (nilai)`) dan Real (format: `realcon (nilai)`)
  - **String dan Char**: String (format: `string (nilai)`) dan karakter (format: `charcon (nilai)`)
  - **Operator**: Aritmatika (`plus`, `minus`, `times`, `idiv`, `rdiv`, `imod`), Logika (`and`, `or`, `not`), dan Relasional (`eql`, `neq`, `lss`, `leq`, `gtr`, `geq`)
  - **Delimiter**: Tanda kurung, kurung siku, koma, titik koma, kolon, dll
- **Penanganan Komentar**: Mendukung komentar dengan format `{ ... }` dan `(* ... *)`
- **Escape Sequence**: Mendukung karakter escape dalam string dengan `''`
- **Output**: Menampilkan token ke terminal dan menyimpan hasil ke file output

## Requirements

- **Compiler**: G++ (C++11 atau lebih tinggi)
- **OS**: Linux/Unix atau Windows dengan MinGW
- **Build Tool**: Make

## Cara Instalasi dan Penggunaan Program

### 1. Instalasi
Pastikan Anda memiliki:
- G++ terinstall di sistem
- GNU Make terinstall

### 2. Kompilasi Program
Gunakan perintah:
```bash
make build
```
Perintah ini akan:
- Membuat folder `obj/` untuk file object
- Membuat folder `bin/` untuk executable
- Mengompilasi semua file `.cpp` menjadi `.o`
- Melakukan linking untuk menghasilkan executable `bin/main`

### 3. Menjalankan Program
Gunakan perintah:
```bash
make run
```
atau jalankan langsung:
```bash
./bin/main
```

### 4. Alur Program
1. Program akan meminta nama file input (file harus berada di folder `test/milestone-1/`)
2. Program membaca dan menganalisis file karakter per karakter
3. Token yang dikenali ditampilkan ke terminal
4. Program meminta nama file untuk menyimpan hasil token
5. Hasil disimpan ke file `test/milestone-1/[nama].txt`

### 5. Contoh Penggunaan
**Input:** File bernama `input-1.txt`
```
program Test1;

var
  a, b: integer;

begin
  a := 5;
  b := a + 10;
end.
```

**Output yang ditampilkan di terminal:**
```
programsy
ident (Test1)
semicolon
varsy
ident (a)
comma
ident (b)
colon
ident (integer)
semicolon
beginsy
ident (a)
becomes
intcon (5)
semicolon
ident (b)
becomes
ident (a)
plus
intcon (10)
semicolon
endsy
period
```

### 6. Target Make Lainnya
```bash
make clean   # Menghapus folder obj/ dan bin/
make help    # Menampilkan informasi bantuan
```

## Pembagian Tugas

13524071 Kalyca Nathania Benedicta Manullang 
- Mengerjakan laporan
- Mengerjakan implementasi program lexer

13524073 Keisha Rizka Syofyani 
- Membuat inisialisasi dokumen laporan
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer

13524087 Muhammad Fakhry Zaki 
- Mengerjakan laporan
- Mengerjakan implementasi program lexer
- Membuat dictionary

13524109 Helena Kristela Sarhawa 
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer



