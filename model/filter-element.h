#ifndef FILTERELEMENT_H
#define FILTERELEMENT_H

#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/packet.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

namespace ns3{
//virtual class filterelement
class FilterElement{
	public:
		virtual bool match(Ptr<Packet> p) = 0;
};


}

#endif
