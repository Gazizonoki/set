#include <vector>
#include <queue>

template <class ValueType>
class Set {
private:
    class Node {
    public:
        Node() = default;
        Node(Node &v) {
            size = v.size;
            minValue = v.minValue;
            maxValue = v.maxValue;
            isLeafHolder = v.isLeafHolder;
        }
        Node(std::size_t size, ValueType key): size(size), minValue(key), maxValue(key) {}

        void fix() {
            size = 0;
            if (!sons.empty()) {
                maxValue = sons[0]->maxValue;
                minValue = sons[0]->minValue;
            }
            for (Node* son : sons) {
                size += son->size;
                if (maxValue < son->maxValue) {
                    maxValue = son->maxValue;
                }
                if (son->minValue < minValue) {
                    minValue = son->minValue;
                }
            }
        }

        void addSon(Node* vertex) {
            if (vertex == nullptr) {
                return;
            }
            if (!sons.empty()) {
                vertex->leftBrother = sons.back();
                vertex->rightBrother = sons.back()->rightBrother;
                sons.back()->rightBrother = vertex;
                if (vertex->rightBrother != nullptr) {
                    vertex->rightBrother->leftBrother = vertex;
                }
            }
            sons.push_back(vertex);
            for (std::size_t i = sons.size() - 1; i > 0; --i) {
                if (sons[i]->maxValue < sons[i - 1]->maxValue) {
                    std::swap(sons[i - 1], sons[i]);
                    swapNodes(sons[i - 1], sons[i]);
                }
            }
        }
        void eraseSon(Node* vertex) {
            if (vertex == nullptr) {
                return;
            }
            for (std::size_t i = 1; i < sons.size(); ++i) {
                if (sons[i - 1] == vertex) {
                    std::swap(sons[i - 1], sons[i]);
                    swapNodes(sons[i - 1], sons[i]);
                }
            }
            sons.pop_back();
            if (vertex->leftBrother != nullptr) {
                vertex->leftBrother->rightBrother = vertex->rightBrother;
            }
            if (vertex->rightBrother != nullptr) {
                vertex->rightBrother->leftBrother = vertex->leftBrother;
            }
            delete vertex;
        }
        
        void swapNodes(Node* first, Node* second) {
            first->leftBrother = second->leftBrother;
            second->rightBrother = first->rightBrother;
            first->rightBrother = second;
            second->leftBrother = first;
            if (second->rightBrother != nullptr) {
                second->rightBrother->leftBrother = second;
            }
            if (first->leftBrother != nullptr) {
                first->leftBrother->rightBrother = first;
            }
        }

        std::vector <Node*> sons;
        std::size_t size = 0;
        Node* leftBrother = nullptr;
        Node* rightBrother = nullptr;
        bool isLeafHolder = false;
        ValueType minValue, maxValue;
    };

    void clear(Node* v) {
        for (Node* son : v->sons) {
            clear(son);
        }
        delete v;
    }

    Node* root = nullptr;
public:
    class iterator {
    public:
        iterator() = default;
        iterator(Node* v, bool isEnd): v(v), isEnd(isEnd) {}
        ValueType operator*() const {
            return v->minValue;
        }
        ValueType* operator->() const {
            return &(v->minValue);
        }
        iterator& operator++() {
            if (v == nullptr)
                return *this;
            if (v->rightBrother != nullptr) {
                v = v->rightBrother;
            }
            else {
                isEnd = true;
            }
            return *this;
        }
        iterator& operator--() {
            if (v == nullptr)
                return *this;
            if (isEnd) {
                isEnd = false;
            }
            else {
                if (v->leftBrother != nullptr) {
                    v = v->leftBrother;
                }
            }
            return *this;
        }

        iterator operator++(int) {
            iterator old(*this);
            ++(*this);
            return old;
        }

        iterator operator--(int) {
            iterator old(*this);
            --(*this);
            return old;
        }

        bool operator==(const Set<ValueType>::iterator &other) const {
            return v == other.v && (isEnd == other.isEnd || v == nullptr);
        }

