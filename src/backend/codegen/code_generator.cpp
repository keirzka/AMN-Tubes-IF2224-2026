#include "backend/codegen/code_generator.hpp"
#include <stdexcept>
#include <algorithm>

CodeGenerator::CodeGenerator(const SymbolTable& st):st_(st),pc_(0),currentLevel_(0){}

std::vector<Instruction> CodeGenerator::generate(const std::shared_ptr<ASTNode>& root){
    instructions_.clear();
    pc_=0;
    currentLevel_=0;
    if(root&&root->kind=="Program"){
        genProgram(root);
    }
    return instructions_;
}

const std::vector<Instruction>& CodeGenerator::getInstructions() const{
    return instructions_;
}

int CodeGenerator::emit(Mnemonic m,int level,int operand){
    int idx=pc_;
    instructions_.emplace_back(pc_,m,level,operand);
    pc_++;
    return idx;
}

void CodeGenerator::patch(int idx,int operand){
    instructions_[idx].operand=operand;
}

int CodeGenerator::currentPc() const{
    return pc_;
}

std::shared_ptr<ASTNode> CodeGenerator::findChild(const std::shared_ptr<ASTNode>& node,const std::string& kind) const{
    if(!node) return nullptr;
    for(auto& c:node->children){
        if(c&&c->kind==kind) return c;
    }
    return nullptr;
}

OprCode CodeGenerator::opToOprCode(const std::string& op) const{
    if(op=="+") return OprCode::ADD;
    if(op=="or") return OprCode::ADD;
    if(op=="-") return OprCode::SUB;
    if(op=="*") return OprCode::MUL;
    if(op=="and") return OprCode::MUL;
    if(op=="div") return OprCode::DIV;
    if(op=="/") return OprCode::DIV;
    if(op=="mod") return OprCode::MOD;
    if(op=="=") return OprCode::EQL;
    if(op=="<>") return OprCode::NEQ;
    if(op=="<") return OprCode::LSS;
    if(op==">=") return OprCode::GEQ;
    if(op==">") return OprCode::GTR;
    if(op=="<=") return OprCode::LEQ;
    throw std::invalid_argument("opToOprCode: operator tidak dikenal: "+op);
}

bool CodeGenerator::isPredefinedIO(const std::string& name) const{
    std::string lower=name;
    for(auto& c:lower) c=(char)tolower((unsigned char)c);
    return lower=="writeln"||lower=="write"||lower=="readln"||lower=="read";
}

void CodeGenerator::genProgram(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    int blockIdx=0;
    auto blockChild=findChild(node,"Block");
    if(blockChild&&blockChild->block_index>=0){
        blockIdx=blockChild->block_index;
    }
    int intIdx=emit(Mnemonic::INT,0,0);
    int frameSize=resolveFrameSize(st_,blockIdx);
    patch(intIdx,frameSize);
    auto declNode=findChild(node,"Declarations");
    if(declNode){
        genDeclarations(declNode,blockIdx);
    }
    if(blockChild){
        genBlock(blockChild);
    }
    emit(Mnemonic::RET,0,0);
}

void CodeGenerator::genDeclarations(const std::shared_ptr<ASTNode>& node,int /*blockIdx*/){
    if(!node) return;
    for(auto& child:node->children){
        if(!child) continue;
        if(child->kind=="ProcedureDecl"){
            genProcedureDecl(child);
        } else if(child->kind=="FunctionDecl"){
            genFunctionDecl(child);
        }
    }
}

void CodeGenerator::genBlock(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    for(auto& child:node->children){
        if(!child) continue;
        genStatement(child);
    }
}

void CodeGenerator::genStatement(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    const std::string& k=node->kind;
    if(k=="Assign"){
        genAssign(node);
    } else if(k=="If"){
        genIf(node);
    } else if(k=="While"){
        genWhile(node);
    } else if(k=="Repeat"){
        genRepeat(node);
    } else if(k=="For"){
        genFor(node);
    } else if(k=="Case"){
        genCase(node);
    } else if(k=="ProcCall"){
        std::string lower=node->text;
        for(auto& c:lower) c=(char)tolower((unsigned char)c);
        if(lower=="writeln"){
            genWriteln(node);
        } else if(lower=="write"){
            genWrite(node);
        } else{
            genProcCall(node);
        }
    } else if(k=="Block"){
        genBlock(node);
    }
}

