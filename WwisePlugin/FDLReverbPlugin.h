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

#pragma once

#include <AK/Wwise/Plugin.h>
#include <string>
#include <AK/Wwise/Plugin/PluginDef.h>

using namespace std;
using namespace AK::Wwise::Plugin;

/// See https://www.audiokinetic.com/library/edge/?source=SDK&id=plugin__dll.html
/// for the documentation about Authoring plug-ins
class FDLReverbPlugin final
    : public AK::Wwise::Plugin::AudioPlugin
    , public AK::Wwise::Plugin::MediaConverter
    , public AK::Wwise::Plugin::RequestObjectMedia
    , public AK::Wwise::Plugin::Notifications::ObjectMedia
    , public AK::Wwise::Plugin::RequestHost
{
public:
    FDLReverbPlugin();
    ~FDLReverbPlugin();

    /// This function is called by Wwise to obtain parameters that will be written to a bank.
    /// Because these can be changed at run-time, the parameter block should stay relatively small.
    // Larger data should be put in the Data Block.
    bool GetBankParameters(const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter) const override;
    void NotifyPropertyChanged(const GUID& in_guidPlatform, const char* in_szPropertyName) override;
    ConversionResult ConvertFile(
        const GUID& in_guidPlatform,
        const BasePlatformID& in_basePlatform,
        const AkOSChar* in_szSourceFile,
        const AkOSChar* in_szDestFile,
        AkUInt32 in_uSampleRate,
        AkUInt32 in_uBlockLength,
        IProgress* in_pProgress,
        IWriteString* io_pError) const override;
    uint32_t GetCurrentConversionSettingsHash(
        const GUID& in_guidPlatform,
        AkUInt32 in_uSampleRate,
        AkUInt32 in_uBlockLength) const override;

private:
    string soundname = "Default";
    string getsfxwaapi = "\"\\Actor - Mixer Hierarchy\\Default Work Unit\\IR_wavs\" select children where originalFilePath : \"" + soundname + "\"";
    string getsfxwaapijs = "\\Actor - Mixer Hierarchy\\Default Work Unit\\IR_wavs select children where originalFilePath : \"" + soundname + "\"";
    string originalFilePath;
    wstring wOriginalFilePath;
};


DECLARE_AUDIOPLUGIN_CONTAINER(FDLReverb);	// Exposes our PluginContainer structure that contains the info for our plugin
