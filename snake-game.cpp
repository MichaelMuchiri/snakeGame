#include <deque>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <GL/glut.h>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 20;
const float SNAKE_SPEED = 0.5f; // Movement speed in seconds
const float INITIAL_SNAKE_LENGTH = 3;
const float TITLE_SCREEN_DURATION = 3.0f; // Duration of the title screen in seconds

bool titleScreenActive = false; //!
bool menuScreenActive = true;
bool gameActive = false;
bool levelsMenuActive = false;
bool scoreboardActive = false;

enum class Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
};

std::deque<SnakeSegment> snake;
Direction currentDirection = Direction::RIGHT;
bool gameOver = false;
int foodX, foodY;
int score = 0;

enum class MenuOption { PLAY, LEVELS, EXIT };
enum class Level { EASY, MEDIUM, HARD };

MenuOption currentMenuOption;
Level currentLevel = Level::MEDIUM; // Default level

void reshape(int width, int height);
void init();
void timer(int value);
void display();
void displayTitleScreen();
void displayMenuScreen();
void displayGameScreen();
void displayLevelsMenu();
void displayScoreboard();
void handleInput(unsigned char key, int x, int y);
void handleMenuInput(unsigned char key, int x, int y);
void handleGameInput(unsigned char key, int x, int y);
void handleLevelsMenuInput(unsigned char key, int x, int y);
void handleScoreboardInput(unsigned char key, int x, int y);
void initializeSnake();
void generateFood();
bool checkCollisionWithFood(int x, int y);
float getSnakeSpeed();
void moveSnake();
void resetTimer();
void resetGame();
void drawSnake();
void drawFood();
void drawGrid();

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Snake Game");

    init();
    initializeSnake();
    generateFood();

    // Register the keyboard callback function
    glutKeyboardFunc(handleInput);

    // Set the initial display function
    glutDisplayFunc(display);

    // glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    // Set up the timer to transition to the menu screen after TITLE_SCREEN_DURATION seconds
    glutTimerFunc(TITLE_SCREEN_DURATION * 1000, timer, 0);
}

void timer(int value) {
    if (titleScreenActive) {
        titleScreenActive = false;
        menuScreenActive = true;
        glutDisplayFunc(displayMenuScreen);
    }
    if (gameActive) {
        glutPostRedisplay();
        glutTimerFunc(1000 / (getSnakeSpeed() * 15), timer, 0); // Update at approximately 15 frames per second
    }
    glutPostRedisplay();
}

void display() {
    if (titleScreenActive) {
        displayTitleScreen();
    } else if (menuScreenActive) {
        displayMenuScreen();
    } else if (levelsMenuActive) {
        displayLevelsMenu();
    } else if (gameActive) {
        displayGameScreen();
    } else if (scoreboardActive) {
        displayScoreboard();
    }
}

void displayTitleScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up the text properties
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    glRasterPos2i(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2);
    std::string title = "Snake";
    for (char c : title) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();
}

void displayMenuScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up the text properties
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    glRasterPos2i(300, 400);
    std::string play = "Play";
    for (char c : play) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 300);
    std::string levels = "Levels";
    for (char c : levels) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 200);
    std::string exit = "Exit";
    for (char c : exit) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();
}

void displayGameScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (!gameOver) {
        moveSnake();
    } else {
        gameActive = false;
        scoreboardActive = true;
        glutPostRedisplay();
    }

    drawGrid(); // Draw grid first
    drawSnake(); // Draw snake on top
    drawFood(); // Draw food

    glutSwapBuffers();
}

void displayLevelsMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Set up the text properties
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    glRasterPos2i(300, 400);
    std::string easy = "1. Easy";
    for (char c : easy) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 300);
    std::string medium = "2. Medium";
    for (char c : medium) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 200);
    std::string hard = "3. Hard";
    for (char c : hard) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();
}

void displayScoreboard() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Display scoreboard
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    glRasterPos2i(300, 400);
    std::string scoreText = "Your score: " + std::to_string(score); // Append score
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 300);
    std::string restart = "1. Restart";
    for (char c : restart) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glRasterPos2i(300, 200);
    std::string menu = "2. Menu";
    for (char c : menu) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    glutSwapBuffers();
}

void handleInput(unsigned char key, int x, int y) {
    if (gameActive) {
        handleGameInput(key, x, y);
    } else if (scoreboardActive) {
        handleScoreboardInput(key, x, y);
    } else if (menuScreenActive) {
        handleMenuInput(key, x, y);
    } else if (levelsMenuActive) {
        handleLevelsMenuInput(key, x, y);
    }
}

void handleMenuInput(unsigned char key, int x, int y) {
    switch (key) {
        case '1':
            // Start the game
            gameActive = true;
            menuScreenActive = false;
            glutPostRedisplay();
            break;
        case '2':
            // Display levels menu
            levelsMenuActive = true;
            menuScreenActive = false;
            glutPostRedisplay();
            break;
        case '3':
            // Exit the game
            exit(0);
            break;
    }
}

void handleGameInput(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
        case 'W':
            if (currentDirection != Direction::DOWN)
                currentDirection = Direction::UP;
            break;
        case 's':
        case 'S':
            if (currentDirection != Direction::UP)
                currentDirection = Direction::DOWN;
            break;
        case 'a':
        case 'A':
            if (currentDirection != Direction::RIGHT)
                currentDirection = Direction::LEFT;
            break;
        case 'd':
        case 'D':
            if (currentDirection != Direction::LEFT)
                currentDirection = Direction::RIGHT;
            break;
        case 27:
            gameActive = false;
            scoreboardActive = true;
            break;
    }
}

