/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DEFICIT_ROUND_ROBIN_H
#define DEFICIT_ROUND_ROBIN_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/nstime.h"

namespace ns3 {

class TraceContainer;

class DeficitRoundRobin: public Queue {
public:
	static TypeId GetTypeId(void);

	DeficitRoundRobin();

	~DeficitRoundRobin();

	void SetMode(DeficitRoundRobin::QueueMode mode);
	DeficitRoundRobin::QueueMode GetMode(void);

private:

	virtual bool DoEnqueue(Ptr<QueueItem> p);
	virtual Ptr<QueueItem> DoDequeue(void);
	virtual Ptr<const QueueItem> DoPeek(void) const;

	// returns 0 for 1st queue
	// returns 1 for 2nd queue
	uint16_t Classify(Ptr<Packet> p);

	// returns the queue number to be peeked (for DoPeek() method)
	int32_t GetQueueToBePeeked() const;

	// packet classification port
	uint32_t m_secondQueuePort;

    // DRR Parameters Qi set for a constant bits
    // for each queue to served.
    uint32_t m_quantum_size;
        
    // a counter for each unusable Qi.
    // resets to 0 when the queue is empty.
    uint32_t m_first_dc;
    uint32_t m_second_dc;
    bool m_serve_queue2;

	std::queue<Ptr<Packet> > m_firstQueue;
	uint32_t m_bytesInFirstQueue;
	uint32_t m_firstMaxPackets;
	uint32_t m_firstMaxBytes;
	uint32_t m_firstPacketsToBeServed;
	uint32_t m_firstMeanPacketSize;
	uint32_t m_firstRemainingPacketsToBeServed;

	std::queue<Ptr<Packet> > m_secondQueue;
	uint32_t m_bytesInSecondQueue;
	uint32_t m_secondMaxPackets;
	uint32_t m_secondMaxBytes;
	uint32_t m_secondPacketsToBeServed;
	uint32_t m_secondMeanPacketSize;
	uint32_t m_secondRemainingPacketsToBeServed;

	QueueMode m_mode;

	bool m_isInitialized;


	// These variables are temporarily used for showing progression percentage
	uint64_t m_passedBytes;
	uint8_t m_20PercentCompleted;
	uint8_t m_40PercentCompleted;
	uint8_t m_60PercentCompleted;
	uint8_t m_80PercentCompleted;
	uint64_t m_totalExpectedBytes;

	uint64_t max_firstQueue;
	uint64_t max_secondQueue;
};

} // namespace ns3

#endif /* DEFICIT_ROUND_ROBIN_H */

