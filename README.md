# 📚 Library Management System in C++

A console-based **Library Management System** developed in C++ using Data Structures and Algorithms (DSA). This project simulates common library operations, including book management, member registration, book issuing and returning, reservations, and searching.

The project implements data structures from scratch to demonstrate their practical applications in a real-world system.

## ✨ Features

- **Add Books** — Add books with a title, author, category, and number of copies.
- **Member Registration** — Register library members using a unique PRN.
- **Issue Books** — Issue available books to registered members with issue and due dates.
- **Return Books** — Return borrowed books and calculate overdue fines.
- **Book Reservations** — Maintain a first-in, first-out (FIFO) reservation queue.
- **Search Books** — Search by book ID or full/partial title.
- **View All Books** — Display books in alphabetical order by title.
- **Search by Author** — Sort books by author and search by author-name prefix.
- **Popular Books** — Display the most frequently issued books.
- **Undo Transactions** — Undo the most recent book issue or return.
- **Member Book Details** — View books currently issued to a member.
- **Reservation Details** — View the waiting list for a particular book.
- **View All Members** — Display registered members sorted by PRN.

## 🧠 Data Structures and Algorithms Used

| Data Structure / Algorithm | Application | Complexity |
|---|---|---|
| Hash Table (Separate Chaining) | Search books by ID and members by PRN | Average O(1) |
| Binary Search Tree (BST) | Search books by title and display titles alphabetically | Average O(log n) search |
| Singly Linked List | Track books issued to each member | O(k) traversal |
| Queue (FIFO) | Manage book reservation lists | O(1) enqueue/dequeue |
| Stack (LIFO) | Undo the latest issue or return transaction | O(1) push/pop |
| Max-Heap | Identify the most frequently issued books | O(log n) extraction |
| Merge Sort | Sort books by author and members by PRN | O(n log n) |
| Binary Search | Search author names in a sorted list | O(log n) |

*Note: Actual BST performance depends on tree balance. The implementation does not use a self-balancing BST.*

## ⚙️ Requirements

- A C++ compiler that supports C++11 or later.
- Git (optional, for version control).
- Visual Studio Code (recommended for development).

## 🚀 Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/jay-cpp/LIBRARY-MANAGEMENT-SYSTEM.git
cd LIBRARY-MANAGEMENT-SYSTEM
```

### 2. Compile the Program

Make sure `g++` is installed and available in your terminal.

```bash
g++ -std=c++11 library_dsa.cpp -o library
```

### 3. Run the Program

**Windows PowerShell:**

```powershell
.\library.exe
```

**Linux / macOS:**

```bash
./library
```

## 📋 Main Menu

When the program starts, it loads sample data consisting of 8 books and 3 members.

| Option | Operation |
|---|---|
| 1 | Add Book |
| 2 | Register Member |
| 3 | Issue Book |
| 4 | Return Book |
| 5 | Reserve Book |
| 6 | Search Book by ID |
| 7 | Search Book by Title |
| 8 | View All Books (A–Z) |
| 9 | View Books by Author |
| 10 | Popular Books |
| 11 | Undo Last Transaction |
| 12 | View a Member's Issued Books |
| 13 | View Reservations of a Book |
| 14 | View All Members |
| 0 | Exit |

## 📏 Library Rules

- A member can borrow a maximum of **3 books** at a time.
- The loan period is **14 days**.
- The overdue fine is **₹2 per late day**, according to the program's date calculation.
- Book reservations are served in FIFO order.
- A member cannot borrow the same book twice simultaneously.

## 🗂️ Project Structure

```text
LIBRARY-MANAGEMENT-SYSTEM/
├── library_dsa.cpp    # Main C++ source code
└── README.md          # Project documentation
```

## ⚠️ Notes

- The program loads sample books and members every time it starts.
- The current implementation stores data in memory; it does not persist changes to a database or file between runs.
- The undo feature applies to the most recent book issue or return transaction.
- The project is designed as a console application for learning and demonstrating DSA concepts.

## 🎯 Learning Objectives

- Understand how data structures support practical applications.
- Implement searching and sorting algorithms in C++.
- Manage book loans and reservations using linked lists, queues, and stacks.
- Apply hashing and tree-based indexing to improve lookup efficiency.
- Practice object-oriented programming and dynamic memory management.

## 👨‍💻 Author

**Jay Jethva**

---

If you find this project useful for learning Data Structures and Algorithms in C++, feel free to explore and improve it.
# LIBRARY-MANAGEMENT-SYSTEM
