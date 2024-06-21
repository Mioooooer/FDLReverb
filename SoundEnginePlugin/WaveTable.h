/*
  ==============================================================================

    WaveTable.h
    Created: 30 Mar 2023 4:27:49pm
    Author:  quanjx

  ==============================================================================
*/

#pragma once
#include <vector>


namespace WaveTable
{
    /** Impulse Response. **/
    class AllWaveTable
    {
    public:
        AllWaveTable() {}

        void setImpulseResponseData(const std::vector<std::vector<std::vector<float>>>& data) {
            tableIR = data;
        }
        std::vector<std::vector<std::vector<float> > > tableIR;
       

    private:
        
    };

}