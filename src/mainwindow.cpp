#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    const double vertStep = 15.0; //шаг по вертикали в пикселях
    const double horStep = 30.0;  //шаг по горизонтали
    const double sX = 150.0; // начальная координата X системы координат в пикселях
    const double sY = 50.0 * vertStep + 70.0; // начальная координата Y системы координат в пикселях
    const double fX = sX + horStep * 50.0; // конечная координата X системы координат в пикселях
    const double fY = sY - 50.0 * vertStep; // конечная координата X системы координат в пикселях

    QPainter painter(this);
    QPen pen;

    pen.setColor(Qt::gray);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    // серые прерывистые линии

    for (int i = 1; i < 11; ++i){
        painter.drawLine(sX + i * horStep * 5, sY, sX + i * horStep * 5, fY);
    }

    for (int i = 1; i < 11; ++i){
        painter.drawLine(sX, sY - i * vertStep * 5 , fX , sY - i * vertStep * 5);
    }

    // черные линии

    pen.setColor(QColor(30,30,30));
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    painter.drawLine(sX, sY, fX + 50, sY);           // горизотальная основная
    painter.drawLine(sX, sY, sX, fY - 25);        // вертикальная основная

    // шкала на горизонтальной линии
    for (int i = 1; i < 51; ++i){
        painter.drawLine(sX + i * horStep, sY, sX + i * horStep, sY - 4.0);
    }

    for (int i = 1; i < 11; ++i){
        painter.drawLine(sX + i * horStep * 5, sY, sX + i * horStep * 5, sY - 8.0);
    }

    // шкала на вертикальной линии
    for (int i = 1; i < 51; ++i){
        painter.drawLine(sX, sY - i * vertStep , sX + 4.0, sY - i * vertStep);
    }

    for (int i = 1; i < 11; ++i){
        painter.drawLine(sX, sY - i * vertStep * 5.0 , sX + 8.0, sY - i * vertStep * 5.0);
    }

    //рисуем только если не пустой масив данных
    if (!samples.isEmpty()){

        QVector<QPointF> valuePoints(samples.size());
        double min_x = samples[0].first;
        double max_x = samples[0].first;
        double min_y = samples[0].second;
        double max_y = samples[0].second;

        for (int i = 0; i < samples.size(); ++i){
            min_x = qMin(samples[i].first, min_x);
            max_x = qMax(samples[i].first, max_x);
            min_y = qMin(samples[i].second, min_y);
            max_y = qMax(samples[i].second, max_y);
        }
        double dist_x_value = max_x - min_x;
        double dist_y_value = max_y - min_y;

        double dist_x_screen = fX - sX;
        double dist_y_screen = sY - fY;

        // переводим массив точек в координаты экрана в пикселях

        for (int i = 0; i < samples.size(); ++i){
            valuePoints[i] = QPointF( sX + (samples[i].first - min_x) * dist_x_screen/ dist_x_value ,
                                      sY - (samples[i].second - min_y) * dist_y_screen/ dist_y_value);
        }

        //style “dots connected with lines”
        // линии
        pen.setColor(QColor(Qt::blue));
        painter.setPen(pen);
        painter.drawPolyline(valuePoints);

        // точки
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(Qt::blue));

        for (int i = 0; i < samples.size(); ++i){
            painter.drawEllipse(valuePoints[i], 3, 3);
        }

        // надписи на шкалах
        pen.setColor(QColor(30,30,30));
        painter.setPen(pen);

        QFont verdanaFont("Verdana", 10);
        painter.setFont(verdanaFont);

        for (int i = 0; i < 11; ++i){
            double value_y = i * dist_y_value / 10.0 + min_y;
            QString s = QString::number(value_y, 'e', 10);
            painter.drawText(QPoint(5, sY - i * vertStep * 5.0 + 3) , s);
        }

        painter.rotate(-90.0);

        for (int i = 0; i < 11; ++i){
            double value_x = i * dist_x_value / 10.0 + min_x;
            QString s1 = QString::number(value_x, 'e', 10);
            painter.drawText(QPoint(-955, sX + 5 + i * horStep * 5) , s1);
        }

        painter.rotate(90.0);
    }
}

void MainWindow::readFile(QString filename)
{
    QFile file(filename);
    if (!file.exists())
    {
        QErrorMessage fileError;
        fileError.showMessage("File not found");
        fileError.exec();
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        QErrorMessage fileOpenError;
        fileOpenError.showMessage(file.errorString());
        fileOpenError.exec();
        return;
    }

    samples.clear();

    QTextStream stream(&file);
    QErrorMessage sizeError;
    bool validate1;
    bool validate2;
    while(!stream.atEnd()){
        QString line = stream.readLine();
        if (!line.isEmpty()){
            line.trimmed();
            if (line.at(0) == '#'){
                continue;
            }

            // Measurement data: <timestamp> <value>
            QStringList sample = line.split(" ");
            if (sample.size() < 2){
                sizeError.showMessage("Wrong file format");
                samples.clear();
                sizeError.exec();
                return;
            }


            double times = sample.at(0).toDouble(&validate1);
            double values = sample.at(1).toDouble(&validate2);
            if(!validate1 || !validate2){
                sizeError.showMessage("Wrong file format");
                samples.clear();
                sizeError.exec();
                return;
            }
            samples.emplaceBack(times, values);
        }
    }
    file.close();
}

void MainWindow::on_pushButton_clicked()
{
    QString str;
    str = QFileDialog::getOpenFileName(this, "Выбор файла", "C:\\", "*.ssd;; *.rsd;; *.txt");

    readFile(str);

}

