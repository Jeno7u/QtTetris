#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>


#include "startmenu.h"

StartMenu::StartMenu(int WindowWidth, int WindowHeight, QWidget* parent) :
    WindowWidth(WindowWidth), WindowHeight(WindowHeight), QGraphicsView(parent), scene(new QGraphicsScene(this))
{
    if (menu){
        return;
    }

    setBackground();
    createButtons();

    this->setScene(scene);
    this->setFixedSize(WindowWidth, WindowHeight);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene->setSceneRect(0, 0, WindowWidth, WindowHeight);

}

void StartMenu::setBackground()
{
    QPixmap image(":/startmenu.png");

    menu = new QGraphicsPixmapItem();
    menu->setPixmap(image);
    scene->addItem(menu);
}

void StartMenu::createButtons()
{
    startButton = new QPushButton();
    startButton->setText("Start Game");
    startButton->setParent(this);

    startButton->setStyleSheet(
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

    startButton->setGeometry(WindowWidth/2 - 100, WindowHeight/2 - 60,200, 52);

    connect(startButton, &QPushButton::clicked, this, &StartMenu::startGameClicked);
}

// void StartMenu::mousePressEvent(QMouseEvent* event)
// {
//     QGraphicsView::mousePressEvent(event);
//     qDebug() << 1;  // Emit the signal when clicked
// }

