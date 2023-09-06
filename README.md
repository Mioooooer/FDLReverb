# FDLReverb
A Wwise Plugin convolutional reverb.
It's basicly a simplified convolutional reverb which contains IR data itself.
The IR is converted directly from .wav file to float by python scripts.

TODO:
1. Up or down sampling function and drag to substitude IR function.
2. Dry data not delayed when input data not enough, but wet data delayed, and it will cause the delay time between wet and dry sound increasing.
