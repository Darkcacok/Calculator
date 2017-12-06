#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QKeyEvent>
#include <QPushButton>

class Button;

class Calculator: public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent=0);

private slots:
    void digitClicked();                    //нажатие на цифру
    void additiveOperatorClicked();         //нажаите на + или -
    void multiplicativeOperatorClicked();   //нажатие на * или /
    void pointClicked();                    //нажатие на , || .
    void equalClicked();                    //нажаите на =
    void backspaceClicked();                //нажатие на <--
    void clearAll();                        //нажатие на C(отчистить все)



private:
    Button *createButton(const QString &text,const char *member);
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);

    double sumSoFar;
    double factorSoFar;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;

    QLineEdit *display;
    QLineEdit *display2;

    static const int NumDigitButtons = 10;
    Button *digitButtons[NumDigitButtons];

    Qt::Key Key(int);

protected:
    void keyPressEvent(QKeyEvent *event)  Q_DECL_OVERRIDE;

};


#endif // CALCULATOR_H
