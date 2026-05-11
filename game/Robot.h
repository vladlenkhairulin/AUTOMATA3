#ifndef AUTOMATA3_ROBOT_H
#define AUTOMATA3_ROBOT_H
#include <string>
#include <vector>

enum class Direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

struct PushInfo {
    int oldX = 0;
    int oldY = 0;
    int newX = 0;
    int newY = 0;
    bool valid = false;
};


class Robot {
private:
    int width = 0;
    int height = 0;
    int roboX = 0;
    int roboY = 0;
    int exitX = 0;
    int exitY = 0;
    Direction dir = Direction::EAST;
    std::vector<std::vector<bool>> walls;
    PushInfo lastPush;
    Direction turnLeft(Direction d);
    Direction turnRight(Direction d);
    Direction turnAround(Direction d);
    Direction getRelativeDir(const std::string& cmd);
    int dx(Direction dir);
    int dy(Direction dir);
    bool inside(int x, int y);
    bool vacant(int x, int y);
public:
    bool loadGameInfo(const std::string& filename);
    bool move(const std::string& cmd);
    bool pushWall(const std::string& cmd);
    bool undo();
    unsigned int getDist(const std::string& cmd);
    void render();
};


#endif //AUTOMATA3 _ROBOT_H