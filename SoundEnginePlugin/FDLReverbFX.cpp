/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "FDLReverbFX.h"
#include "../FDLReverbConfig.h"


AK::IAkPlugin* CreateFDLReverbFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FDLReverbFX());
}

AK::IAkPluginParam* CreateFDLReverbFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FDLReverbFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(FDLReverbFX, AkPluginTypeEffect, FDLReverbConfig::CompanyID, FDLReverbConfig::PluginID)

FDLReverbFX::FDLReverbFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

FDLReverbFX::~FDLReverbFX()
{
}

AKRESULT FDLReverbFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (FDLReverbFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    const auto numChannels = in_rFormat.GetNumChannels();
    nSampleRate = in_rFormat.uSampleRate;

    AkUInt8* pPluginData = NULL;
    AkUInt32 uPluginDataSize;
    in_pContext->GetPluginMedia(0, pPluginData, uPluginDataSize);
    //if (pPluginData == NULL)
    //    return AK_Fail; // û�г弤��Ӧ��ʱ������ʵ���������

    if(pPluginData == NULL)
    {
        irByPass = true; // 没有冲激响应的时候bypass混响部分，为了在wwise创作工具内使用时的方便，此处不返回AK_Fail
        return AK_Success;
    }
    else
    {
        unsigned int drChannels;
        unsigned int drSampleRate;
        drwav_uint64 totalPCMFrameCount;
        float* pSampleData = drwav_open_memory_and_read_pcm_frames_f32(pPluginData,
            uPluginDataSize,
            &drChannels,
            &drSampleRate,
            &totalPCMFrameCount,
            NULL);
        if (pSampleData == NULL)
        {
            // Error opening and reading wav file.
            irByPass = true;
        }
        else 
        {
            irByPass = false;
            // ׼�������� WaveTable::AllWaveTable �е�����
            std::vector<std::vector<std::vector<float>>> impulseResponseData(1);
            int minChannel = (drChannels > numChannels) ? numChannels : drChannels;
            impulseResponseData[0].resize(minChannel);
            for (unsigned int channel = 0; channel < minChannel; ++channel)
            {
                impulseResponseData[0][channel].resize(totalPCMFrameCount);
                for (drwav_uint64 frame = 0; frame < totalPCMFrameCount; ++frame)
                {
                    impulseResponseData[0][channel][frame] = pSampleData[frame * drChannels + channel];
                }
            }

            myWaveTable.setImpulseResponseData(impulseResponseData);

            // ��� drwav ��Ҫ���������ͷŲ�������
            drwav_free(pSampleData, NULL);
        }
    }

    toProcess.resize(numChannels);
    toPass.resize(numChannels);
    reverbVector.resize(numChannels);
    filterVector.resize(numChannels);
    int IRTableIndex = (int)round(m_pParams->RTPC.fIRTable);
    if(IRTableIndex >= myWaveTable.tableIR.size())
    {
        IRTableIndex = myWaveTable.tableIR.size()-1;
    }
    else if(IRTableIndex < 0)
    {
        IRTableIndex = 0;
    }
    for(int i = 0; i < numChannels; i++)
    {
        int IRChannelIndex = i;
        if(IRChannelIndex >= myWaveTable.tableIR[IRTableIndex].size())
        {
            IRChannelIndex = myWaveTable.tableIR[IRTableIndex].size() - 1;
        }
        reverbVector[i].initIRAfterFFTAndInputDelayLine(myWaveTable.tableIR[IRTableIndex][IRChannelIndex]);
        filterVector[i].resize(4);
        filterVector[i][0].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter1Curve), nSampleRate, m_pParams->RTPC.fFilter1Freq, m_pParams->RTPC.fFilter1Q, m_pParams->RTPC.fFilter1Gain);
        filterVector[i][0].bypass = m_pParams->RTPC.nFilter1InsertPos == 0;
        filterVector[i][1].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter2Curve), nSampleRate, m_pParams->RTPC.fFilter2Freq, m_pParams->RTPC.fFilter2Q, m_pParams->RTPC.fFilter2Gain);
        filterVector[i][1].bypass = m_pParams->RTPC.nFilter2InsertPos == 0;
        filterVector[i][2].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter3Curve), nSampleRate, m_pParams->RTPC.fFilter3Freq, m_pParams->RTPC.fFilter3Q, m_pParams->RTPC.fFilter3Gain);
        filterVector[i][2].bypass = m_pParams->RTPC.nFilter3InsertPos == 0;
        filterVector[i][3].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter4Curve), nSampleRate, m_pParams->RTPC.fFilter4Freq, m_pParams->RTPC.fFilter4Q, m_pParams->RTPC.fFilter4Gain);
        filterVector[i][3].bypass = m_pParams->RTPC.nFilter4InsertPos == 0;
    }


    return AK_Success;
}

