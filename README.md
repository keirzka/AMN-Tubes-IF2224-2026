# Tugas Besar Teori Bahasa Formal dan Automata IF2224
Disusun oleh:
- 13524071 Kalyca Nathania B Manullang
- 13524073 Keisha Rizka Syofyani
- 13524087 Muhammad Fakhry Zaki
- 13524109 Helena Kristela Sarhawa

## Deskripsi Program

Pada milestone 1, program ini adalah **Lexical Analyzer (Lexer)** untuk bahasa pemrograman mirip Pascal yang mengimplementasikan automata hingga dalam mengenali token-token dari kode sumber. Program membaca file teks berisi kode sumber dan menganalisisnya karakter per karakter, kemudian mengubahnya menjadi daftar token yang sesuai dengan definisi bahasa.

Kemudian pada milestone 2, program dilanjutkan dengan implementasi **Syntax Analyzer (Parser)**. Pada bagian ini, program akan menerima hasil list token yang didapat dari lexer. Token-token tersebut kemudian akan dibangun menjadi sebuah parse tree. Pada bagian ini juga, program sudah bisa menganalisi beberapa error syntaxis, seperti penulisan nama variabel yang menggunakan simbol aneh ('*', '&', dan lain lain).

Selanjutnya, pada milestone 3, implementasi dilanjutkan dengan **Semantic Analyzer** dan **Decorated AST Builder**. Melanjutkan dari bagian sebelumnya, program menerima hasil parse tree yang didapat dari Parser kemudian dilakukan analisis semantic, seperti pengecekan kecocokan tipe data, dan lain lain. Output dari bagian ini ada dua, pertama adalah *Symbol Table* dan kedua adalah *Decorated AST*. *Symbol table* menyimpan informasi semua identifier yang didefinisikan dan digunakan dan *Decorated AST* menyimpan informasi seperti parse tree namun dalam bentuk yang lebih praktikal dan sederhana. 

### Fitur Utama:
- **Pembacaan File**: Membaca file input dari folder `test/milestone-x/`
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
program Hello;

var
  a, b: integer;

begin
  a := 5;
  b := a + 10;
  writeln('Result = ', b);
end.
```

**Output hasil lexer yang ditampilkan di terminal:**
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

**Output hasil parser yang ditampilkan di terminal:**
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

**Output hasil symbol table yang ditampilkan di terminal:**

```
[tab]
 Index          Identifier         Object           Type       Ref     Nrm     Lev   Address
     0                 and           TYPE         NOTYPE         0       1       0         0
     1               array           TYPE         NOTYPE         0       1       0         0
     2               begin           TYPE         NOTYPE         0       1       0         0
     3                case           TYPE         NOTYPE         0       1       0         0
     4               const           TYPE         NOTYPE         0       1       0         0
     5                 div           TYPE         NOTYPE         0       1       0         0
     6              downto           TYPE         NOTYPE         0       1       0         0
     7                  do           TYPE         NOTYPE         0       1       0         0
     8                else           TYPE         NOTYPE         0       1       0         0
     9                 end           TYPE         NOTYPE         0       1       0         0
    10                 for           TYPE         NOTYPE         0       1       0         0
    11            function           TYPE         NOTYPE         0       1       0         0
    12                  if           TYPE         NOTYPE         0       1       0         0
    13                 mod           TYPE         NOTYPE         0       1       0         0
    14                 not           TYPE         NOTYPE         0       1       0         0
    15                  of           TYPE         NOTYPE         0       1       0         0
    16                  or           TYPE         NOTYPE         0       1       0         0
    17           procedure           TYPE         NOTYPE         0       1       0         0
    18             program           TYPE         NOTYPE         0       1       0         0
    19              record           TYPE         NOTYPE         0       1       0         0
    20              repeat           TYPE         NOTYPE         0       1       0         0
    21             integer           TYPE         NOTYPE         0       1       0         0
    22                real           TYPE         NOTYPE         0       1       0         0
    23             boolean           TYPE         NOTYPE         0       1       0         0
    24                char           TYPE         NOTYPE         0       1       0         0
    25              string           TYPE         NOTYPE         0       1       0         0
    26                then           TYPE         NOTYPE         0       1       0         0
    27                  to           TYPE         NOTYPE         0       1       0         0
    28                type           TYPE         NOTYPE         0       1       0         0
    29               until           TYPE         NOTYPE         0       1       0         0
    30                 var           TYPE         NOTYPE         0       1       0         0
    31               while           TYPE         NOTYPE         0       1       0         0
    32             Integer           TYPE        INTEGER         0       1       0         0
    33                Real           TYPE           REAL         0       1       0         0
    34                Char           TYPE           CHAR         0       1       0         0
    35             Boolean           TYPE        BOOLEAN         0       1       0         0
    36              String           TYPE         STRING         0       1       0         0
    37                True       CONSTANT        BOOLEAN         0       1       0         1
    38               False       CONSTANT        BOOLEAN         0       1       0         0
    39             writeln      PROCEDURE         NOTYPE         0       1       0         0
    40              readln      PROCEDURE         NOTYPE         0       1       0         0
    41               write      PROCEDURE         NOTYPE         0       1       0         0
    42                read      PROCEDURE         NOTYPE         0       1       0         0
    43               Hello        PROGRAM         NOTYPE         0       1       0         0
    44                   a       VARIABLE        INTEGER         0       1       0         0
    45                   b       VARIABLE        INTEGER         0       1       0         0
