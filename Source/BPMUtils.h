#pragma once
struct BPMUtils
{
    static double quarterNoteMs(double bpm)
    {
        return (60.0 / bpm) * 1000.0;
    }

    static double noteMs(double bpm, double division)
    {
        return quarterNoteMs(bpm) / division;
    }
};