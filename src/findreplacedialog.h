#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QTextDocument>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QPlainTextEdit;
class QLabel;

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = nullptr);
    void setEditor(QPlainTextEdit *editor);

private slots:
    void findNext();
    void replace();
    void replaceAll();

private:
    QPlainTextEdit *editor;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QCheckBox *caseSensitiveCheckBox;
    QPushButton *findButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;
    QLabel *statusLabel;

    bool find(bool forward = true, bool showError = true);
    void updateUI();
};

#endif // FINDREPLACEDIALOG_H
