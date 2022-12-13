#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectTexParam1();
    void connectTexParam2();
    void connectNear();
    void connectFar();
    void connectBezier();
    void connectFlow();
    void connectFlowSpeed();
    void connectParallax();
    void connectWorley();
    void connectUploadFile();

    Realtime *realtime;
    QCheckBox *bezier;
    QCheckBox *parallax;
    QCheckBox *flow;
    QPushButton *uploadFile;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *texp1Slider;
    QSlider *texp2Slider;
    QSpinBox *texp1Box;
    QDoubleSpinBox *texp2Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;
    QSlider *speedSlider;
    QSpinBox *speedBox;

    QCheckBox *worley;

private slots:
    void onBezier();
    void onParallax();
    void onFlow();
    void onUploadFile();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onValChangeTexP1(int newValue);
    void onValChangeTexP2(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);
    void onValChangeSpeedSlider(int newValue);
    void onWorley();
};
