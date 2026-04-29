#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QImage>

class QLabel;
class QSlider;
class QPushButton;
class QGraphicsScene;
class ImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void openFolder();
    void nextSlice();
    void prevSlice();
    void updateSlice(int index);
    void updateImage();

private:
    QVector<QImage> slices;
    int currentIndex;

    ImageView *view;
    QGraphicsScene *scene;

    QSlider *slider;
    QLabel *infoLabel;

    QSlider *brightnessSlider;
    QSlider *contrastSlider;

    QImage applyAdjustments(const QImage &img);
};

#endif
