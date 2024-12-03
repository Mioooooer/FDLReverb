/*
  ==============================================================================

    MyDSP.h
    Created: 30 Mar 2023 3:58:58pm
    Author:  quanjx

  ==============================================================================
*/

#pragma once
#include <array>
#include <deque>
#include <cmath>
#include "STFT/STFT.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace DSP
{
    enum class FilterType
    {
        LowShelf = 0,
        Peaking,
        HighShelf,
        LowPass,
        BandPass,
        BandPassConstGain,
        HighPass,
        Notch,
        AllPass
    };
    /** iir filter with any Q-factor and Gain. */
    class IIRFilter
    {
    public:
        IIRFilter() : b0(1.0f), b1(0.0f), b2(0.0f), a1(0.0f), a2(0.0f), bypass(true), state({0, 0}) {}

        bool bypass;
        
        void setLPFCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = (1 - cos_omega) / 2 * scale;
            b1 = (1 - cos_omega) * scale;
            b2 = b0;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }

        void setHPFCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = (1 + cos_omega) / 2 * scale;
            b1 = -(1 + cos_omega) * scale;
            b2 = b0;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }
        
        void setConstGainBPFCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = alpha * scale;
            b1 = 0;
            b2 = -alpha * scale;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }

        void setBPFCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = Q * alpha * scale;
            b1 = 0;
            b2 = -alpha * scale * Q;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }

        void setNotchCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = scale;
            b1 = -2 * cos_omega * scale;
            b2 = b0;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }

        void setAPFCoefficients(double sampleRate, float frequency, float Q)
        {
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha);

            b0 = (1 - alpha) * scale;
            b1 = -2 * cos_omega * scale;
            b2 = 1;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha) * scale;
        }

        void setPeakingCoefficients(double sampleRate, float frequency, float Q, float gain)
        {
            const auto A = powf(10, gain/20);//audio eq cookbook use gain/40, should be fine with gain/20
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / (1 + alpha/A);

            b0 = (1 + A * alpha) * scale;
            b1 = -2 * cos_omega * scale;
            b2 = (1 - A * alpha) * scale;
            a1 = -2 * cos_omega * scale;
            a2 = (1 - alpha / A) * scale;
        }

        void setLowShelfCoefficients(double sampleRate, float frequency, float Q, float gain)
        {
            const auto A = powf(10, gain/20);//audio eq cookbook use gain/40, should be fine with gain/20
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / ((A + 1) + (A - 1) * cos_omega + 2 * alpha * powf(A, 0.5));

            b0 = A * ((A + 1) - (A - 1) * cos_omega + 2 * alpha * powf(A, 0.5)) * scale;
            b1 = 2 * A * ((A - 1) - (A + 1) * cos_omega) * scale;
            b2 = A * ((A + 1) - (A - 1) * cos_omega - 2 * alpha * powf(A, 0.5)) * scale;
            a1 = -2 * ((A - 1) + (A + 1) * cos_omega) * scale;
            a2 = ((A + 1) + (A - 1) * cos_omega - 2 * alpha * powf(A, 0.5)) * scale;
        }

        void setHighShelfCoefficients(double sampleRate, float frequency, float Q, float gain)
        {
            const auto A = powf(10, gain/20);//audio eq cookbook use gain/40, should be fine with gain/20
            const auto omega = 2 * (float)M_PI * (frequency / sampleRate);
            const auto sin_omega = std::sin(omega);
            const auto cos_omega = std::cos(omega);
            const auto alpha = sin_omega / (2 * Q);

            const auto scale = 1 / ((A + 1) - (A - 1) * cos_omega + 2 * alpha * powf(A, 0.5));

            b0 = A * ((A + 1) + (A - 1) * cos_omega + 2 * alpha * powf(A, 0.5)) * scale;
            b1 = -2 * A * ((A - 1) + (A + 1) * cos_omega) * scale;
            b2 = A * ((A + 1) + (A - 1) * cos_omega - 2 * alpha * powf(A, 0.5)) * scale;
            a1 = 2 * ((A - 1) - (A + 1) * cos_omega) * scale;
            a2 = ((A + 1) - (A - 1) * cos_omega - 2 * alpha * powf(A, 0.5)) * scale;
        }

        void setCoefficientsByEnum(FilterType eFilterType, double sampleRate, float frequency, float Q, float gain)
        {
            switch (eFilterType)
            {
            case FilterType::LowShelf:
                setLowShelfCoefficients(sampleRate, frequency, Q, gain);
                break;
            case FilterType::Peaking:
                setPeakingCoefficients(sampleRate, frequency, Q, gain);
                break;
            case FilterType::HighShelf:
                setHighShelfCoefficients(sampleRate, frequency, Q, gain);
                break;
            case FilterType::LowPass:
                setLPFCoefficients(sampleRate, frequency, Q);
                break;
            case FilterType::BandPass:
                setBPFCoefficients(sampleRate, frequency, Q);
                break;
            case FilterType::BandPassConstGain:
                setConstGainBPFCoefficients(sampleRate, frequency, Q);
                break;
            case FilterType::HighPass:
                setHPFCoefficients(sampleRate, frequency, Q);
                break;
            case FilterType::Notch:
                setNotchCoefficients(sampleRate, frequency, Q);
                break;
            case FilterType::AllPass:
                setAPFCoefficients(sampleRate, frequency, Q);
                break;
            default:
                break;
            }
        }

        void process(float *src, float *dst, int numSamples)
        {
            auto lv1 = state[0];
            auto lv2 = state[1];
            for (size_t i = 0; i < numSamples; ++i)
            {
                const auto input = src[i];
                const auto output = (b0 * input) + lv1;
                dst[i] = output;

                lv1 = (b1 * input) - (a1 * output) + lv2;
                lv2 = (b2 * input) - (a2 * output);
            }
            state[0] = lv1;
            state[1] = lv2;
        }

        void process(std::deque<double>& src, std::deque<double>& dst, int numSamples)
        {
            auto lv1 = state[0];
            auto lv2 = state[1];
            for (size_t i = 0; i < numSamples; ++i)
            {
                const auto input = src[i];
                const auto output = (b0 * input) + lv1;
                dst[i] = output;

                lv1 = (b1 * input) - (a1 * output) + lv2;
                lv2 = (b2 * input) - (a2 * output);
            }
            state[0] = lv1;
            state[1] = lv2;
        }


    private:
        /** Filter coefficients. */
        float b0, b1, b2, a1, a2;
        /** Filter state between audio buffer. */
        std::array<float, 2> state;
    };

    class MyUtils
    {
    public:
        MyUtils() {
        }

        float calculateRMS(const float *data, int numSamples)
        {
            if (numSamples <= 0)
            {
                return 0;
            }

            double sum = 0.0;
            for (size_t i = 0; i < numSamples; i++)
            {
                auto sample = data[i];
                sum += sample * sample;
            }

            return std::sqrt(sum / numSamples);
        }

        float calculateRMS(std::deque<double>& data, int numSamples)
        {
            if (numSamples <= 0)
            {
                return 0;
            }

            double sum = 0.0;
            for (size_t i = 0; i < numSamples; i++)
            {
                auto sample = data[i];
                sum += sample * sample;
            }

            return std::sqrt(sum / numSamples);
        }

        void complexMultiply(double a, double b, double c, double d, double& e, double& f)
        {
            e = a*c - b*d;
            f = a*d + b*c;
        }

        void applySTFT(std::deque<double>& bufferin, std::deque<double>& bufferout)
        {
            double* tempIn = new double[shift];
            double* tempOut = new double[frame + 2];
            int restNum = bufferin.size() % shift;
            if (restNum != 0)
            {
                for (int i = 0; i < (shift - restNum); i++)
                {
                    bufferin.emplace_back(0.0);
                }
            }
            
            int numShift = bufferin.size() / shift;
            for (int i = 0; i < numShift; i++)
            {
                for (int j = 0; j < shift; j++)
                {
                    tempIn[j] = bufferin[i*shift+j];
                }
                mySTFT.stft(tempIn, tempOut);
                for(int k = 0; k < (frame+2); k++)
                {
                    bufferout.emplace_back(tempOut[k]);
                }
            }
            delete[] tempIn;
            delete[] tempOut;
            
        }

        void applyISTFT(std::deque<double>& bufferin, std::deque<double>& bufferout)
        {
            // bufferout would be equal to or larger than sampleNum due to adding zero when applySTFT.
            double* tempIn = new double[frame + 2];
            double* tempOut = new double[shift];
            int numFrame = bufferin.size() / (frame+2);
            for(int i = 0; i < numFrame; i++)
            {
                for (int j = 0; j < (frame + 2); j++)
                {
                    tempIn[j] = bufferin[i*(frame + 2) + j];
                }
                mySTFT.istft(tempIn, tempOut);
                for (int k = 0; k < shift; k++)
                {
                    bufferout.emplace_back(tempOut[k]);
                }
            }
            delete[] tempIn;
            delete[] tempOut;

        }

        void applyVocoder(std::deque<double>& carrierIn, std::deque<double>& modulatorIn, std::deque<double>& vocoderOut)
        {
            std::deque<double> carrierSTFT;
            std::deque<double> modulatorSTFT;
            std::deque<double> multipliedSTFT;
            applySTFT(carrierIn, carrierSTFT);
            applySTFT(modulatorIn, modulatorSTFT);
            for(int i = 0; i < carrierSTFT.size(); i = i + 2)//complexMultiply
            {
                multipliedSTFT.emplace_back(carrierSTFT[i] * modulatorSTFT[i] - carrierSTFT[i+1] * modulatorSTFT[i+1]);
                multipliedSTFT.emplace_back(carrierSTFT[i] * modulatorSTFT[i+1] + carrierSTFT[i+1] * modulatorSTFT[i]);
            }
            applyISTFT(multipliedSTFT, vocoderOut);
        }

        int getFrameSize(void)
        {
            return frame;
        }

        int getShiftSize(void)
        {
            return shift;
        }

        void applyFDL(std::deque<double>& waveIn, std::deque<double>& FDLOut)
        {
            //std::deque<double> carrierSTFT;
            std::deque<double> waveSTFT;
            std::deque<double> multipliedSTFT;
            //applySTFT(IRIn, carrierSTFT);
            applySTFT(waveIn, waveSTFT);
            int numFrame = waveSTFT.size() / (frame+2);
            for(int i = 0; i < numFrame; i++)
            {
                pushToInputDelayLine(waveSTFT);
                calculateComplexMult(multipliedSTFT);
            }
            applyISTFT(multipliedSTFT, FDLOut);
        }

        void pushToInputDelayLine(std::deque<double>& bufferin)
        {
            std::deque<double> tempIn;
            for (int i = 0; i < (frame + 2); i++)
            {
                tempIn.emplace_back(bufferin[i]);
            }
            bufferin.erase(bufferin.begin(), bufferin.begin()+(frame + 2));
            InputDelayLine.emplace_back(tempIn);
            InputDelayLine.erase(InputDelayLine.begin(), InputDelayLine.begin()+1);
        }

        void calculateComplexMult(std::deque<double>& outputBuffer)
        {
            std::deque<double> tempBuffer;
            tempBuffer.resize(frame+2, 0.0);
            for(int i = 0; i < IRAfterFFT.size(); i++)
            {
                for(int j = 0; j < tempBuffer.size(); j = j + 2)
                {
                    tempBuffer[j] += InputDelayLine[IRAfterFFT.size()-1-i][j] * IRAfterFFT[i][j] - InputDelayLine[IRAfterFFT.size()-1-i][j+1] * IRAfterFFT[i][j+1];
                    tempBuffer[j+1] += InputDelayLine[IRAfterFFT.size()-1-i][j] * IRAfterFFT[i][j+1] + InputDelayLine[IRAfterFFT.size()-1-i][j+1] * IRAfterFFT[i][j];
                }
            }
            for(int i = 0; i < tempBuffer.size(); i++)
            {
                outputBuffer.emplace_back(tempBuffer[i]);
            }
        }

        void initIRAfterFFTAndInputDelayLine(std::deque<float> waveTable)
        {
            double* tempIn = new double[frame];
            for (int m = 0; m < frame; m++)
            {
                tempIn[m] = 0.0;
            }
            double* tempOut = new double[frame + 2];
            int restNum = waveTable.size() % shift;
            if (restNum != 0)
            {
                for (int i = 0; i < (shift - restNum); i++)
                {
                    waveTable.emplace_back(0.0);
                }
            }
            
            int numShift = waveTable.size() / shift;
            IRAfterFFT.resize(numShift);
            InputDelayLine.resize(numShift);
            for (int i = 0; i < numShift; i++)
            {
                for (int j = 0; j < shift; j++)
                {
                    tempIn[j] = waveTable[i*shift+j];
                }
                tableSTFT.stft(tempIn, tempOut);
                for(int k = 0; k < (frame+2); k++)
                {
                    IRAfterFFT[i].emplace_back(tempOut[k]);
                    InputDelayLine[i].emplace_back(0.0);
                }
            }
            delete[] tempIn;
            delete[] tempOut;
        }

        void reinitIRAndInputDelayLine(std::deque<float> waveTable)
        {
            IRAfterFFT.clear();
            InputDelayLine.clear();
            initIRAfterFFTAndInputDelayLine(waveTable);
        }
        
        

    private:
        const int ch = 1;
        //const int frame = 512;
        //const int shift = 128;
        const int frame = 1024;
        const int shift = 256;
        STFT mySTFT = STFT(ch,frame,shift);
        STFT tableSTFT = STFT(ch,frame,frame);
        std::deque<std::deque<double> > IRAfterFFT;
        std::deque<std::deque<double> > InputDelayLine;

    };

    

}
