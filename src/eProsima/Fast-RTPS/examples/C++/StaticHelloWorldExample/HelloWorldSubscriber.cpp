// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file HelloWorldSubscriber.cpp
 *
 */

#include "HelloWorldSubscriber.h"
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/Domain.h>
#include <fastrtps/utils/eClock.h>


HelloWorldSubscriber::HelloWorldSubscriber():mp_participant(nullptr),
mp_subscriber(nullptr)
{
}

bool HelloWorldSubscriber::init()
{
    ParticipantAttributes PParam;
   PParam.rtps.setName("HelloWorldSubscriber");
    PParam.rtps.builtin.use_SIMPLE_EndpointDiscoveryProtocol = false;
    PParam.rtps.builtin.use_STATIC_EndpointDiscoveryProtocol = true;
    PParam.rtps.builtin.setStaticEndpointXMLFilename("HelloWorldPublisher.xml");
    mp_participant = Domain::createParticipant(PParam);
    if(mp_participant==nullptr)
        return false;

    //REGISTER THE TYPE

    Domain::registerType(mp_participant,&m_type);
    //CREATE THE SUBSCRIBER
    SubscriberAttributes Rparam;
    Rparam.topic.topicKind = NO_KEY;
    Rparam.topic.topicDataType = "HelloWorld";
    Rparam.topic.topicName = "HelloWorldTopic";
    Rparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS;
    Rparam.setUserDefinedID(3);
    Rparam.setEntityID(4);
    mp_subscriber = Domain::createSubscriber(mp_participant,Rparam,(SubscriberListener*)&m_listener);

    if(mp_subscriber == nullptr)
        return false;


    return true;
}

HelloWorldSubscriber::~HelloWorldSubscriber() {
    // TODO Auto-generated destructor stub
    Domain::removeParticipant(mp_participant);
}

void HelloWorldSubscriber::SubListener::onSubscriptionMatched(Subscriber* /*sub*/,MatchingInfo& info)
{
    if(info.status == MATCHED_MATCHING)
    {
        n_matched++;
        std::cout << "Subscriber matched"<<std::endl;
    }
    else
    {
        n_matched--;
        std::cout << "Subscriber unmatched"<<std::endl;
    }
}

void HelloWorldSubscriber::SubListener::onNewDataMessage(Subscriber* sub)
{
    if(sub->takeNextData((void*)&m_Hello, &m_info))
    {
        if(m_info.sampleKind == ALIVE)
        {
            this->n_samples++;
            // Print your structure data here.
            std::cout << "Message "<<m_Hello.message()<< " "<< m_Hello.index()<< " RECEIVED"<<std::endl;
        }
    }

}


void HelloWorldSubscriber::run()
{
    std::cout << "Subscriber running. Please press enter to stop the Subscriber" << std::endl;
    std::cin.ignore();
}

void HelloWorldSubscriber::run(uint32_t number)
{
    std::cout << "Subscriber running until "<< number << "samples have been received"<<std::endl;
    while(number < this->m_listener.n_samples)
        eClock::my_sleep(500);
}
