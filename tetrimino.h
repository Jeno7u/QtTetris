#ifndef TETRIMINO_H
#define TETRIMINO_H

#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QRect>
#include <QKeyEvent>
#include <QTimer>


class Board;


class Tetrimino: public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    // так храним положение блоков для различных тетримино
    const int figures[7][4] =
    {
        1, 3, 5, 7, // I
        0, 2, 3, 5, // Z
        1, 3, 2, 4, // S
        1, 3, 2, 5, // T
        0, 1, 3, 5, // L
        1, 3, 5, 4, // J
        0, 1, 2, 3  // O
    };

    // структура для координат блока внутри фигуры
    struct Offset {int x, y;} blockOffsets[4];


    Tetrimino(int TileSize, int colorIndex, Board * gameBoard, int fallSpeedTime, bool isFocus);


    int TileSize;
    int colorIndex;
    int fallSpeedTime;
    bool isFocus;
    QTimer* timer = new QTimer();

    void drawBlocks();
    void handleKeyEvent(QKeyEvent *event);
    bool collisionCheck();

~Tetrimino() {
    if (timer) {
        timer->stop();
        delete timer;
    }
}

public slots:
    void moveDown();

signals:
    void landed();

private:
    Board* gameBoard;
    QPixmap tileImage;

    void rotate(qreal angle);
    void loadSprite();
    void calculateOffsets();
};

#endif // TETRIMINO_H
