#include <GLFW/glfw3.h>
#include <iostream>
#include <deque>
#include <random>

// Window dimensions
const int WIDTH = 800;
const int HEIGHT = 600;

// Snake properties
const int GRID_SIZE = 20;
const int INITIAL_SNAKE_LENGTH = 3;
const float SNAKE_SPEED = 0.1f;

// Directions
enum class Direction { UP, DOWN, LEFT, RIGHT };
Direction currentDirection = Direction::RIGHT;

// Snake structure
struct SnakeSegment {
    int x, y;
};

std::deque<SnakeSegment> snake;

// Food coordinates
int foodX, foodY;

// Random number generator
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<int> randomGridPos(0, GRID_SIZE - 1);

// Function prototypes
void spawnFood();
void updateSnake();
bool checkCollision();
bool checkFoodCollision();
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void drawSquare(float x, float y, float size);

// Initialize GLFW and OpenGL
bool initOpenGL() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake Game", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    return true;
}

// Spawn initial snake and food
void initGame() {
    snake.clear();
    snake.push_front({ GRID_SIZE / 2, GRID_SIZE / 2 });

    for (int i = 1; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_back({ GRID_SIZE / 2 - i, GRID_SIZE / 2 });
    }

    spawnFood();
}

// Main game loop
void gameLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw snake
        glColor3f(0.0f, 1.0f, 0.0f); // Snake color
        for (const auto& segment : snake) {
            drawSquare(segment.x * GRID_SIZE, segment.y * GRID_SIZE, GRID_SIZE);
        }

        // Draw food
        glColor3f(1.0f, 0.0f, 0.0f); // Food color
        drawSquare(foodX * GRID_SIZE, foodY * GRID_SIZE, GRID_SIZE);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update snake movement
        static float delta = 0.0f;
        static float interval = SNAKE_SPEED;
        delta += interval;

        if (delta >= 1.0f) {
            delta -= 1.0f;
            updateSnake();

            if (checkCollision()) {
                std::cout << "Game Over!" << std::endl;
                initGame();
            }

            if (checkFoodCollision()) {
                snake.push_back({ foodX, foodY });
                spawnFood();
            }
        }
    }
}

// Update snake position based on current direction
void updateSnake() {
    SnakeSegment newHead = snake.front();

    switch (currentDirection) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
    }

    snake.push_front(newHead);
    snake.pop_back(); // Remove the tail segment
}

// Check if snake collided with itself or the window edges
bool checkCollision() {
    int headX = snake.front().x;
    int headY = snake.front().y;

    // Check window edges
    if (headX < 0 || headX >= GRID_SIZE || headY < 0 || headY >= GRID_SIZE) {
        return true;
    }

    // Check self-collision
    for (auto it = snake.begin() + 1; it != snake.end(); ++it) {
        if (headX == it->x && headY == it->y) {
            return true;
        }
    }

    return false;
}

// Check if snake ate the food
bool checkFoodCollision() {
    return snake.front().x == foodX && snake.front().y == foodY;
}

// Spawn food at a random position
void spawnFood() {
    do {
        foodX = randomGridPos(rng);
        foodY = randomGridPos(rng);
    } while (std::find_if(snake.begin(), snake.end(), [&](const SnakeSegment& segment) {
        return segment.x == foodX && segment.y == foodY;
    }) != snake.end());
}

// Keyboard input callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_UP:
                if (currentDirection != Direction::DOWN)
                    currentDirection = Direction::UP;
                break;
            case GLFW_KEY_DOWN:
                if (currentDirection != Direction::UP)
                    currentDirection = Direction::DOWN;
                break;
            case GLFW_KEY_LEFT:
                if (currentDirection != Direction::RIGHT)
                    currentDirection = Direction::LEFT;
                break;
            case GLFW_KEY_RIGHT:
                if (currentDirection != Direction::LEFT)
                    currentDirection = Direction::RIGHT;
                break;
            default:
                break;
        }
    }
}

// Draw a square at (x, y) with given size
void drawSquare(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}

int main() {
    if (!initOpenGL()) {
        return -1;
    }

    initGame();

    GLFWwindow* window = glfwGetCurrentContext();
    gameLoop(window);

    glfwTerminate();
    return 0;
}
