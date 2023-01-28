#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>

using namespace std;


// FUNCTIONS DEFINITION
bool isValid(int MAX_L, int MAX_R, int MAX_C,
             std::vector<std::vector<std::vector<char>>> table,
             std::vector<std::vector<std::vector<bool>>> visited,
             int l, int r, int c);

bool dfsSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
               std::vector<std::vector<std::vector<char>>> table,
               std::vector<std::vector<std::vector<bool>>> visited,
               int startL, int startR, int startC);

bool bfsSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
               std::vector<std::vector<std::vector<char>>> table,
               std::vector<std::vector<std::vector<bool>>> visited,
               int startL, int startR, int startC);

bool dqSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
              std::vector<std::vector<std::vector<char>>> table,
              std::vector<std::vector<std::vector<bool>>> visited,
              int startL, int startR, int startC);

// Data Structure
struct Position {
    int l, r, c;
};

int main(int argc, char *argv[]) {
//    ios_base::sync_with_stdio(false);

    std::string line;
    std::string type;
    std::string message;
    std::string levelTitle;
    int maxL = 0;
    int maxR = 0;
    int maxC = 0;
    int startL = 0;
    int startR = 0;
    int startC = 0;
    int r = 0;
    int c = 0;
    int l = 0;
    int opt;
    int long_index = 0;
    bool sFlag = false;
    bool qFlag = false;
    bool dFlag = false;
    bool mFlag = false;
    bool lFlag = false;
    stack<pair<int, pair<int, int>>> stack;
    queue<pair<int, pair<int, int>>> queue;
    deque<pair<int, pair<int, int>>> deque;

    struct option long_options[] = {
            {"stack",  no_argument,       0, 's'},
            {"queue",  no_argument,       0, 'q'},
            {"deque",  no_argument,       0, 'd'},
            {"output", required_argument, 0, 'o'},
            {"help",   no_argument,       0, 'h'},
            {0,        0,                 0, 0}
    };


    while ((opt = getopt_long(argc, argv, "sqo:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 's':
                // --stack or -s option
                sFlag = true;
                break;
            case 'q':
                // --queue or -q option
                qFlag = true;
                break;
            case 'd':
                // --deque or -d option
                dFlag = true;
                break;
            case 'o':
                // --output option
                if (strcmp(optarg, "M") == 0) {
                    mFlag = true;
                } else if (strcmp(optarg, "L") == 0) {
                    lFlag = true;
                } else {
                    std::cerr << "ERROR! Invalid argument for output option detected for the output.";
                }
                break;
            case 'h':
                // Help
                std::cout << "Ship application takes the following (optional) command line options:\n"
                             "\n"
                             "--stack, -s\n"
                             "The search container will be used like a stack and the routing scheme will perform a depth first search (DFS).\n"
                             "The stack option may be invoked by calling the program with either --stack or -s, it accepts no arguments.\n"
                             "\n"
                             "--queue, -q\n"
                             "The search container will be used like a queue and the routing scheme will perform a breadth first search (BFS). \n"
                             "The queue option may be invoked by calling the program with either --queue or -q, it accepts no arguments.\n"
                             "\n"
                             "--output (M|L), -o (M|L)\n"
                             "Indicates the output file format by the required argument, M (map format) or L (coordinate list format). \n"
                             "If this option is not provided when running the program, the default output format is map format. If specified, \n"
                             "this option requires that an argument be provided, and the autograder will only use valid arguments.\n"
                             "\n"
                             "--help, -h\n"
                             "This causes the program to print a helpful message about how to use the program.\n"
                             "***************************************************\n"
                             "\n"
                             "Legal Command Lines: \n"
                             "\n"
                             "$ ./ship --stack < infile > outfile\n"
                             "This will run the program using a stack and map output mode.\n"
                             "\n"
                             "$ ./ship --queue --output M < infile > outfile\n"
                             "This will run the program using a queue and map output mode.\n"
                             "\n"
                             "$ ./ship --stack --output L < infile > outfile\n"
                             "Output option requires an argument describing the output mode.";
                exit(0);
            default:
                // Default error
                std::cerr << "ERROR! Invalid options detected.";
                exit(0);
        }
    }

    // argument check
    if (!sFlag && !qFlag && !dFlag) {
        std::cerr << "ERROR! Invalid option. At least one of the Stack, "
                     "Queue or Dequeue should be entered as an option to the application.";
        return 1;
    }
    if (sFlag + qFlag + dFlag > 1) {
        std::cerr << "ERROR! Invalid option. You are allowed to enter only one data structure type.";
        return 1;
    }

    if (!lFlag && !mFlag) {
        std::cerr << "ERROR! Invalid option. Invalid output mode.";
        return 1;
    }
    if (lFlag + mFlag > 1) {
        std::cerr << "ERROR! Invalid option. You are allowed to enter at least one output mode.";
        return 1;
    }

    // init sizes
    std::cin >> type;
    std::cin >> maxL;
    std::cin >> maxR;
    maxC = maxR; // square table
    for (int i = 0; i < 2; ++i) {
        std::getline(std::cin, message);
    }

    // create a vector of entered sizes
    std::vector<std::vector<std::vector<char>>> table(maxL);
    for (r = 0; r < maxL; r++) {
        table[r].resize(maxR);
        for (int j = 0; j < maxR; j++) {
            table[r][j].resize(maxR);
        }
    }
    std::vector<std::vector<std::vector<bool>>> visited(maxL);
    for (r = 0; r < maxL; r++) {
        visited[r].resize(maxR);
        for (int j = 0; j < maxR; j++) {
            visited[r][j].resize(maxR);
        }
    }

    // init table
    r = 0;
    c = 0;
    l = -1;
    while (std::getline(std::cin, line)) {
        if (line.find("//") != 0) {
            for (c = 0; c < maxR; c++) {
                table[l][r][c] = line[c];
                if (line[c] == 'S') {
                    startL = l;
                    startR = r;
                    startC = c;
                }
            }
            r++;
        } else {
            r = 0;
            l++;
        }
    }

    if (mFlag) {
        cout << "Start in level " << startL << ", row " << startR << ", column " << startC << "\n";
    } else if (lFlag) {
        cout << "//path taken\n";
    }

    // run the search algorithm
    if (sFlag) {
        dfsSearch(maxL, maxR, maxC, mFlag, lFlag, table, visited, startL, startR, startC);
    } else if (qFlag) {
        bfsSearch(maxL, maxR, maxC, mFlag, lFlag, table, visited, startL, startR, startC);
    } else if (dFlag) {
        dqSearch(maxL, maxR, maxC, mFlag, lFlag, table, visited, startL, startR, startC);
    }

    return 0;
}

// Checking for the valid movements
bool isValid(int MAX_L, int MAX_R, int MAX_C,
             std::vector<std::vector<std::vector<char>>> table,
             std::vector<std::vector<std::vector<bool>>> visited,
             int l, int r, int c) {
    if (l < 0 || l >= MAX_L) return false;
    if (r < 0 || r >= MAX_R) return false;
    if (c < 0 || c >= MAX_C) return false;
    if (table[l][r][c] == '#') return false;
    if (visited[l][r][c]) return false;
    return true;
}

// Using Stack
bool dfsSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
               std::vector<std::vector<std::vector<char>>> table,
               std::vector<std::vector<std::vector<bool>>> visited,
               int startL, int startR, int startC) {
    stack<Position> stack;
    stack.push({startL, startR, startC});
    visited[startL][startR][startC] = true;

    char method;
    bool elevator = false;

    while (!stack.empty()) {
        Position p = stack.top();
        stack.pop();

        if (table[p.l][p.r][p.c] == 'H') {
            if (mFlag) {
                for (int ll = 0;ll < MAX_L; ++ll) {
                    cout << "//level " << ll << "\n";
                    for (int rr = 0; rr < MAX_R; ++rr) {
                        for (int cc = 0; cc < MAX_C; ++cc) {
                            cout << table[ll][rr][cc];
                        }
                        cout << "\n";
                    }
                }
            }
            return true;
        }

        // Check neighbor cells
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r - 1, p.c)) {
            stack.push({p.l, p.r - 1, p.c});
            method = 'n';
            visited[p.l][p.r - 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c + 1)) {
            stack.push({p.l, p.r, p.c + 1});
            method = 'e';
            visited[p.l][p.r][p.c + 1] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r + 1, p.c)) {
            stack.push({p.l, p.r + 1, p.c});
            method = 's';
            visited[p.l][p.r + 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c - 1)) {
            stack.push({p.l, p.r, p.c - 1});
            method = 'w';
            visited[p.l][p.r][p.c - 1] = true;
        }
        if (table[p.l][p.r][p.c] == 'E' && isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l + 1, p.r, p.c)) {
            stack.push({p.l + 1, p.r, p.c});
            visited[p.l + 1][p.r][p.c] = true;
            if (table[p.l][p.r][p.c] == 'E') {
                elevator = true;
            }
        }

        table[p.l][p.r][p.c] = method;
        if (!elevator) {
            if(lFlag) {
                cout << "(" << p.l << "," << p.r << "," << p.c << ", " << method << ")\n";
            }
        }
        elevator = false;
    }

    cout << "RESULT! Goal not found.";
    return false;
}

