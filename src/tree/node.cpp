#include "node.hpp"

// Constructor untuk membuat node dengan label
Node::Node(const std::string& label){
    this->label = label;
}

// Tambahkan child ke node ini
void Node::addChild(Node* child){
    this->children.push_back(child);
}

// Destructor untuk cleanup memory
Node::~Node(){
    for(Node* child : children){
        delete child;
    }
}