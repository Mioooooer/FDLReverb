/*
  ==============================================================================

    WaveTable.h
    Created: 30 Mar 2023 4:27:49pm
    Author:  quanjx

  ==============================================================================
*/

#pragma once
#include <deque>


namespace WaveTable
{
    /** Impulse Response. **/
    class AllWaveTable
    {
    public:
        AllWaveTable() {}

        void setImpulseResponseData(const std::deque<std::deque<std::deque<float>>>& data) {
            tableIR = data;
        }
        std::deque<std::deque<std::deque<float> > > tableIR;
       

    private:
        
    };

}