#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

class Extra_Board {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{ {
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    } };
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;

    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir : directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({ p });
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s : discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    Extra_Board() {};
    Extra_Board(std::array<std::array<int, SIZE>, SIZE> new_board, int now_player) {
        this->board = new_board;
        this->cur_player = now_player;
        this->done = false;
        this->winner = -1;
        for (int i = 0; i < 3; i++) {
            disc_count[i] = 0;
        }
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                disc_count [board[i][j]]++;
            }
        }
    }
    ~Extra_Board() {};
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if (!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
};

int player;
const int SIZE = 8;
Point decision;
std::array<std::array<int, SIZE>, SIZE> game_board;
std::vector<Point> next_valid_spots;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> game_board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

// Prepare state value calculating.
const std::array<Point, 4> corner = { {
    Point(0, 0), Point(7, 7), Point(0, 7), Point(7, 0)
} };
const std::array<Point, 4> star = { {
    Point(1, 1), Point(6, 6), Point(1, 6), Point(6, 1)
} };
const std::array<Point, 9> directions{ {
       Point(-1, -1), Point(-1, 0), Point(-1, 1),
       Point(0, -1), Point(0, 0), Point(0, 1),
       Point(1, -1), Point(1, 0), Point(1, 1)
} };

int state_value(Extra_Board state) {
    if (state.disc_count[state.EMPTY] < 12) {
        return state.disc_count[player] - state.disc_count[state.get_next_player(player)];
    }
    else {
        int val = 0;
        std::array<std::array<int, SIZE>, SIZE> board = state.board;
        // Corner 1*1
        for (Point center : corner) {
            if (board[center.y][center.x] == player) {
                val += 16;
            }
            else if (board[center.y][center.x]) {
                val -= 16;
            }
            else {
                val -= 8;
            }
        }
        // Star places.
        for (Point center : star) {
            if (board[center.y][center.x] == player) {
                val -= 4;
            }
            else if (board[center.y][center.x]) {
                val += 4;
            }
            else {
                val += 2;
            }
        }
        return val;
    }
}

// Fixed from Wikipedia.
int tree_search(std::array<std::array<int, SIZE>, SIZE> board, Point expected_point, int cur_player, int depth, int alpha, int beta, bool maxi) {
    // Update board and get possible moves.
    Extra_Board new_state(board, cur_player);
    new_state.put_disc(expected_point);
    int next_player = new_state.cur_player;
    std::array<std::array<int, SIZE>, SIZE> new_board = new_state.board;
    std::vector<Point> new_valid_spots = new_state.next_valid_spots;
    if (!depth || new_state.done) {
        return state_value(new_state);
    }
    else {
        int val;
        // minimax.
        if (maxi) {
            val = -1000;
            for (auto& new_expected_point: new_valid_spots) {
                val = std::max(val, tree_search(new_board, new_expected_point, next_player, depth - 1, alpha, beta, false));
                alpha = std::max(alpha, val);
                if (beta <= alpha) {
                    break;     // beta pruning.
                }
            }
        }
        else {
            val = 1000;
            for (auto& new_expected_point: new_valid_spots) {
                val = std::min(val, tree_search(new_board, new_expected_point, next_player, depth - 1, alpha, beta, true));
                beta = std::min(beta, val);
                if (beta <= alpha) {
                    break;   // alpha pruning.
                }
            }
        }
        return val;
    }
}

void write_valid_spot(std::ofstream& fout) {
    Extra_Board cur_state(game_board, player);
    int val;
    int max = -1023;
    int depth = 4;
    for (Point p : next_valid_spots) {
        val = tree_search(cur_state.board, p, cur_state.cur_player, depth, -1000, 1000, false);
        if (val >= max) {
            max = val;
            decision = p;
        }
    }
    // Remember to flush the output to ensure the last action is written to file.
    fout << decision.x << " " << decision.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
