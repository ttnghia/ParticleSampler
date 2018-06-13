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

#include "ParticleSampler.h"

#include <Banana/Utils/JSONHelpers.h>
#include <QtAppHelpers/QtAppUtils.h>
#include <ParticleTools/ParticleSerialization.h>

#include <fstream>
#include <QDebug>
#include <QDir>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::startRelaxation()
{
    m_bStop = false;
    if(m_RelaxationFutureObj.valid()) {
        m_RelaxationFutureObj.wait();
    }
    m_RelaxationFutureObj = std::async(std::launch::async, [&] { doSampling(); });
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::finishImgExport()
{
    m_bWaitForSavingImg = false;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::doSampling()
{
    m_Generator->updateRelaxParams();
    UInt maxIters = getRelaxParams().front()->maxIters;
    UInt frame    = 1;
    for(; frame <= maxIters; ++frame) {
        bool bConverged = m_Generator->doFrameRelaxation(frame);

        emit iterationFinished(frame, m_Generator->getMinDistanceRatio());
        emit vizDataChanged();

        if(m_bExportImg) {
            m_bWaitForSavingImg = true;
            while(m_bWaitForSavingImg) {
                ;
            }
        }

        if(m_bStop) {
            emit relaxationPaused();
            return;
        }
        if(bConverged) {
            emit relaxationFinished(true);
            m_bStop = true;
            return;
        }
    }

    m_bStop = true;
    m_Generator->reportFailed(frame);
    emit relaxationFinished(false);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::stop()
{
    m_bStop = true;
}

void ParticleSampler::reset()
{
    m_bStop = true;
    changeScene(m_Scene);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::changeScene(const QString& scene)
{
    m_Scene = scene;
    emit iterationFinished(0, 0);
    ////////////////////////////////////////////////////////////////////////////////
    // wait until the simulation stop before modifying the scene
    if(m_RelaxationFutureObj.valid()) {
        m_bStop = true;
        m_RelaxationFutureObj.wait();
    }
    ////////////////////////////////////////////////////////////////////////////////
    QString       sceneFile = QtAppUtils::getDefaultPath("Scenes") + "/" + scene;
    std::ifstream inFile(sceneFile.toStdString());
    __BNN_REQUIRE(inFile.is_open());
    nlohmann::json jParams = nlohmann::json::parse(inFile);
    inFile.close();
    ////////////////////////////////////////////////////////////////////////////////
    m_Generator->loadScene(sceneFile.toStdString());
    ////////////////////////////////////////////////////////////////////////////////
    m_VizData->resetData();
    for(Int i = 0; i < m_ParticleData->dimension; ++i) {
        m_ParticleData->boxMin[i] -= m_ParticleData->particleRadius;
        m_ParticleData->boxMax[i] += m_ParticleData->particleRadius;
    }
    ////////////////////////////////////////////////////////////////////////////////
    if(m_bReloadVizData) {
        if(jParams.find("VisualizationParameters") != jParams.end()) {
            auto jVizParams    = jParams["VisualizationParameters"];
            auto bReadCamPos   = JSONHelpers::readVector(jVizParams, m_VizData->cameraPosition, "CameraPosition");
            auto bReadCamFocus = JSONHelpers::readVector(jVizParams, m_VizData->cameraFocus, "CameraFocus");
            if(!bReadCamPos && !bReadCamFocus) {
                m_VizData->cameraPosition = DEFAULT_CAMERA_POSITION;
                m_VizData->cameraFocus    = DEFAULT_CAMERA_FOCUS;
            }

            if(jVizParams.find("Light") != jVizParams.end()) {
                m_VizData->lights.resize(jVizParams["Light"].size());
                for(size_t i = 0; i < jVizParams["Light"].size(); ++i) {
                    auto& jObj = jVizParams["Light"][i];
                    Vec3f tmp;
                    if(JSONHelpers::readVector(jObj, tmp, "Position")) { m_VizData->lights[i].position = Vec4f(tmp, 1.0f); }
                    if(JSONHelpers::readVector(jObj, tmp, "Ambient")) { m_VizData->lights[i].ambient = Vec4f(tmp, 1.0f); }
                    if(JSONHelpers::readVector(jObj, tmp, "Diffuse")) { m_VizData->lights[i].diffuse = Vec4f(tmp, 1.0f); }
                    if(JSONHelpers::readVector(jObj, tmp, "Specular")) { m_VizData->lights[i].specular = Vec4f(tmp, 1.0f); }
                }
                emit lightsChanged(m_VizData->lights);
            }
            if(jVizParams.find("CapturePath") != jVizParams.end()) {
                String capturePath;
                JSONHelpers::readValue(jVizParams, capturePath, "CapturePath");
                emit capturePathChanged(QString::fromStdString(capturePath));
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        emit dimensionChanged();
        emit domainChanged(m_ParticleData->boxMin, m_ParticleData->boxMax);
        emit cameraChanged();
    }
    emit vizDataChanged();
    emit numParticleChanged(m_ParticleData->nTotalParticles);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleSampler::enableExportImg(bool bEnable)
{
    m_bExportImg = bEnable;
}
