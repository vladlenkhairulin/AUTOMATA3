#include "Robot.h"
#include <fstream>
#include <iostream>
#include <limits>

Direction Robot::turnLeft(Direction direction) {
    if (direction == Direction::NORTH) return Direction::WEST;
    else if (direction == Direction::EAST) return Direction::NORTH;
    else if (direction == Direction::SOUTH) return Direction::EAST;
    return Direction::SOUTH;
}
Direction Robot::turnRight(Direction direction) {
    if (direction == Direction::NORTH) return Direction::EAST;
    else if (direction == Direction::WEST) return Direction::NORTH;
    else if (direction == Direction::SOUTH) return Direction::WEST;
    return Direction::SOUTH;
}

Direction Robot::turnAround(Direction direction) {
    return turnRight(turnRight(direction));
}

Direction Robot::getRelativeDir(const std::string &cmd) {
    if (cmd == "FORW" || cmd == "GETF" || cmd == "PUSHF") return dir;
    if (cmd == "BACK" || cmd == "GETB" || cmd == "PUSHB") return turnAround(dir);
    if (cmd == "LEFT" || cmd == "GETL" || cmd == "PUSHL") return turnLeft(dir);
    return turnRight(dir);
}

int Robot::dx(Direction direction) {
    if (direction == Direction::EAST) return 1;
    else if (direction == Direction::WEST) return -1;
    return 0;
}

int Robot::dy(Direction direction) {
    if (direction == Direction::NORTH) return -1;
    else if (direction == Direction::SOUTH) return 1;
    return 0;
}

bool Robot::inside(int x, int y) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

bool Robot::vacant(int x, int y) {
    if (!inside(x,y)) return false;
    if (walls[y][x]) return false;
    return true;
}

bool Robot::loadGameInfo(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) return false;
    std::string word;
    while (in >> word) {
        if (word == "SIZE") {
            in >> width >> height;
            walls.resize(height);
            for (int y = 0; y<height; y++) walls[y].resize(width, false);
        }
        else if (word == "WALL") {
            int x, y;
            in >> x >> y;
            if (inside(x, y)) walls[y][x] = true;
        }
        else if (word == "ROBOT") {
            char direction;
            in >> roboX >> roboY >> direction;
            if (direction == 'N') dir = Direction::NORTH;
            else if (direction == 'E') dir = Direction::EAST;
            else if (direction == 'S') dir = Direction::SOUTH;
            else if (direction == 'W') dir = Direction::WEST;
        }
        else if (word == "EXIT") {
            in >> exitX >> exitY;
        }
    }

    if (!inside(roboX, roboY)) return false;
    if (!inside(exitX, exitY)) return false;
    if (walls[roboY][roboX]) {
        std::cout << "Robot is placed inside wall" << std::endl;
        return false;
    }
    if (walls[exitY][exitX]) {
        std::cout << "Exit is placed inside wall" << std::endl;
        return false;
    }
    if (roboX == exitX && roboY == exitY) {
        std::cout << "Robot and exit have same coordinates" << std::endl;
        return false;
    }
    return true;
}

bool Robot::move(const std::string& cmd) {
    Direction newDir = getRelativeDir(cmd);
    int newX = roboX + dx(newDir);
    int newY = roboY + dy(newDir);
    if (!vacant(newX, newY)) {
        return false;
    }
    dir = newDir;
    roboX = newX;
    roboY = newY;
    render();
    return true;
}

bool Robot::pushWall(const std::string& cmd) {
    Direction pushDir = getRelativeDir(cmd);
    int wallX = roboX + dx(pushDir);
    int wallY = roboY + dy(pushDir);
    int newX = wallX + dx(pushDir);
    int newY = wallY + dy(pushDir);
    if (!inside(newX, newY) || !inside(wallX, wallY)){
        return false;
    }
    if (!walls[wallY][wallX]) {
        return false;
    }
    if (!vacant(newX, newY)) {
        return false;
    }
    walls[wallY][wallX] = false;
    walls[newY][newX] = true;
    lastPush.oldX = wallX;
    lastPush.oldY = wallY;
    lastPush.newX = newX;
    lastPush.newY = newY;
    lastPush.valid = true;
    render();
    return true;
}

bool Robot::undo() {
    if (!lastPush.valid) {
        return false;
    }
    if (roboX == lastPush.oldX && roboY == lastPush.oldY) {
        return false;
    }
    walls[lastPush.newY][lastPush.newX] = false;
    walls[lastPush.oldY][lastPush.oldX] = true;
    lastPush.valid = false;
    render();
    return true;
}

unsigned int Robot::getDist(const std::string &cmd) {
    Direction direction = getRelativeDir(cmd);
    int x = roboX;
    int y = roboY;
    unsigned int dist = 0;
    while (true) {
        if (x == exitX && y == exitY) return dist;
        int newX = x + dx(direction);
        int newY = y + dy(direction);
        if (!inside(newX, newY)) break;
        if (walls[newY][newX]) break;
        x = newX;
        y = newY;
        dist++;
    }
    return std::numeric_limits<unsigned int>::max();
}

void Robot::render() {
    std::cout << std::endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == roboX && y == roboY) std::cout << "R";
            else if (x == exitX && y == exitY) std::cout << "E";
            else if (walls[y][x]) std::cout << "#";
            else std::cout << ".";
        }
        std::cout << std::endl;
    }
}

