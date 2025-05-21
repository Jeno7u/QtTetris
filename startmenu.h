#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPushButton>


#ifndef STARTMENU_H
#define STARTMENU_H

class StartMenu : public QGraphicsView
{
    Q_OBJECT

public:
    StartMenu(int WindowWidth, int WindowHeight, QWidget *parent = nullptr);

    int WindowWidth;
    int WindowHeight;

    QGraphicsScene* scene;
    QGraphicsPixmapItem* menu;

    QPushButton* startButton;

signals:
    void startGameClicked();

private:
    void setBackground();
    void createButtons();
};

#endif // STARTMENU_H
