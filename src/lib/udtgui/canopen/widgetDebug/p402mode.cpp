#include "p402mode.h"

P402Mode::P402Mode(QWidget *parent)
    :QWidget(parent)
{
    _node = nullptr;
}

Node *P402Mode::node() const
{
    return _node;
}

uint8_t P402Mode::axis() const
{
    return _axis;
}

void P402Mode::updateData()
{

}

void P402Mode::reset()
{

}

void P402Mode::stop()
{

}
