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

#pragma once

#include <QWidget>
#include <QtWidgets>

#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/MaterialSelector.h>
#include <QtAppHelpers/OpenGLController.h>
#include <ParticleTools/SPHBasedRelaxation.h>

#include "Common.h"
#include "RenderWidget.h"
#include "ParticleSampler.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class Controller : public OpenGLController
{
    Q_OBJECT
    friend class MainWindow;
public:
    explicit Controller(RenderWidget* renderWidget, ParticleSampler* particleSampler, QWidget* parent = nullptr, int width = 300) :
        OpenGLController(static_cast<OpenGLWidget*>(renderWidget), parent, width), m_RenderWidget(renderWidget), m_ParticleSampler(particleSampler)
    {
        setupGUI();
        connectWidgets();
    }

public slots:
    void updateRelaxParams();
    void setDefaultParams();

private:
    QWidget*     m_RelaxationControllers       = new QWidget;
    QVBoxLayout* m_LayoutRelaxationControllers = new QVBoxLayout;

    ////////////////////////////////////////////////////////////////////////////////
    void setupGUI();
    void connectWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    // main objects
    RenderWidget*    m_RenderWidget    = nullptr;
    ParticleSampler* m_ParticleSampler = nullptr;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // material
    void setupMaterialControllers();
    MaterialSelector* m_msParticleMaterial;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // color mode
    void setupColorModeControllers();
    QSignalMapper* m_smParticleColorMode;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // capture controller
    void setupCaptureControllers();
    QCheckBox*        m_chkEnableOutput;
    BrowsePathWidget* m_OutputPath;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // scene controller
    void setupSceneControllers();
    EnhancedComboBox* m_cbScene;
    QPushButton*      m_btnReloadScene;
    QCheckBox*        m_chkReloadVizData;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // sampling parameters controller
    void setupSamplingParametersControllers();
    EnhancedComboBox* m_cbMaxIterations;
    EnhancedComboBox* m_cbCheckFrequency;
    EnhancedComboBox* m_cbIntersectionThreshold;
    EnhancedComboBox* m_cbInitialJitter;

    EnhancedComboBox* m_cbSPHCFLFactor;
    EnhancedComboBox* m_cbSPHPressureStiffness;
    EnhancedComboBox* m_cbSPHViscosity;
    EnhancedComboBox* m_cbSPHOverlapThreshold;
    EnhancedComboBox* m_cbSPHNearKernelRadiusRatio;
    EnhancedComboBox* m_cbSPHNearPressureStiffness;
    EnhancedComboBox* m_cbSPHBoundaryRestitution;

    QPushButton* m_btnResetParams;
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // buttons
    void setupButtons();
    QPushButton* m_btnStartStopRelaxation;
    QPushButton* m_btnResetCamera;
    QPushButton* m_btnClipViewPlane;
    QPushButton* m_btnEditClipPlane;
};