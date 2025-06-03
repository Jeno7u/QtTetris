#ifndef BLOCK_H
#define BLOCK_H

#include <QGraphicsPixmapItem>

class Block : public QGraphicsPixmapItem
{
public:
    Block(QPixmap pixmap, QGraphicsItem* parent = nullptr);
};

#endif // BLOCK_H
