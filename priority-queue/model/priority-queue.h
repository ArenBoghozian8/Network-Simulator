/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef STRICT_PRIORITY_QUEUE_H
#define STRICT_PRIORITY_QUEUE_H

#include <queue>
//#include "ns3/ptr.h"
#include <vector>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/base-class.h"
#include "ns3/destination-portnumber.h"


namespace ns3 {

class TraceContainer;

class StrictPriorityQueue : public BaseClass {
public:
  static TypeId GetTypeId (void);
  StrictPriorityQueue ();
  ~StrictPriorityQueue ();

  void SetMode (StrictPriorityQueue::QueueMode mode);
  StrictPriorityQueue::QueueMode GetMode (void);

private:

   bool DoEnqueue(Ptr<QueueItem> p);
   Ptr<QueueItem> DoDequeue(void);
   Ptr<const QueueItem> DoPeek (void) const;

  // returns 0 for low
  // returns 1 for high 
  uint16_t classify (Ptr<QueueItem> p);
  void schedule(Ptr<QueueItem> p);
  uint32_t m_priorityPort;

  uint32_t m_highMaxPackets;
  uint32_t m_lowMaxPackets;

  uint32_t m_highMaxBytes;
  uint32_t m_lowMaxBytes;

  // QueueMode m_mode;

};

} // namespace ns3

#endif /* PRIORITY_QUEUE_H */

