#ifndef P401OPTIONWIDGET_H
#define P401OPTIONWIDGET_H

#include <QWidget>

class IndexSpinBox;
class Node;

class P401OptionWidget : public QWidget
{
    Q_OBJECT
public:
    P401OptionWidget(uint8_t channel, QWidget *parent = nullptr);

    void readAllObject();

public slots:
    void setNode(Node *node);

private:
    uint8_t _channel;
    Node *_node;

    IndexSpinBox *_diSchmittTriggersHigh;
    IndexSpinBox *_diSchmittTriggersLow;

    // Create widgets
    void createWidgets();
};

#endif // P401OPTIONWIDGET_H
