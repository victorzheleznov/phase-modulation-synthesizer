#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Operator.h"

class Algorithm
{
public:
    float process (Operator* ops, bool* isOutput)
    {
        float algorithmOut = 0.0f;
        float opSampleA, opSampleB, opSampleC, opSampleD;
        for (int i = 0; i < numOperators; i++)
            isOutput[i] = false;
        switch (algorithm)
        {
        case 0: // D -> C -> B -> A
            // specify output operators
            isOutput[0] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleC);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset (opSampleB);
            algorithmOut = ops[0].process();
            break;
        case 1:
            // specify output operators
            isOutput[0] = true;
            // process algorithm
            opSampleD = ops[3].process();
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset ((opSampleC + opSampleD) / 2);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset (opSampleB);
            algorithmOut = ops[0].process();
            break;
        case 2:
            // specify output operators
            isOutput[0] = true;
            // process algorithm
            opSampleD = ops[3].process();
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleC);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset ((opSampleB + opSampleD) / 2);
            algorithmOut = ops[0].process();
            break;
        case 3:
            // specify output operators
            isOutput[0] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleD);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset ((opSampleB + opSampleC) / 2);
            algorithmOut = ops[0].process();
            break;
        case 4:
            // specify output operators
            isOutput[0] = true;
            isOutput[1] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleC);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset (opSampleC);
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleB) / 2;
            break;
        case 5:
            // specify output operators
            isOutput[0] = true;
            isOutput[1] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleC);
            opSampleB = ops[1].process();
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleB) / 2;
            break;
        case 6:
            // specify output operators
            isOutput[0] = true;
            // process algorithm
            opSampleD = ops[3].process();
            opSampleC = ops[2].process();
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset ((opSampleB + opSampleC + opSampleD) / 3);
            algorithmOut = ops[0].process();
            break;
        case 7:
            // specify output operators
            isOutput[0] = true;
            isOutput[2] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset (opSampleB);
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleC) / 2;
            break;
        case 8:
            // specify output operators
            isOutput[0] = true;
            isOutput[1] = true;
            isOutput[2] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            ops[1].setOscPhaseOffset (opSampleD);
            opSampleB = ops[1].process();
            ops[0].setOscPhaseOffset (opSampleD);
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleB + opSampleC) / 3;
            break;
        case 9:
            // specify output operators
            isOutput[0] = true;
            isOutput[1] = true;
            isOutput[2] = true;
            // process algorithm
            opSampleD = ops[3].process();
            ops[2].setOscPhaseOffset (opSampleD);
            opSampleC = ops[2].process();
            opSampleB = ops[1].process();
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleB + opSampleC) / 3;
            break;
        case 10:
            // specify output operators
            isOutput[0] = true;
            isOutput[1] = true;
            isOutput[2] = true;
            isOutput[3] = true;
            // process algorithm
            opSampleD = ops[3].process();
            opSampleC = ops[2].process();
            opSampleB = ops[1].process();
            opSampleA = ops[0].process();
            algorithmOut = (opSampleA + opSampleB + opSampleC + opSampleD) / 4;
            break;
        }
        return algorithmOut;
    }

    void startNote (Parameters* _param)
    {
        algorithm = *_param->algorithm;
        numOperators = _param->numOperators;
    }
private:
    int algorithm;
    int numOperators;
};

#endif // ALGORITHM_H
