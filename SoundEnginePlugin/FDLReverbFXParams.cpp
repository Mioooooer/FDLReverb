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

#include "FDLReverbFXParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

FDLReverbFXParams::FDLReverbFXParams()
{
}

FDLReverbFXParams::~FDLReverbFXParams()
{
}

FDLReverbFXParams::FDLReverbFXParams(const FDLReverbFXParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* FDLReverbFXParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, FDLReverbFXParams(*this));
}

AKRESULT FDLReverbFXParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default parameters here
        RTPC.fMix = 0.0f;
        RTPC.fIRTable = 0.0f;
        RTPC.bEnableEqControls = false;

        RTPC.nFilter1InsertPos = 0;
        RTPC.nFilter1Curve = 0;
        RTPC.fFilter1Gain = 0.0;
        RTPC.fFilter1Freq = 100.0;
        RTPC.fFilter1Q = 1.0;

        RTPC.nFilter2InsertPos = 0;
        RTPC.nFilter2Curve = 1;
        RTPC.fFilter2Gain = 0.0;
        RTPC.fFilter2Freq = 300.0;
        RTPC.fFilter2Q = 1.0;

        RTPC.nFilter3InsertPos = 0;
        RTPC.nFilter3Curve = 1;
        RTPC.fFilter3Gain = 0.0;
        RTPC.fFilter3Freq = 1000.0;
        RTPC.fFilter3Q = 1.0;

        RTPC.nFilter4InsertPos = 0;
        RTPC.nFilter4Curve = 2;
        RTPC.fFilter4Gain = 0.0;
        RTPC.fFilter4Freq = 5000.0;
        RTPC.fFilter4Q = 1.0;

        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT FDLReverbFXParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT FDLReverbFXParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    RTPC.fMix = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fIRTable = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.bEnableEqControls = READBANKDATA(bool, pParamsBlock, in_ulBlockSize);

    RTPC.nFilter1InsertPos = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.nFilter1Curve = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter1Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter1Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter1Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.nFilter2InsertPos = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.nFilter2Curve = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter2Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter2Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter2Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.nFilter3InsertPos = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.nFilter3Curve = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter3Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter3Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter3Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.nFilter4InsertPos = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.nFilter4Curve = READBANKDATA(AkInt32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter4Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter4Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.fFilter4Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT FDLReverbFXParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case PARAM_MIX_ID:
        RTPC.fMix = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_MIX_ID);
        break;
    case PARAM_IRTABLE_ID:
        RTPC.fIRTable = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_IRTABLE_ID);
        break;
    case PARAM_EnableEqControls_ID:
        RTPC.bEnableEqControls = *(bool*)(in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_EnableEqControls_ID);
        break;

    case PARAM_Filter1InsertPos_ID:
        RTPC.nFilter1InsertPos = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter1InsertPos_ID);
        break;
    case PARAM_Filter1Curve_ID:
        RTPC.nFilter1Curve = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter1Curve_ID);
        break;
    case PARAM_Filter1Gain_ID:
        RTPC.fFilter1Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter1Gain_ID);
        break;
    case PARAM_Filter1Freq_ID:
        RTPC.fFilter1Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter1Freq_ID);
        break;
    case PARAM_Filter1Q_ID:
        RTPC.fFilter1Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter1Q_ID);
        break;
    
    case PARAM_Filter2InsertPos_ID:
        RTPC.nFilter2InsertPos = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter2InsertPos_ID);
        break;
    case PARAM_Filter2Curve_ID:
        RTPC.nFilter2Curve = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter2Curve_ID);
        break;
    case PARAM_Filter2Gain_ID:
        RTPC.fFilter2Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter2Gain_ID);
        break;
    case PARAM_Filter2Freq_ID:
        RTPC.fFilter2Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter2Freq_ID);
        break;
    case PARAM_Filter2Q_ID:
        RTPC.fFilter2Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter2Q_ID);
        break;

    case PARAM_Filter3InsertPos_ID:
        RTPC.nFilter3InsertPos = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter3InsertPos_ID);
        break;
    case PARAM_Filter3Curve_ID:
        RTPC.nFilter3Curve = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter3Curve_ID);
        break;
    case PARAM_Filter3Gain_ID:
        RTPC.fFilter3Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter3Gain_ID);
        break;
    case PARAM_Filter3Freq_ID:
        RTPC.fFilter3Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter3Freq_ID);
        break;
    case PARAM_Filter3Q_ID:
        RTPC.fFilter3Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter3Q_ID);
        break;

    case PARAM_Filter4InsertPos_ID:
        RTPC.nFilter4InsertPos = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter4InsertPos_ID);
        break;
    case PARAM_Filter4Curve_ID:
        RTPC.nFilter4Curve = *((AkInt32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter4Curve_ID);
        break;
    case PARAM_Filter4Gain_ID:
        RTPC.fFilter4Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter4Gain_ID);
        break;
    case PARAM_Filter4Freq_ID:
        RTPC.fFilter4Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter4Freq_ID);
        break;
    case PARAM_Filter4Q_ID:
        RTPC.fFilter4Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(PARAM_Filter4Q_ID);
        break;

    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
