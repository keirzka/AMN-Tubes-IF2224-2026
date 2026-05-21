
#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <memory>
#include <string>
#include <vector>

struct ASTNode {
	std::string kind;
	std::string text;
	int type;
	int tab_index;
	int lev;
	int block_index;
	std::vector<std::shared_ptr<ASTNode>> children;

	explicit ASTNode(const std::string& kindName, const std::string& textValue = "");
	void addChild(const std::shared_ptr<ASTNode>& child);
};

std::shared_ptr<ASTNode> makeAstNode(const std::string& kindName,
									 const std::string& textValue = "");

extern std::shared_ptr<ASTNode> g_astRoot;

#endif
