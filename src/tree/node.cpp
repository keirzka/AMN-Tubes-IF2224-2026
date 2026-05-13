#include "node.hpp"

// Constructor untuk membuat node dengan label
Node::Node(const std::string& lbl) : label(lbl), sem_type(-1), sem_tab_index(-1), sem_lev(-1), sem_initialized(false){}

// Tambahkan child ke node ini
void Node::addChild(Node* child){
    this->children.push_back(child);
}

std::string Node::getLabel() const {
    return this->label;
}

// Destructor untuk cleanup memory
Node::~Node(){
    for(Node* child : children){
        delete child;
    }
}

// Set semua field semantik sekaligus
void Node::annotate(int typeCode, int tabIndex, int lexLevel) {
    sem_type = typeCode;
    sem_tab_index = tabIndex;
    sem_lev = lexLevel;
}

bool Node::isAnnotated() const {
    return (sem_type != -1);
}