// Using Queue
bool bfsSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
               std::vector<std::vector<std::vector<char>>> table,
               std::vector<std::vector<std::vector<bool>>> visited,
               int startL, int startR, int startC) {
    queue<Position> queue;
    queue.push({startL, startR, startC});
    visited[startL][startR][startC] = true;

    char method;
    bool elevator = false;

    while (!queue.empty()) {
        Position p = queue.front();
        queue.pop();
        for (int i = 0; i < queue.size(); ++i) {
            queue.pop(); // cleans queue at each move
        }

        if (table[p.l][p.r][p.c] == 'H') {
            if (mFlag) {
                for (int ll = 0;ll < MAX_L; ++ll) {
                    cout << "//level " << ll << "\n";
                    for (int rr = 0; rr < MAX_R; ++rr) {
                        for (int cc = 0; cc < MAX_C; ++cc) {
                            cout << table[ll][rr][cc];
                        }
                        cout << "\n";
                    }
                }
            }
            return true;
        }

        // Check neighbor cells
        bool met = false;
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r - 1, p.c)) {
            queue.push({p.l, p.r - 1, p.c});
            if(!met) {
                method = 'n';
                met = true;
            }
            visited[p.l][p.r - 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c + 1)) {
            queue.push({p.l, p.r, p.c + 1});
            if(!met) {
                method = 'e';
                met = true;
            }
            visited[p.l][p.r][p.c + 1] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r + 1, p.c)) {
            queue.push({p.l, p.r + 1, p.c});
            if(!met) {
                method = 's';
                met = true;
            }
            visited[p.l][p.r + 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c - 1)) {
            queue.push({p.l, p.r, p.c - 1});
            if(!met) {
                method = 'w';
                met = true;
            }
            visited[p.l][p.r][p.c - 1] = true;
        }
        if (table[p.l][p.r][p.c] == 'E' && isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l + 1, p.r, p.c)) {
            queue.push({p.l + 1, p.r, p.c});
            visited[p.l + 1][p.r][p.c] = true;
            elevator = true;
        }

        table[p.l][p.r][p.c] = method;
        if (!elevator) {
            if(lFlag) {
                cout << "(" << p.l << "," << p.r << "," << p.c << ", " << method << ")\n";
            }
        }
        elevator = false;
    }

    cout << "RESULT! Goal not found.";
    return false;
}


