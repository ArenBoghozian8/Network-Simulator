/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "deficit-round-robin.h"

#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

#include <math.h>

NS_LOG_COMPONENT_DEFINE("DeficitRoundRobin");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(DeficitRoundRobin);

TypeId DeficitRoundRobin::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::DeficitRoundRobinQueue").SetParent<Queue>().AddConstructor<
			        DeficitRoundRobin>()
			.AddAttribute("SecondQueueMaxPackets",
					"The maximum number of packets accepted by the second weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&DeficitRoundRobin::m_secondMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstQueueMaxPackets",
					"The maximum number of packets accepted by the first weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&DeficitRoundRobin::m_firstMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondQueueMaxBytes",
					"The maximum number of bytes accepted by the second weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&DeficitRoundRobin::m_secondMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstQueueMaxBytes",
					"The maximum number of bytes accepted by the first weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&DeficitRoundRobin::m_firstMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondQueuePort",
					"The destination port number for second queue traffic.",
					UintegerValue(3000),
					MakeUintegerAccessor(&DeficitRoundRobin::m_secondQueuePort),
					MakeUintegerChecker<uint32_t>())
			.AddAttribute("TotalExpectedBytes",
					"Total expected bytes passed before end of simulation - used for showing progression percentage",
					UintegerValue(0),
					MakeUintegerAccessor(&DeficitRoundRobin::m_totalExpectedBytes),
					MakeUintegerChecker<uint64_t>())

			.AddAttribute("FirstMeanPacketSize",
					"The mean packet size of first queue",
					UintegerValue(1),
					MakeUintegerAccessor(&DeficitRoundRobin::m_firstMeanPacketSize),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondMeanPacketSize",
					"The mean packet size of second queue",
					UintegerValue(1),
					MakeUintegerAccessor(&DeficitRoundRobin::m_secondMeanPacketSize),
					MakeUintegerChecker<uint32_t>())
                        .AddAttribute("QuantumSize",
					"The packet size permitted for each queue",
					UintegerValue(1),
					MakeUintegerAccessor(&DeficitRoundRobin::m_quantum_size),
					MakeUintegerChecker<uint32_t>());

	return tid;
}

DeficitRoundRobin::DeficitRoundRobin() :
		Queue(), m_firstQueue(), m_bytesInFirstQueue(0), m_secondQueue(), m_bytesInSecondQueue(0)
			    {
	NS_LOG_FUNCTION_NOARGS ();

	m_isInitialized = false;

	m_passedBytes = 0;
	m_20PercentCompleted = false;
	m_40PercentCompleted = false;
	m_60PercentCompleted = false;
	m_80PercentCompleted = false;
    m_serve_queue2 = false;
    m_first_dc = 0;
    m_second_dc =0;
	max_firstQueue = 0;
	max_secondQueue = 0;
}

DeficitRoundRobin::~DeficitRoundRobin() {
	NS_LOG_FUNCTION_NOARGS ();
}

void DeficitRoundRobin::SetMode(DeficitRoundRobin::QueueMode mode) {
	NS_LOG_FUNCTION(mode);
	m_mode = mode;
}

DeficitRoundRobin::QueueMode DeficitRoundRobin::GetMode(void) {
	NS_LOG_FUNCTION_NOARGS ();
	return m_mode;
}

uint16_t DeficitRoundRobin::Classify(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);
	PppHeader ppp;
	p->RemoveHeader(ppp);
	Ipv4Header ip;
	p->RemoveHeader(ip);
        
   
	uint16_t weightedQueue;

	uint32_t protocol = ip.GetProtocol();
	if (protocol == 17) {
		UdpHeader udp;
		p->PeekHeader(udp);

		if (udp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("classifier: second queue udp");
			weightedQueue = 1;
		} else {
			NS_LOG_INFO("classifier: first queue udp");
			weightedQueue = 0;
		}
	}

	else if (protocol == 6) {
		TcpHeader tcp;
		p->PeekHeader(tcp);
		if (tcp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("classifier: second queue tcp");
			weightedQueue = 1;
		} else {
			NS_LOG_INFO("classifier: first queue tcp");
			weightedQueue = 0;
		}

	} else {
		NS_LOG_INFO("\tclassifier: unrecognized transport protocol");
		weightedQueue = 0;
	}

	p->AddHeader(ip);
	p->AddHeader(ppp);

	return weightedQueue;
}



