//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//                                .--,       .--,
//                               ( (  \.---./  ) )
//                                '.__/o   o\__.'
//                                   {=  ^  =}
//                                    >  -  <
//     ___________________________.""`-------`"".____________________________
//    /                                                                      \
//    \     This file is part of Banana - a general programming framework    /
//    /                    Created: 2017 by Nghia Truong                     \
//    \                      <nghiatruong.vn@gmail.com>                      /
//    /                      https://ttnghia.github.io                       \
//    \                        All rights reserved.                          /
//    /                                                                      \
//    \______________________________________________________________________/
//                                  ___)( )(___
//                                 (((__) (__)))
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#include "MainWindow.h"

#include <QMouseEvent>
#include <Banana/Utils/Formatters.h>
#include <Banana/System/MemoryUsage.h>
#include <QtAppHelpers/QtAppUtils.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
MainWindow::MainWindow(QWidget* parent) : OpenGLMainWindow(parent)
{
    m_RenderWidget = new RenderWidget(this, m_Sampler->getVizData(), m_Sampler->getParticleData());
    ////////////////////////////////////////////////////////////////////////////////
    setupOpenglWidget(m_RenderWidget);
    setupRenderWidgets();
    setupStatusBar();
    connectWidgets();
    ////////////////////////////////////////////////////////////////////////////////
    setArthurStyle();
    setFocusPolicy(Qt::StrongFocus);
    showFPS(false);
    setWindowTitle("Particle Sampler");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::showEvent(QShowEvent* ev)
{
    QMainWindow::showEvent(ev);
    static bool showed = false;

    if(!showed) {
        showed = true;
        Q_ASSERT(m_Sampler != nullptr);
        updateStatusMemoryUsage();
        finishIteration(0, 0);

        m_Controller->m_cbScene->getComboBox()->setCurrentIndex(1);
        if(m_Controller->m_cbScene->getComboBox()->count() == 2) {} else {
            //            m_Controller->m_cbSimulationScene->setCurrentIndex(QtAppUtils::getDefaultSceneID());
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool MainWindow::processKeyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
        case Qt::Key_Space:
            m_Controller->m_btnStartStopRelaxation->click();
            return true;

        case Qt::Key_X:
            m_Controller->m_btnClipViewPlane->click();
            return true;

        default:
            return OpenGLMainWindow::processKeyPressEvent(event);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::updateWindowTitle(const QString& filePath)
{
    setWindowTitle(QString("Particle Simulation: ") + filePath);
}

void MainWindow::updateStatusRelaxation(const QString& status)
{
    m_lblStatusRelaxationInfo->setText(status);
}

void MainWindow::updateStatusMemoryUsage()
{
    m_lblStatusMemoryUsage->setText(QString("Memory usage: %1 (MBs)").arg(QString::fromStdString(Formatters::toString(getCurrentRSS() / 1048576.0))));
}

void MainWindow::updateStatusNumParticles(unsigned int numParticles)
{
    m_lblStatusNumParticles->setText(QString("Num. particles: %1").arg(QString::fromStdString(Formatters::toString(numParticles))));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::changeScene(bool bReload)
{
    QString sceneFile = m_Controller->m_cbScene->currentText();
    if(sceneFile == "None") {
        return;
    }
    m_Controller->m_btnReloadScene->setDisabled(true);
    m_Controller->m_cbScene->setDisabled(true);
    ////////////////////////////////////////////////////////////////////////////////
    static std::future<void> fut;
    if(fut.valid()) {
        fut.wait();
    }
    fut = std::async(std::launch::async, [&, sceneFile, bReload]
                     {
                         m_Sampler->reloadVizData(bReload ? m_Controller->m_chkReloadVizData->isChecked() : true);
                         m_Sampler->changeScene(sceneFile);
                         m_FrameNumber = 0;
                         updateWindowTitle(QtAppUtils::getDefaultPath("Scenes") + "/" + sceneFile);
                         updateStatusRelaxation("Ready");
                         m_Controller->m_btnReloadScene->setEnabled(true);
                         m_Controller->m_cbScene->setEnabled(true);
                     });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::startStopRelaxation()
{
    if(m_Controller->m_cbScene->currentText() == "None") {
        return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    static std::future<void> fut;
    if(fut.valid()) {
        fut.wait();
    }
    bool isRunning = m_Sampler->isRunning();
    if(!isRunning) {
        fut = std::async(std::launch::async, [&]
                         {
                             m_Controller->updateRelaxParams();
                             m_Sampler->startRelaxation();
                             m_Controller->m_cbScene->setDisabled(true);
                             m_Controller->m_btnReloadScene->setDisabled(true);
                             updateStatusRelaxation("Running relaxation...");
                         });
    } else {
        m_Sampler->stop();
        m_Controller->m_cbScene->setDisabled(false);
        m_Controller->m_btnReloadScene->setDisabled(false);
        updateStatusRelaxation("Stopped");
    }
    m_Controller->m_btnStartStopRelaxation->setText(!isRunning ? QString("Stop") : QString("Resume"));
    m_BusyBar->setBusy(!isRunning);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishIteration(unsigned int iter, float minDistanceRatio)
{
    if(minDistanceRatio > 0) {
        m_lblStatusIteration->setText(QString("Iteration : #%1 | Min. distance ratio: %2").arg(QString::fromStdString(Formatters::toString(iter))).arg(minDistanceRatio));
    } else {
        m_lblStatusIteration->setText(QString("Iteration : #%1").arg(QString::fromStdString(Formatters::toString(iter))));
    }
    ++m_FrameNumber;
    if(m_bExportImg) {
        m_RenderWidget->exportScreenToImage(m_FrameNumber);
        m_Sampler->finishImgExport();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::finishRelaxation(bool bSuccess)
{
    m_Controller->m_btnStartStopRelaxation->setText(QString("Start"));
    if(bSuccess) {
        updateStatusRelaxation("Relaxation finished successfully.");
    } else {
        updateStatusRelaxation("Relaxation failed.");
    }
    m_Controller->m_cbScene->setDisabled(false);
    m_Controller->m_btnReloadScene->setDisabled(false);
    m_BusyBar->reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::pauseRelaxation()
{
    m_Controller->m_btnStartStopRelaxation->setText(QString("Resume"));
    updateStatusRelaxation("Relaxation paused");
    m_Controller->m_cbScene->setDisabled(false);
    m_Controller->m_btnReloadScene->setDisabled(false);
    m_BusyBar->reset();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupRenderWidgets()
{
    m_Controller = new Controller(m_RenderWidget, m_Sampler, this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_GLWidget);
    mainLayout->addWidget(m_Controller);

    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::setupStatusBar()
{
    m_BusyBar = new BusyBar(this, BusyBar::Cycle, 10);
    statusBar()->addPermanentWidget(m_BusyBar);

    m_lblStatusRelaxationInfo = new QLabel(this);
    m_lblStatusRelaxationInfo->setMargin(5);
    m_lblStatusRelaxationInfo->setText("Ready (Press Space to Start/Stop)");
    statusBar()->addPermanentWidget(m_lblStatusRelaxationInfo, 1);

    m_lblStatusIteration = new QLabel(this);
    m_lblStatusIteration->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusIteration, 2);

    m_lblStatusNumParticles = new QLabel(this);
    m_lblStatusNumParticles->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusNumParticles, 1);

    m_lblStatusMemoryUsage = new QLabel(this);
    m_lblStatusMemoryUsage->setMargin(5);
    statusBar()->addPermanentWidget(m_lblStatusMemoryUsage, 1);

    QTimer* memTimer = new QTimer(this);
    connect(memTimer, &QTimer::timeout, [&] { updateStatusMemoryUsage(); });
    memTimer->start(5000);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void MainWindow::connectWidgets()
{
    connect(m_ClipPlaneEditor,                      &ClipPlaneEditor::clipPlaneChanged,    m_RenderWidget, &RenderWidget::setClipPlane);
    connect(m_Controller->m_btnEditClipPlane,       &QPushButton::clicked,                 [&] { m_ClipPlaneEditor->show(); });
    ////////////////////////////////////////////////////////////////////////////////
    // simulation
    connect(m_Controller->m_btnReloadScene,         &QPushButton::clicked,                 [&] { changeScene(true); });
    connect(m_Controller->m_cbScene,                &EnhancedComboBox::currentTextChanged, [&] { changeScene(false); });
    connect(m_Controller->m_btnStartStopRelaxation, &QPushButton::clicked,                 [&] { startStopRelaxation(); });

    connect(m_Controller->m_chkEnableOutput,        &QCheckBox::toggled,                   [&](bool checked)
            {
                m_bExportImg = checked;
                m_Sampler->enableExportImg(checked);
            });
    connect(m_Sampler, &ParticleSampler::capturePathChanged, m_Controller->m_OutputPath,  &BrowsePathWidget::setPath);
    connect(m_Sampler, &ParticleSampler::lightsChanged,      m_Controller->m_LightEditor, &PointLightEditor::changeLights);
    ////////////////////////////////////////////////////////////////////////////////
    // sampling status
    connect(m_Sampler, &ParticleSampler::iterationFinished,  [&](unsigned int iter, float minDistanceRatio)
            {
                QMetaObject::invokeMethod(this, "finishIteration", Qt::QueuedConnection, Q_ARG(unsigned int, iter), Q_ARG(float, minDistanceRatio));
            });
    connect(m_Sampler, &ParticleSampler::relaxationFinished, [&](bool bSuccess)
            {
                QMetaObject::invokeMethod(this, "finishRelaxation", Qt::QueuedConnection, Q_ARG(bool, bSuccess));
            });
    connect(m_Sampler, &ParticleSampler::relaxationPaused,   [&] { QMetaObject::invokeMethod(this, "pauseRelaxation", Qt::QueuedConnection); });
    connect(m_Sampler, &ParticleSampler::numParticleChanged, this,           &MainWindow::updateStatusNumParticles);
    connect(m_Sampler, &ParticleSampler::dimensionChanged,   m_RenderWidget, &RenderWidget::updateSolverDimension);
    connect(m_Sampler, &ParticleSampler::domainChanged,      m_RenderWidget, &RenderWidget::setBox);
    connect(m_Sampler, &ParticleSampler::cameraChanged,      m_RenderWidget, &RenderWidget::updateCamera);
    connect(m_Sampler, &ParticleSampler::vizDataChanged,     m_RenderWidget, &RenderWidget::updateVizData);
}
