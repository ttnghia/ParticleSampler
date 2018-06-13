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

#include "GeneratorInterface.h"
#include <Banana/Utils/Formatters.h>
#include <Banana/Utils/JSONHelpers.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleGeneratorInterface::loadScene(const String& sceneFile)
{
    m_ParticleData->resetData();
    ////////////////////////////////////////////////////////////////////////////////;;
    // load parameters
    std::ifstream inputFile(sceneFile);
    if(!inputFile.is_open()) {
        QMessageBox::critical(nullptr, QString("Error"), QString("Cannot open scene file '%1'!").arg(QString::fromStdString(sceneFile)));
        return;
    }
    auto jParams = JParams::parse(inputFile);
    __BNN_REQUIRE(jParams.find("GlobalParameters") != jParams.end());
    __BNN_REQUIRE(jParams.find("SimulationParameters") != jParams.end());
    __BNN_REQUIRE(jParams.find("ParticleGenerators") != jParams.end());
    __BNN_REQUIRE(JSONHelpers::readValue(jParams["GlobalParameters"], m_ParticleData->dimension, "Dimension"));
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////
    // read particle radius
    if(!JSONHelpers::readValue(jParams["SimulationParameters"], m_ParticleData->particleRadius, "ParticleRadius")) {
        float cellSize;
        float ratioCellSizeRadius;
        __BNN_REQUIRE(JSONHelpers::readValue(jParams["SimulationParameters"], cellSize, "CellSize"));
        __BNN_REQUIRE(JSONHelpers::readValue(jParams["SimulationParameters"], ratioCellSizeRadius, "RatioCellSizePRadius"));
        m_ParticleData->particleRadius = cellSize / ratioCellSizeRadius;
        __BNN_REQUIRE(m_ParticleData->particleRadius > 0);
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // read domain box
    {
        JParams jBoxParams = jParams["SimulationParameters"]["SimulationDomainBox"];
        jBoxParams["GeometryType"] = String("Box");

        if(m_ParticleData->dimension == 2) {
            Vec2f bMin, bMax;
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMin, "BoxMin"));
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, bMax, "BoxMax"));
            memcpy(&m_ParticleData->boxMin[0], &bMin[0], sizeof(float) * m_ParticleData->dimension);
            memcpy(&m_ParticleData->boxMax[0], &bMax[0], sizeof(float) * m_ParticleData->dimension);
        } else {
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, m_ParticleData->boxMin, "BoxMin"));
            __BNN_REQUIRE(JSONHelpers::readVector(jBoxParams, m_ParticleData->boxMax, "BoxMax"));
        }
    }
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////;
    // read boundary objects
    if(jParams.find("AdditionalBoundaryObjects") != jParams.end()) {
        for(auto& jObj : jParams["AdditionalBoundaryObjects"]) {
            String geometryType;
            __BNN_REQUIRE(JSONHelpers::readValue(jObj, geometryType, "GeometryType"));
            __BNN_REQUIRE(!geometryType.empty());

            if(m_ParticleData->dimension == 2) {
                SharedPtr<SimulationObjects::BoundaryObject<2, float>> obj = nullptr;
                if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
                    obj = std::make_shared<SimulationObjects::BoxBoundary<2, float>>(jObj);
                } else {
                    obj = std::make_shared<SimulationObjects::BoundaryObject<2, float>>(jObj);
                }
                __BNN_REQUIRE(obj != nullptr);
                m_BoundaryObjs2D.push_back(obj);
            } else {
                SharedPtr<SimulationObjects::BoundaryObject<3, float>> obj = nullptr;
                if(geometryType == "Box" || geometryType == "box" || geometryType == "BOX") {
                    obj = std::make_shared<SimulationObjects::BoxBoundary<3, float>>(jObj);
                } else {
                    obj = std::make_shared<SimulationObjects::BoundaryObject<3, float>>(jObj);
                }
                __BNN_REQUIRE(obj != nullptr);
                m_BoundaryObjs3D.push_back(obj);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // combine static boundaries
        if(m_ParticleData->dimension == 2) {
            if(m_BoundaryObjs2D.size() > 1) {
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<2, float>>(JParams(), true);
                auto csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<2, float>>(csgBoundary->geometry());
                __BNN_REQUIRE(csgObj != nullptr);

                for(auto& obj : m_BoundaryObjs2D) {
                    csgObj->addObject(obj->geometry(), GeometryObjects::CSGOperations::Union);
                }

                m_BoundaryObjs2D.resize(0);
                m_BoundaryObjs2D.push_back(csgBoundary);
            }
        } else {
            if(m_BoundaryObjs3D.size() > 1) {
                auto csgBoundary = std::make_shared<SimulationObjects::BoundaryObject<3, float>>(JParams(), true);
                auto csgObj      = std::static_pointer_cast<GeometryObjects::CSGObject<3, float>>(csgBoundary->geometry());
                __BNN_REQUIRE(csgObj != nullptr);

                for(auto& obj : m_BoundaryObjs3D) {
                    csgObj->addObject(obj->geometry(), GeometryObjects::CSGOperations::Union);
                }

                m_BoundaryObjs3D.resize(0);
                m_BoundaryObjs3D.push_back(csgBoundary);
            }
        }
    } // end additional boundary
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////;
    // create generators
    m_Generators2D.resize(0);
    m_Generators3D.resize(0);
    if(m_ParticleData->dimension == 2) {
        for(auto& jObj : jParams["ParticleGenerators"]) {
            m_Generators2D.emplace_back(std::make_shared<SimulationObjects::ParticleGenerator<2, float>>(jObj));
        }
    } else {
        for(auto& jObj : jParams["ParticleGenerators"]) {
            m_Generators3D.emplace_back(std::make_shared<SimulationObjects::ParticleGenerator<3, float>>(jObj));
        }
    }
    __BNN_REQUIRE((m_Generators2D.size() > 0) ^ (m_Generators3D.size() > 0));
    ////////////////////////////////////////////////////////////////////////////////;

    ////////////////////////////////////////////////////////////////////////////////
    // add particles without relaxation
    if(m_ParticleData->dimension == 2) {
        for(auto& generator : m_Generators2D) {
            Timer timer;
            timer.tick();
            generator->buildObject(m_ParticleData->particleRadius, m_BoundaryObjs2D);
            timer.tock();
            const auto& objParticles = generator->getObjParticles();
            UInt        nGen         = static_cast<UInt>(objParticles.size());
            if(nGen > 0) {
                m_ParticleData->positionPtrs.push_back(reinterpret_cast<const void*>(objParticles.data()));
                m_ParticleData->positionDataSizes.push_back(static_cast<size_t>(nGen * sizeof(Vec2f)));
                m_ParticleData->nParticles.push_back(nGen);
                ++m_ParticleData->nObjects;
                m_ParticleData->nTotalParticles += nGen;
                m_Logger->printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID() +
                                   String(". Time: ") + timer.getRunTime());
            }
        }
    } else {
        for(auto& generator : m_Generators3D) {
            Timer timer;
            timer.tick();
            generator->buildObject(m_ParticleData->particleRadius, m_BoundaryObjs3D);
            timer.tock();
            const auto& objParticles = generator->getObjParticles();
            UInt        nGen         = static_cast<UInt>(objParticles.size());
            if(nGen > 0) {
                m_ParticleData->positionPtrs.push_back(reinterpret_cast<const void*>(objParticles.data()));
                m_ParticleData->positionDataSizes.push_back(static_cast<size_t>(nGen * sizeof(Vec3f)));
                m_ParticleData->nParticles.push_back(nGen);
                ++m_ParticleData->nObjects;
                m_ParticleData->nTotalParticles += nGen;
                m_Logger->printLog(String("Generated ") + Formatters::toString(nGen) + String(" particles by generator: ") + generator->nameID() +
                                   String(". Time: ") + timer.getRunTime());
            }
        }
    }

    __BNN_REQUIRE(m_ParticleData->positionPtrs.size() > 0);
    ////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////
    // get relax parameters
    {
        m_RelaxParams.resize(0);
        if(m_ParticleData->dimension == 2) {
            for(auto& generator: m_Generators2D) {
                m_RelaxParams.push_back(generator->relaxer().relaxParams());
            }
        } else {
            for(auto& generator: m_Generators3D) {
                m_RelaxParams.push_back(generator->relaxer().relaxParams());
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleGeneratorInterface::updateRelaxParams()
{
    if(m_ParticleData->dimension == 2) {
        for(auto& generator: m_Generators2D) {
            generator->relaxer().updateParams();
        }
    } else {
        for(auto& generator: m_Generators3D) {
            generator->relaxer().updateParams();
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool ParticleGeneratorInterface::doFrameRelaxation(UInt frame)
{
    bool bConvergence = true;
    if(m_ParticleData->dimension == 2) {
        for(auto& generator: m_Generators2D) {
            generator->relaxer().iterate(frame);
            auto tmp = generator->relaxer().checkConvergence(frame);
            bConvergence &= tmp;
        }
    } else {
        for(auto& generator: m_Generators3D) {
            generator->relaxer().iterate(frame);
            auto tmp = generator->relaxer().checkConvergence(frame);
            bConvergence &= tmp;
        }
    }
    return bConvergence;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void ParticleGeneratorInterface::reportFailed(UInt frame)
{
    if(m_ParticleData->dimension == 2) {
        for(auto& generator: m_Generators2D) {
            generator->relaxer().reportFailed(frame);
        }
    } else {
        for(auto& generator: m_Generators3D) {
            generator->relaxer().reportFailed(frame);
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
float ParticleGeneratorInterface::getMinDistanceRatio()
{
    float minRatio = float(1.0e10);
    if(m_ParticleData->dimension == 2) {
        for(auto& generator: m_Generators2D) {
            minRatio = std::min(minRatio, generator->relaxer().getMinDistanceRatio());
        }
    } else {
        for(auto& generator: m_Generators3D) {
            minRatio = std::min(minRatio, generator->relaxer().getMinDistanceRatio());
        }
    }
    return minRatio;
}
