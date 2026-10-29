/*
 * =====================================================================
 *  LIBRARY MANAGEMENT SYSTEM  -  C++ (Data Structures & Algorithms)
 * =====================================================================
 *  The program output looks like a normal library system.
 *  All data structures are written from scratch inside this file:
 *
 *   DATA STRUCTURE / ALGORITHM     USED FOR                         TIME
 *   ------------------------------------------------------------------
 *   1. Hash Table (chaining)       find Book by ID, Member by PRN   O(1)
 *   2. Binary Search Tree (BST)    search / A-Z list of books       O(log n)
 *   3. Singly Linked List          books issued to a member         O(k)
 *   4. Queue (FIFO)               reservation list of a book        O(1)
 *   5. Stack (LIFO)               undo last issue / return          O(1)
 *   6. Max-Heap                   popular (most issued) books       O(k log n)
 *   7. Merge Sort                 sort books by author, members PRN O(n log n)
 *   8. Binary Search              search author in sorted list      O(log n)
 *
 *  Compile :  g++ -std=c++11 library_dsa.cpp -o library
 *  Run     :  ./library          (Windows: library.exe)
 * =====================================================================
 */
#define _CRT_SECURE_NO_WARNINGS   // keeps Visual Studio quiet about localtime
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <ctime>
using namespace std;

const int MAX_BORROW   = 3;   // max books a member can hold at once
const int LOAN_DAYS    = 14;  // loan period
const int FINE_PER_DAY = 2;   // Rs. per day after the due date

// ---------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------
static string toLower(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}
static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
static string formatDate(time_t t) {
    char buf[32];
    strftime(buf, sizeof(buf), "%d-%b-%Y", localtime(&t));
    return string(buf);
}

// One loan: which book, when issued, when due
struct IssueRecord {
    int bookId;
    time_t issued;
    time_t due;
};

// =====================================================================
// DSA 1: SINGLY LINKED LIST
//   Each member owns one list of the books currently issued to them.
//   Adding / removing a loan = inserting / deleting a node.
// =====================================================================
struct LNode {
    IssueRecord data;
    LNode* next;
    LNode(const IssueRecord& d) : data(d), next(nullptr) {}
};

class LinkedList {
    LNode* head;
    int count;
public:
    LinkedList() : head(nullptr), count(0) {}
    ~LinkedList() {
        while (head) { LNode* t = head; head = head->next; delete t; }
    }
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void add(const IssueRecord& r) {               // insert at tail
        LNode* n = new LNode(r);
        if (!head) head = n;
        else { LNode* c = head; while (c->next) c = c->next; c->next = n; }
        count++;
    }
    bool remove(int bookId, IssueRecord& removed) { // delete node by book ID
        LNode *cur = head, *prev = nullptr;
        while (cur) {
            if (cur->data.bookId == bookId) {
                removed = cur->data;
                if (prev) prev->next = cur->next; else head = cur->next;
                delete cur; count--;
                return true;
            }
            prev = cur; cur = cur->next;
        }
        return false;
    }
    bool contains(int bookId) const {
        for (LNode* c = head; c; c = c->next) if (c->data.bookId == bookId) return true;
        return false;
    }
    int size() const { return count; }
    const LNode* first() const { return head; }
};

// =====================================================================
// DSA 2: QUEUE (FIFO) - linked-list based
//   Each book has a queue of members waiting for it (reservations).
//   First member to reserve is the first to be served.
// =====================================================================
template <typename T>
class Queue {
    struct Node { T data; Node* next; Node(const T& d) : data(d), next(nullptr) {} };
    Node *head, *tail;
    int count;
public:
    Queue() : head(nullptr), tail(nullptr), count(0) {}
    ~Queue() { while (!empty()) dequeue(); }
    Queue(const Queue&) = delete;
    Queue& operator=(const Queue&) = delete;

