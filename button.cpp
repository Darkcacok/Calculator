#include <QtWidgets>

#include "button.h"

Button::Button(const QString &text, QWidget *parent)
    :QToolButton(parent)
{
    //По горизонтали размер кнопки Expanding(как можно больше месте)
    //По вертикали Preferred(разумный размер)
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);
}

QSize Button::sizeHint() const
{
    //Задаем размер кнопок
    QSize size = QToolButton::sizeHint();
    size.rheight() +=20;
    size.rwidth() = qMax(size.width(),size.height());
    return size;
}
