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

#include "Common.h"

#include <OpenGLHelpers/OpenGLBuffer.h>
#include <OpenGLHelpers/OpenGLTexture.h>
#include <OpenGLHelpers/MeshObjects/MeshObject.h>
#include <OpenGLHelpers/Material.h>
#include <OpenGLHelpers/Lights.h>
#include <OpenGLHelpers/RenderObjects.h>

#include <QtAppHelpers/QtAppShaderProgram.h>
#include <QtAppHelpers/QtAppMacros.h>
#include <QtAppHelpers/OpenGLWidget.h>
#include <QtAppHelpers/EnhancedMessageBox.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class RenderWidget : public OpenGLWidget
{
    Q_OBJECT
public:
    RenderWidget(QWidget* parent, const SharedPtr<VisualizationData>& vizData, const SharedPtr<ParticleData>& particleData);
private:
    virtual void initOpenGL();
    virtual void resizeOpenGLWindow(int, int);
    virtual void renderOpenGL();
    SharedPtr<VisualizationData> m_VizData;
    SharedPtr<ParticleData>      m_ParticleData;
public slots:
    void updateCamera() { m_Camera->setCamera(m_VizData->cameraPosition, m_VizData->cameraFocus, Vec3f(0, 1, 0)); }
    void updateSolverDimension();
    void updateVizData();

    ////////////////////////////////////////////////////////////////////////////////
    // particles
public slots:
    void setParticleColorMode(int colorMode);
    void setParticleMaterial(const Material::MaterialData& material);
private:
    struct RDataParticle
    {
        SharedPtr<QtAppShaderProgram> shader        = nullptr;
        UniquePtr<OpenGLBuffer>       buffPosition  = nullptr;
        UniquePtr<OpenGLBuffer>       buffColorData = nullptr;
        UniquePtr<Material>           material      = nullptr;

        GLuint VAO;
        GLint  v_Position;
        GLint  v_iColor;
        GLuint ub_CamData;
        GLuint ub_Light;
        GLuint ub_Material;
        GLuint u_nParticles;
        GLuint u_PointRadius;
        GLuint u_PointScale;
        GLuint u_Dimension;
        GLuint u_ScreenHeight;
        GLuint u_DomainHeight;
        GLuint u_ClipPlane;
        GLuint u_ColorMode;
        GLuint u_vColorMin;
        GLuint u_vColorMax;
        GLuint u_ColorMinVal;
        GLuint u_ColorMaxVal;

        GLuint  nParticles = 0;
        GLfloat pointRadius;
        GLfloat pointScale;

        GLint pColorMode  = ParticleColorMode::Ramp;
        float vColorMin   = 0;
        float vColorMax   = 1.0f;
        Vec3f colorMinVal = DEFAULT_COLOR_DATA_MIN;
        Vec3f colorMaxVal = DEFAULT_COLOR_DATA_MAX;
        bool  initialized = false;
    } m_RDataParticle;

    void initRDataParticle();
    void initParticleVAO();
    void renderParticles();
    ////////////////////////////////////////////////////////////////////////////////
};