// Using Deque
bool dqSearch(int MAX_L, int MAX_R, int MAX_C, bool mFlag, bool lFlag,
              std::vector<std::vector<std::vector<char>>> table,
              std::vector<std::vector<std::vector<bool>>> visited,
              int startL, int startR, int startC) {
    deque<Position> deque;
    deque.push_back({startL, startR, startC});
    visited[startL][startR][startC] = true;

    char method;
    bool elevator = false;

    while (!deque.empty()) {
        Position p = deque.front();
//        deque.pop_back();
        deque.pop_front(); // You can either pop from back or front
        for (int i = 0; i < deque.size(); ++i) {
            deque.pop_back();
        }

        if (table[p.l][p.r][p.c] == 'H') {
            if (mFlag) {
                for (int ll = 0; ll < MAX_L; ++ll) {
                    cout << "//level " << ll << "\n";
                    for (int rr = 0; rr < MAX_R; ++rr) {
                        for (int cc = 0; cc < MAX_C; ++cc) {
                            cout << table[ll][rr][cc];
                        }
                        cout << "\n";
                    }
                }
            }
            return true;
        }

        // Check neighbor cells
        bool met = false;
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r - 1, p.c)) {
            deque.push_back({p.l, p.r - 1, p.c});
            if (!met) {
                method = 'n';
                met = true;
            }
            visited[p.l][p.r - 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c + 1)) {
            deque.push_back({p.l, p.r, p.c + 1});
            if (!met) {
                method = 'e';
                met = true;
            }
            visited[p.l][p.r][p.c + 1] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r + 1, p.c)) {
            deque.push_back({p.l, p.r + 1, p.c});
            if (!met) {
                method = 's';
                met = true;
            }
            visited[p.l][p.r + 1][p.c] = true;
        }
        if (isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l, p.r, p.c - 1)) {
            deque.push_back({p.l, p.r, p.c - 1});
            if (!met) {
                method = 'w';
                met = true;
            }
            visited[p.l][p.r][p.c - 1] = true;
        }
        if (table[p.l][p.r][p.c] == 'E' && isValid(MAX_L, MAX_R, MAX_C, table, visited, p.l + 1, p.r, p.c)) {
            deque.push_back({p.l + 1, p.r, p.c});
            visited[p.l + 1][p.r][p.c] = true;
            elevator = true;
        }

        table[p.l][p.r][p.c] = method;
        if (!elevator) {
            if (lFlag) {
                cout << "(" << p.l << "," << p.r << "," << p.c << ", " << method << ")\n";
            }
        }
        elevator = false;
    }
    cout << "RESULT! Goal not found.";
    return false;

}