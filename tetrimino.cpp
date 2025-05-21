#include "tetrimino.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QRect>
#include <QDebug>
#include <QList>
#include <QPointF>
// #include <QRound>

#include "board.h"
#include "block.h"


Tetrimino::Tetrimino(int TileSize, int colorIndex, Board * gameBoard, int fallSpeedTime, bool isFocus):
    TileSize(TileSize), colorIndex(colorIndex), gameBoard(gameBoard), fallSpeedTime(fallSpeedTime), isFocus(isFocus)
{
    drawBlocks();

    timer->setInterval(fallSpeedTime);
    connect(timer, SIGNAL(timeout()), this, SLOT(moveDown()));

    timer->start(fallSpeedTime);
}

void Tetrimino::drawBlocks()
{
    loadSprite(); // загружаем спрайт блока

    calculateOffsets(); // вычисляем offsets для каждого блока из фигуры

    for (int i = 0; i < 4; i++)
    {
        // создание отдельных блоков, добавление offsets и соединение их в одну Tetrimino
        QGraphicsPixmapItem * block = new Block(tileImage);

        block->setPos(blockOffsets[i].x * TileSize, blockOffsets[i].y * TileSize);

        this->addToGroup(block);


        // устанавливаем точку вращения фигуры
        if (i == 1)
        {
            this->setTransformOriginPoint(blockOffsets[i].x * TileSize, blockOffsets[i].y * TileSize);
        }
    }

    // устанавливаем начальную позицию падения
    this->setPos((5 * TileSize + 18), (1 * TileSize + 14));


}

void Tetrimino::rotate(qreal angle) {
    setRotation(rotation() + angle);
}

void Tetrimino::handleKeyEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left){
        setX(x() - TileSize);

        if (collisionCheck())
        {
            this->setX(x() + TileSize);
            return;
        }
    }

    else if (event->key() == Qt::Key_Right){
        setX(x() + TileSize);

        if (collisionCheck())
        {
            this->setX(x() - TileSize);
            return;
        }
    }

    else if (event->key() == Qt::Key_Up){
        rotate(90);

        if (collisionCheck())
        {
            rotate(-90);
            return;
        }
    }

    else if (event->key() == Qt::Key_Down){
        setY(y() + TileSize);

        if (collisionCheck())
        {
            this->setY(y() - TileSize);
            return;
        }
    }

    else if (event->key() == Qt::Key_Space) {
        while (true){

            setY(y() + TileSize);

            if (collisionCheck())
            {
                this->setY(y() - TileSize);

                // логика "приземления" тетримино
                timer->stop();
                emit landed();
                return;
            }
        }
    }
}

bool Tetrimino::collisionCheck()
{

    // проверки на границу поля
    if((sceneBoundingRect().left() < 44) || (sceneBoundingRect().right() > 44 + TileSize * 10) || (sceneBoundingRect().bottom() > TileSize * 19))
    {
        return true;
    }

    // проверяем на столкновение с другими обьектами
    QList<QGraphicsItem*> sceneItems = gameBoard->scene->items();
    for (int i = 0; i < sceneItems.size(); i++) {
        QGraphicsItem* sceneItem = sceneItems[i];
        // пропускаем столкновение с задним фоном и само собой
        if (sceneItem == gameBoard->background) {
            continue;
        }

        if (sceneItem == this) {
            continue;
        }

        if (sceneItem == gameBoard->nextPiece) {
            continue;
        }


        for (QGraphicsItem* block : gameBoard->nextPiece->childItems()) {
            if (sceneItem == block) {
                continue;
            }
        }


        QList<QGraphicsItem*> tetriminoChildItems = this->childItems();
        for (QGraphicsItem* tetriminoChildItem : tetriminoChildItems) {
            // пропускаем столкновение с блоками внутри самой фигуры
            if (sceneItem == tetriminoChildItem) {
                continue;
            }

            if (tetriminoChildItem->collidesWithItem(sceneItem)) {
                return true;
            }
        }
    }
    return false;
}

// логика движения фигуры вниз спустя время
void Tetrimino::moveDown()
{
    setY(y() + TileSize);

    if (collisionCheck())
    {
        this->setY(y() - TileSize);

        // логика "приземления" тетримино
        timer->stop();
        emit landed();
        return;
    }
}

void Tetrimino::loadSprite()
{
    QPixmap spriteSheet(":/tiles.png");

    QRect tileRect(TileSize * colorIndex, 0, TileSize, TileSize);
    tileImage = spriteSheet.copy(tileRect);
}

void Tetrimino::calculateOffsets()
{
    for (int i = 0; i < 4; i++)
    {
        blockOffsets[i].x = figures[colorIndex][i] % 2;
        blockOffsets[i].y = figures[colorIndex][i] / 2;
    }
}
