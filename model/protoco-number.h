#include "ns3/uinteger.h"
#include "ns3/ipv4-header.h"
#include "filter-element.h"

namespace ns3{

class ProtocolNumber: public FilterElement{
	private:
		uint32_t value;
	
	public:
		ProtocolNumber(uint32_t num);
		bool match(Ptr<Packet> p);


};


}
	
