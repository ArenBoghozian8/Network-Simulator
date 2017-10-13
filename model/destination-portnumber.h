#include "ns3/uinteger.h"
#include "ns3/ipv4-header.h"
#include "filter-element.h"


namespace ns3{

class DestinationPortNumber : public FilterElement{
	private:	
		uint32_t value;
	
	public:
		DestinationPortNumber(uint32_t num);
		bool match(Ptr<Packet> p);

};


}