bool DeficitRoundRobin::DoEnqueue(Ptr<QueueItem> p) {
	NS_LOG_FUNCTION(this << p);

	if (m_isInitialized == false)
	{
		m_isInitialized = true;
	}

	uint16_t weightedQueue = Classify(p->GetPacket());

	// Second Queue
	if (weightedQueue == 1) {
		if (m_mode == QUEUE_MODE_PACKETS
				&& (m_secondQueue.size() >= m_secondMaxPackets)) {
			NS_LOG_LOGIC("Queue full (at max packets) -- dropping pkt");
			Drop(p->GetPacket());
			return false;
		}

		if (m_mode == QUEUE_MODE_BYTES
				&& (m_bytesInSecondQueue + p->GetPacket()->GetSize() >= m_secondMaxBytes)) {
			NS_LOG_LOGIC(
					"Queue full (packet would exceed max bytes) -- dropping pkt");
			Drop(p->GetPacket());
			return false;
		}

		m_bytesInSecondQueue += p->GetPacket()->GetSize();
		m_secondQueue.push(p->GetPacket());

		NS_LOG_LOGIC("Number packets " << m_secondQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return true;
	}

	// First Queue
	else if (weightedQueue == 0) {
		if (m_mode == QUEUE_MODE_PACKETS
				&& (m_firstQueue.size() >= m_firstMaxPackets)) {
			NS_LOG_LOGIC("Queue full (at max packets) -- dropping pkt");
			Drop(p->GetPacket());
			return false;
		}

		if (m_mode == QUEUE_MODE_BYTES
				&& (m_bytesInFirstQueue + p->GetPacket()->GetSize() >= m_firstMaxBytes)) {
			NS_LOG_LOGIC(
					"Queue full (packet would exceed max bytes) -- dropping pkt");
			Drop(p->GetPacket());
			return false;
		}

		m_bytesInFirstQueue += p->GetPacket()->GetSize();
		m_firstQueue.push(p->GetPacket());

		NS_LOG_LOGIC("Number packets " << m_firstQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return true;
	}

	// This normally never happens unless Classify() has been changed
	else {
		return false;
	}
}



Ptr<QueueItem> DeficitRoundRobin::DoDequeue(void) {
	NS_LOG_FUNCTION(this);

    if(!m_firstQueue.empty() || !m_secondQueue.empty()){
        if (!m_firstQueue.empty() && !m_serve_queue2) { //Check == false
                m_first_dc += m_quantum_size;
	            Ptr<Packet> packet1 = m_firstQueue.front();

               if(packet1->GetSize() < m_quantum_size){
                        m_firstQueue.pop();
                        m_first_dc -= packet1->GetSize();
                }
	            m_bytesInFirstQueue -= packet1->GetSize();
                
                if(m_firstQueue.empty()){
                        m_first_dc = 0;
                }
    
	            NS_LOG_LOGIC("Popped " << packet1);
	            NS_LOG_LOGIC("Number packets " << m_firstQueue.size ());
	            NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);
                NS_LOG_LOGIC("Queue1 is being served");

                m_serve_queue2 = true;
                NS_LOG_LOGIC("Defecit Counter Queue1" << m_first_dc);
	            return new QueueItem(packet1);
        }

        else if(!m_secondQueue.empty() && m_serve_queue2) { //check == true
                m_second_dc += m_quantum_size;
	            Ptr<Packet> packet2 = m_secondQueue.front();
                
                if(packet2->GetSize() < m_quantum_size){
                        m_secondQueue.pop();
                        m_second_dc -= packet2->GetSize();
                }

	            m_bytesInSecondQueue -= packet2->GetSize();
                
                if(m_secondQueue.empty()){
                        m_second_dc = 0;
                }

	            NS_LOG_LOGIC("Popped " << packet2);
	            NS_LOG_LOGIC("Number packets " << m_secondQueue.size ());
	            NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);
                NS_LOG_LOGIC("Queue2 is being served");

                m_serve_queue2=false;
                NS_LOG_LOGIC("Defecit Counter Queue2" << m_second_dc);
	            return new QueueItem(packet2);
        }
        else
                return NULL;
    }

    else{

		NS_LOG_LOGIC("all queues empty");
		return NULL;
    }
}



Ptr<const QueueItem> DeficitRoundRobin::DoPeek(void) const {
	NS_LOG_FUNCTION(this);

	int32_t queueToBeServed = GetQueueToBePeeked();

	if (queueToBeServed == 0) {

		Ptr<Packet> pack1 = m_firstQueue.front ();
		NS_LOG_LOGIC("Number packets " << m_firstQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);
                
		return new QueueItem(pack1);
	}

	else if (queueToBeServed == 1) {

		Ptr<Packet> pack2 = m_secondQueue.front ();
		NS_LOG_LOGIC("Number packets " << m_secondQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return new QueueItem(pack2);
	}

	else {
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}
}


int32_t DeficitRoundRobin::GetQueueToBePeeked() const
{
	if (m_firstRemainingPacketsToBeServed > 0)
	{
		if (!m_firstQueue.empty())
		{
			return 0;
		}
	}

	if (m_secondRemainingPacketsToBeServed > 0)
	{
		if (!m_secondQueue.empty())
		{
			return 1;
		}
	}

	if (!m_firstQueue.empty())
	{
		return 0;
	}

	if (!m_secondQueue.empty())
	{
		return 1;
	}

	return -1;
}

} // namespace ns3
