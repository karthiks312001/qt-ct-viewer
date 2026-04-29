#include "mainwindow.h"
#include "imageview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QDir>
#include <QGraphicsScene>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentIndex(0)
{
    QWidget *central = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Buttons
    QPushButton *openBtn = new QPushButton("Open");
    QPushButton *prevBtn = new QPushButton("Prev");
    QPushButton *nextBtn = new QPushButton("Next");

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(openBtn);
    topLayout->addWidget(prevBtn);
    topLayout->addWidget(nextBtn);

    infoLabel = new QLabel("0 / 0");
    topLayout->addWidget(infoLabel);

    // Viewer
    view = new ImageView;
    scene = new QGraphicsScene(this);
    view->setScene(scene);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Slider
    slider = new QSlider(Qt::Horizontal);
    slider->setFixedHeight(30);
    resize(1000, 700);                // default size
    setMinimumSize(800, 600);         // prevent too small


    // Brightness / Contrast
    brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(-100, 100);

    contrastSlider = new QSlider(Qt::Horizontal);
    contrastSlider->setRange(1, 300);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(view);
    mainLayout->addWidget(slider);
    mainLayout->addWidget(new QLabel("Brightness"));
    mainLayout->addWidget(brightnessSlider);
    mainLayout->addWidget(new QLabel("Contrast"));
    mainLayout->addWidget(contrastSlider);

    central->setLayout(mainLayout);
    setCentralWidget(central);

    // Connections
    connect(openBtn, SIGNAL(clicked()), this, SLOT(openFolder()));
    connect(prevBtn, SIGNAL(clicked()), this, SLOT(prevSlice()));
    connect(nextBtn, SIGNAL(clicked()), this, SLOT(nextSlice()));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateSlice(int)));

    connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(updateImage()));
    connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(updateImage()));
}

void MainWindow::openFolder()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Open Folder");
    QDir dir(dirPath);

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp";

    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

    slices.clear();

    foreach(QFileInfo file, files)
    {
        QImage img(file.absoluteFilePath());

        if(!img.isNull())
        {
            // ✅ Declare gray BEFORE using it
            QImage gray = img.convertToFormat(QImage::Format_Indexed8);

            QVector<QRgb> table;
            for(int i = 0; i < 256; i++)
                table.push_back(qRgb(i, i, i));

            gray.setColorTable(table);

            slices.append(gray);
        }
    }

    if(!slices.isEmpty())
    {
        currentIndex = 0;
        slider->setMaximum(slices.size() - 1);
        updateSlice(0);
    }
}

void MainWindow::updateSlice(int index)
{
    if(slices.isEmpty()) return;

    currentIndex = index;
    updateImage();
}

void MainWindow::updateImage()
{
    if(slices.isEmpty()) return;

    QImage img = applyAdjustments(slices[currentIndex]);

    scene->clear();
    scene->addPixmap(QPixmap::fromImage(img));

    view->resetTransform(); // reset zoom
    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    infoLabel->setText(QString("%1 / %2")
                       .arg(currentIndex + 1)
                       .arg(slices.size()));
}

void MainWindow::nextSlice()
{
    if(currentIndex < slices.size() - 1)
    {
        slider->setValue(currentIndex + 1);
    }
}

void MainWindow::prevSlice()
{
    if(currentIndex > 0)
    {
        slider->setValue(currentIndex - 1);
    }
}

QImage MainWindow::applyAdjustments(const QImage &img)
{
    if(img.isNull()) return img;

    QImage result = img;

    // Middle values
    int brightness = brightnessSlider->value();        // 0 = neutral
    double contrast = contrastSlider->value() / 100.0; // 100 = 1.0

    for(int y = 0; y < result.height(); y++)
    {
        uchar *line = result.scanLine(y);

        for(int x = 0; x < result.width(); x++)
        {
            int val = line[x];

            // Core formula (centered at 128)
            val = (val - 128) * contrast + 128 + brightness;

            // Clamp to valid range
            val = qBound(0, val, 255);

            line[x] = val;
        }
    }

    return result;
}