AKRESULT FDLReverbFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT FDLReverbFX::Reset()
{
    return AK_Success;
}

AKRESULT FDLReverbFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void FDLReverbFX::Execute(AkAudioBuffer* io_pBuffer)
{
    if(irByPass == true)
    {
        return;
    }

    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();

    int IRTableIndex = (int)round(m_pParams->RTPC.fIRTable);
    if(IRTableIndex >= myWaveTable.tableIR.size())
    {
        IRTableIndex = myWaveTable.tableIR.size()-1;
    }
    else if(IRTableIndex < 0)
    {
        IRTableIndex = 0;
    }
    AkUInt32 tailSize = myWaveTable.tableIR[IRTableIndex][0].size();
    m_FXTailHandler.HandleTail( io_pBuffer, tailSize );

    for (AkUInt32 channel = 0; channel < uNumChannels; ++channel)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)io_pBuffer->GetChannel(channel);

        //-----------------------------
        auto numSamples = io_pBuffer->uValidFrames;
        int currentShiftNum = myUtils.getShiftSize();

        // create input vector for stft
        std::vector<double> inputVector;
        std::vector<double> carrierVector;

        // output_signal would be equal to or larger than numSamples due to adding zero when applySTFT.
        std::vector<double> output_signal;


        //make compatible for any buffer size, by compare buffer size to shift num
        if (numSamples >= currentShiftNum)
        {
            
            for (size_t i = 0; i < numSamples; ++i)
            {
                inputVector.emplace_back(pBuf[i]);
            }
            
            reverbVector[channel].applyFDL(inputVector, output_signal);
        }
        else
        {
            for (size_t i = 0; i < numSamples; ++i)
            {
                toProcess[channel].emplace_back(pBuf[i]);
            }
            
            if (toProcess[channel].size() >= currentShiftNum)
            {
                for (size_t i = 0; i < currentShiftNum; ++i)
                {
                    inputVector.emplace_back(toProcess[channel][i]);
                }
                toProcess[channel].erase(toProcess[channel].begin(), toProcess[channel].begin()+currentShiftNum);
                
                reverbVector[channel].applyFDL(inputVector, toPass[channel]);
            }

            if (toPass[channel].size() > 0)
            {
                for (size_t i = 0; i < numSamples; ++i)
                {
                    output_signal.emplace_back(toPass[channel][i]);
                }
                toPass[channel].erase(toPass[channel].begin(), toPass[channel].begin()+numSamples);
            }
            else
            {
                for (size_t i = 0; i < numSamples; ++i)
                {
                    output_signal.emplace_back(0.0);
                }
            }
            
        }

        
        //-----------------update filter setting---------------------
        if(m_pParams->NonRTPC.bHasSetFilter)
        {
            filterVector[channel][0].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter1Curve), nSampleRate, m_pParams->RTPC.fFilter1Freq, m_pParams->RTPC.fFilter1Q, m_pParams->RTPC.fFilter1Gain);
            filterVector[channel][0].bypass = m_pParams->RTPC.nFilter1InsertPos == 0;
            filterVector[channel][1].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter2Curve), nSampleRate, m_pParams->RTPC.fFilter2Freq, m_pParams->RTPC.fFilter2Q, m_pParams->RTPC.fFilter2Gain);
            filterVector[channel][1].bypass = m_pParams->RTPC.nFilter2InsertPos == 0;
            filterVector[channel][2].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter3Curve), nSampleRate, m_pParams->RTPC.fFilter3Freq, m_pParams->RTPC.fFilter3Q, m_pParams->RTPC.fFilter3Gain);
            filterVector[channel][2].bypass = m_pParams->RTPC.nFilter3InsertPos == 0;
            filterVector[channel][3].setCoefficientsByEnum(static_cast<DSP::FilterType>(m_pParams->RTPC.nFilter4Curve), nSampleRate, m_pParams->RTPC.fFilter4Freq, m_pParams->RTPC.fFilter4Q, m_pParams->RTPC.fFilter4Gain);
            filterVector[channel][3].bypass = m_pParams->RTPC.nFilter4InsertPos == 0;
            m_pParams->NonRTPC.bHasSetFilter = false;
        }
        //-----------------update filter setting---------------------
        //------eq here-----------------
        if (m_pParams->RTPC.bEnableEqControls == true)
        {
            for(int filterIndex = 0; filterIndex < 4; ++filterIndex)
            {
                if (!filterVector[channel][filterIndex].bypass)
                {
                    filterVector[channel][filterIndex].process(output_signal, output_signal, numSamples);
                }
            }
        }
        //------eq end------------------
        for (size_t i = 0; i < numSamples; ++i)
        {
            auto mix = m_pParams->RTPC.fMix;
            pBuf[i] = output_signal[i] * mix + pBuf[i] * (1 - mix);
        }
        //-----------------------------

    }


    if (m_pParams->NonRTPC.bHasSetMedia == true)
    {
        AkUInt8* pPluginData = NULL;
        AkUInt32 uPluginDataSize;
        m_pContext->GetPluginMedia(0, pPluginData, uPluginDataSize);
        //if (pPluginData == NULL)
        //    return AK_Fail; // û�г弤��Ӧ��ʱ������ʵ���������
        if (pPluginData != NULL)
        {
            unsigned int drChannels;
            unsigned int drSampleRate;
            drwav_uint64 totalPCMFrameCount;
            float* pSampleData = drwav_open_memory_and_read_pcm_frames_f32(
                pPluginData,
                uPluginDataSize,
                &drChannels,
                &drSampleRate,
                &totalPCMFrameCount,
                NULL);

            if(pSampleData != NULL) 
            {
                // ׼�������� WaveTable::AllWaveTable �е�����
                std::vector<std::vector<std::vector<float>>> impulseResponseData(1);
                int minChannel = (drChannels > uNumChannels) ? uNumChannels : drChannels;
                impulseResponseData[0].resize(minChannel);
                for (unsigned int channel = 0; channel < minChannel; ++channel)
                {
                    impulseResponseData[0][channel].resize(totalPCMFrameCount);
                    for (drwav_uint64 frame = 0; frame < totalPCMFrameCount; ++frame)
                    {
                        impulseResponseData[0][channel][frame] = pSampleData[frame * drChannels + channel];
                    }
                }

                myWaveTable.setImpulseResponseData(impulseResponseData);

                // ��� drwav ��Ҫ���������ͷŲ�������
                drwav_free(pSampleData, NULL);
            }
        }

        //toProcess.resize(uNumChannels);
        //toPass.resize(uNumChannels);
        reverbVector.resize(uNumChannels);
        int IRTableIndex = (int)round(m_pParams->RTPC.fIRTable);
        if(IRTableIndex >= myWaveTable.tableIR.size())
        {
            IRTableIndex = myWaveTable.tableIR.size()-1;
        }
        else if(IRTableIndex < 0)
        {
            IRTableIndex = 0;
        }
        for(int i = 0; i < uNumChannels; i++)
        {
            int IRChannelIndex = i;
            if(IRChannelIndex >= myWaveTable.tableIR[IRTableIndex].size())
            {
                IRChannelIndex = myWaveTable.tableIR[IRTableIndex].size() - 1;
            }
            reverbVector[i].initIRAfterFFTAndInputDelayLine(myWaveTable.tableIR[IRTableIndex][IRChannelIndex]);
        }

        m_pParams->NonRTPC.bHasSetMedia = false;
    }
}

AKRESULT FDLReverbFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
