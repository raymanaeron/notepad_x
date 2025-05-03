#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

// Forward declare the EditorWidget class to avoid circular dependencies
class EditorWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);

private:
    QTabWidget *tabWidget;
    int untitledCount;
};

#endif // MAINWINDOW_H
