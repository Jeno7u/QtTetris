#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QPushButton>

#include "tetrimino.h"

class Board : public QGraphicsView
{
    Q_OBJECT

public:
    QGraphicsScene* scene;
    QGraphicsPixmapItem* background;

    Board(int TileSize, int WindowWidth, int WindowHeight, QWidget *parent = nullptr);

    Tetrimino* nextPiece;

    void startGame();
    void resetGame();

signals:
    void startMenuButtonClicked();

private:
    int TileSize;
    int WindowWidth, WindowHeight;
    int score = 0;
    int totalLinesCleared = 0;
    int level = 1;
    QList<int> colorIndexesBag;

    Tetrimino* currentPiece;
    QLabel* scoreLabel;
    QLabel* levelLabel;
    bool isRunning = true;

    void createLabels();
    int getRandomColor();
    void spawnPiece();
    void setNextPiece();

    void convertTetriminoToBlocks();
    void clearFullLines();
    bool isLineFull(int y);
    void removeLine(int y);
    void moveLinesDown(int delta_y);

    QPushButton* resetButton;
    QPushButton* startMenuButton;
    QWidget* overlay;
    QLabel* gameOverLabel;

    void createGameOverButtons();
    void gameOver();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void handleLanded();
};

#endif // BOARD_H