    void enqueue(const T& v) {                      // add at rear
        Node* n = new Node(v);
        if (tail) tail->next = n; else head = n;
        tail = n; count++;
    }
    void pushFront(const T& v) {                    // put back at front (used by undo)
        Node* n = new Node(v);
        n->next = head; head = n;
        if (!tail) tail = n;
        count++;
    }
    T dequeue() {                                   // remove from front
        Node* t = head; T v = t->data;
        head = head->next;
        if (!head) tail = nullptr;
        delete t; count--;
        return v;
    }
    const T& front() const { return head->data; }
    bool empty() const { return head == nullptr; }
    int size() const { return count; }
    bool contains(const T& x) const {
        for (Node* c = head; c; c = c->next) if (c->data == x) return true;
        return false;
    }
    vector<T> toVector() const {
        vector<T> v;
        for (Node* c = head; c; c = c->next) v.push_back(c->data);
        return v;
    }
};

// =====================================================================
// DSA 3: STACK (LIFO) - linked-list based
//   Every issue / return is pushed here. "Undo last transaction"
//   pops the most recent one and reverses it.
// =====================================================================
template <typename T>
class Stack {
    struct Node { T data; Node* next; Node(const T& d, Node* n) : data(d), next(n) {} };
    Node* topNode;
    int count;
public:
    Stack() : topNode(nullptr), count(0) {}
    ~Stack() { while (!empty()) pop(); }
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    void push(const T& v) { topNode = new Node(v, topNode); count++; }
    T pop() {
        Node* t = topNode; T v = t->data;
        topNode = t->next; delete t; count--;
        return v;
    }
    bool empty() const { return topNode == nullptr; }
    int size() const { return count; }
};

// =====================================================================
// DSA 4: HASH TABLE (separate chaining)
//   Key (string) -> hash function -> bucket -> chain of nodes.
//   Used for Book ID -> Book  and  PRN -> Member.  Lookup is O(1).
// =====================================================================
template <typename V>
class HashTable {
    struct Node { string key; V* val; Node* next; };
    static const int SIZE = 53;                     // prime number of buckets
    Node* table[SIZE];

    unsigned int hashFn(const string& key) const {  // djb2 string hash
        unsigned long long h = 5381;
        for (size_t i = 0; i < key.size(); i++) h = h * 33 + (unsigned char)key[i];
        return (unsigned int)(h % SIZE);
    }
public:
    HashTable() { for (int i = 0; i < SIZE; i++) table[i] = nullptr; }
    ~HashTable() {
        for (int i = 0; i < SIZE; i++) {
            Node* c = table[i];
            while (c) { Node* n = c->next; delete c->val; delete c; c = n; }
        }
    }
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void put(const string& key, V* val) {           // insert at head of chain
        unsigned int h = hashFn(key);
        Node* n = new Node{key, val, table[h]};
        table[h] = n;
    }
    V* get(const string& key) const {               // average O(1)
        for (Node* c = table[hashFn(key)]; c; c = c->next)
            if (c->key == key) return c->val;
        return nullptr;
    }
    vector<V*> all() const {
        vector<V*> out;
        for (int i = 0; i < SIZE; i++)
            for (Node* c = table[i]; c; c = c->next) out.push_back(c->val);
        return out;
    }
};

// =====================================================================
// Core records
// =====================================================================
struct Book {
    int id;
    string title, author, category;
    int total, available, issueCount;
    Queue<string> waitlist;                         // PRNs of members waiting (FIFO)
    Book(int i, const string& t, const string& a, const string& c, int n)
        : id(i), title(t), author(a), category(c),
          total(n), available(n), issueCount(0) {}
};

struct Member {
    string prn, name, dept;
    LinkedList issued;                              // linked list of loans
    Member(const string& p, const string& n, const string& d) : prn(p), name(n), dept(d) {}
};

struct Action {                                     // one entry on the undo stack
    char type;                                      // 'I' = issue, 'R' = return
    int bookId;
    string prn;
    bool wasQueued;                                 // issue was served from reservation list
    IssueRecord rec;                                // loan details (needed to undo a return)
};

// =====================================================================
// DSA 5: BINARY SEARCH TREE - keyed by lower-case book title
//   Search a title in O(log n); in-order traversal gives A-Z order.
// =====================================================================
struct BNode {
    string key;
    Book* book;
    BNode *left, *right;
};

