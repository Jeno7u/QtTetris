#include <QTimer>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QString>
#include <QMutableListIterator>
#include <QFontDatabase>
#include <QPushButton>
#include <random>


#include "board.h"

Board::Board(int TileSize, int WindowWidth, int WindowHeight, QWidget* parent)
    : TileSize(TileSize), WindowWidth(WindowWidth), WindowHeight(WindowHeight), QGraphicsView(parent), scene(new QGraphicsScene(this))
{
    // устанавливаем задний фон
    QPixmap image(":/background.png");
    background = new QGraphicsPixmapItem();
    background->setPixmap(image);
    scene->addItem(background);


    setScene(scene);
    setFixedSize(WindowWidth, WindowHeight);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    scene->setSceneRect(0, 0, WindowWidth, WindowHeight);

    createLabels();
}

void Board::startGame()
{
    int nextPieceColorIndex = getRandomColor();
    nextPiece = new Tetrimino(TileSize, nextPieceColorIndex, this, 30000, false);

    spawnPiece();    // создаём первую фигуру
    currentPiece->setFocus();
}

void Board::createLabels()
{
    scoreLabel = new QLabel("0", this);
    scoreLabel->setStyleSheet("QLabel { color: #828588; font-size: 32px; font-family: 'Rockwell'; font-weight: bold; }");
    scoreLabel->setGeometry(345, 75, 125, 62);
    scoreLabel->setAlignment(Qt::AlignCenter);

    levelLabel = new QLabel("0", this);
    levelLabel->setStyleSheet("QLabel { color: #828588; font-size: 32px; font-family: 'Rockwell'; font-weight: bold; }");
    levelLabel->setGeometry(345, 210, 125, 62);
    levelLabel->setAlignment(Qt::AlignCenter);

    // QGraphicsTextItem *textItem = scene->addText("Game Over", QFont("Arial", 30));
    // textItem->setDefaultTextColor(Qt::red);
    // textItem->setPos(scene->width()/2 - textItem->boundingRect().width()/2,
    //                  scene->height()/2);
}

int Board::getRandomColor()
{
    std::random_device rd;
    std::mt19937 rng = std::mt19937(rd());
    if (colorIndexesBag.size() == 0) {
        colorIndexesBag = {0, 1, 2, 3, 4, 5, 6};
        std::shuffle(colorIndexesBag.begin(), colorIndexesBag.end(), rng);
    }

    int colorIndex = colorIndexesBag.back();
    colorIndexesBag.pop_back();

    return colorIndex;
}

void Board::spawnPiece()
{
    if (!isRunning) {
        return;
    }

    int colorIndex = nextPiece->colorIndex;

    int fallSpeedTime = 1000 - level * 100;
    currentPiece = new Tetrimino(TileSize, colorIndex, this, fallSpeedTime, true);
    if (currentPiece->collisionCheck()) {
        gameOver();
        return;
    }
    scene->addItem(currentPiece);
    connect(currentPiece, &Tetrimino::landed, this, &Board::handleLanded);


    setNextPiece();
}

void Board::setNextPiece()
{
    int colorIndex = getRandomColor();
    if (nextPiece->scene()) {
        scene->removeItem(nextPiece);
    }
    nextPiece = new Tetrimino(TileSize, colorIndex, this, 30000, false);

    QPointF position = QPointF(405, 420) - QPointF(nextPiece->boundingRect().width() / 2, nextPiece->boundingRect().height() / 2);
    if (colorIndex == 0) {
        position -= QPointF(TileSize, 0);
    }
    nextPiece->setPos(position);
    scene->addItem(nextPiece);
}

void Board::handleLanded()
{
    convertTetriminoToBlocks();

    clearFullLines();
    spawnPiece();
}

void Board::convertTetriminoToBlocks()
{
    // сохраняем позиции блоков на сцене
    QList<QPointF> blocksPos;
    QList<QGraphicsItem*> blocks = currentPiece->childItems();
    qreal rotationCurrentPiece = currentPiece->rotation();
    for (QGraphicsItem* block : blocks){
        // работает на гениальности и божьей помощи, никогда не трогать
        block->setTransformOriginPoint(TileSize / 2, TileSize / 2);
        block->setRotation(360 - rotationCurrentPiece);
        blocksPos.append(block->mapToScene(QPointF(0, 0)));
    }

    // удаляем фигуру с сцены
    scene->removeItem(currentPiece);

    // добавляем отдельные блоки фигуры на сцену
    for (int i = 0; i < blocks.size(); i++) {
        QGraphicsItem* block = blocks[i];

        currentPiece->removeFromGroup(block);

        block->setPos(blocksPos[i]);
        scene->addItem(block);
    }
    delete currentPiece;
    scene->update();
}

void Board::keyPressEvent(QKeyEvent* event)
{
    if (currentPiece) {
        currentPiece->handleKeyEvent(event); // You'll need to add this
    }
}

