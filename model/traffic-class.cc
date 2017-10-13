#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include <queue>

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"
#include "ns3/queue.h"
#include "traffic-class.h"

namespace ns3{


void TrafficClass::setBytes(uint32_t num){
	this->bytes = num;

}

void TrafficClass::setPackets(uint32_t num){
	this->packets = num;

}

void TrafficClass::setMaxPackets(uint32_t num){
	this->max_packets = num;

}

void TrafficClass::setMaxBytes(uint32_t num){
	this->max_bytes = num;

}

//void TrafficClass::setWeight(double_t num){
//	this->weight = num;

//}

void TrafficClass::setPriorityLevel(uint32_t num){
	this->priority_level = num;

}

void TrafficClass::setIsDefault(bool check){
	this->isDefault = check;

}

bool TrafficClass::match(Ptr<Packet> p){
	bool check = true;
	for(unsigned int i=0; i<filters.size(); i++){
		for(unsigned int j=0; j<filters[i]->element.size(); j++){
			if(filters[i]->element[j]->match(p) == false){
				check = false;
				break;
			}	
		}
		if(check)
			return true;
	}
	
	return false;
}


}//ns3 namespace