class BST {
    BNode* root;
    BNode* insert(BNode* n, const string& k, Book* b) {
        if (!n) return new BNode{k, b, nullptr, nullptr};
        if (k < n->key) n->left = insert(n->left, k, b);
        else            n->right = insert(n->right, k, b);
        return n;
    }
    void inorder(BNode* n, vector<Book*>& out) const {
        if (!n) return;
        inorder(n->left, out);                      // left subtree
        out.push_back(n->book);                     // node
        inorder(n->right, out);                     // right subtree -> sorted order
    }
    void destroy(BNode* n) {
        if (!n) return;
        destroy(n->left); destroy(n->right); delete n;
    }
public:
    BST() : root(nullptr) {}
    ~BST() { destroy(root); }
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    void insert(const string& k, Book* b) { root = insert(root, k, b); }
    Book* search(const string& k) const {           // go left/right by comparison
        BNode* c = root;
        while (c) {
            if (k == c->key) return c->book;
            c = (k < c->key) ? c->left : c->right;
        }
        return nullptr;
    }
    vector<Book*> sortedList() const {
        vector<Book*> out; inorder(root, out); return out;
    }
};

// =====================================================================
// DSA 6: MAX-HEAP - ordered by issueCount
//   Root is always the most-issued book. Used for "Popular Books".
// =====================================================================
class MaxHeap {
    vector<Book*> a;
    void siftDown(int i) {
        int n = (int)a.size();
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, m = i;
            if (l < n && a[l]->issueCount > a[m]->issueCount) m = l;
            if (r < n && a[r]->issueCount > a[m]->issueCount) m = r;
            if (m == i) break;
            swap(a[i], a[m]); i = m;
        }
    }
public:
    void build(const vector<Book*>& v) {            // heapify: O(n)
        a = v;
        for (int i = (int)a.size() / 2 - 1; i >= 0; i--) siftDown(i);
    }
    Book* pop() {                                   // remove max: O(log n)
        Book* top = a[0];
        a[0] = a.back(); a.pop_back();
        if (!a.empty()) siftDown(0);
        return top;
    }
    bool empty() const { return a.empty(); }
};

// =====================================================================
// DSA 7: MERGE SORT  (divide, sort halves, merge) - O(n log n)
//   Generic: sorts books by author and members by PRN.
// =====================================================================
template <typename T>
static void mergeSort(vector<T*>& v, int l, int r, bool (*lessThan)(const T*, const T*)) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSort(v, l, m, lessThan);
    mergeSort(v, m + 1, r, lessThan);
    vector<T*> tmp;
    int i = l, j = m + 1;
    while (i <= m && j <= r) {
        if (lessThan(v[j], v[i])) tmp.push_back(v[j++]);   // stable merge
        else                      tmp.push_back(v[i++]);
    }
    while (i <= m) tmp.push_back(v[i++]);
    while (j <= r) tmp.push_back(v[j++]);
    for (size_t k = 0; k < tmp.size(); k++) v[l + (int)k] = tmp[k];
}
static bool byAuthor(const Book* x, const Book* y) { return toLower(x->author) < toLower(y->author); }
static bool byPrn(const Member* x, const Member* y) { return x->prn < y->prn; }

// =====================================================================
// DSA 8: BINARY SEARCH on the author-sorted array - O(log n)
//   Returns index of the first author starting with `prefix`, or -1.
// =====================================================================
static int binarySearchAuthor(const vector<Book*>& v, const string& prefix) {
    int lo = 0, hi = (int)v.size();
    while (lo < hi) {
        int mid = (lo + hi) / 2;
        if (toLower(v[mid]->author) < prefix) lo = mid + 1;   // go right
        else hi = mid;                                        // go left
    }
    if (lo < (int)v.size() && toLower(v[lo]->author).compare(0, prefix.size(), prefix) == 0)
        return lo;
    return -1;
}

