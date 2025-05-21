#include <QApplication>
#include <QStackedWidget>
#include <functional>

#include "startmenu.h"
#include "board.h"

const int WindowWidth = 512;
const int WindowHeight = 512;
const int TileSize = 26;

Board* gameBoard = nullptr;

void startGame(QStackedWidget* stackedWidget, StartMenu* menu) {
    if (gameBoard) {
        stackedWidget->removeWidget(gameBoard);
        delete gameBoard;
    }

    // Create new board
    gameBoard = new Board(TileSize, WindowWidth, WindowHeight);
    stackedWidget->addWidget(gameBoard);
    stackedWidget->setCurrentWidget(gameBoard);

    QObject::connect(gameBoard, &Board::startMenuButtonClicked,
                     [=]() {
                         gameBoard->resetGame();
                         stackedWidget->setCurrentWidget(menu);
                     });

    gameBoard->startGame();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStackedWidget* stackedWidget = new QStackedWidget;

    StartMenu* menu = new StartMenu(WindowWidth, WindowHeight);
    stackedWidget->addWidget(menu);  // Index 0

    QObject::connect(menu, &StartMenu::startGameClicked,
                     [=]() { startGame(stackedWidget, menu); });

    stackedWidget->setCurrentWidget(menu);
    stackedWidget->show();

    return a.exec();
}
