#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit mainWindow(QWidget *parent = 0);
    ~mainWindow();
public slots:
    void updateWindow();
    void openFile();
    void diaporama();
    void fileInfo();
    void preferences();
    void zoomIn();
    void zoomOut();
    void mirorHorizontal();
    void mirorVertical();
    void rightRotate();
    void leftRotate();

signals:
    void imageChanged();

protected:
    void loadFileList(QStringList l);
    QStringList getImagesDir(QString dir);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void changeEvent(QEvent *e);
    void drawImage();
    void setupConnexions();

private:
    QSize m_oldSize;
    Ui::mainWindow *ui;
    QStringList m_files;
    QImage* m_image;
    QPixmap m_pixmap;
    QMovie* m_animated;
    QLabel* m_label;
    QScrollArea* sc;
    int m_current;
};

#endif // MAINWINDOW_H