void CodeGenerator::genLiteral(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    int tc=node->type;
    if(tc==static_cast<int>(TypeCode::INTEGER)){
        int val=0;
        try{
            val=std::stoi(node->text);
        } catch(...){
        }
        emit(Mnemonic::LIT,0,val);
    } else if(tc==static_cast<int>(TypeCode::BOOLEAN)){
        std::string lower=node->text;
        for(auto& c:lower) c=(char)tolower((unsigned char)c);
        int val=(lower=="true"||lower=="1")?1:0;
        emit(Mnemonic::LIT,0,val);
    } else if(tc==static_cast<int>(TypeCode::CHAR)){
        char ch='\0';
        if(node->text.size()>=2) ch=node->text[1];
        emit(Mnemonic::LIT,0,static_cast<int>(ch));
    } else if(tc==static_cast<int>(TypeCode::REAL)){
        int val=0;
        try{
            val=static_cast<int>(std::stod(node->text));
        } catch(...){
        }
        emit(Mnemonic::LIT,0,val);
    } else if(tc==static_cast<int>(TypeCode::STRING)){
        int val=static_cast<int>(node->text.size());
        emit(Mnemonic::LIT,0,val);
    } else{
        int val=0;
        try{
            val=std::stoi(node->text);
        } catch(...){
        }
        emit(Mnemonic::LIT,0,val);
    }
}

void CodeGenerator::genVar(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    int tabIdx=node->tab_index;
    if(tabIdx<0||tabIdx>=static_cast<int>(st_.tab.size())){
        emit(Mnemonic::LOD,0,0);
        return;
    }
    int varLevel=st_.tab[tabIdx].lev;
    int levelDiff=resolveLevel(currentLevel_,varLevel);
    int offset=resolveVarAddress(st_,tabIdx);
    emit(Mnemonic::LOD,levelDiff,offset);
}

void CodeGenerator::genExpression(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    const std::string& k=node->kind;
    if(k=="Literal"){
        genLiteral(node);
    } else if(k=="Var"){
        genVar(node);
    } else if(k=="BinOp"){
        genBinOp(node);
    } else if(k=="UnaryOp"){
        genUnaryOp(node);
    } else if(k=="ProcCall"){
        genFunctionCall(node);
    } else if(k=="Expression"){
        for(auto& child:node->children) genExpression(child);
    }
}

void CodeGenerator::genBinOp(const std::shared_ptr<ASTNode>& node){
    if(!node||node->children.size()<2) return;
    genExpression(node->children[0]);
    genExpression(node->children[1]);
    OprCode opr=opToOprCode(node->text);
    emit(Mnemonic::OPR,0,static_cast<int>(opr));
}

void CodeGenerator::genUnaryOp(const std::shared_ptr<ASTNode>& node){
    if(!node||node->children.empty()) return;
    genExpression(node->children[0]);
    if(node->text=="-"){
        emit(Mnemonic::OPR,0,static_cast<int>(OprCode::NEG));
    } else if(node->text=="not"){
        emit(Mnemonic::LIT,0,0);
        emit(Mnemonic::OPR,0,static_cast<int>(OprCode::EQL));
    }
}

void CodeGenerator::genFunctionCall(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    int tabIdx=node->tab_index;
    if(tabIdx<0||tabIdx>=static_cast<int>(st_.tab.size())) return;
    int funcLevel=st_.tab[tabIdx].lev;
    int funcAddr=st_.tab[tabIdx].adr;
    auto argsNode=findChild(node,"Args");
    if(argsNode){
        for(auto& arg:argsNode->children) genExpression(arg);
    }
    int levelDiff=resolveLevel(currentLevel_,funcLevel);
    emit(Mnemonic::CAL,levelDiff,funcAddr);
}

void CodeGenerator::genAssign(const std::shared_ptr<ASTNode>& node){
    if(!node||node->children.size()<2) return;
    auto target=node->children[0];
    auto value=node->children[1];
    genExpression(value);
    int tabIdx=target->tab_index;
    if(tabIdx<0||tabIdx>=static_cast<int>(st_.tab.size())){
        emit(Mnemonic::STO,0,0);
        return;
    }
    int varLevel=st_.tab[tabIdx].lev;
    int levelDiff=resolveLevel(currentLevel_,varLevel);
    int offset=resolveVarAddress(st_,tabIdx);
    emit(Mnemonic::STO,levelDiff,offset);
}

void CodeGenerator::genWrite(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    auto argsNode=findChild(node,"Args");
    if(!argsNode||argsNode->children.empty()) return;
    for(auto& arg:argsNode->children){
        genExpression(arg);
        emit(Mnemonic::OPR,0,static_cast<int>(OprCode::WRT));
    }
}

void CodeGenerator::genWriteln(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    auto argsNode=findChild(node,"Args");
    if(!argsNode||argsNode->children.empty()){
        emit(Mnemonic::OPR,0,static_cast<int>(OprCode::WRTLN));
        return;
    }
    int n=static_cast<int>(argsNode->children.size());
    for(int i=0;i<n;i++){
        genExpression(argsNode->children[i]);
        if(i<n-1){
            emit(Mnemonic::OPR,0,static_cast<int>(OprCode::WRT));
        } else{
            emit(Mnemonic::OPR,0,static_cast<int>(OprCode::WRTLN));
        }
    }
}

