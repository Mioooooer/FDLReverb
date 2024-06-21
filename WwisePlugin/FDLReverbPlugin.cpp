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

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <Windows.h> 
#include <codecvt>
#include <locale>
#include <filesystem>
#include <ctype.h>
#include "FDLReverbPlugin.h"
#include "../SoundEnginePlugin/FDLReverbFXFactory.h"
#include <AK/Tools/Common/AkAllocator.h>
#include <AK/Tools/Common/AkFNVHash.h>
#include "RapidJsonInclude/rapidjson/document.h"
#include "RapidJsonInclude/rapidjson/stringbuffer.h"
#include "RapidJsonInclude/rapidjson/writer.h"

using namespace std;
using namespace AK::Wwise::Plugin;

FDLReverbPlugin::FDLReverbPlugin()
{
}

FDLReverbPlugin::~FDLReverbPlugin()
{
}

bool FDLReverbPlugin::GetBankParameters(const GUID & in_guidPlatform, AK::Wwise::Plugin::DataWriter& in_dataWriter) const
{
    // Write bank data here
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Mix"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "IRTable"));
    in_dataWriter.WriteBool(m_propertySet.GetBool(in_guidPlatform, "EnableEqControls"));

    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter1InsertPos"));
    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter1Curve"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter1Gain"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter1Freq"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter1Q"));

    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter2InsertPos"));
    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter2Curve"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter2Gain"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter2Freq"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter2Q"));

    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter3InsertPos"));
    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter3Curve"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter3Gain"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter3Freq"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter3Q"));

    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter4InsertPos"));
    in_dataWriter.WriteInt32(m_propertySet.GetInt32(in_guidPlatform, "Filter4Curve"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter4Gain"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter4Freq"));
    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "Filter4Q"));

    in_dataWriter.WriteReal32(m_propertySet.GetReal32(in_guidPlatform, "IRType"));
    in_dataWriter.WriteBool(m_propertySet.GetBool(in_guidPlatform, "HasSetMedia"));

    return true;
}

void FDLReverbPlugin::NotifyPropertyChanged(const GUID& in_guidPlatform, const char* in_szPropertyName)
{
    // ����������Ƿ�Ϊ "IRType"
    if (strcmp(in_szPropertyName, "IRType") == 0)
    {
        // ֻ�� IRType ����ʱִ�����²���
        soundname = m_propertySet.GetString(in_guidPlatform, "IRType");
        m_propertySet.SetValueBool(in_guidPlatform, "HasSetMedia", false);

        // ����wwise authoring api
        // Ϊ����ʹ���ʹ��Ĭ�Ϸ������
        AK::Wwise::Mallocator alloc;
        AK::Wwise::SafeAllocator<char> szResults(&alloc);
        AK::Wwise::SafeAllocator<char> szError(&alloc);

        // ������ѯ���
        std::ostringstream oss;
        oss << R"(where name = ")" << soundname << R"(")";
        getsfxwaapijs = oss.str();

        // �������ת��
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wGetsfxwaapijs = converter.from_bytes(getsfxwaapijs);

        // ����һ�������ó�������� JSON ����
        rapidjson::Document args;
        rapidjson::Value waqlValue;
        waqlValue.SetString(getsfxwaapijs.c_str(), args.GetAllocator());
        args.SetObject();
        args.AddMember("waql", waqlValue, args.GetAllocator());

        // ����һ���������ز����� JSON ����
        rapidjson::Document conditions;
        conditions.SetObject();

        // ���� JSON ���鲢����Ԫ��
        rapidjson::Value returnArray(rapidjson::kArrayType);
        rapidjson::Document::AllocatorType& allocator = conditions.GetAllocator(); // ��ȡ������
        returnArray.PushBack("name", allocator).PushBack("originalFilePath", allocator);

        // ���������ӵ� conditions ������
        conditions.AddMember("return", returnArray, allocator);

        // �� JSON ��������ַ�����
        rapidjson::StringBuffer args_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> args_writer(args_buffer);
        args.Accept(args_writer);
        rapidjson::StringBuffer conditions_buffer;
        rapidjson::Writer<rapidjson::StringBuffer> conditions_writer(conditions_buffer);
        conditions.Accept(conditions_writer);

        // �� Wwise Authoring API ����Զ�̳������
        m_host.WaapiCall("ak.wwise.core.object.get", args_buffer.GetString(), conditions_buffer.GetString(), alloc, szResults, szError);
        if (!szError)
        {
            // �������json
            rapidjson::Document results;
            results.Parse(szResults);

            if (results.HasMember("return") && results["return"].IsArray() && results["return"].Size() > 0)
            {
                const rapidjson::Value& firstObject = results["return"][0];
                if (firstObject.HasMember("originalFilePath"))
                {
                    originalFilePath = firstObject["originalFilePath"].GetString();
                    std::wstring wOriginalFilePath = converter.from_bytes(originalFilePath);

                    const AkOSChar* akOriginalFilePath = wOriginalFilePath.c_str();
                    m_objectMedia.SetMediaSource(akOriginalFilePath, 0, true);
                    m_propertySet.SetValueBool(in_guidPlatform, "HasSetMedia", true);

                    // ��ȡ�����ý��Դ������
                    unsigned int mediaSourceCount = m_objectMedia.GetMediaSourceCount();
                    std::wostringstream woss;
                    woss << L"Plugin Debug: MediaSourceCount: " << mediaSourceCount << L"\n";

                    // ��ȡ�����ָ��������ý��Դ���ļ���
                    AkOSChar mediaSourceFileName[512];
                    if (m_objectMedia.GetMediaSourceFileName(mediaSourceFileName, 512, 0) > 0)
                    {
                        std::wstring wMediaSourceFileName(mediaSourceFileName);
                    }

                    // ��ȡ�����ָ��������ý��Դ��ԭʼ�ļ�·��
                    AkOSChar mediaSourcePath[512];
                    if (m_objectMedia.GetMediaSourceOriginalFilePath(mediaSourcePath, 512, 0) > 0)
                    {
                        std::wstring wMediaSourcePath(mediaSourcePath);
                    }

                    // ��ȡ�����ԭʼĿ¼
                    AkOSChar originalDirectory[512];
                    if (m_objectMedia.GetOriginalDirectory(originalDirectory, 512) > 0)
                    {
                        std::wstring wOriginalDirectory(originalDirectory);
                    }

                    // ��ȡ�����ת��Ŀ¼
                    AkOSChar convertedDirectory[512];
                    if (m_objectMedia.GetConvertedDirectory(convertedDirectory, 512, in_guidPlatform) > 0)
                    {
                        std::wstring wConvertedDirectory(convertedDirectory);
                    }

                    // Notify Wwise the data needs to be reconverted
                    m_host.NotifyInternalDataChanged(
                        AK::IAkPluginParam::ALL_PLUGIN_DATA_ID,
                        true /* in_bMakeProjectDirty */
                    );
                }
            }
        }
    }
}

