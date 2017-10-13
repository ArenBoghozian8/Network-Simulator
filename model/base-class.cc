/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "base-class.h"

namespace ns3 {

BaseClass::BaseClass(){

}


void BaseClass::setMode(BaseClass::QueueMode mode){
        m_mode = mode;

}


BaseClass::QueueMode BaseClass::getMode(){
        return m_mode;

}




}

