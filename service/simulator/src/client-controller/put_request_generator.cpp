/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "put_request_generator.h"
#include "simulator_resource_model.h"
#include "logger.h"

#define TAG "PUT_REQUEST_GEN"

PUTRequestGenerator::PUTRequestGenerator(int id, RequestSenderSP &requestSender,
        SimulatorResourceModelSP &representation,
        AutoRequestGeneration::ProgressStateCallback callback)
    :   AutoRequestGeneration(RequestType::RQ_TYPE_GET, id, requestSender, callback),
        m_rep(representation),
        m_status(false),
        m_stopRequested(false) {}

PUTRequestGenerator::PUTRequestGenerator(int id, RequestSenderSP &requestSender,
        const std::map<std::string, std::vector<std::string>> &queryParams,
        SimulatorResourceModelSP &representation,
        AutoRequestGeneration::ProgressStateCallback callback)
    :   AutoRequestGeneration(RequestType::RQ_TYPE_GET, id, requestSender, callback),
        m_queryParamGen(queryParams),
        m_rep(representation),
        m_status(false),
        m_stopRequested(false) {}

void PUTRequestGenerator::startSending()
{
    // Check the representation
    if (!m_rep)
    {
        OC_LOG(ERROR, TAG, "Invalid Representation given!");
        throw SimulatorException(SIMULATOR_ERROR, "Invalid representation detected!");
    }

    // Check if the operation is already in progress
    std::lock_guard<std::mutex> lock(m_statusLock);
    if (m_status)
    {
        OC_LOG(ERROR, TAG, "Operation already in progress !");
        throw OperationInProgressException("Another PUT request generation session is already in progress!");
    }

    // Create thread and start sending requests in dispatched thread
    m_thread = std::make_shared<std::thread>(&PUTRequestGenerator::SendAllRequests, this);
    m_status = true;
    m_thread->detach();
}

void PUTRequestGenerator::stopSending()
{
    m_stopRequested = true;
}

void PUTRequestGenerator::SendAllRequests()
{
    OC_LOG(DEBUG, TAG, "Sending OP_START event");
    m_callback(m_id, OP_START);

    bool hasNext = false;
    std::map<std::string, SimulatorResourceModel::Attribute> attributes = m_rep->getAttributes();
    if (!attributes.size())
        return;

    std::vector<std::shared_ptr<AttributeGenerator>> attributeGenList;
    for (auto & attribute : attributes)
    {
        attributeGenList.push_back(std::make_shared<AttributeGenerator>(attribute.second));
    }

    do
    {
        if (!m_stopRequested)
        {
            // Get the next possible queryParameter
            std::map<std::string, std::string> queryParam = m_queryParamGen.next();

            while (true)
            {
                SimulatorResourceModelSP repModel = std::make_shared<SimulatorResourceModel>();

                for (auto & attributeGen : attributeGenList)
                {
                    if (attributeGen->hasNext())
                    {
                        AttributeSP attr = attributeGen->next();
                        int type = attr->getValueType();
                        if (!type)
                        {
                            int attributeValue = attr->getValue<int>();
                            std::string attributeName = attr->getName();
                            repModel->addAttribute(attributeName, attributeValue);
                        }
                        else
                        {
                            std::string attributeValue = attr->getValue<std::string>();
                            std::string attributeName = attr->getName();
                            repModel->addAttribute(attr->getName(), attributeValue);
                        }

                        hasNext = true;
                    }
                    else
                    {
                        AttributeSP attr = attributeGen->previous();
                        int type = attr->getValueType();
                        if (!type)
                        {
                            int attributeValue = attr->getValue<int>();
                            repModel->addAttribute(attr->getName(), attributeValue);
                        }
                        else
                        {
                            std::string attributeValue = attr->getValue<std::string>();
                            repModel->addAttribute(attr->getName(), attributeValue);
                        }
                    }
                }

                if (hasNext)
                {
                    // Send the request
                    m_requestSender->sendRequest(queryParam, repModel,
                                                 std::bind(&PUTRequestGenerator::onResponseReceived, this,
                                                           std::placeholders::_1, std::placeholders::_2), true);

                    m_requestCnt++;
                    hasNext = false;
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }
    while (m_queryParamGen.hasNext());

    m_requestsSent = true;
    completed();
}

void PUTRequestGenerator::onResponseReceived(SimulatorResult result,
        SimulatorResourceModelSP repModel)
{
    OC_LOG_V(INFO, TAG, "Response recieved result:%d", result);
    m_responseCnt++;
    completed();
}

void PUTRequestGenerator::completed()
{
    if (m_requestCnt == m_responseCnt)
    {
        if (m_stopRequested)
        {
            OC_LOG(DEBUG, TAG, "Sending OP_ABORT event");
            m_callback(m_id, OP_ABORT);
        }
        else
        {
            OC_LOG(DEBUG, TAG, "Sending OP_COMPLETE event");
            m_callback(m_id, OP_COMPLETE);
        }
    }
}