// =====================================================================
// Display helpers (normal library-style tables)
// =====================================================================
static void printBookTable(const vector<Book*>& v, bool showIssued = false) {
    cout << left << setw(6) << "ID" << setw(30) << "Book Title" << setw(22) << "Author"
         << setw(14) << "Category" << setw(11) << "Available";
    if (showIssued) cout << "Times Issued";
    cout << "\n" << string(showIssued ? 95 : 83, '-') << "\n";
    for (size_t i = 0; i < v.size(); i++) {
        const Book* b = v[i];
        cout << left << setw(6) << b->id << setw(30) << b->title.substr(0, 28)
             << setw(22) << b->author.substr(0, 20) << setw(14) << b->category.substr(0, 12)
             << setw(11) << (to_string(b->available) + "/" + to_string(b->total));
        if (showIssued) cout << b->issueCount;
        cout << "\n";
    }
}

// =====================================================================
// LIBRARY - ties all the data structures together
// =====================================================================
class Library {
    HashTable<Book> books;          // DSA: Hash Table   (Book ID -> Book)
    HashTable<Member> members;      // DSA: Hash Table   (PRN -> Member)
    BST titleIndex;                 // DSA: BST          (title -> Book)
    Stack<Action> history;          // DSA: Stack        (undo)
    int nextBookId;
public:
    Library() : nextBookId(101) {}

    // ---------------- add book / register member ----------------
    int addBook(const string& title, const string& author, const string& cat, int copies, bool quiet = false) {
        int id = nextBookId++;
        Book* b = new Book(id, title, author, cat, copies);
        books.put(to_string(id), b);                 // Hash Table insert
        titleIndex.insert(toLower(title), b);        // BST insert
        if (!quiet) {
            cout << "Book added successfully.\n";
            cout << "  Book ID  : " << id << "\n  Title    : " << title << "\n  Author   : " << author
                 << "\n  Category : " << cat << "\n  Copies   : " << copies << "\n";
        }
        return id;
    }
    void registerMember(const string& prn, const string& name, const string& dept, bool quiet = false) {
        if (members.get(prn)) { cout << "A member with PRN " << prn << " already exists.\n"; return; }
        members.put(prn, new Member(prn, name, dept)); // Hash Table insert
        if (!quiet) {
            cout << "Member registered successfully.\n";
            cout << "  PRN        : " << prn << "\n  Name       : " << name
                 << "\n  Department : " << dept << "\n";
        }
    }

    // ---------------- issue ----------------
    void issueBook(int bookId, const string& prn) {
        Book* b = books.get(to_string(bookId));      // Hash Table lookup O(1)
        Member* m = members.get(prn);                // Hash Table lookup O(1)
        if (!b) { cout << "Book not found.\n"; return; }
        if (!m) { cout << "Member with PRN " << prn << " not found.\n"; return; }
        if (m->issued.contains(bookId)) { cout << m->name << " already has this book.\n"; return; }
        if (m->issued.size() >= MAX_BORROW) {
            cout << m->name << " has reached the limit of " << MAX_BORROW << " books.\n"; return;
        }
        if (b->available == 0) {
            cout << "Sorry, no copies of '" << b->title << "' are available. You can reserve it.\n"; return;
        }
        if (!b->waitlist.empty() && b->waitlist.front() != prn) {   // Queue front = next in line
            Member* nxt = members.get(b->waitlist.front());
            cout << "This book is reserved for " << (nxt ? nxt->name : "another member") << ".\n"; return;
        }
        bool wasQueued = !b->waitlist.empty();
        if (wasQueued) b->waitlist.dequeue();        // Queue dequeue (first reserved is served)

        IssueRecord rec;
        rec.bookId = bookId;
        rec.issued = time(nullptr);
        rec.due = rec.issued + (time_t)LOAN_DAYS * 24 * 60 * 60;
        b->available--;
        b->issueCount++;
        m->issued.add(rec);                          // Linked List insert
        Action a = {'I', bookId, prn, wasQueued, rec};
        history.push(a);                             // Stack push (for undo)

        cout << "Book issued successfully.\n";
        cout << "  Book        : " << b->title << " (ID " << b->id << ")\n";
        cout << "  Issued to   : " << m->name << " (PRN " << m->prn << ")\n";
        cout << "  Issue date  : " << formatDate(rec.issued) << "\n";
        cout << "  Due date    : " << formatDate(rec.due) << "\n";
    }

