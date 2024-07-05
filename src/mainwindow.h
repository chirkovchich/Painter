#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QErrorMessage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void readFile(QString filename);
private:
    Ui::MainWindow *ui;
    QVector<QPair<double, double>> samples;
    QVector<QPair<double, double>> samples1;
protected:
    void paintEvent(QPaintEvent *) override ;
private slots:
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