void handleLevelsMenuInput(unsigned char key, int x, int y) {
    switch (key) {
        case '1':
            // Set difficulty level to Easy
            currentLevel = Level::EASY;
            glutPostRedisplay();
            break;
        case '2':
            // Set difficulty level to Medium
            currentLevel = Level::MEDIUM;
            glutPostRedisplay();
            break;
        case '3':
            // Set difficulty level to Hard
            currentLevel = Level::HARD;
            glutPostRedisplay();
            break;
    }
    // Return to the menu
    levelsMenuActive = false;
    menuScreenActive = true;
}

void handleScoreboardInput(unsigned char key, int x, int y) {
    switch (key) {
        case '1':
        case 'r':
        case 'R':
            // Restart the game
            gameActive = true;
            scoreboardActive = false;
            resetGame();
            glutPostRedisplay();
            break;
        case '2':
            // Return to the menu
            scoreboardActive = false;
            menuScreenActive = true;
            //! resetGame();
            glutPostRedisplay();
            break;
    }
}

void initializeSnake() {
    int startY = WINDOW_HEIGHT / (2 * GRID_SIZE) * GRID_SIZE;
    int startX = WINDOW_WIDTH / (2 * GRID_SIZE) * GRID_SIZE;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_front({startX + i * GRID_SIZE, startY});
    }
}

void generateFood() {
    std::srand(std::time(nullptr));
    int maxX = WINDOW_WIDTH / GRID_SIZE;
    int maxY = WINDOW_HEIGHT / GRID_SIZE;
    foodX = std::rand() % maxX * GRID_SIZE;
    foodY = std::rand() % maxY * GRID_SIZE;
}

bool checkCollisionWithFood(int x, int y) {
    return x == foodX && y == foodY;
}

float getSnakeSpeed() {
    switch (currentLevel) {
        case Level::EASY:
            return SNAKE_SPEED * 0.5f;
        case Level::MEDIUM:
            return SNAKE_SPEED;
        case Level::HARD:
            return SNAKE_SPEED * 2.0f;
        default:
            return SNAKE_SPEED;
    }
}

void moveSnake() {
    SnakeSegment newHead = snake.front();
    switch (currentDirection) {
        case Direction::UP:
            newHead.y += GRID_SIZE;
            break;
        case Direction::DOWN:
            newHead.y -= GRID_SIZE;
            break;
        case Direction::LEFT:
            newHead.x -= GRID_SIZE;
            break;
        case Direction::RIGHT:
            newHead.x += GRID_SIZE;
            break;
    }

    // Handle boundary conditions to ensure snake reappears on the opposite side
    if (newHead.x < 0)
        newHead.x = WINDOW_WIDTH - GRID_SIZE;
    else if (newHead.x >= WINDOW_WIDTH)
        newHead.x = 0;
    if (newHead.y < 0)
        newHead.y = WINDOW_HEIGHT - GRID_SIZE;
    else if (newHead.y >= WINDOW_HEIGHT)
        newHead.y = 0;

    // Check if the new head collides with the snake's body
    for (const auto& segment : snake) {
        if (newHead.x == segment.x && newHead.y == segment.y) {
            gameOver = true;
            return;
        }
    }

    // Check if the snake eats the food
    if (checkCollisionWithFood(newHead.x, newHead.y)) {
        // Increase snake's length
        snake.push_front(newHead);
        generateFood();
        score++; // Increment score
    } else {
        // Move the snake by adding a new head and removing the tail
        snake.push_front(newHead);
        snake.pop_back();
    }
}

void resetTimer() {
    glutTimerFunc(0, timer, 0); // Reset the timer
}

void resetGame() {
    snake.clear();
    initializeSnake();
    generateFood();
    gameOver = false;
    score = 0;
    currentDirection = Direction::RIGHT; // Reset current direction to default
    resetTimer(); // Reset the timer
}

void drawSnake() {
    glColor3f(0.0f, 1.0f, 0.0f); // Green color for snake
    for (const auto& segment : snake) {
        glBegin(GL_QUADS);
        glVertex2f(segment.x, segment.y);
        glVertex2f(segment.x + GRID_SIZE, segment.y);
        glVertex2f(segment.x + GRID_SIZE, segment.y + GRID_SIZE);
        glVertex2f(segment.x, segment.y + GRID_SIZE);
        glEnd();
    }
}

void drawFood() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for food
    glBegin(GL_QUADS);
    glVertex2f(foodX, foodY);
    glVertex2f(foodX + GRID_SIZE, foodY);
    glVertex2f(foodX + GRID_SIZE, foodY + GRID_SIZE);
    glVertex2f(foodX, foodY + GRID_SIZE);
    glEnd();
}

void drawGrid() {
    glColor3f(1.0f, 1.0f, 1.0f); // White color for grid
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int x = 0; x <= WINDOW_WIDTH; x += GRID_SIZE) {
        glVertex2f(x, 0);
        glVertex2f(x, WINDOW_HEIGHT);
    }
    for (int y = 0; y <= WINDOW_HEIGHT; y += GRID_SIZE) {
        glVertex2f(0, y);
        glVertex2f(WINDOW_WIDTH, y);
    }
    glEnd();
}
