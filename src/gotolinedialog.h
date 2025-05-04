#ifndef GOTOLINEDIALOG_H
#define GOTOLINEDIALOG_H

#include <QDialog>

class QSpinBox;
class QPushButton;
class QPlainTextEdit;

class GoToLineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoToLineDialog(QWidget *parent = nullptr);
    void setEditor(QPlainTextEdit *editor);

private slots:
    void goToLine();

private:
    QPlainTextEdit *editor;
    QSpinBox *lineNumberSpinBox;
    QPushButton *goButton;
    QPushButton *closeButton;
};

#endif // GOTOLINEDIALOG_H
