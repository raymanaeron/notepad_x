#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class EditorWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void createNewTab();
    void closeCurrentTab();
    void onTabCloseRequested(int index);
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void updateTabText(int index);
    void documentModified(bool modified);

private:
    QTabWidget *tabWidget;
    int untitledCount;
    
    void createActions();
    void createMenus();
    EditorWidget *currentEditor();
    bool ensureHasOpenTab();
    bool maybeSaveAll();
};

#endif // MAINWINDOW_H
