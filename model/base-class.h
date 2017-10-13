/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef BASE_CLASS_H
#define BASE_CLASS_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/nstime.h"
#include "traffic-class.h"

namespace ns3 {

class BaseClass: public Queue{
        private:
                TrafficClass* traffic_class;
                BaseClass::QueueMode m_mode;
                std::vector<TrafficClass*> q_class;
                bool Enqueue(Ptr<Packet> p);
                void Dequeue(Ptr<Packet> p);
                bool DoEnqueue(Ptr<Packet> p);
                void DoDequeue(Ptr<Packet> p);
                virtual void schedule(Ptr<Packet> p);
                virtual int classify(Ptr<Packet> p);

        public:
                BaseClass();
                void SetMode(BaseClass::QueueMode mode);
                BaseClass::QueueMode getMode();
                



};

}

#endif /* BASE_CLASS_H */

