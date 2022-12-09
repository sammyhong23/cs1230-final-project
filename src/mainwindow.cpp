#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
    QLabel *parallax_label = new QLabel(); // Filters label
    parallax_label->setText("Parallax Mapping");
    parallax_label->setFont(font);
    QLabel *flow_label = new QLabel(); // Extra Credit label
    flow_label->setText("Flow Mapping");
    flow_label->setFont(font);
    QLabel *texgen_label = new QLabel(); // Extra Credit label
    texgen_label->setText("Texture Generation");
    texgen_label->setFont(font);
    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");
    QLabel *texparam1_label = new QLabel(); // Parameter 1 label
    texparam1_label->setText("Parameter 1:");
    QLabel *texparam2_label = new QLabel(); // Parameter 2 label
    texparam2_label->setText("Parameter 2:");



    // Create checkbox for per-pixel filter
    bezier = new QCheckBox();
    bezier->setText(QStringLiteral("Follow Bezier Curve"));
    bezier->setChecked(false);

    // Create checkbox for kernel-based filter
    parallax = new QCheckBox();
    parallax->setText(QStringLiteral("Parallax Mapping"));
    parallax->setChecked(false);

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setText(QStringLiteral("Upload Scene File"));

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    QGroupBox *texp1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *texl1 = new QHBoxLayout();
    QGroupBox *texp2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *texl2 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(25);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(25);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(25);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(25);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    // Create slider controls to control parameters
    texp1Slider = new QSlider(Qt::Orientation::Horizontal); // Texture Parameter 1 slider
    texp1Slider->setTickInterval(1);
    texp1Slider->setMinimum(1);
    texp1Slider->setMaximum(25);
    texp1Slider->setValue(1);

    texp1Box = new QSpinBox();
    texp1Box->setMinimum(1);
    texp1Box->setMaximum(25);
    texp1Box->setSingleStep(1);
    texp1Box->setValue(1);

    texp2Slider = new QSlider(Qt::Orientation::Horizontal); // Texture Parameter 2 slider
    texp2Slider->setTickInterval(1);
    texp2Slider->setMinimum(1);
    texp2Slider->setMaximum(25);
    texp2Slider->setValue(1);

    texp2Box = new QSpinBox();
    texp2Box->setMinimum(1);
    texp2Box->setMaximum(25);
    texp2Box->setSingleStep(1);
    texp2Box->setValue(1);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Adds the slider and number box to the parameter layouts
    texl1->addWidget(texp1Slider);
    texl1->addWidget(texp1Box);
    texp1Layout->setLayout(texl1);

    texl2->addWidget(texp2Slider);
    texl2->addWidget(texp2Box);
    texp2Layout->setLayout(texl2);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    flow = new QCheckBox();
    flow->setText(QStringLiteral("Flow Mapping"));
    flow->setChecked(false);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(bezier);
    vLayout->addWidget(parallax_label);
    vLayout->addWidget(parallax);

    vLayout->addWidget(flow_label);
    vLayout->addWidget(flow);

    vLayout->addWidget(texgen_label);
    vLayout->addWidget(texparam1_label);
    vLayout->addWidget(texp1Layout);
    vLayout->addWidget(texparam2_label);
    vLayout->addWidget(texp2Layout);

    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeP1(5);
    onValChangeP2(5);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(10.f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectBezier();
    connectParallax();
    connectFlow();
    connectUploadFile();
    connectTexParam1();
    connectTexParam2();
    connectParam1();
    connectParam2();
    connectNear();
    connectFar();
}

void MainWindow::connectBezier() {
    connect(bezier, &QCheckBox::clicked, this, &MainWindow::onBezier);
}

void MainWindow::connectParallax() {
    connect(parallax, &QCheckBox::clicked, this, &MainWindow::onParallax);
}

void MainWindow::connectFlow() {
    connect(flow, &QCheckBox::clicked, this, &MainWindow::onFlow);
}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectTexParam1() {
    connect(texp1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeTexP1);
    connect(texp1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTexP1);
}

void MainWindow::connectTexParam2() {
    connect(texp2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeTexP2);
    connect(texp2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeTexP2);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::onBezier() {
    settings.bezier = !settings.bezier;
    realtime->settingsChanged();
}

void MainWindow::onParallax() {
    settings.parallax = !settings.parallax;
    realtime->settingsChanged();
}

void MainWindow::onFlow() {
    settings.flow = !settings.flow;
    realtime->settingsChanged();
}


void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"), QDir::homePath(), tr("Scene Files (*.xml)"));
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->sceneChanged();
}


void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTexP1(int newValue) {
    texp1Slider->setValue(newValue);
    texp1Box->setValue(newValue);
    settings.texGenParam1 = texp1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeTexP2(int newValue) {
    texp2Slider->setValue(newValue);
    texp2Box->setValue(newValue);
    settings.texGenParam2 = texp2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}
