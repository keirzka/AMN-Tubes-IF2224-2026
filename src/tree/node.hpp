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

    explicit Node(const std::string& lbl);
    void addChild(Node* child);
    std::string getLabel() const;
    ~Node();
    void annotate(int typeCode, int tabIndex, int lexLevel);
    void setInitialized(bool val);
    bool isAnnotated() const;
};

#endif