    // ---------------- return ----------------
    void returnBook(int bookId, const string& prn) {
        Book* b = books.get(to_string(bookId));
        Member* m = members.get(prn);
        if (!b) { cout << "Book not found.\n"; return; }
        if (!m) { cout << "Member with PRN " << prn << " not found.\n"; return; }
        IssueRecord rec;
        if (!m->issued.remove(bookId, rec)) {        // Linked List delete
            cout << m->name << " has not borrowed this book.\n"; return;
        }
        b->available++;
        Action a = {'R', bookId, prn, false, rec};
        history.push(a);                             // Stack push (for undo)

        time_t now = time(nullptr);
        int fine = 0;
        if (now > rec.due) {
            int daysLate = (int)((now - rec.due) / (24 * 60 * 60));
            if (daysLate > 0) fine = daysLate * FINE_PER_DAY;
        }
        cout << "Book returned successfully.\n";
        cout << "  Book        : " << b->title << " (ID " << b->id << ")\n";
        cout << "  Returned by : " << m->name << " (PRN " << m->prn << ")\n";
        cout << "  Return date : " << formatDate(now) << "\n";
        if (fine > 0) cout << "  Late fine   : Rs. " << fine << "\n";
        else          cout << "  Late fine   : None\n";

        if (!b->waitlist.empty()) {                  // Queue front = who gets it next
            Member* nxt = members.get(b->waitlist.front());
            cout << "  Note: this book is reserved. Please keep it for "
                 << (nxt ? nxt->name : "the next member") << " (PRN " << b->waitlist.front() << ").\n";
        }
    }

    // ---------------- reserve ----------------
    void reserveBook(int bookId, const string& prn) {
        Book* b = books.get(to_string(bookId));
        Member* m = members.get(prn);
        if (!b) { cout << "Book not found.\n"; return; }
        if (!m) { cout << "Member with PRN " << prn << " not found.\n"; return; }
        if (m->issued.contains(bookId)) { cout << m->name << " already has this book.\n"; return; }
        if (b->waitlist.contains(prn)) { cout << m->name << " has already reserved this book.\n"; return; }
        if (b->available > 0 && b->waitlist.empty()) {
            cout << "A copy is available right now. You can issue it directly.\n"; return;
        }
        b->waitlist.enqueue(prn);                    // Queue enqueue (added at the rear)
        cout << "Book reserved successfully.\n";
        cout << "  Book           : " << b->title << "\n";
        cout << "  Reserved by    : " << m->name << " (PRN " << m->prn << ")\n";
        cout << "  Queue position : " << b->waitlist.size() << "\n";
    }

    // ---------------- undo last transaction ----------------
    void undoLast() {
        if (history.empty()) { cout << "No transaction to undo.\n"; return; }
        Action a = history.pop();                    // Stack pop (most recent first)
        Book* b = books.get(to_string(a.bookId));
        Member* m = members.get(a.prn);
        if (a.type == 'I') {                         // cancel an issue
            IssueRecord tmp;
            m->issued.remove(a.bookId, tmp);
            b->available++; b->issueCount--;
            if (a.wasQueued) b->waitlist.pushFront(a.prn);   // restore reservation
            cout << "Last transaction cancelled: issue of '" << b->title << "' to " << m->name << ".\n";
        } else {                                     // cancel a return
            m->issued.add(a.rec);
            b->available--;
            cout << "Last transaction cancelled: return of '" << b->title << "' by " << m->name << ".\n";
        }
    }

