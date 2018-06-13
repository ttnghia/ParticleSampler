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

#include <QtAppHelpers/QtAppUtils.h>
#include "Common.h"
#include "Controller.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::updateRelaxParams()
{
    const auto& relaxParams = m_ParticleSampler->getRelaxParams();
    for(const auto& params: relaxParams) {
        params->maxIters           = m_cbMaxIterations->currentTextAsUInt();
        params->checkFrequency     = m_cbCheckFrequency->currentTextAsUInt();
        params->intersectThreshold = m_cbIntersectionThreshold->currentTextAsFloat();
        params->initialJitterRatio = m_cbInitialJitter->currentTextAsFloat();

        params->particleRadius        = m_ParticleSampler->getParticleData()->particleRadius;
        params->CFLFactor             = m_cbSPHCFLFactor->currentTextAsFloat();
        params->pressureStiffness     = m_cbSPHPressureStiffness->currentTextAsFloat();
        params->viscosity             = m_cbSPHViscosity->currentTextAsFloat();
        params->overlapThresholdRatio = m_cbSPHOverlapThreshold->currentTextAsFloat();
        params->nearKernelRadiusRatio = m_cbSPHNearKernelRadiusRatio->currentTextAsFloat();
        params->nearPressureStiffness = m_cbSPHNearPressureStiffness->currentTextAsFloat();
        params->boundaryRestitution   = m_cbSPHBoundaryRestitution->currentTextAsFloat();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setDefaultParams()
{
    m_cbMaxIterations->setCurrentIndex(0);
    m_cbCheckFrequency->setCurrentIndex(2);
    m_cbIntersectionThreshold->setCurrentIndex(2);
    m_cbInitialJitter->setCurrentIndex(1);

    m_cbSPHCFLFactor->setCurrentIndex(4);
    m_cbSPHPressureStiffness->setCurrentIndex(6);
    m_cbSPHViscosity->setCurrentIndex(5);
    m_cbSPHOverlapThreshold->setCurrentIndex(2);
    m_cbSPHNearKernelRadiusRatio->setCurrentIndex(3);
    m_cbSPHNearPressureStiffness->setCurrentIndex(7);
    m_cbSPHBoundaryRestitution->setCurrentIndex(5);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupGUI()
{
    m_MainTab->setTabText(0, "Render Controllers");
    m_RelaxationControllers->setLayout(m_LayoutRelaxationControllers);
    m_MainTab->insertTab(0, m_RelaxationControllers, "Relaxation");
    m_MainTab->setCurrentIndex(0);
    ////////////////////////////////////////////////////////////////////////////////
    setupMaterialControllers();
    setupColorModeControllers();
    setupCaptureControllers();
    setupSceneControllers();
    setupSamplingParametersControllers();
    setupButtons();
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutMainControllers->addStretch();
    m_LayoutRelaxationControllers->addStretch();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::connectWidgets()
{
    connect(m_smBackgroundMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), [&](int backgroundMode)
            {
                m_chkRenderBox->setChecked(backgroundMode == OpenGLWidget::BackgroundMode::SkyBox || backgroundMode == OpenGLWidget::BackgroundMode::Color);
            });

    ////////////////////////////////////////////////////////////////////////////////
    // materials and particle color mode
    connect(m_smParticleColorMode, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), m_RenderWidget, &RenderWidget::setParticleColorMode);
    connect( m_msParticleMaterial, &MaterialSelector::materialChanged,                                m_RenderWidget, &RenderWidget::setParticleMaterial);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene
    connect(m_OutputPath, &BrowsePathWidget::pathChanged, m_RenderWidget, &RenderWidget::setCapturePath);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // relaxation
    connect(m_btnResetParams, &QPushButton::clicked, this, &Controller::setDefaultParams);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    connect(  m_btnResetCamera, &QPushButton::clicked, m_RenderWidget, &RenderWidget::resetCameraPosition);
    connect(m_btnClipViewPlane, &QPushButton::clicked, m_RenderWidget, &RenderWidget::enableClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupMaterialControllers()
{
    m_msParticleMaterial = new MaterialSelector;
    m_msParticleMaterial->setCustomMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_msParticleMaterial->setDefaultCustomMaterial(true);
    QGridLayout* layoutMaterial = new QGridLayout;
    layoutMaterial->addLayout(m_msParticleMaterial->getLayout(), 0, 0, 1, 2);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grMaterial = new QGroupBox("Material");
    grMaterial->setLayout(layoutMaterial);
    m_LayoutMainControllers->addWidget(grMaterial);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupColorModeControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    QRadioButton* rdbColorRandom  = new QRadioButton("Random");
    QRadioButton* rdbColorRamp    = new QRadioButton("Ramp");
    QRadioButton* rdbColorUniform = new QRadioButton("Uniform Color");
    QRadioButton* rdbColorObjIdx  = new QRadioButton("Object Index");
    rdbColorRamp->setChecked(true);
    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutColorMode = new QGridLayout;
    layoutColorMode->addWidget( rdbColorRandom, 0, 0, 1, 1);
    layoutColorMode->addWidget(   rdbColorRamp, 0, 1, 1, 1);
    layoutColorMode->addWidget(rdbColorUniform, 1, 0, 1, 1);
    layoutColorMode->addWidget( rdbColorObjIdx, 1, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_smParticleColorMode = new QSignalMapper(this);
    connect( rdbColorRandom, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(   rdbColorRamp, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect(rdbColorUniform, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));
    connect( rdbColorObjIdx, SIGNAL(clicked()), m_smParticleColorMode, SLOT(map()));

    m_smParticleColorMode->setMapping( rdbColorRandom, static_cast<int>(ParticleColorMode::Random));
    m_smParticleColorMode->setMapping(   rdbColorRamp, static_cast<int>(ParticleColorMode::Ramp));
    m_smParticleColorMode->setMapping(rdbColorUniform, static_cast<int>(ParticleColorMode::UniformMaterial));
    m_smParticleColorMode->setMapping( rdbColorObjIdx, static_cast<int>(ParticleColorMode::ObjectIndex));
    ////////////////////////////////////////////////////////////////////////////////
    QVBoxLayout* layoutColorCtrls = new QVBoxLayout;
    layoutColorCtrls->addLayout(layoutColorMode);
    ////////////////////////////////////////////////////////////////////////////////
    QGroupBox* grColorMode = new QGroupBox;
    grColorMode->setTitle("Particle Color");
    grColorMode->setLayout(layoutColorCtrls);
    m_LayoutMainControllers->addWidget(grColorMode);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupCaptureControllers()
{
    m_OutputPath = new BrowsePathWidget("Browse");
    m_OutputPath->setPath(QtAppUtils::getDefaultCapturePath());
    m_chkEnableOutput = new QCheckBox("Export to Images");
    QVBoxLayout* layoutOutput = new QVBoxLayout;
    layoutOutput->addWidget(m_chkEnableOutput);
    layoutOutput->addLayout(m_OutputPath->getLayout());
    QGroupBox* grpOutput = new QGroupBox;
    grpOutput->setTitle("Screenshot");
    grpOutput->setLayout(layoutOutput);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutMainControllers->addWidget(grpOutput);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSceneControllers()
{
    m_cbScene = new EnhancedComboBox;
    m_cbScene->addItem(QString("None"));
    m_cbScene->addItems(QtAppUtils::getFiles(QtAppUtils::getDefaultPath("Scenes")));
    m_btnReloadScene = new QPushButton(" Reload ");
    ////////////////////////////////////////////////////////////////////////////////
    m_chkReloadVizData = new QCheckBox("Reload vizualization data");
    ////////////////////////////////////////////////////////////////////////////////
    QHBoxLayout* layoutReloadScene = new QHBoxLayout;
    layoutReloadScene->addLayout(m_cbScene->getLayout(), 10);
    layoutReloadScene->addStretch(1);
    layoutReloadScene->addWidget(m_btnReloadScene, 10);
    QVBoxLayout* layoutScene = new QVBoxLayout;
    layoutScene->addLayout(layoutReloadScene);
    layoutScene->addLayout(       QtAppUtils::getLayoutSeparator(5));
    layoutScene->addWidget(m_chkReloadVizData);
    QGroupBox* grScene = new QGroupBox;
    grScene->setTitle("Scene");
    grScene->setLayout(layoutScene);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRelaxationControllers->addWidget(grScene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupSamplingParametersControllers()
{
    ////////////////////////////////////////////////////////////////////////////////
    m_cbMaxIterations         = new EnhancedComboBox;
    m_cbCheckFrequency        = new EnhancedComboBox;
    m_cbIntersectionThreshold = new EnhancedComboBox;
    m_cbInitialJitter         = new EnhancedComboBox;

    m_cbMaxIterations->addItems({ "100", "500", "1000", "2000", "5000", "10000" });
    m_cbCheckFrequency->addItems({ "1", "5", "10", "20", "30", "40", "50", "100" });
    m_cbIntersectionThreshold->addItems({ "1.5", "1.6", "1.7", "1.8", "1.9", "2.0" });
    m_cbInitialJitter->addItems({ "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0" });

    QGridLayout* layoutStopCriteria = new QGridLayout;
    int          row                = 0;
    layoutStopCriteria->addWidget(new QLabel("Max. iters:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbMaxIterations->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(new QLabel("Check frequency:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbCheckFrequency->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(new QLabel("Intersection threshold:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbIntersectionThreshold->getLayout(), row++, 1, 1, 2);
    layoutStopCriteria->addWidget(new QLabel("Initial jitter:    "), row, 0, 1, 1);
    layoutStopCriteria->addLayout(m_cbInitialJitter->getLayout(), row++, 1, 1, 2);

    QGroupBox* grStopCriteria = new QGroupBox;
    grStopCriteria->setTitle("Global Parameters");
    grStopCriteria->setLayout(layoutStopCriteria);
    m_LayoutRelaxationControllers->addWidget(grStopCriteria);
    ////////////////////////////////////////////////////////////////////////////////
    m_cbSPHCFLFactor             = new EnhancedComboBox;
    m_cbSPHPressureStiffness     = new EnhancedComboBox;
    m_cbSPHViscosity             = new EnhancedComboBox;
    m_cbSPHOverlapThreshold      = new EnhancedComboBox;
    m_cbSPHNearKernelRadiusRatio = new EnhancedComboBox;
    m_cbSPHNearPressureStiffness = new EnhancedComboBox;
    m_cbSPHBoundaryRestitution   = new EnhancedComboBox;

    m_cbSPHCFLFactor->addItems({ "0.001", "0.005", "0.01", "0.05", "0.1", "0.5", "1.0" });
    m_cbSPHViscosity->addItems({ "0.001", "0.005", "0.01", "0.05", "0.1", "0.5", "1.0" });
    m_cbSPHOverlapThreshold->addItems({ "0.001", "0.01", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0" });
    m_cbSPHNearKernelRadiusRatio->addItems({ "1.5", "1.75", "2.0", "2.1", "2.2", "2.3", "2.4", "2.5" });
    m_cbSPHBoundaryRestitution->addItems({ "0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0" });

    m_cbSPHPressureStiffness->addItem("0");
    m_cbSPHNearPressureStiffness->addItem("0");
    int K = 10;
    for(int i = 1; i <= 4; ++i) {
        m_cbSPHPressureStiffness->addItem(QString("%1").arg(K));
        m_cbSPHPressureStiffness->addItem(QString("%1").arg(K * 5));
        m_cbSPHNearPressureStiffness->addItem(QString("%1").arg(K));
        m_cbSPHNearPressureStiffness->addItem(QString("%1").arg(K * 5));
        K *= 10;
    }

    QGridLayout* layoutSPHParameters = new QGridLayout;
    row = 0;
    layoutSPHParameters->addWidget(new QLabel("CFL factor:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHCFLFactor->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Pressure stiffness:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHPressureStiffness->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Viscosity:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHViscosity->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Overlap threshold:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHOverlapThreshold->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Near kernel radius ratio:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHNearKernelRadiusRatio->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Near pressure stiffness:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHNearPressureStiffness->getLayout(), row++, 2, 1, 1);
    layoutSPHParameters->addWidget(new QLabel("Boundary restitution:    "), row, 0, 1, 2);
    layoutSPHParameters->addLayout(m_cbSPHBoundaryRestitution->getLayout(), row++, 2, 1, 1);

    QGroupBox* grSPHParameters = new QGroupBox;
    grSPHParameters->setTitle("SPH Solver Parameters");
    grSPHParameters->setLayout(layoutSPHParameters);
    ////////////////////////////////////////////////////////////////////////////////
    m_LayoutRelaxationControllers->addWidget(grSPHParameters);
    setDefaultParams();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void Controller::setupButtons()
{
    m_btnResetParams = new QPushButton("Reset Parameters");
    m_LayoutRelaxationControllers->addSpacing(10);
    m_LayoutRelaxationControllers->addWidget(m_btnResetParams);
    ////////////////////////////////////////////////////////////////////////////////
    m_btnStartStopRelaxation = new QPushButton("Start Relaxation");
    m_btnResetCamera         = new QPushButton("Reset Camera");
    m_btnEditClipPlane       = new QPushButton("Edit Clip Plane");
    m_btnClipViewPlane       = new QPushButton("Clip View");
    m_btnClipViewPlane->setCheckable(true);

    ////////////////////////////////////////////////////////////////////////////////
    QGridLayout* layoutButtons = new QGridLayout;
    layoutButtons->addWidget(m_btnStartStopRelaxation, 0, 0, 1, 2);
    layoutButtons->addWidget(        m_btnResetCamera, 1, 0, 1, 2);
    layoutButtons->addWidget(      m_btnClipViewPlane, 2, 0, 1, 1);
    layoutButtons->addWidget(      m_btnEditClipPlane, 2, 1, 1, 1);
    ////////////////////////////////////////////////////////////////////////////////
    m_MainLayout->addStretch();
    m_MainLayout->addLayout(layoutButtons);
}
