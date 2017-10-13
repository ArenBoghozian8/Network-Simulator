#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include <queue>

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/queue.h"

#include "filter.h"

namespace ns3{

class TrafficClass
{
	private:
		ns3::Filter* fil;
		uint32_t bytes;
		uint32_t packets;
		uint32_t max_bytes;
		uint32_t max_packets;
		//double_t weight;   //Doesnt name a type double_t
		uint32_t priority_level;
		bool isDefault;
		std::queue<Ptr<Packet> > m_queue;
		std::vector<ns3::Filter*> filters;	//int will be replaced with filter class

	public:
		void setBytes(uint32_t num);
		void setPackets(uint32_t num);
		void setMaxPackets(uint32_t num);
		void setMaxBytes(uint32_t num);
		//void setWeight(double_t num);
		void setPriorityLevel(uint32_t num);
		void setIsDefault(bool check);
		bool match(Ptr<Packet> p);


};

}
