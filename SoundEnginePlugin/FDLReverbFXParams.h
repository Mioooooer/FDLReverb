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

#ifndef FDLReverbFXParams_H
#define FDLReverbFXParams_H

#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/PluginServices/AkFXParameterChangeHandler.h>

// Add parameters IDs here, those IDs should map to the AudioEnginePropertyID
// attributes in the xml property definition.
static const AkPluginParamID PARAM_MIX_ID = 0;
static const AkPluginParamID PARAM_IRTABLE_ID = 1;
static const AkPluginParamID PARAM_EnableEqControls_ID = 2;

static const AkPluginParamID PARAM_Filter1InsertPos_ID = 3;
static const AkPluginParamID PARAM_Filter1Curve_ID = 4;
static const AkPluginParamID PARAM_Filter1Gain_ID = 5;
static const AkPluginParamID PARAM_Filter1Freq_ID = 6;
static const AkPluginParamID PARAM_Filter1Q_ID = 7;

static const AkPluginParamID PARAM_Filter2InsertPos_ID = 8;
static const AkPluginParamID PARAM_Filter2Curve_ID = 9;
static const AkPluginParamID PARAM_Filter2Gain_ID = 10;
static const AkPluginParamID PARAM_Filter2Freq_ID = 11;
static const AkPluginParamID PARAM_Filter2Q_ID = 12;

static const AkPluginParamID PARAM_Filter3InsertPos_ID = 13;
static const AkPluginParamID PARAM_Filter3Curve_ID = 14;
static const AkPluginParamID PARAM_Filter3Gain_ID = 15;
static const AkPluginParamID PARAM_Filter3Freq_ID = 16;
static const AkPluginParamID PARAM_Filter3Q_ID = 17;

static const AkPluginParamID PARAM_Filter4InsertPos_ID = 18;
static const AkPluginParamID PARAM_Filter4Curve_ID = 19;
static const AkPluginParamID PARAM_Filter4Gain_ID = 20;
static const AkPluginParamID PARAM_Filter4Freq_ID = 21;
static const AkPluginParamID PARAM_Filter4Q_ID = 22;
static const AkUInt32 NUM_PARAMS = 23;

struct FDLReverbRTPCParams
{
    AkReal32 fMix;
    AkReal32 fIRTable;
    bool bEnableEqControls;

    AkInt32 nFilter1InsertPos;
    AkInt32 nFilter1Curve;
    AkReal32 fFilter1Gain;
    AkReal32 fFilter1Freq;
    AkReal32 fFilter1Q;

    AkInt32 nFilter2InsertPos;
    AkInt32 nFilter2Curve;
    AkReal32 fFilter2Gain;
    AkReal32 fFilter2Freq;
    AkReal32 fFilter2Q;

    AkInt32 nFilter3InsertPos;
    AkInt32 nFilter3Curve;
    AkReal32 fFilter3Gain;
    AkReal32 fFilter3Freq;
    AkReal32 fFilter3Q;

    AkInt32 nFilter4InsertPos;
    AkInt32 nFilter4Curve;
    AkReal32 fFilter4Gain;
    AkReal32 fFilter4Freq;
    AkReal32 fFilter4Q;
};

struct FDLReverbNonRTPCParams
{
};

struct FDLReverbFXParams
    : public AK::IAkPluginParam
{
    FDLReverbFXParams();
    FDLReverbFXParams(const FDLReverbFXParams& in_rParams);

    ~FDLReverbFXParams();

    /// Create a duplicate of the parameter node instance in its current state.
    IAkPluginParam* Clone(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Initialize the plug-in parameter node interface.
    /// Initializes the internal parameter structure to default values or with the provided parameter block if it is valid.
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Called by the sound engine when a parameter node is terminated.
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Set all plug-in parameters at once using a parameter block.
    AKRESULT SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Update a single parameter at a time and perform the necessary actions on the parameter changes.
    AKRESULT SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize) override;

    AK::AkFXParameterChangeHandler<NUM_PARAMS> m_paramChangeHandler;

    FDLReverbRTPCParams RTPC;
    FDLReverbNonRTPCParams NonRTPC;
};

#endif // FDLReverbFXParams_H
