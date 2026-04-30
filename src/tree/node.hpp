// node.hpp
#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>

class Node {
public:
    std::string label;
    std::vector<Node*> children;

    // Constructor untuk membuat node dengan label
    Node(const std::string& label);

    // Tambahkan child ke node ini
    void addChild(Node* child);

    // Destructor untuk cleanup memory
    ~Node();
};

#endif