[atab]
 Index     Index Type   Element Type      ERef     Low    High    ElSize      Size
     0         NOTYPE         NOTYPE         0       0       0         1         0
[btab]
 Index           Last     Last Param     Param Size       Var Size
     0             45              0              0              2
```

**Output hasil AST yang ditampilkan di terminal:**

```
ProgramNode(name: 'Hello') -> tab_index:43, type:NOTYPE, lev:0
    Declarations
        VarDecl(name: 'a') -> tab_index:44, type:INTEGER, lev:0
        VarDecl(name: 'b') -> tab_index:45, type:INTEGER, lev:0
    Block -> block_index:0, lev:0
        Assign('a' := ...)
            Var('a') -> tab_index:44, type:INTEGER, lev:0
            Literal(5) -> type:INTEGER, lev:0
        Assign('b' := ...)
            Var('b') -> tab_index:45, type:INTEGER, lev:0
            BinOp '+' -> tab_index:0, type:INTEGER, lev:0
                Var('a') -> tab_index:44, type:INTEGER, lev:0
                Literal(10) -> type:INTEGER, lev:0
        procedure/function-call-statement name: writeln -> tab_index:39, type:NOTYPE, lev:0
            Args
                Literal('Result = ') -> type:STRING, lev:0
                Var('b') -> tab_index:45, type:INTEGER, lev:0
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


13524073 Keisha Rizka Syofyani 
- Membuat inisialisasi dokumen laporan
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer
- Membuat implementasi core program parser dan program utama
- Refactor dan menyesuaikan struktur file dan program Milestone 1 serta revisi program pada Milestone 1
- Membuat inisialisasi laporan, quality controlling isi laporan, mengerjakan bagian kesimpulan dan saran, serta finishing readme dan submission milestone 2


13524087 Muhammad Fakhry Zaki 
- Mengerjakan laporan milestone 1
- Mengerjakan implementasi program lexer
- Membuat dictionary
- Membuat implementasi program bagian expression parser dan operator
- Membuat implementasi parser tree builder
- Melakukan testing dan bug fixing
- Mengerjakan laporan bagian testing program


13524109 Helena Kristela Sarhawa 
- Membuat diagram transisi DFA
- Mengerjakan implementasi program lexer
- Membuat implementasi program bagian declaration parser dan menangani edge case parsing declaration
- Mengerjakan laporan tugas bagian landasan teori milestone 2