    // ---------------- searching / listing books ----------------
    void searchById(int id) {
        Book* b = books.get(to_string(id));          // Hash Table lookup O(1)
        if (!b) { cout << "No book found with ID " << id << ".\n"; return; }
        vector<Book*> one(1, b);
        printBookTable(one);
    }
    void searchByTitle(const string& query) {
        string q = toLower(query);
        Book* b = titleIndex.search(q);              // BST search O(log n)
        if (b) { vector<Book*> one(1, b); printBookTable(one); return; }
        vector<Book*> all = titleIndex.sortedList(); // BST in-order traversal
        vector<Book*> hits;
        for (size_t i = 0; i < all.size(); i++)
            if (toLower(all[i]->title).find(q) != string::npos) hits.push_back(all[i]);
        if (hits.empty()) { cout << "No matching books found.\n"; return; }
        printBookTable(hits);
    }
    void showAllBooks() {
        vector<Book*> v = titleIndex.sortedList();   // BST in-order = A-Z by title
        cout << "All books (A-Z):\n";
        printBookTable(v);
    }
    void showBooksByAuthor() {
        vector<Book*> v = books.all();
        mergeSort(v, 0, (int)v.size() - 1, byAuthor); // Merge Sort by author
        cout << "Books sorted by author:\n";
        printBookTable(v);

        cout << "\nSearch author name (press Enter to skip): ";
        string q;
        if (!getline(cin, q)) return;
        q = toLower(trim(q));
        if (q.empty()) return;
        int idx = binarySearchAuthor(v, q);          // Binary Search O(log n)
        if (idx < 0) { cout << "No books found for that author.\n"; return; }
        vector<Book*> hits;
        for (size_t i = idx; i < v.size() && toLower(v[i]->author).compare(0, q.size(), q) == 0; i++)
            hits.push_back(v[i]);
        cout << "\n";
        printBookTable(hits);
    }

    // ---------------- popular books ----------------
    void popularBooks(int k) {
        MaxHeap h; h.build(books.all());             // Max-Heap build O(n)
        vector<Book*> top;
        while (!h.empty() && (int)top.size() < k) {
            Book* b = h.pop();                       // Max-Heap extract-max O(log n)
            if (b->issueCount == 0) break;
            top.push_back(b);
        }
        if (top.empty()) { cout << "No books have been issued yet.\n"; return; }
        cout << "Most popular books:\n";
        printBookTable(top, true);
    }

    // ---------------- member views ----------------
    void showMemberBooks(const string& prn) {
        Member* m = members.get(prn);
        if (!m) { cout << "Member with PRN " << prn << " not found.\n"; return; }
        cout << "Member     : " << m->name << "\nPRN        : " << m->prn
             << "\nDepartment : " << m->dept << "\nBooks issued: " << m->issued.size() << "\n";
        if (m->issued.size() == 0) return;
        cout << "\n" << left << setw(6) << "ID" << setw(32) << "Book Title"
             << setw(14) << "Issue Date" << "Due Date" << "\n" << string(64, '-') << "\n";
        for (const LNode* n = m->issued.first(); n; n = n->next) {   // Linked List traversal
            Book* b = books.get(to_string(n->data.bookId));
            cout << left << setw(6) << n->data.bookId << setw(32) << (b ? b->title.substr(0, 30) : "?")
                 << setw(14) << formatDate(n->data.issued) << formatDate(n->data.due) << "\n";
        }
    }
    void showReservations(int bookId) {
        Book* b = books.get(to_string(bookId));
        if (!b) { cout << "Book not found.\n"; return; }
        vector<string> q = b->waitlist.toVector();   // Queue traversal (front -> rear)
        if (q.empty()) { cout << "No reservations for '" << b->title << "'.\n"; return; }
        cout << "Reservations for '" << b->title << "':\n";
        for (size_t i = 0; i < q.size(); i++) {
            Member* m = members.get(q[i]);
            cout << "  " << i + 1 << ". " << (m ? m->name : "?") << " (PRN " << q[i] << ")\n";
        }
    }
    void showAllMembers() {
        vector<Member*> v = members.all();
        mergeSort(v, 0, (int)v.size() - 1, byPrn);   // Merge Sort by PRN
        cout << left << setw(14) << "PRN" << setw(22) << "Name" << setw(28) << "Department"
             << "Books Issued\n" << string(76, '-') << "\n";
        for (size_t i = 0; i < v.size(); i++)
            cout << left << setw(14) << v[i]->prn << setw(22) << v[i]->name.substr(0, 20)
                 << setw(28) << v[i]->dept.substr(0, 26) << v[i]->issued.size() << "\n";
    }

