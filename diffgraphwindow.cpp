#include "diffgraphwindow.h"
#include "ui_diffgraphwindow.h"

diffGraphWindow::diffGraphWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::diffGraphWindow)
{
    ui->setupUi(this);
    ui->qwtPlot->setCanvasBackground(Qt::black);
    zoom_x = new QwtPlotMagnifier( ui->qwtPlot->canvas());
    zoom_y = new QwtPlotMagnifier( ui->qwtPlot->canvas() );
    grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::white,1,Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray,1,Qt::DotLine));
    grid->attach(ui->qwtPlot);
    zoom_x->setMouseButton(Qt::MidButton);
    zoom_x->setWheelModifiers(Qt::ShiftModifier);
    zoom_x->setAxisEnabled(QwtPlot::xBottom, true);
    zoom_x->setAxisEnabled(QwtPlot::yRight ,false);
    zoom_x->setAxisEnabled(QwtPlot::yLeft ,false);
    zoom_y->setMouseButton(Qt::MidButton);
    zoom_y->setWheelModifiers(Qt::ControlModifier);
    zoom_y->setAxisEnabled(QwtPlot::xBottom,false);
    zoom_y->setAxisEnabled(QwtPlot::yRight,true);
    zoom_y->setAxisEnabled(QwtPlot::yLeft,true);
    QwtPlotPanner *d_panner = new QwtPlotPanner(ui->qwtPlot->canvas() );
    d_panner->setMouseButton( Qt::RightButton );
    curveDiff = new QwtPlotCurve("Diff");
    curveMems = new QwtPlotCurve("Mems");
    curvePiezo = new QwtPlotCurve("Piezo");

    curveDiff->setPen(QPen(Qt::blue,2,Qt::SolidLine));
    curveMems->setPen(QPen(Qt::red,2,Qt::SolidLine));
    curvePiezo->setPen(QPen(Qt::green,2,Qt::SolidLine));

    curveDiff->attach(ui->qwtPlot);
    curveMems->attach(ui->qwtPlot);
    curvePiezo->attach(ui->qwtPlot);
    ui->qwtPlot->setAxisAutoScale(QwtPlot::xBottom,false);
    ui->qwtPlot->setAxisAutoScale(QwtPlot::yLeft,true);
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft,-10,10,1 );
    ui->qwtPlot->setAxisScale(QwtPlot::xBottom,0,30,5 );

    samplingFreq=250;
    callCounter=0;
    angle1=0,angle0=0;


}


void diffGraphWindow::setMemsAngleSample(qreal angle)
{

}
void diffGraphWindow::setPiezoAngleSample(quint8 channel, qreal angle)
{
    quint16 pixel1,pixel2,pixels,maxPoints;
    qreal diff;
    qreal T;
    qreal n;
    qreal maxX = ui->qwtPlot->axisScaleDiv(QwtPlot::xBottom).interval().maxValue();
    pixel1=ui->qwtPlot->canvasMap(QwtPlot::xBottom).transform(0);
    pixel2=ui->qwtPlot->canvasMap(QwtPlot::xBottom).transform(maxX);
    //общее кол-во пикселей = poin2-point1
    //период дескритизации = T, значит , допустим для 30 сек = 10/T семплов, допустим 7500 семплов
    //
    T=1/(qreal)samplingFreq;
    maxPoints = maxX/T; //7500
    pixels= pixel2-pixel1;
    callCounter++;
    n=T*maxPoints/pixels;
    if (qIsNaN(angle))
    {
        if (piezo.isEmpty())
        {
            angle=0;
        }
        else
        {
            angle=piezo.last();
        }
    }
    if (channel==0)
    {
       angle0=angle;
       if (angle1==0)
       {
           return;
       }
       diff=angle1-angle0;
       angle0=0;
    }

    if (channel==1)
    {
       angle1=angle;

       if (angle0==0)
       {
           return;
       }
       diff=angle1-angle0;
       angle1=0;
    }



    if (pixels < maxPoints) //pixel2-pixel =500;
    {
        if (callCounter%(maxPoints/pixels))
        {
            qreal last=0;
            piezo.append(diff);
            if (!t.isEmpty()) last = t.last();
            t.append(n + last);

        }
    }
    else
    {
       qreal last=0;
       if (!t.isEmpty()) last = t.last();
       t.append(T + last);
       piezo.append(diff);
    }

    if (piezo.size()>pixels)
    {
        piezo.pop_front();
        t.pop_front();
        ui->qwtPlot->setAxisScale(QwtPlot::xBottom,t.at(0),t.last(),2 );
    }

    curvePiezo->setSamples(t,piezo);
    ui->qwtPlot->replot();

}

diffGraphWindow::~diffGraphWindow()
{
    delete ui;
}
