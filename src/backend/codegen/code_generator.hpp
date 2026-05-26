#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <vector>
#include <memory>
#include <string>
#include "backend/instruction.hpp"
#include "backend/codegen/address_resolver.hpp"
#include "semantic/symbol_table.hpp"
#include "tree/ast_node.hpp"

class CodeGenerator {
public:
    explicit CodeGenerator(const SymbolTable& st);
    std::vector<Instruction> generate(const std::shared_ptr<ASTNode>& root);
    const std::vector<Instruction>& getInstructions() const;
    void genProgram(const std::shared_ptr<ASTNode>& node);
    void genDeclarations(const std::shared_ptr<ASTNode>& node, int blockIdx);
    void genBlock(const std::shared_ptr<ASTNode>& node);
    void genStatement(const std::shared_ptr<ASTNode>& node);
    void genLiteral(const std::shared_ptr<ASTNode>& node);
    void genVar(const std::shared_ptr<ASTNode>& node);
    void genExpression(const std::shared_ptr<ASTNode>& node);
    void genBinOp(const std::shared_ptr<ASTNode>& node);
    void genUnaryOp(const std::shared_ptr<ASTNode>& node);
    void genFunctionCall(const std::shared_ptr<ASTNode>& node);
    void genAssign(const std::shared_ptr<ASTNode>& node);
    void genWrite(const std::shared_ptr<ASTNode>& node);
    void genWriteln(const std::shared_ptr<ASTNode>& node);
    void genProcCall(const std::shared_ptr<ASTNode>& node);
    void genIf(const std::shared_ptr<ASTNode>& node);
    void genWhile(const std::shared_ptr<ASTNode>& node);
    void genRepeat(const std::shared_ptr<ASTNode>& node);
    void genProcedureDecl(const std::shared_ptr<ASTNode>& node);
    void genFunctionDecl(const std::shared_ptr<ASTNode>& node);
    void genFor(const std::shared_ptr<ASTNode>& node);
    void genCase(const std::shared_ptr<ASTNode>& node);

private:
    const SymbolTable& st_;
    std::vector<Instruction> instructions_;
    int pc_;
    int currentLevel_;
    int emit(Mnemonic m, int level, int operand);
    void patch(int idx, int operand);
    int currentPc() const;
    std::shared_ptr<ASTNode> findChild(const std::shared_ptr<ASTNode>& node, const std::string& kind) const;
    OprCode opToOprCode(const std::string& op) const;
    bool isPredefinedIO(const std::string& name) const;
};

#endif