        bool operator!=(const Set<ValueType>::iterator &other) const {
            return !(*this == other);
        }
    private:
        Node* v = nullptr;
        bool isEnd = false;
    };

    Set(): root(nullptr) {}

    template<class sequenceIterator>
    Set(sequenceIterator first, sequenceIterator last) {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(std::initializer_list<ValueType> init) {
        for (auto it = init.begin(); it != init.end(); ++it) {
            insert(*it);
        }
    }

    Set& operator=(const Set s) {
        if (root != nullptr) {
            clear(root);
            root = nullptr;
        }
        if (s.root == nullptr) {
            return *this;
        }
        Node* lastVertex = nullptr;
        std::queue<std::pair<Node*, Node*>> q;
        root = new Node(*s.root);
        q.push({s.root, root});
        while (!q.empty()) {
            Node* oldVertex;
            Node* newVertex;
            oldVertex = q.front().first;
            newVertex = q.front().second;
            q.pop();

            for (auto son : oldVertex->sons) {
                newVertex->sons.push_back(new Node(*son));
                q.push({son, newVertex->sons.back()});
            }
            if (lastVertex != nullptr) {
                newVertex->leftBrother = lastVertex;
                lastVertex->rightBrother = newVertex;
            }
            lastVertex = newVertex;
        }
        return *this;
    }

    Set(const Set &s) {
        if (s.root == nullptr) {
            return;
        }
        Node* lastVertex = nullptr;
        std::queue<std::pair<Node*, Node*>> q;
        root = new Node(*s.root);
        q.push({s.root, root});
        while (!q.empty()) {
            Node* oldVertex;
            Node* newVertex;
            oldVertex = q.front().first;
            newVertex = q.front().second;
            q.pop();

            for (auto son : oldVertex->sons) {
                newVertex->sons.push_back(new Node(*son));
                q.push({son, newVertex->sons.back()});
            }
            if (lastVertex != nullptr) {
                newVertex->leftBrother = lastVertex;
                lastVertex->rightBrother = newVertex;
            }
            lastVertex = newVertex;
        }
    }

    ~Set() {
        if (root != nullptr) {
            clear(root);
            root = nullptr;
        }
    }

    iterator begin() const {
        Node* cur = root;
        while (cur != nullptr && !cur->sons.empty()) {
            cur = cur->sons[0];
        }
        return iterator(cur, false);
    }

    iterator end() const {
        Node* cur = root;
        while (cur != nullptr && !cur->sons.empty()) {
            cur = cur->sons.back();
        }
        return iterator(cur, true);
    }

    iterator find(ValueType key) const {
        if (root == nullptr) {
            return end();
        }
        Node* cur = root;
        while (!cur->sons.empty()) {
            bool found = false;
            for (Node* son : cur->sons) {
                if (!(key < son->minValue) && !(son->maxValue < key)) {
                    cur = son;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return end();
            }
        }
        if (!(cur->minValue < key) && !(key < cur->minValue)) {
            return iterator(cur, false);
        }
        return end();
    }

    iterator lower_bound(ValueType key) const {
        if (root == nullptr) {
            return end();
        }
        Node* cur = root;
        while (!cur->sons.empty()) {
            bool found = false;
            for (Node* son : cur->sons) {
                if (!(son->maxValue < key)) {
                    cur = son;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return end();
            }
        }

        return iterator(cur, false);
    }

    void insert(ValueType key) {
        if (root == nullptr) {
            root = new Node(1, key);
            root->isLeafHolder = true;
            root->sons.push_back(new Node(1, key));
            return;
        }
        Node* cur = root;
        std::vector<Node*> path;
        while (!cur->isLeafHolder) {
            path.push_back(cur);
            bool found = false;
            for (Node* son : cur->sons) {
                if (!(son->maxValue < key)) {
                    cur = son;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cur = cur->sons.back();
            }
        }
        bool found = false;
        for (Node* son : cur->sons) {
            if (!(son->minValue < key) && !(key < son->minValue)) {
                found = true;
                break;
            }
        }
        if (found) {
            return;
        }
        path.push_back(cur);
        cur->addSon(new Node(1, key));
        Node* secondRoot = nullptr;
        while (!path.empty()) {
            cur = path.back();
            path.pop_back();
            cur->fix();
            if (cur->sons.size() == 4) {
                Node* newVertex = new Node(*cur);
                newVertex->sons.push_back(cur->sons[2]);
                newVertex->sons.push_back(cur->sons[3]);
                cur->sons.pop_back();
                cur->sons.pop_back();
                cur->fix();
                newVertex->fix();
                if (!path.empty()) {
                    path.back()->addSon(newVertex);
                }
                else {
                    secondRoot = newVertex;
                }
            }
        }
        if (secondRoot != nullptr) {
            Node* newRoot = new Node();
            newRoot->addSon(root);
            newRoot->addSon(secondRoot);
            newRoot->fix();
            std::swap(root, newRoot);
        }
    }

    std::size_t getHeight() {
        std::size_t h = 0;
        Node* cur = root;
        while (cur != nullptr && !cur->sons.empty()) {
            cur = cur->sons[0];
            ++h;
        }
        return h;
    }

    void erase(ValueType key) {
        if (root == nullptr) {
            return;
        }
        Node* cur = root;
        std::vector<Node*> path;

        while (!cur->isLeafHolder) {
            path.push_back(cur);
            bool found = false;
            for (Node* son : cur->sons) {
                if (!(son->maxValue < key)) {
                    cur = son;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return;
            }
        }
        path.push_back(cur);
        Node* erased = nullptr;
        for (Node* v : cur->sons) {
            if (!(v->minValue < key) && !(key < v->minValue)) {
                erased = v;
                break;
            }
        }
        if (erased == nullptr) {
            return;
        }
        while (!path.empty()) {
            cur = path.back();
            path.pop_back();
            cur->eraseSon(erased);
            cur->fix();
            erased = nullptr;
            if (cur->leftBrother != nullptr) {
                cur->leftBrother->fix();
            }
            if (cur->rightBrother != nullptr) {
                cur->rightBrother->fix();
            }
            if (cur->sons.size() == 1) {
                if (cur->leftBrother != nullptr && cur->leftBrother->sons.size() == 3) {
                    cur->sons.push_back(cur->sons[0]);
                    cur->sons[0] = cur->leftBrother->sons.back();
                    cur->leftBrother->sons.pop_back();
                    cur->leftBrother->fix();
                }
                else {
                    if (cur->rightBrother != nullptr && cur->rightBrother->sons.size() == 3) {
                        cur->sons.push_back(cur->rightBrother->sons.front());
                        for (std::size_t i = 1; i < 3; ++i) {
                            cur->rightBrother->sons[i - 1] = cur->rightBrother->sons[i];
                        }
                        cur->rightBrother->sons.pop_back();
                        cur->rightBrother->fix();
                    }
                    else {
                        if (cur->leftBrother != nullptr) {
                            cur->leftBrother->sons.push_back(cur->sons[0]);
                            erased = cur;
                            cur->sons.clear();
                            cur->leftBrother->fix();
                        }
                        else {
                            if (cur->rightBrother != nullptr) {
                                cur->rightBrother->sons.push_back(nullptr);
                                for (std::size_t i = 2; i > 0; --i) {
                                    cur->rightBrother->sons[i] = cur->rightBrother->sons[i - 1];
                                }
                                cur->rightBrother->sons[0] = cur->sons[0];
                                erased = cur;
                                cur->sons.clear();
                                cur->rightBrother->fix();
                            }
                            else {
                                if (cur->size != 1) {
                                    root = cur->sons[0];
                                    delete cur;
                                    root->fix();
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            cur->fix();
        }
        if (root->sons.empty()) {
            delete root;
            root = nullptr;
        }
    }

    std::size_t size() const {
        if (root == nullptr) {
            return 0;
        }
        return root->size;
    }

    bool empty() const {
        return size() == 0;
    }
};