void Board::clearFullLines()
{
    int countFullLines = 0;
    for (int y = 14; y < 18 * TileSize + 14 ; y += TileSize) {
        if (isLineFull(y)) {
            removeLine(y);
            moveLinesDown(y);

            countFullLines++;
        }
    }

    totalLinesCleared += countFullLines;
    level = totalLinesCleared / 10;
    if (level > 10) {
        level = 10;
    }
    levelLabel->setText(QString("%1").arg(level));
    scene->update();

    if (countFullLines > 0) {
        switch(countFullLines) {
        case 1: score += 40 * (level + 1); break;
        case 2: score += 100 * (level + 1); break;
        case 3: score += 300 * (level + 1); break;
        case 4: score += 1200 * (level + 1); break;
        }
        scoreLabel->setText(QString("%1").arg(score));
        scene->update();
    }
}

bool Board::isLineFull(int y)
{
    int countBlocksY = 0;
    QList<QGraphicsItem*> sceneItems = scene->items();
    for (int i = 0; i < sceneItems.size(); i++) {
        QGraphicsItem* sceneItem = sceneItems[i];
        if (sceneItem == background) {
            continue;
        }

        if (sceneItem->scenePos().y() == y) {
            countBlocksY++;
        }
    }

    if (countBlocksY == 10) {
        return true;
    }
    return false;
}

void Board::removeLine(int y)
{
    QListIterator<QGraphicsItem*> it(scene->items());
    while (it.hasNext()) {
        QGraphicsItem* sceneItem = it.next();

        if (sceneItem == background) {
            continue;
        }

        if (sceneItem->scenePos().y() == y) {
            scene->removeItem(sceneItem);
            delete sceneItem;
        }
    }
    scene->update();
}

void Board::moveLinesDown(int y)
{
    QList<QGraphicsItem*> sceneItems = scene->items();
    for (QGraphicsItem* sceneItem : sceneItems){
        if (sceneItem == background) {
            continue;
        }

        if (sceneItem->scenePos().y() < y){
            sceneItem->setPos(sceneItem->scenePos().x(), sceneItem->scenePos().y() + TileSize);
        }
    }
}

void Board::createGameOverButtons()
{
    resetButton = new QPushButton();
    resetButton->setText("Reset");
    resetButton->setParent(this);

    resetButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #737373;"
        "   border: none;"
        "   color: white;"
        "   padding: 2px 8px;"
        "   text-align: center;"
        "   text-decoration: none;"
        "   font-family: 'Rockwell';"
        "   font-size: 16px;"
        "   margin: 4px 2px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #404040;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #272727;"
        "}"
        );

    resetButton->setGeometry(WindowWidth / 2 - 100, WindowHeight / 2 + 10,200, 52);
    resetButton->show();

    connect(resetButton, &QPushButton::clicked, this, &Board::resetGame);


    startMenuButton = new QPushButton();
    startMenuButton->setText("To Start Menu");
    startMenuButton->setParent(this);

    startMenuButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #737373;"
        "   border: none;"
        "   color: white;"
        "   padding: 2px 8px;"
        "   text-align: center;"
        "   text-decoration: none;"
        "   font-family: 'Rockwell';"
        "   font-size: 16px;"
        "   margin: 4px 2px;"
        "   border-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #404040;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #272727;"
        "}"
        );

    startMenuButton->setGeometry(WindowWidth / 2 - 100, WindowHeight / 2 + 170,200, 52);
    startMenuButton->show();
    connect(startMenuButton, &QPushButton::clicked, this, &Board::startMenuButtonClicked);
}

void Board::gameOver()
{
    isRunning = false;
    overlay = new QWidget(this);
    overlay->setGeometry(0, 0, width(), height());
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);");
    overlay->show();

    gameOverLabel = new QLabel("GAME OVER", this);
    gameOverLabel->setStyleSheet("QLabel { color: red; font-size: 60px; font-family: 'Rockwell'; font-weight: bold; }");
    gameOverLabel->setGeometry(0, 0, scene->width(), scene->height() - 60);
    gameOverLabel->setAlignment(Qt::AlignCenter);
    gameOverLabel->raise();
    gameOverLabel->show();

    createGameOverButtons();

    currentPiece->timer->stop();
    nextPiece->timer->stop();
}
void Board::resetGame()
{
    if (overlay) {
        overlay->hide();
        overlay->deleteLater();
        overlay = nullptr;
    }

    if (gameOverLabel) {
        gameOverLabel->hide();
        gameOverLabel->deleteLater();
        gameOverLabel = nullptr;
    }

    if (resetButton) {
        resetButton->hide();
        resetButton->deleteLater();
        resetButton = nullptr;
    }

    if (startMenuButton) {
        startMenuButton->hide();
        startMenuButton->deleteLater();
        startMenuButton = nullptr;
    }

    if (scene) {
        delete scene;
    }

    level = 0;
    score = 0;
    scoreLabel->setText(QString("%1").arg(score));

    scene = new QGraphicsScene(this);
    setScene(scene);

    // 3. Re-add background and reset state
    background = new QGraphicsPixmapItem(QPixmap(":/background.png"));
    scene->addItem(background);
    scene->setSceneRect(0, 0, WindowWidth, WindowHeight);

    isRunning = true;
    startGame();
}