ConversionResult FDLReverbPlugin::ConvertFile(
    const GUID& in_guidPlatform,
    const BasePlatformID& in_basePlatform,
    const AkOSChar* in_szSourceFile,
    const AkOSChar* in_szDestFile,
    AkUInt32 in_uSampleRate,
    AkUInt32 in_uBlockLength,
    IProgress* in_pProgress,
    IWriteString* io_pError) const
{
    // Convert the original source to a converted file.
    // At minimum, copy the original file to converted
    std::error_code ec;
    std::filesystem::copy(
        in_szSourceFile,
        in_szDestFile,
        std::filesystem::copy_options::overwrite_existing,
        ec
    );

    if (ec)
    {
        return ConversionFailed;
    }
    return ConversionSuccess;
}

uint32_t FDLReverbPlugin::GetCurrentConversionSettingsHash(
    const GUID& in_guidPlatform,
    AkUInt32 in_uSampleRate,
    AkUInt32 in_uBlockLength) const
{
    AK::FNVHash32 hashFunc;

    // ʹ��Ӱ��ת��Ĳ������ɹ�ϣֵ��
    // ��ȡԴ�ļ����ơ�
    wchar_t szInputFileName[_MAX_PATH];
    auto size = m_objectMedia.GetMediaSourceFileName(szInputFileName, _MAX_PATH, 0);
    if (size == 0)
        return 0;

    for (int i = 0; i < size; ++i)
    {
        szInputFileName[i] = tolower(szInputFileName[i]);
    }
    return hashFunc.Compute(szInputFileName, size);
}

DEFINE_AUDIOPLUGIN_CONTAINER(FDLReverb);											// Create a PluginContainer structure that contains the info for our plugin
EXPORT_AUDIOPLUGIN_CONTAINER(FDLReverb);											// This is a DLL, we want to have a standardized name
ADD_AUDIOPLUGIN_CLASS_TO_CONTAINER(                                             // Add our CLI class to the PluginContainer
    FDLReverb,        // Name of the plug-in container for this shared library
    FDLReverbPlugin,  // Authoring plug-in class to add to the plug-in container
    FDLReverbFX       // Corresponding Sound Engine plug-in class
);
DEFINE_PLUGIN_REGISTER_HOOK

DEFINEDUMMYASSERTHOOK;							// Placeholder assert hook for Wwise plug-ins using AKASSERT (cassert used by default)