void CodeGenerator::genProcCall(const std::shared_ptr<ASTNode>& node){
    if(!node) return;
    // stub
    int tabIdx=node->tab_index;
    if(tabIdx<0||tabIdx>=static_cast<int>(st_.tab.size())) return;
    auto argsNode=findChild(node,"Args");
    if(argsNode){
        for(auto& arg:argsNode->children) genExpression(arg);
    }
    int levelDiff=resolveLevel(currentLevel_,st_.tab[tabIdx].lev);
    emit(Mnemonic::CAL,levelDiff,st_.tab[tabIdx].adr);
}

void CodeGenerator::genIf(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    // Cari child: Condition, Then, Else
    auto condNode = findChild(node, "Condition");
    auto thenNode = findChild(node, "Then");
    auto elseNode = findChild(node, "Else");

    // 1. Generate kondisi
    if (condNode && !condNode->children.empty())
        genExpression(condNode->children[0]);

    // 2. Emit JPC (lompat ke else/end jika kondisi false)
    int jpcIdx = emit(Mnemonic::JPC, 0, 0); // operand di-patch nanti

    // 3. Generate body THEN
    if (thenNode)
        for (auto& s : thenNode->children) genStatement(s);

    if (elseNode) {
        // 4. Emit JMP ke end (lewati else)
        int jmpIdx = emit(Mnemonic::JMP, 0, 0);

        // 5. Patch JPC ke awal else
        patch(jpcIdx, currentPc());

        // 6. Generate body ELSE
        for (auto& s : elseNode->children) genStatement(s);

        // 7. Patch JMP ke setelah else
        patch(jmpIdx, currentPc());
    } else {
        // Tidak ada else: patch JPC ke instruksi setelah then
        patch(jpcIdx, currentPc());
    }
}

void CodeGenerator::genWhile(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    auto condNode = findChild(node, "Condition");
    auto bodyNode = findChild(node, "Body");

    // 1. Catat posisi awal kondisi
    int startPc = currentPc();

    // 2. Generate kondisi
    if (condNode && !condNode->children.empty())
        genExpression(condNode->children[0]);

    // 3. JPC ke end (keluar jika false)
    int jpcIdx = emit(Mnemonic::JPC, 0, 0);

    // 4. Generate body
    if (bodyNode)
        for (auto& s : bodyNode->children) genStatement(s);

    // 5. JMP kembali ke kondisi
    emit(Mnemonic::JMP, 0, startPc);

    // 6. Patch JPC ke instruksi setelah JMP
    patch(jpcIdx, currentPc());
}

void CodeGenerator::genRepeat(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    auto bodyNode = findChild(node, "Body");
    auto condNode = findChild(node, "Condition");

    // 1. Catat awal body
    int startPc = currentPc();

    // 2. Generate body
    if (bodyNode)
        for (auto& s : bodyNode->children) genStatement(s);

    // 3. Evaluasi kondisi UNTIL
    if (condNode && !condNode->children.empty())
        genExpression(condNode->children[0]);

    // 4. JPC kembali ke start jika kondisi false (belum terpenuhi)
    emit(Mnemonic::JPC, 0, startPc);
}

void CodeGenerator::genProcedureDecl(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    int tabIdx = node->tab_index;

    // Catat address prosedur (PC sekarang = entry point)
    if (tabIdx >= 0 && tabIdx < (int)st_.tab.size()) {
        // patch address di symbol table — biasanya dilakukan via const_cast
        // karena st_ adalah const ref; koordinasikan dengan tim jika perlu
    }

    int procStart = currentPc();

    // Simpan level sebelumnya
    int savedLevel = currentLevel_;
    currentLevel_++;

    // Cari block_index untuk frame size
    int blkIdx = node->block_index >= 0 ? node->block_index : 0;
    int intIdx = emit(Mnemonic::INT, 0, 0);
    int frameSize = resolveFrameSize(st_, blkIdx);
    patch(intIdx, frameSize);

    // Generate declarations lokal (prosedur/fungsi bersarang)
    auto declNode = findChild(node, "Declarations");
    if (declNode) genDeclarations(declNode, blkIdx);

    // Generate body
    auto blockNode = findChild(node, "Block");
    if (blockNode) genBlock(blockNode);

    // RET di akhir
    emit(Mnemonic::RET, 0, 0);

    currentLevel_ = savedLevel;
}

