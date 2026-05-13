// node.hpp
#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>

class Node {
public:
    std::string label;
    std::vector<Node*> children;
    int sem_type;
    int sem_tab_index;
    int sem_lev;
    bool sem_initialized;

    // Constructor untuk membuat node dengan label
    explicit Node(const std::string& lbl);

    // Tambahkan child ke node ini
    void addChild(Node* child);

    std::string getLabel () const;

    // Destructor untuk cleanup memory
    ~Node();

    // Set semua field semantik sekaligus
    void annotate(int typeCode, int tabIndex, int lexLevel);
    
    bool isAnnotated() const;
};

#endif