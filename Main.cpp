// Created by Metehan Pamuklu
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime> 
#include <chrono>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());//from stackoverflow
struct Cell {//to hold attirbutes of cells walls and visited info
public:
    bool visited;
    bool walls[4]; // 0 up,  1 right, 2 down, 3 left
    int x, y;
    Cell() : visited(false), x(0), y(0) {
        // initialize all walls to true initially
        for (int i = 0; i < 4; ++i) {
            walls[i] = true;
        }
    }
    Cell(int x1, int y1) : visited(false), x(x1), y(y1) {
        // initialize all walls to true initially
        for (int i = 0; i < 4; ++i) {
            walls[i] = true;
        }
    }
};
//creating stack class in order to create maze with template
template<typename M>
class Stack {
private:
    struct Node {
        M data;
        Node* next;
        Node(const M& coordinates) : data(coordinates), next(nullptr) {}
    };
    Node* Top;
public:
    Stack() : Top(nullptr) {}
    ~Stack() {
        while (!isEmpty()) {
            pop();//dealocato memory
        }
    }
    void push(const M& coordinates) {
        Node* nextnode = new Node(coordinates);
        nextnode->next = Top;//linked list structed
        Top = nextnode;
    }
    void pop() {
        Node* temp = Top;
        Top = Top->next;//linked list structed
        delete temp;
    }
    bool isEmpty() const {
        return Top == nullptr;
    }
    M& top() {
        return Top->data;//to get info of tops coordinates while creating maze or finding path
    }
};
vector<vector<vector<Cell>>> allMazes;//global vector to hold every maze cell info for finding a path
// creating a class and function in it to create maze
class MazeCreator {
private:
    int rows;
    int cols;
    vector<vector<Cell>> maze;//hold current maze info of cells
    Stack<pair<int, int>> maze_stack;//for algorithm to keep coordinates
public:
    MazeCreator(int rows, int cols) : rows(rows), cols(cols) {
        maze.resize(rows);
        for (int i = 0; i < rows; ++i) {
            maze[i].resize(cols);
            for (int j = 0; j < cols; ++j) {
                maze[i][j] = Cell(i, j); //creating all coordinates for maze vector
            }
        }
    }
    //function of creating maze
    void createMaze() {
        maze_stack.push(make_pair(0, 0));//first element of stack initalizing to 0,0
        maze[0][0].visited = true;//set it as a visited
        int totalcells = rows * cols;
        int visited_total_cells = 1;
        while (totalcells > visited_total_cells) {//visiting all the cells
            vector<int> neigbour_cells;//keeping info of how many possible neigbour
            int x = maze_stack.top().first;//using coordinates from stack
            int y = maze_stack.top().second;
            maze[x][y].visited = true;
            if (y < cols - 1 && !maze[x][y + 1].visited) {
                neigbour_cells.push_back(0); // Up
            }
            if (x < rows- 1 && !maze[x + 1][y].visited) {
                neigbour_cells.push_back(1); // Right
            }
            if (y > 0 && !maze[x][y - 1].visited) {
                neigbour_cells.push_back(2); // Down
            }
            if (x > 0 && !maze[x - 1][y].visited) {
                neigbour_cells.push_back(3); // Left
            }
            //checkes all possible neigbours
            if (!neigbour_cells.empty()) {
               
                shuffle(neigbour_cells.begin(), neigbour_cells.end(), rng); //shuffling the vector so that the possible directions can be random
                int next_cell = neigbour_cells[0];
                //getting random neigbour cell nnumber from neigbour vector
                switch (next_cell) {
                case 0: // Up
                    maze[x][y].walls[0] = false;
                    maze[x][y + 1].walls[2] = false;
                    maze_stack.push(make_pair(x, y + 1));
                    break;
                case 1: // Right
                    maze[x][y].walls[1] = false;
                    maze[x + 1][y].walls[3] = false;
                    maze_stack.push(make_pair(x + 1, y));
                    break;
                case 2: // Down
                    maze[x][y].walls[2] = false;
                    maze[x][y - 1].walls[0] = false;
                    maze_stack.push(make_pair(x, y - 1));
                    break;
                case 3: // Left
                    maze[x][y].walls[3] = false;
                    maze[x - 1][y].walls[1] = false;
                    maze_stack.push(make_pair(x - 1, y));
                    break;
                }         
                visited_total_cells++;//increasing if cell moved
            }
            else {
                maze_stack.pop();//algorithm works if no possible neighbour
            }
        }
        allMazes.push_back(maze);//oush maze vector info to global mazes vector when it  finished
    }
    void printMazeToFile(const string& filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "Unable to open file: " << filename << endl;
            return;
        }
        // Write number of rows and columns to the file
        outFile << cols << " " << rows << endl;
        for (int j = 0; j < cols; ++j) {
            for (int i = 0; i < rows; ++i) {
                const Cell& cell = maze[i][j];
                outFile << "x=" << cell.x << " y=" << cell.y;

                // Write walls using if statements
                if (cell.walls[3]) // Left
                    outFile << " l=1";
                else
                    outFile << " l=0";

                if (cell.walls[1]) // Right
                    outFile << " r=1";
                else
                    outFile << " r=0";

                if (cell.walls[0]) // Up
                    outFile << " u=1";
                else
                    outFile << " u=0";

                if (cell.walls[2]) // Down
                    outFile << " d=1";
                else
                    outFile << " d=0";

                outFile << endl;
            }
        }
        outFile.close();
    }
};
void pathfinder(int start_x, int start_y, int end_x, int end_y,int maze_wanted,int rows,int cols) {
    vector<vector<Cell>> maze = allMazes[maze_wanted-1]; //getting info of wanted maze from global vector of all mazes
    Stack<pair<int, int>> maze_stack;//same algorithm as creating maze
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            maze[i][j].visited = false;
        }//making all cells unvisited because we made them visited  before
    }
    maze_stack.push(make_pair(start_x,start_y));
    maze[start_x][start_y].visited = true;
    int current_x = maze_stack.top().first;
    int current_y = maze_stack.top().second;
    bool check = true;
    vector<pair<int, int>> path;//to store data of visited cells while finding path
    while (check) {
        //checking possible route using visited info corner info or wall exist
        int current_x = maze_stack.top().first;
        int current_y = maze_stack.top().second;
        if (current_x < rows - 1 && !maze[current_x+1][current_y].visited && !maze[current_x][current_y].walls[1]) { // Right
            maze_stack.push(make_pair(current_x+1, current_y));
            maze[current_x+1][current_y].visited = true;
            path.push_back(make_pair(current_x, current_y));
        }
        else if (current_y > 0 && !maze[current_x][current_y-1].visited && !maze[current_x][current_y].walls[2]) { // Down
            maze_stack.push(make_pair(current_x, current_y-1));
            maze[current_x][current_y-1].visited = true;
            path.push_back(make_pair(current_x, current_y));

        }
        else if (current_x > 0 && !maze[current_x-1][current_y].visited && !maze[current_x][current_y].walls[3]) { // Left
            maze_stack.push(make_pair(current_x-1, current_y));
            maze[current_x-1][current_y].visited = true;
            path.push_back(make_pair(current_x, current_y));

        }
        else if (current_y < cols - 1 && !maze[current_x][current_y+1].visited && !maze[current_x][current_y].walls[0]) { // Up
            maze_stack.push(make_pair(current_x, current_y+1));
            maze[current_x][current_y+1].visited = true;
            path.push_back(make_pair(current_x, current_y));

        }
        else if ((current_x == end_x && current_y == end_y)) {
            check = false;//getting out of while loop if we reached wanted end points
        }
        else {
            path.pop_back();//deleting from vector if cell is blocked
            maze_stack.pop();

        }
    }
    path.push_back(make_pair(end_x, end_y));
    // Write path coordinates to a file
    string filename = "maze_" + to_string(maze_wanted) + "_path_" +
        to_string(start_x) + "_" + to_string(start_y) + "_" +
        to_string(end_x) + "_" + to_string(end_y) + ".txt";
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Unable to open file: " << filename << endl;
        return;
    }
    for (const auto& cell : path) {
        outFile << cell.first << " " << cell.second << endl;
    }
    outFile.close();
};
int main() {
    int rows, cols, numMazes, maze_Index, start_x, start_y, end_x, end_y, maze_wanted;
    cout << "Enter the number of mazes: ";
    cin >> numMazes;
    cout << "Enter the number of rows and columns (M and N): ";
    cin >> rows >> cols;
    for (int i = 0; i < numMazes; ++i) {
        MazeCreator creator(rows, cols);
        creator.createMaze();
        creator.printMazeToFile("maze_" + to_string(i + 1) + ".txt");
    }
    cout << "All mazes are generated.";
    cout << endl<<endl;
    cout << "Enter a maze ID between 1 to"<<" " << numMazes << " " << "inclusive to find a path : ";
    cin >> maze_wanted;
    cout << "Enter x and y coordinates of the entry points (x,y) or (column,row): ";
    cin >> start_x >> start_y;
    cout << "Enter x and y coordinates of the exit points (x,y) or (column,row): ";
    cin >> end_x >> end_y;
    pathfinder(start_x, start_y, end_x, end_y, maze_wanted,rows,cols);
    return 0;
}
