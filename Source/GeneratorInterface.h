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

#include <Banana/Utils/Logger.h>
#include <SimulationObjects/ParticleGenerator.h>
#include <SimulationObjects/BoundaryObject.h>
#include <ParticleTools/SPHBasedRelaxation.h>

#include <QMessageBox>
#include "Common.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class ParticleGeneratorInterface
{
public:
    ParticleGeneratorInterface(const SharedPtr<ParticleData>& particleData) : m_ParticleData(particleData)
    {
        Logger::initialize();
        m_Logger = Logger::createLogger("Generator");
    }

    ~ParticleGeneratorInterface() { Logger::shutdown(); }
    ////////////////////////////////////////////////////////////////////////////////
    void loadScene(const String& sceneFile);
    void updateRelaxParams();
    bool doFrameRelaxation(UInt frame);
    void reportFailed(UInt frame);
    ////////////////////////////////////////////////////////////////////////////////
    auto& getRelaxParams() { return m_RelaxParams; }
    float getMinDistanceRatio();
    ////////////////////////////////////////////////////////////////////////////////

private:
    ////////////////////////////////////////////////////////////////////////////////
    Vector<SharedPtr<SimulationObjects::BoundaryObject<2, float>>>    m_BoundaryObjs2D;
    Vector<SharedPtr<SimulationObjects::BoundaryObject<3, float>>>    m_BoundaryObjs3D;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<2, float>>> m_Generators2D;
    Vector<SharedPtr<SimulationObjects::ParticleGenerator<3, float>>> m_Generators3D;
    SharedPtr<Logger>                                                 m_Logger;
    SharedPtr<ParticleData>                                           m_ParticleData;
    Vector<SharedPtr<ParticleTools::SPHRelaxationParameters<float>>>  m_RelaxParams;
};
