#include "veins_inet/Attack.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"

#include "veins_inet/VeinsInetSampleMessage_m.h"

using namespace inet;

int spoofingNode = 0;   // antes do envio da mensagem maliciosa, somente node[0] pode ser acessado pelo Attack.
                        // após o Attack, os outros nós processam a mensagem maliciosa e alteram as rotas.
Define_Module(Attack);

Attack::Attack()
{
}

bool Attack::startApplication()
{
    if (getParentModule()->getIndex() == spoofingNode) {
        traciVehicle->setSpeed(0);
        auto sendFakeAccident = [this]() {
            getParentModule()->getDisplayString().setTagArg("i", 1, "red");
            traciVehicle->setSpeed(0);

            auto payload = makeShared<VeinsInetSampleMessage>();
            payload->setChunkLength(B(100));
            payload->setRoadId(traciVehicle->getRoadId().c_str());
            timestampPayload(payload);

            auto packet = createPacket("accident");
            packet->insertAtBack(payload);
            sendPacket(std::move(packet));
        };
        simtime_t timeInterval = 35; // 35s
        timerManager.create(veins::TimerSpecification(sendFakeAccident).interval(timeInterval));
    }

    return true;
}

void Attack::processPacket(std::shared_ptr<inet::Packet> pk)
{

}

bool Attack::stopApplication()
{
    return true;
}

Attack::~Attack()
{

}
