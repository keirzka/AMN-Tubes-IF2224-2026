# Tugas Besar Teori Bahasa Formal dan Automata IF2224
Disusun oleh:
- 13524071 Kalyca Nathania B Manullang
- 13524073 Keisha Rizka Syofyani
- 13524087 Muhammad Fakhry Zaki
- 13524109 Helena Kristela Sarhawa

## Deskripsi Program

Pada milestone 1, program ini adalah **Lexical Analyzer (Lexer)** untuk bahasa pemrograman mirip Pascal yang mengimplementasikan automata hingga dalam mengenali token-token dari kode sumber. Program membaca file teks berisi kode sumber dan menganalisisnya karakter per karakter, kemudian mengubahnya menjadi daftar token yang sesuai dengan definisi bahasa.

Kemudian pada milestone 2, program dilanjutkan dengan implementasi **Syntax Analyzer (Parser)**. Pada bagian ini, program akan menerima hasil list token yang didapat dari lexer. Token-token tersebut kemudian akan dibangun menjadi sebuah parse tree. Pada bagian ini juga, program sudah bisa menganalisi beberapa error syntaxis, seperti penulisan nama variabel yang menggunakan simbol aneh ('*', '&', dan lain lain).

Selanjutnya pada milestone 3, program dikembangkan dengan implementasi Semantic Analyzer. Pada tahap ini, parse tree yang dihasilkan parser akan dianalisis lebih lanjut untuk memastikan program benar secara semantik. Semantic analyzer melakukan traversal AST menggunakan visitor pattern dan melakukan berbagai pengecekan seperti declaration checking, scope checking, type checking, assignment compatibility, validasi parameter prosedur/fungsi, serta validasi akses array dan record. Selain itu, milestone ini juga menghasilkan Decorated AST dan Symbol Table yang berisi informasi identifier, tipe data, scope, dan anotasi semantik lainnya sebagai persiapan menuju tahap compiler berikutnya.

### Fitur Utama:
- **Lexical Analysis**
  - Tokenisasi source code menjadi token-token bahasa Arion
  - Mendukung keyword, identifier, operator, literal, delimiter, dan komentar
  - Mendukung escape sequence pada string
    
- **Syntax Analysis**
  - Recursive Descent Parser untuk membangun parse tree
  - Mendukung parsing declaration, expression, statement, procedure, function, array, dan record
  - Menampilkan parse tree hasil parsing
  - Menangani syntax error pada source code
    
- **Semantic Analysis**
  - Traversal AST menggunakan visitor pattern dan DFS
  - Type checking dan assignment compatibility
  - Scope checking menggunakan lexical scoping
  - Declaration checking melalui symbol table
  - Validasi parameter prosedur dan fungsi
  - Mendukung array, record, subrange, dan control flow
  - Menghasilkan Decorated AST
  - Menghasilkan Symbol Table (tab, btab, dan atab)
  - Semantic error handling tanpa menghentikan program

- **Output**
  - Menampilkan hasil lexer, parser, semantic analysis, symbol table, dan Decorated AST ke terminal
  - Menyimpan hasil output ke file

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
1. Program meminta nama file input
2. Lexer melakukan tokenisasi source code
3. Parser membangun parse tree dari token hasil lexer
4. Semantic analyzer melakukan traversal AST dan validasi semantik
5. Jika terdapat semantic error, seluruh error ditampilkan sekaligus
6. Jika berhasil, program mencetak:
   - Parse Tree / Decorated AST
   - Symbol Table
   - Hasil semantic analysis
7. Hasil output disimpan ke file pada folder milestone terkait

### 5. Contoh Penggunaan
**Input:** File bernama `input-1.txt`
```
program Hello;

var
  a, b: integer;

begin
  a := 5;
  b := a + 10;
  writeln('Result = ', b);
end.
```

**Output yang hasil lexer ditampilkan di terminal:**
```
programsy
ident (Hello)
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
ident (writeln)
lparent
string (Result = )
comma
ident (b)
rparent
semicolon
endsy
period
```

**Output yang hasil parser ditampilkan di terminal:**
```
<program>
    <program-header>
        programsy
        ident (Hello)
        semicolon
    <declaration-part>
        <var-declaration>
            varsy
            <identifier-list>
                ident (a)
                comma
                ident (b)
            colon
            <type>
                ident (integer)
            semicolon
    <compound-statement>
        beginsy
        <statement-list>
            <assignment-statement>
                <variable>
                    ident (a)
                becomes
                <expression>
                    <simple-expression>
                        <term>
                            <factor>
                                intcon (5)
            semicolon
            <assignment-statement>
                <variable>
                    ident (b)
                becomes
                <expression>
                    <simple-expression>
                        <term>
                            <factor>
                                ident (a)
                        <additive-operator>
                            plus
                        <term>
                            <factor>
                                intcon (10)
            semicolon
            <procedure/function-call>
                ident (writeln)
                lparent
                <parameter-list>
                    <expression>
                        <simple-expression>
                            <term>
                                <factor>
                                    string (Result = )
                    comma
                    <expression>
                        <simple-expression>
                            <term>
                                <factor>
                                    ident (b)
                rparent
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
- Mengerjakan laporan milestone 1
- Mengerjakan implementasi program lexer
- Membuat implementasi program bagian statement dan control flow 
- Mengerjakan laporan tugas bagian implementasi program milestone 2
- Mengerjakan implementasi type checker dan visitor declaration
- Mengerjakan laporan bagian hasil implementasi program milestone 


13524073 Keisha Rizka Syofyani 
- Membuat inisialisasi dokumen laporan
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer
- Membuat implementasi core program parser dan program utama
- Refactor dan menyesuaikan struktur file dan program Milestone 1 serta revisi program pada Milestone 1
- Membuat inisialisasi laporan, quality controlling isi laporan, mengerjakan bagian kesimpulan dan saran, serta finishing readme dan submission milestone 2
- Mengerjakan implementasi visitor expression
- Mengerjakan laporan bagian landasan teori, kesimpulan, saran, dan finishing milestone 3


13524087 Muhammad Fakhry Zaki 
- Mengerjakan laporan milestone 1
- Mengerjakan implementasi program lexer
- Membuat dictionary
- Membuat implementasi program bagian expression parser dan operator
- Membuat implementasi parser tree builder
- Melakukan testing dan bug fixing
- Mengerjakan laporan bagian testing program pada milestone 2
- Mengerjakan implementasi visitor statement, control flow, dan output program semantic analysis
- Mengerjakan laporan bagian testing program pada milestone 3


13524109 Helena Kristela Sarhawa 
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer
- Membuat implementasi program bagian declaration parser dan menangani edge case parsing declaration
- Mengerjakan laporan tugas bagian landasan teori milestone 2
- Mengerjakan implementasi prgram symbol table dan node
- Mengerjakan laporan bagian perancangan program





