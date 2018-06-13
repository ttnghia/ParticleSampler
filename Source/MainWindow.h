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

#include "ParticleSampler.h"
#include "RenderWidget.h"
#include "Controller.h"
#include "Common.h"

#include <QtAppHelpers/OpenGLMainWindow.h>
#include <QtAppHelpers/BrowsePathWidget.h>
#include <QtAppHelpers/OpenGLWidgetTestRender.h>
#include <QtAppHelpers/ClipPlaneEditor.h>
#include <QtAppHelpers/BusyBar.h>

#include <QEvent>
#include <memory>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class MainWindow : public OpenGLMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

protected:
    virtual bool processKeyPressEvent(QKeyEvent* event) override;
    virtual void showEvent(QShowEvent* ev);

public slots:
    void updateWindowTitle(const QString& filePath);
    void updateStatusRelaxation(const QString& status);
    void updateStatusMemoryUsage();
    void updateStatusNumParticles(unsigned int numParticles);
    void changeScene(bool bReload);
    void startStopRelaxation();
    void finishIteration(unsigned int iter, float minDistanceRatio);
    void finishRelaxation(bool bSuccess);
    void pauseRelaxation();

private:
    void setupRenderWidgets();
    void setupStatusBar();
    void connectWidgets();

    ////////////////////////////////////////////////////////////////////////////////
    RenderWidget* m_RenderWidget            = nullptr;
    Controller*   m_Controller              = nullptr;
    QLabel*       m_lblStatusNumParticles   = nullptr;
    QLabel*       m_lblStatusRelaxationInfo = nullptr;
    QLabel*       m_lblStatusMemoryUsage    = nullptr;
    QLabel*       m_lblStatusIteration      = nullptr;
    BusyBar*      m_BusyBar;

    int  m_FrameNumber = 0;
    bool m_bExportImg  = false;

    ParticleSampler* m_Sampler         = new ParticleSampler();
    ClipPlaneEditor* m_ClipPlaneEditor = new ClipPlaneEditor();
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
Q_DECLARE_METATYPE(Vec3f)
inline auto idVec3f = qRegisterMetaType<Vec3f>();
Q_DECLARE_METATYPE(Vector<PointLights::PointLightData>)
inline auto idVecPL = qRegisterMetaType<Vector<PointLights::PointLightData>>();