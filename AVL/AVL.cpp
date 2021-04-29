#include <iostream>
#include <exception>
#include <string>
#include <cassert>

template <typename Key, typename Value>
class avl_tree;

class avl_testcase;

template <typename Key, typename Value>
class avl_node {
public:
    avl_node(avl_node* parent, Key key, Value value = Value())
        :
        key(key),
        value(value),
        height(0),
        left(nullptr),
        right(nullptr),
        parent(parent)
    {
        if (parent) {
            if (parent->key > key) {
                parent->left = this;
            }
            else {
                parent->right = this;
            }
        }
    }

    ~avl_node() 
    {

    }
    Key key;
    Value value;

    bool leaf() const {
        return !left && !right;
    }

private:
    int height;
    avl_node* left;
    avl_node* right;
    avl_node* parent;

    static void set_left(avl_node* node, avl_node* child) {
       // assert((node == nullptr && child == nullptr) || node != child);
        if (node) node->left = child;
        if (child) child->parent = node;
    }

    static void set_right(avl_node* node, avl_node* child) {
       // assert((node == nullptr && child == nullptr) || node != child);
        if (node) node->right = child;
        if (child) child->parent = node;
    }

    void set_left(avl_node* node) {
        set_left(this, node);
    }

    void set_right(avl_node* node) {
        set_right(this, node);
    }

    void ll() {
        avl_node* p = parent;
        avl_node* a = this;
        avl_node* b = left;
        avl_node* c = left->right;

        b->set_right(a);
        a->set_left(c);
        set_left(p, b);

        a->upd();
        b->upd();
    }
    
    void rr() {
        avl_node* p = parent;
        avl_node* a = this;
        avl_node* b = right;
        avl_node* c = right->left;

        b->set_left(a);
        a->set_right(c);
        set_right(p, b);

        a->upd();
        b->upd();
    }

    void lr() {
        avl_node* p = parent;
        avl_node* a = this;
        avl_node* b = left;
        avl_node* c = left->right;
        avl_node* d = c ? c->right : nullptr;

        a->set_left(d);
        b->set_right(nullptr);
        c->set_left(b);
        c->set_right(a);
        set_left(p, c);

        a->upd();
        b->upd();
        c->upd();
    }

    void rl() {
        avl_node* p = parent;
        avl_node* a = this;
        avl_node* b = right;
        avl_node* c = right->left;
        avl_node* d = c ? c->left : nullptr;

        a->set_right(d);
        b->set_left(nullptr);
        c->set_right(b);
        c->set_left(a);
        set_right(p, c);

        a->upd();
        b->upd();
        c->upd();
    }

    int lh() {
        return left ? left->height : -1;
    }

    int rh() {
        return right ? right->height : -1;
    }

    void upd() {
        height = std::max(lh(), rh()) + 1;
    }

public:
    friend class avl_tree<Key, Value>;
    friend class avl_testcase;
};

template <typename Key, typename Value>
class avl_tree {
public:
    typedef avl_node<Key, Value> node_type;

    avl_tree() : root(nullptr), size_(0) {}

    void add(const Key& key, const Value& value) {
        if (!root) {
            root = new node_type(nullptr, key, value);
            size_++;
        }
        else {
            node_type* p;
            node_type* node = take(key, &p);
            if (!node) {
                node = new node_type(p, key);
                size_++;
            }
            node->value = value;
            root = valance(node);
        }
    }

    Value get(const Key& key) {
        node_type* node = take(key);
        if (node) return node->value;
        throw std::exception("not found: ");
    }

    bool containsKey(const Key& key) {
        return take(key);
    }

