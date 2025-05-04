#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setEditorMargins(int left, int top, int right, int bottom);
    void updateLineNumberAreaForTheme(bool isDark);
    
    // Add explicit setter for theme state
    void setDarkTheme(bool dark) { isDarkTheme = dark; }
    
    // Add zoom functionality
    void zoomIn(int range = 1);
    void zoomOut(int range = 1);
    void resetZoom();
    int getCurrentZoomLevel() const { return zoomLevel; }
    void setZoomLevel(int level);

signals:
    void zoomLevelChanged(int zoomLevel); // Add this signal

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *e) override; // Add this method

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    LineNumberArea *lineNumberArea;
    int zoomLevel; // Track the current zoom level
    const int DEFAULT_FONT_SIZE = 10; // Default font size in points
    bool isDarkTheme; // Keep track of current theme

    friend class LineNumberArea;
};

#endif // CODEEDITOR_H
