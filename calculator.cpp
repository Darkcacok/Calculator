#include <QtWidgets>

#include <cmath>

#include "button.h"
#include "calculator.h"

Button *button;

Calculator::Calculator(QWidget *parent)
    :QWidget(parent)
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;

    display = new QLineEdit("0");

    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);//заполнение справо налево
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 15);
    display->setFont(font);


    display2 = new QLineEdit("");
    display2->setReadOnly(true);
    display2->setAlignment(Qt::AlignRight);//заполнение справо налево
    display2->setMaxLength(100);

    font.setPointSize(font.pointSize() - 5);
    display2->setFont(font);

    for(int i = 0; i < NumDigitButtons; ++i)
    {
        digitButtons[i] = createButton(QString::number(i),SLOT(digitClicked()));
        digitButtons[i]->setShortcut(Key(i));
    }
    Button *pointButton = createButton(tr("."),SLOT(pointClicked()));
    pointButton->setShortcut(Qt::Key_Comma|Qt::Key_Period);

    Button *backspaceButton = createButton(tr("<--"), SLOT(backspaceClicked()));
    Button *clearAllButton = createButton(tr("Clear"), SLOT(clearAll()));
    backspaceButton->setShortcut(Qt::Key_Backspace);
    clearAllButton->setShortcut(Qt::Key_Delete);

    Button *divisionButton = createButton(tr("\303\267"), SLOT(multiplicativeOperatorClicked()));
    Button *timesButton = createButton(tr("\303\227"), SLOT(multiplicativeOperatorClicked()));
    Button *minusButton = createButton(tr("-"), SLOT(additiveOperatorClicked()));
    Button *plusButton = createButton(tr("+"), SLOT(additiveOperatorClicked()));
    Button *equalButton = createButton(tr("="), SLOT(equalClicked()));
    divisionButton->setShortcut(Qt::Key_Slash);
    timesButton->setShortcut(Qt::Key_Asterisk);
    minusButton->setShortcut(Qt::Key_Minus);
    plusButton->setShortcut(Qt::Key_Plus);
    button = equalButton;

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->addWidget(display2, 0, 0, 1, 4);
    mainLayout->addWidget(display, 1, 0, 1, 4);
    mainLayout->addWidget(clearAllButton, 2, 0, 1, 2);
    mainLayout->addWidget(backspaceButton, 2, 2);

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 3;
        int column = ((i - 1) % 3);
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 6, 1);
    mainLayout->addWidget(pointButton, 6, 0);

    mainLayout->addWidget(divisionButton, 2, 3);
    mainLayout->addWidget(timesButton, 3, 3);
    mainLayout->addWidget(minusButton, 4, 3);
    mainLayout->addWidget(plusButton, 5, 3);

    mainLayout->addWidget(equalButton, 6, 2, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Calculator"));
}


void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    int digitValue = clickedButton->text().toInt();
    if(display->text() == "0" && digitValue == 0.0)
        return;
    if(waitingForOperand){
        display->clear();
        waitingForOperand = false;
    }
    display->setText(display->text() + QString::number(digitValue));
}


void Calculator::additiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if(waitingForOperand)
    {
        QString text = display2->text();
        if(text.isEmpty())
            text = display->text();
        else
            text.chop(1);
        display2->setText(text + clickedButton->text());
        pendingMultiplicativeOperator.clear();
    }
    else
         display2->setText(display2->text() + display->text() + clickedButton->text());

    if (!pendingMultiplicativeOperator.isEmpty() && !waitingForOperand)
    {
        if (!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty() && !waitingForOperand)
    {
        if (!calculate(operand, pendingAdditiveOperator))
        {
            abortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    }
    else
    {
        sumSoFar = operand;
    }


    pendingAdditiveOperator = clickedOperator;

    waitingForOperand = true;
}


void Calculator::multiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if(waitingForOperand)
    {
        QString text = display2->text();
        if(text.isEmpty())
            text = display->text();
        else
            text.chop(1);
        if(pendingMultiplicativeOperator.isEmpty())
            text = "(" + text + ")";
        display2->setText(text + clickedButton->text());
        pendingAdditiveOperator.clear();
    }
    else
         display2->setText(display2->text() + display->text() + clickedButton->text());


    if (!pendingMultiplicativeOperator.isEmpty()  && !waitingForOperand)
    {
        if (!calculate(operand, pendingMultiplicativeOperator))
        {
            abortOperation();
            return;
        }
        display->setText(QString::number(factorSoFar));
    }
    else
    {
        factorSoFar = operand;
    }


    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;

}


void Calculator::equalClicked()
{
     double operand = display->text().toDouble();

     display2->setText("");

     if (!pendingMultiplicativeOperator.isEmpty() && !waitingForOperand) {
         if (!calculate(operand, pendingMultiplicativeOperator)) {
             abortOperation();
             return;
         }
         operand = factorSoFar;
         factorSoFar = 0.0;
         pendingMultiplicativeOperator.clear();
     }
     if (!pendingAdditiveOperator.isEmpty() && !waitingForOperand) {
         if (!calculate(operand, pendingAdditiveOperator)) {
             abortOperation();
             return;
         }
         pendingAdditiveOperator.clear();
     } else {
         sumSoFar = operand;
     }

     display->setText(QString::number(sumSoFar));
     sumSoFar = 0.0;
     waitingForOperand = true;
     button->setDown(false);
}


void Calculator::pointClicked()
{
    if (waitingForOperand)
        display->setText("0");
    if (!display->text().contains('.'))
        display->setText(display->text() + tr("."));
    waitingForOperand = false;
}


void Calculator::backspaceClicked()
{
    if(waitingForOperand)
        return;

    QString text = display->text();
    text.chop(1);
    if(text.isEmpty()){
        text="0";
        waitingForOperand = true;
    }
    display->setText(text);
}


void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    waitingForOperand = true;

    pendingMultiplicativeOperator.clear();
    pendingAdditiveOperator.clear();

    display->setText("0");
    display2->setText("");
}



Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("нельзя"));;
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}

Qt::Key Calculator::Key(int i)
{
    switch (i)
    {
    case 0:
        return Qt::Key_0;
    case 1:
        return Qt::Key_1;
    case 2:
        return Qt::Key_2;
    case 3:
        return Qt::Key_3;
    case 4:
        return Qt::Key_4;
    case 5:
        return Qt::Key_5;
    case 6:
        return Qt::Key_6;
    case 7:
        return Qt::Key_7;
    case 8:
        return Qt::Key_8;
    case 9:
        return Qt::Key_9;
    }

}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Equal)
    {equalClicked();button->setDown(true);}
}

void Calculator::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter || event->key() == Qt::Key_Equal)
        button->setDown(false);
}