    // ---------------- sample data ----------------
    void loadSample() {
        addBook("The Hobbit", "J.R.R. Tolkien", "Fantasy", 2, true);
        addBook("1984", "George Orwell", "Fiction", 2, true);
        addBook("Atomic Habits", "James Clear", "Self-Help", 3, true);
        addBook("Sapiens", "Yuval Noah Harari", "History", 2, true);
        addBook("Clean Code", "Robert C. Martin", "Programming", 1, true);
        addBook("Introduction to Algorithms", "Thomas H. Cormen", "Programming", 2, true);
        addBook("Animal Farm", "George Orwell", "Fiction", 2, true);
        addBook("The Alchemist", "Paulo Coelho", "Fiction", 1, true);
        registerMember("2201001", "Aarav Patil", "Computer Engineering", true);
        registerMember("2201002", "Sneha Kulkarni", "Information Technology", true);
        registerMember("2201003", "Rohan Deshmukh", "Computer Engineering", true);
    }
};

// =====================================================================
// Input helpers
// =====================================================================
static string readString(const string& prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) { cout << "\nGoodbye!\n"; exit(0); }
    return trim(s);
}
static int readInt(const string& prompt) {
    while (true) {
        string s = readString(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        cout << "Please enter a valid number.\n";
    }
}

static void showMenu() {
    cout << "\n==========================================================\n";
    cout << "                LIBRARY MANAGEMENT SYSTEM\n";
    cout << "==========================================================\n";
    cout << "   1. Add Book\n";
    cout << "   2. Register Member\n";
    cout << "   3. Issue Book\n";
    cout << "   4. Return Book\n";
    cout << "   5. Reserve Book\n";
    cout << "   6. Search Book by ID\n";
    cout << "   7. Search Book by Title\n";
    cout << "   8. View All Books (A-Z)\n";
    cout << "   9. View Books by Author\n";
    cout << "  10. Popular Books\n";
    cout << "  11. Undo Last Transaction\n";
    cout << "  12. View Member's Issued Books\n";
    cout << "  13. View Reservations of a Book\n";
    cout << "  14. View All Members\n";
    cout << "   0. Exit\n";
    cout << "----------------------------------------------------------\n";
}

int main() {
    Library lib;
    lib.loadSample();
    cout << "Library ready: 8 books and 3 members loaded.\n";

    while (true) {
        showMenu();
        int choice = readInt("Enter your choice: ");
        cout << "\n";
        switch (choice) {
            case 1: {
                string t = readString("Book title : ");
                string a = readString("Author     : ");
                string c = readString("Category   : ");
                int n = readInt("Copies     : ");
                cout << "\n";
                if (t.empty() || a.empty() || n < 1) cout << "Invalid book details.\n";
                else lib.addBook(t, a, c, n);
                break;
            }
            case 2: {
                string prn = readString("Member PRN  : ");
                string n = readString("Member name : ");
                string d = readString("Department  : ");
                cout << "\n";
                if (prn.empty() || n.empty()) cout << "PRN and name cannot be empty.\n";
                else lib.registerMember(prn, n, d);
                break;
            }
            case 3: { int b = readInt("Book ID    : "); string p = readString("Member PRN : "); cout << "\n"; lib.issueBook(b, p); break; }
            case 4: { int b = readInt("Book ID    : "); string p = readString("Member PRN : "); cout << "\n"; lib.returnBook(b, p); break; }
            case 5: { int b = readInt("Book ID    : "); string p = readString("Member PRN : "); cout << "\n"; lib.reserveBook(b, p); break; }
            case 6: { int b = readInt("Book ID : "); cout << "\n"; lib.searchById(b); break; }
            case 7: { string t = readString("Book title (full or partial): "); cout << "\n"; lib.searchByTitle(t); break; }
            case 8: lib.showAllBooks(); break;
            case 9: lib.showBooksByAuthor(); break;
            case 10: lib.popularBooks(5); break;
            case 11: lib.undoLast(); break;
            case 12: { string p = readString("Member PRN : "); cout << "\n"; lib.showMemberBooks(p); break; }
            case 13: { int b = readInt("Book ID : "); cout << "\n"; lib.showReservations(b); break; }
            case 14: lib.showAllMembers(); break;
            case 0: cout << "Thank you for using the Library Management System.\n"; return 0;
            default: cout << "Invalid choice. Please enter a number from 0 to 14.\n";
        }
    }
}