void CodeGenerator::genFunctionDecl(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    int savedLevel = currentLevel_;
    currentLevel_++;

    int blkIdx = node->block_index >= 0 ? node->block_index : 0;
    int intIdx = emit(Mnemonic::INT, 0, 0);
    patch(intIdx, resolveFrameSize(st_, blkIdx));

    auto declNode = findChild(node, "Declarations");
    if (declNode) genDeclarations(declNode, blkIdx);

    auto blockNode = findChild(node, "Block");
    if (blockNode) genBlock(blockNode);

    // Return value sudah di stack (di-push oleh genAssign ke var fungsi)
    emit(Mnemonic::RET, 0, 0);

    currentLevel_ = savedLevel;
}

void CodeGenerator::genFor(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    // Struktur node For: [Var, From, To/Downto, Body]
    // Cari child berdasarkan kind
    auto varNode  = findChild(node, "Var");
    auto fromNode = findChild(node, "From");
    auto toNode   = findChild(node, "To");      // to
    auto downNode = findChild(node, "Downto");  // downto (jika ada)
    auto bodyNode = findChild(node, "Body");

    bool isDownto = (downNode != nullptr);
    auto limitNode = isDownto ? downNode : toNode;

    // 1. Init: assign nilai awal ke loop var
    if (fromNode && varNode) {
        genExpression(fromNode->children.empty() ? fromNode : fromNode->children[0]);
        int tabIdx = varNode->tab_index;
        int varLevel = st_.tab[tabIdx].lev;
        int levelDiff = resolveLevel(currentLevel_, varLevel);
        int offset = resolveVarAddress(st_, tabIdx);
        emit(Mnemonic::STO, levelDiff, offset);
    }

    // 2. Label start kondisi
    int startPc = currentPc();

    // 3. Evaluasi kondisi: loopVar <= limit (to) atau loopVar >= limit (downto)
    if (varNode) genVar(varNode);
    if (limitNode) {
        if (!limitNode->children.empty())
            genExpression(limitNode->children[0]);
        else
            genExpression(limitNode);
    }
    // to: var <= limit → LEQ; downto: var >= limit → GEQ
    emit(Mnemonic::OPR, 0, static_cast<int>(isDownto ? OprCode::GEQ : OprCode::LEQ));

    // 4. JPC keluar jika kondisi false
    int jpcIdx = emit(Mnemonic::JPC, 0, 0);

    // 5. Generate body
    if (bodyNode)
        for (auto& s : bodyNode->children) genStatement(s);

    // 6. Increment/decrement loop var
    if (varNode) {
        genVar(varNode);
        emit(Mnemonic::LIT, 0, 1);
        emit(Mnemonic::OPR, 0, static_cast<int>(isDownto ? OprCode::SUB : OprCode::ADD));
        int tabIdx = varNode->tab_index;
        int varLevel = st_.tab[tabIdx].lev;
        int levelDiff = resolveLevel(currentLevel_, varLevel);
        int offset = resolveVarAddress(st_, tabIdx);
        emit(Mnemonic::STO, levelDiff, offset);
    }

    // 7. JMP kembali ke kondisi
    emit(Mnemonic::JMP, 0, startPc);

    // 8. Patch JPC
    patch(jpcIdx, currentPc());
}

void CodeGenerator::genCase(const std::shared_ptr<ASTNode>& node){
    if (!node) return;

    // Simpan ekspresi case ke variabel sementara via assign ke slot stack
    // Atau evaluasi ulang tiap branch (lebih sederhana)
    auto exprNode = findChild(node, "Expression");

    std::vector<int> jmpToEndIndices;

    for (auto& child : node->children) {
        if (!child || child->kind != "CaseBranch") continue;

        auto labelNode = findChild(child, "Label");  // nilai konstanta
        auto bodyNode2 = findChild(child, "Body");

        // Evaluasi ekspresi case
        if (exprNode && !exprNode->children.empty())
            genExpression(exprNode->children[0]);
        else if (exprNode)
            genExpression(exprNode);

        // Load konstanta label
        if (labelNode) genExpression(labelNode);

        // EQL
        emit(Mnemonic::OPR, 0, static_cast<int>(OprCode::EQL));

        // JPC ke branch berikutnya jika tidak cocok
        int jpcIdx = emit(Mnemonic::JPC, 0, 0);

        // Body branch
        if (bodyNode2)
            for (auto& s : bodyNode2->children) genStatement(s);

        // JMP ke end
        int jmpIdx = emit(Mnemonic::JMP, 0, 0);
        jmpToEndIndices.push_back(jmpIdx);

        // Patch JPC ke instruksi setelah JMP (= branch berikutnya)
        patch(jpcIdx, currentPc());
    }

    // Patch semua JMP ke end
    int endPc = currentPc();
    for (int idx : jmpToEndIndices)
        patch(idx, endPc);
}