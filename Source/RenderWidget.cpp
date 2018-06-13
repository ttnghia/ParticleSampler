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

#include <Banana/ParallelHelpers/Scheduler.h>
#include <Banana/ParallelHelpers/ParallelSTL.h>
#include "RenderWidget.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RenderWidget::RenderWidget(QWidget* parent, const SharedPtr<VisualizationData>& vizData, const SharedPtr<ParticleData>& particleData) :
    OpenGLWidget(parent), m_VizData(vizData), m_ParticleData(particleData)
{
    updateCamera();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initOpenGL()
{
    initRDataParticle();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::resizeOpenGLWindow(int, int height)
{
    m_RDataParticle.pointScale = static_cast<GLfloat>(height) / tanf(55.0 * 0.5 * M_PI / 180.0);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderOpenGL()
{
    renderParticles();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateSolverDimension()
{
    if(m_ParticleData->dimension == 3) {
        m_Camera->setProjection(Camera::PerspectiveProjection);
    } else {
        m_Camera->setProjection(Camera::OrthographicProjection);
        m_Camera->setOrthoBox(m_ParticleData->boxMin.x * 1.01f, m_ParticleData->boxMax.x * 1.01f, m_ParticleData->boxMin.y * 1.01f, m_ParticleData->boxMax.y * 1.01f);
    }

    makeCurrent();
    initParticleVAO();
    doneCurrent();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::updateVizData()
{
    Q_ASSERT(m_RDataParticle.initialized);
    makeCurrent();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffPosition->uploadDataAsync(m_ParticleData->positionPtrs, 0, m_ParticleData->positionDataSizes);
    ////////////////////////////////////////////////////////////////////////////////
    // color data
    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex) {
        m_RDataParticle.vColorMin = 0;
        m_RDataParticle.vColorMax = m_ParticleData->nObjects > 1u ? static_cast<float>(m_ParticleData->nObjects - 1) : 1.0f;
    }
    ////////////////////////////////////////////////////////////////////////////////
    doneCurrent();
    m_RDataParticle.nParticles  = m_ParticleData->nTotalParticles;
    m_RDataParticle.pointRadius = m_ParticleData->particleRadius;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleColorMode(int colorMode)
{
    Q_ASSERT(colorMode < ParticleColorMode::NumColorMode);
    Q_ASSERT(m_RDataParticle.initialized);
    m_RDataParticle.pColorMode = colorMode;
    ////////////////////////////////////////////////////////////////////////////////
    if(colorMode == ParticleColorMode::ObjectIndex) {
        updateVizData();
        makeCurrent();
        initParticleVAO();
        doneCurrent();
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initRDataParticle()
{
    m_RDataParticle.shader = std::make_shared<QtAppShaderProgram>("RenderPointSphere");
    m_RDataParticle.shader->addVertexShaderFromResource(":/Shaders/particle.vs.glsl");
    m_RDataParticle.shader->addFragmentShaderFromResource(":/Shaders/particle.fs.glsl");
    m_RDataParticle.shader->link();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.v_Position = m_RDataParticle.shader->getAtributeLocation("v_Position");
    m_RDataParticle.v_iColor   = m_RDataParticle.shader->getAtributeLocation("v_iColor");
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.ub_CamData  = m_RDataParticle.shader->getUniformBlockIndex("CameraData");
    m_RDataParticle.ub_Light    = m_RDataParticle.shader->getUniformBlockIndex("Lights");
    m_RDataParticle.ub_Material = m_RDataParticle.shader->getUniformBlockIndex("Material");
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.u_nParticles   = m_RDataParticle.shader->getUniformLocation("u_nParticles");
    m_RDataParticle.u_PointRadius  = m_RDataParticle.shader->getUniformLocation("u_PointRadius");
    m_RDataParticle.u_PointScale   = m_RDataParticle.shader->getUniformLocation("u_PointScale");
    m_RDataParticle.u_Dimension    = m_RDataParticle.shader->getUniformLocation("u_Dimension");
    m_RDataParticle.u_ScreenHeight = m_RDataParticle.shader->getUniformLocation("u_ScreenHeight");
    m_RDataParticle.u_DomainHeight = m_RDataParticle.shader->getUniformLocation("u_DomainHeight");
    m_RDataParticle.u_ColorMode    = m_RDataParticle.shader->getUniformLocation("u_ColorMode");
    m_RDataParticle.u_vColorMin    = m_RDataParticle.shader->getUniformLocation("u_vColorMin");
    m_RDataParticle.u_vColorMax    = m_RDataParticle.shader->getUniformLocation("u_vColorMax");
    m_RDataParticle.u_ColorMinVal  = m_RDataParticle.shader->getUniformLocation("u_ColorMinVal");
    m_RDataParticle.u_ColorMaxVal  = m_RDataParticle.shader->getUniformLocation("u_ColorMaxVal");
    m_RDataParticle.u_ClipPlane    = m_RDataParticle.shader->getUniformLocation("u_ClipPlane");
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffPosition = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffPosition->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffColorData = std::make_unique<OpenGLBuffer>();
    m_RDataParticle.buffColorData->createBuffer(GL_ARRAY_BUFFER, 1, nullptr, GL_DYNAMIC_DRAW);
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.material = std::make_unique<Material>();
    m_RDataParticle.material->setMaterial(CUSTOM_PARTICLE_MATERIAL);
    m_RDataParticle.material->uploadDataToGPU();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.initialized = true;
    initParticleVAO();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::initParticleVAO()
{
    Q_ASSERT(m_RDataParticle.initialized);
    glCall(glGenVertexArrays(1, &m_RDataParticle.VAO));
    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnableVertexAttribArray(m_RDataParticle.v_Position));
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.buffPosition->bind();
    glCall(glVertexAttribPointer(m_RDataParticle.v_Position, m_ParticleData->dimension, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(0)));
    ////////////////////////////////////////////////////////////////////////////////
    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex) {
        m_RDataParticle.buffColorData->bind();
        glCall(glEnableVertexAttribArray(m_RDataParticle.v_iColor));
        glCall(glVertexAttribIPointer(m_RDataParticle.v_iColor, 1, GL_SHORT, 0, reinterpret_cast<GLvoid*>(0)));
    }
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(0));
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::renderParticles()
{
    Q_ASSERT(m_RDataParticle.initialized);

    m_RDataParticle.shader->bind();
    ////////////////////////////////////////////////////////////////////////////////
    m_UBufferCamData->bindBufferBase();
    m_Lights->bindUniformBuffer();
    m_RDataParticle.material->bindUniformBuffer();
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_CamData,  m_UBufferCamData->getBindingPoint());
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Light,    m_Lights->getBufferBindingPoint());
    m_RDataParticle.shader->bindUniformBlock(m_RDataParticle.ub_Material, m_RDataParticle.material->getBufferBindingPoint());
    ////////////////////////////////////////////////////////////////////////////////
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_nParticles,   m_RDataParticle.nParticles);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointRadius,  m_RDataParticle.pointRadius);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_PointScale,   m_RDataParticle.pointScale);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_Dimension,    m_ParticleData->dimension);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ScreenHeight, height());
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_DomainHeight, (m_Camera->getOrthoBoxMax().y - m_Camera->getOrthoBoxMin().y) * 0.9f);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMode,    m_RDataParticle.pColorMode);
    m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ClipPlane,    m_ClipPlane);
    ////////////////////////////////////////////////////////////////////////////////
    if(m_RDataParticle.pColorMode == ParticleColorMode::ObjectIndex) {
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_vColorMin,   m_RDataParticle.vColorMin);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_vColorMax,   m_RDataParticle.vColorMax);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMinVal, m_RDataParticle.colorMinVal);
        m_RDataParticle.shader->setUniformValue(m_RDataParticle.u_ColorMaxVal, m_RDataParticle.colorMaxVal);
    }
    ////////////////////////////////////////////////////////////////////////////////
    glCall(glBindVertexArray(m_RDataParticle.VAO));
    glCall(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE));
    glCall(glDrawArrays(GL_POINTS, 0, m_RDataParticle.nParticles));
    glCall(glBindVertexArray(0));
    m_RDataParticle.shader->release();
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void RenderWidget::setParticleMaterial(const Material::MaterialData& material)
{
    makeCurrent();
    m_RDataParticle.material->setMaterial(material);
    m_RDataParticle.material->uploadDataToGPU();
    doneCurrent();
}
