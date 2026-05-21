
#include "tree/ast_node.hpp"

std::shared_ptr<ASTNode> g_astRoot = nullptr;

ASTNode::ASTNode(const std::string& kindName, const std::string& textValue)
	: kind(kindName), text(textValue), type(-1), tab_index(-1), lev(-1), block_index(-1) {}

void ASTNode::addChild(const std::shared_ptr<ASTNode>& child) {
	if (child) {
		children.push_back(child);
	}
}

std::shared_ptr<ASTNode> makeAstNode(const std::string& kindName,
									 const std::string& textValue) {
	return std::make_shared<ASTNode>(kindName, textValue);
}