    node_type* lower_bound(const Key& key) {
        node_type* node = root;

        while (node) {
            if ((key <= node->key && !node->left) || (key > node->key && !node->right)) {
                while (node) {
                    if (key <= node->key) return node;
                    node = node->parent;
                }
                return nullptr;
            }
            if (key <= node->key) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return node;
    }

    node_type* upper_bound(const Key& key) {
        node_type* node = root;

        while (node) {
            if ((key < node->key && !node->left) || (key >= node->key && !node->right)) {
                while (node) {
                    if (key < node->key) return node;
                    node = node->parent;
                }
                return nullptr;
            }
            if (key < node->key) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return node;
    }

    node_type* reverse_upper_bound(const Key& key) {
        node_type* node = root;

        while (node) {
            if ((key <= node->key && !node->left) || (key > node->key && !node->right)) {
                while (node) {
                    if (key > node->key) return node;
                    node = node->parent;
                }
                return nullptr;
            }
            if (key <= node->key) {
                node = node->left;
            }
            else {
                node = node->right;
            }
        }
        return node;
    }

    Key next(const Key& key) {
        return upper_bound(key)->key;
    }

    Key prev(const Key& key) {
        return reverse_upper_bound(key)->key;
    }

    bool remove(const Key& key) {
        node_type* node = take(key);
        if (!node) {
            return false;
        }
        if (node->leaf()) {
            if (node->parent->left == node) {
                node->parent->set_left(nullptr);
            }
            else {
                node->parent->set_right(nullptr);
            }
            valance(node->parent);
        }
        else if (node->left) {
            node_type* near = reverse_upper_bound(key);
            node_type* vnode = near->parent;
            near->set_left(node->left);
            near->set_right(node->right);
            valance(vnode);
        }
        else {
            node_type* near = upper_bound(key);
            node_type* vnode = near->parent;
            near->set_left(node->left);
            near->set_right(node->right);
            valance(vnode);
        }
        delete node;
        return true;
    }

    Value operator[](const Key& key) {
        return get(key);
    }

    int height() {
        return root ? root->height + 1 : 0;
    }

    size_t size() const {
        return size_;
    }

private:
    node_type* valance(node_type* node) {
        assert(node != nullptr);

        node_type* root = nullptr;
        while (node) {
            int lh = node->lh();
            int rh = node->rh();
            node->upd();
            if (lh - rh >= 2) {
                if (node->left->lh() > node->left->rh()) {
                    node->ll();
                }
                else {
                    node->lr();
                }
            }
            else if (lh - rh <= -2) {
                if (node->left->lh() < node->left->rh()) {
                    node->rr();
                }
                else {
                    node->rl();
                }
            }
            if (node->parent == nullptr) {
                root = node;
            }
            node = node->parent;
        }
        return root;
    }

    node_type* take(Key key, node_type** parent = nullptr) {
        node_type* node = root;

        while (node) {
            if (node->key == key)
                break;
            if (parent) {
                *parent = node;
            }
            if (key < node->key)
                node = node->left;
            else
                node = node->right;
        }
        return node;
    }
private:
    node_type* root;
    size_t size_;
};

class avl_testcase {
public:
    void test_all() {
        test_ll();
        test_lr();
        test_rr();
        test_rl();
    }

private:
    void test_ll() {
        avl_node<int, std::string>* node0 = new avl_node<int, std::string>(nullptr, 10, "root");
        avl_node<int, std::string>* node1 = new avl_node<int, std::string>(nullptr, 9, "node1");
        avl_node<int, std::string>* node2 = new avl_node<int, std::string>(nullptr, 8, "node2");
        avl_node<int, std::string>* node3 = new avl_node<int, std::string>(nullptr, 7, "node3");
        avl_node<int, std::string>* node4 = new avl_node<int, std::string>(nullptr, 6, "node4");

        node0->set_left(node1);
        node1->set_left(node2);
        node2->set_left(node3);
        node2->set_right(node4);

        assert(node1->parent == node0);
        assert(node1->left == node2);
        assert(node1->right == nullptr);

        assert(node2->parent == node1);
        assert(node2->left == node3);
        assert(node2->right == node4);

        assert(node3->parent == node2);
        assert(node3->left == nullptr);
        assert(node3->right == nullptr);

        node1->ll();

        assert(node1->parent == node2);
        assert(node1->left == node4);
        assert(node1->right == nullptr);

        assert(node2->parent == node0);
        assert(node2->left == node3);
        assert(node2->right == node1);

        assert(node3->parent == node2);
        assert(node3->left == nullptr);
        assert(node3->right == nullptr);
    }

    void test_lr() {
        avl_node<int, std::string>* node0 = new avl_node<int, std::string>(nullptr, 10, "root");
        avl_node<int, std::string>* node1 = new avl_node<int, std::string>(nullptr, 9, "node1");
        avl_node<int, std::string>* node2 = new avl_node<int, std::string>(nullptr, 8, "node2");
        avl_node<int, std::string>* node3 = new avl_node<int, std::string>(nullptr, 7, "node3");
        avl_node<int, std::string>* node4 = new avl_node<int, std::string>(nullptr, 6, "node4");

        node0->set_left(node1);
        node1->set_left(node2);
        node2->set_right(node3);
        node3->set_right(node4);

        assert(node1->parent == node0);
        assert(node1->left == node2);
        assert(node1->right == nullptr);

        assert(node2->parent == node1);
        assert(node2->left == nullptr);
        assert(node2->right == node3);

        assert(node3->parent == node2);
        assert(node3->left == nullptr);
        assert(node3->right == node4);

        node1->lr();

        assert(node1->parent == node3);
        assert(node1->left == node4);
        assert(node1->right == nullptr);

        assert(node2->parent == node3);
        assert(node2->left == nullptr);
        assert(node2->right == nullptr);

        assert(node3->parent == node0);
        assert(node3->left == node2);
        assert(node3->right == node1);
    }

    void test_rr() {
        avl_node<int, std::string>* node0 = new avl_node<int, std::string>(nullptr, 10, "root");
        avl_node<int, std::string>* node1 = new avl_node<int, std::string>(nullptr, 9, "node1");
        avl_node<int, std::string>* node2 = new avl_node<int, std::string>(nullptr, 8, "node2");
        avl_node<int, std::string>* node3 = new avl_node<int, std::string>(nullptr, 7, "node3");
        avl_node<int, std::string>* node4 = new avl_node<int, std::string>(nullptr, 6, "node4");

        node0->set_right(node1);
        node1->set_right(node2);
        node2->set_right(node3);
        node2->set_left(node4);

        assert(node1->parent == node0);
        assert(node1->left == nullptr);
        assert(node1->right == node2);

        assert(node2->parent == node1);
        assert(node2->left == node4);
        assert(node2->right == node3);

        assert(node3->parent == node2);
        assert(node3->left == nullptr);
        assert(node3->right == nullptr);

        node1->rr();

        assert(node1->parent == node2);
        assert(node1->left == nullptr);
        assert(node1->right == node4);

        assert(node2->parent == node0);
        assert(node2->left == node1);
        assert(node2->right == node3);

        assert(node3->parent == node2);
        assert(node3->left == nullptr);
        assert(node3->right == nullptr);
    }

    void test_rl() {
        avl_node<int, std::string>* node0 = new avl_node<int, std::string>(nullptr, 10, "root");
        avl_node<int, std::string>* node1 = new avl_node<int, std::string>(nullptr, 9, "node1");
        avl_node<int, std::string>* node2 = new avl_node<int, std::string>(nullptr, 8, "node2");
        avl_node<int, std::string>* node3 = new avl_node<int, std::string>(nullptr, 7, "node3");
        avl_node<int, std::string>* node4 = new avl_node<int, std::string>(nullptr, 6, "node4");

        node0->set_right(node1);
        node1->set_right(node2);
        node2->set_left(node3);
        node3->set_left(node4);

        assert(node1->parent == node0);
        assert(node1->left == nullptr);
        assert(node1->right == node2);

        assert(node2->parent == node1);
        assert(node2->left == node3);
        assert(node2->right == nullptr);

        assert(node3->parent == node2);
        assert(node3->left == node4);
        assert(node3->right == nullptr);

        node1->rl();

        assert(node1->parent == node3);
        assert(node1->left == nullptr);
        assert(node1->right == node4);

        assert(node2->parent == node3);
        assert(node2->left == nullptr);
        assert(node2->right == nullptr);

        assert(node3->parent == node0);
        assert(node3->left == node1);
        assert(node3->right == node2);
    }
};

using namespace std;

int main()
{
    avl_testcase testcase;
    testcase.test_all();

    avl_tree<int, std::string> tree;
    assert(tree.height() == 0);
    assert(tree.size() == 0);
    tree.add(100, "A");
    assert(tree.height() == 1);
    assert(tree.size() == 1);
    tree.add(90, "B");
    assert(tree.height() == 2);
    assert(tree.size() == 2);
    tree.add(80, "C");
    assert(tree.height() == 2);
    assert(tree.size() == 3);
    tree.add(70, "D");
    assert(tree.height() == 3);
    assert(tree.size() == 4);
    tree.add(60, "E");
    assert(tree.height() == 3);
    assert(tree.size() == 5);
    tree.add(50, "FF");
    tree.add(50, "F");
    assert(tree.height() == 3);
    assert(tree.size() == 6);
    tree.add(40, "G");
    assert(tree.height() == 3);
    assert(tree.size() == 7);
    tree.add(30, "H");
    assert(tree.height() == 4);
    assert(tree.size() == 8);
    tree.add(20, "I");
    assert(tree.height() == 4);
    assert(tree.size() == 9);
    tree.add(10, "J");
    assert(tree.height() == 4);
    assert(tree.size() == 10);

    assert(tree[50] == "F");
    assert(tree.containsKey(50) == true);
    assert(tree.containsKey(51) == false);


    // test upper bound
    assert(tree.upper_bound(1)->key == 10);
    assert(tree.upper_bound(10)->key == 20);
    assert(tree.upper_bound(11)->key == 20);
    assert(tree.upper_bound(20)->key == 30);
    assert(tree.upper_bound(21)->key == 30);
    assert(tree.upper_bound(30)->key == 40);
    assert(tree.upper_bound(31)->key == 40);
    assert(tree.upper_bound(40)->key == 50);
    assert(tree.upper_bound(41)->key == 50);
    assert(tree.upper_bound(50)->key == 60);
    assert(tree.upper_bound(51)->key == 60);
    assert(tree.upper_bound(60)->key == 70);
    assert(tree.upper_bound(61)->key == 70);
    assert(tree.upper_bound(70)->key == 80);
    assert(tree.upper_bound(71)->key == 80);
    assert(tree.upper_bound(80)->key == 90);
    assert(tree.upper_bound(81)->key == 90);
    assert(tree.upper_bound(90)->key == 100);
    assert(tree.upper_bound(91)->key == 100);
    assert(tree.upper_bound(100) == nullptr);
    assert(tree.upper_bound(101) == nullptr);

    // test reverse upper bound
    assert(tree.reverse_upper_bound(10) == nullptr);
    assert(tree.reverse_upper_bound(11)->key == 10);
    assert(tree.reverse_upper_bound(20)->key == 10);
    assert(tree.reverse_upper_bound(21)->key == 20);
    assert(tree.reverse_upper_bound(30)->key == 20);
    assert(tree.reverse_upper_bound(31)->key == 30);
    assert(tree.reverse_upper_bound(40)->key == 30);
    assert(tree.reverse_upper_bound(41)->key == 40);
    assert(tree.reverse_upper_bound(50)->key == 40);
    assert(tree.reverse_upper_bound(51)->key == 50);
    assert(tree.reverse_upper_bound(60)->key == 50);
    assert(tree.reverse_upper_bound(61)->key == 60);
    assert(tree.reverse_upper_bound(70)->key == 60);
    assert(tree.reverse_upper_bound(71)->key == 70);
    assert(tree.reverse_upper_bound(80)->key == 70);
    assert(tree.reverse_upper_bound(81)->key == 80);
    assert(tree.reverse_upper_bound(90)->key == 80);
    assert(tree.reverse_upper_bound(91)->key == 90);
    assert(tree.reverse_upper_bound(100)->key == 90);
    assert(tree.reverse_upper_bound(101)->key == 100);

    // test lower bound
    assert(tree.lower_bound(1)->key == 10);
    assert(tree.lower_bound(10)->key == 10);
    assert(tree.lower_bound(11)->key == 20);
    assert(tree.lower_bound(20)->key == 20);
    assert(tree.lower_bound(21)->key == 30);
    assert(tree.lower_bound(30)->key == 30);
    assert(tree.lower_bound(31)->key == 40);
    assert(tree.lower_bound(40)->key == 40);
    assert(tree.lower_bound(41)->key == 50);
    assert(tree.lower_bound(50)->key == 50);
    assert(tree.lower_bound(51)->key == 60);
    assert(tree.lower_bound(60)->key == 60);
    assert(tree.lower_bound(61)->key == 70);
    assert(tree.lower_bound(70)->key == 70);
    assert(tree.lower_bound(71)->key == 80);
    assert(tree.lower_bound(80)->key == 80);
    assert(tree.lower_bound(81)->key == 90);
    assert(tree.lower_bound(90)->key == 90);
    assert(tree.lower_bound(91)->key == 100);
    assert(tree.lower_bound(100)->key == 100);
    assert(tree.lower_bound(101) == nullptr);
}