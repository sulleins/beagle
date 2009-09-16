/*
 *  BeagleCPU4StateImpl.cpp
 *  BEAGLE
 *
 * Copyright 2009 Phylogenetic Likelihood Working Group
 *
 * This file is part of BEAGLE.
 *
 * BEAGLE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * BEAGLE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with BEAGLE.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * @author Andrew Rambaut
 * @author Marc Suchard
 * @author Daniel Ayres
 * @author Mark Holder
 */

#ifdef HAVE_CONFIG_H
#include "libhmsbeagle/config.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cassert>

#include "libhmsbeagle/beagle.h"
#include "libhmsbeagle/CPU/BeagleCPUImpl.h"
#include "libhmsbeagle/CPU/BeagleCPU4StateImpl.h"


#ifdef PAD_MATRICES
    #define OFFSET    5    // For easy conversion between 4/5
#else
    #define OFFSET    4
#endif


using namespace beagle;
using namespace beagle::cpu;

#if defined (BEAGLE_IMPL_DEBUGGING_OUTPUT) && BEAGLE_IMPL_DEBUGGING_OUTPUT
const bool DEBUGGING_OUTPUT = true;
#else
const bool DEBUGGING_OUTPUT = false;
#endif

BeagleCPU4StateImpl::~BeagleCPU4StateImpl() {
    // free all that stuff...
    // If you delete partials, make sure not to delete the last element
    // which is TEMP_SCRATCH_PARTIAL twice.
}

///////////////////////////////////////////////////////////////////////////////
// private methods

/*
 * Calculates partial likelihoods at a node when both children have states.
 */
void BeagleCPU4StateImpl::calcStatesStates(double* destP,
                                     const int* states1,
                                     const double* matrices1,
                                     const int* states2,
                                     const double* matrices2) {

    int v = 0;
    int w = 0;

    for (int l = 0; l < kCategoryCount; l++) {

        for (int k = 0; k < kPatternCount; k++) {

            const int state1 = states1[k];
            const int state2 = states2[k];

            destP[v    ] = matrices1[w            + state1] * 
                           matrices2[w            + state2];
            destP[v + 1] = matrices1[w + OFFSET*1 + state1] * 
                           matrices2[w + OFFSET*1 + state2];
            destP[v + 2] = matrices1[w + OFFSET*2 + state1] * 
                           matrices2[w + OFFSET*2 + state2];
            destP[v + 3] = matrices1[w + OFFSET*3 + state1] * 
                           matrices2[w + OFFSET*3 + state2];
           v += 4;
        }
        
        w += OFFSET*4;
    }
}

void BeagleCPU4StateImpl::calcStatesStatesFixedScaling(double* destP,
                                     const int* states1,
                                     const double* matrices1,
                                     const int* states2,
                                     const double* matrices2,
                                     const double* scaleFactors) {
    
    int v = 0;
    int w = 0;
    
    for (int l = 0; l < kCategoryCount; l++) {
        
        for (int k = 0; k < kPatternCount; k++) {
            
            const int state1 = states1[k];
            const int state2 = states2[k];
            const double scaleFactor = scaleFactors[k];
            
            destP[v    ] = matrices1[w            + state1] * 
                           matrices2[w            + state2] / scaleFactor;
            destP[v + 1] = matrices1[w + OFFSET*1 + state1] * 
                           matrices2[w + OFFSET*1 + state2] / scaleFactor;
            destP[v + 2] = matrices1[w + OFFSET*2 + state1] * 
                           matrices2[w + OFFSET*2 + state2] / scaleFactor;
            destP[v + 3] = matrices1[w + OFFSET*3 + state1] * 
                           matrices2[w + OFFSET*3 + state2] / scaleFactor;
            v += 4;
        }
        
        w += OFFSET*4;
    }
}

/*
 * Calculates partial likelihoods at a node when one child has states and one has partials.
 */
void BeagleCPU4StateImpl::calcStatesPartials(double* destP,
                                       const int* states1,
                                       const double* matrices1,
                                       const double* partials2,
                                       const double* matrices2) {

    double sum0, sum1, sum2, sum3;
    int u = 0;
    int v = 0;
    int w = 0;
    
    for (int l = 0; l < kCategoryCount; l++) {
        
        // Prefetch matrix 2
        double m200 = matrices2[w + OFFSET*0 + 0];
        double m201 = matrices2[w + OFFSET*0 + 1];
        double m202 = matrices2[w + OFFSET*0 + 2];
        double m203 = matrices2[w + OFFSET*0 + 3];
        double m210 = matrices2[w + OFFSET*1 + 0];
        double m211 = matrices2[w + OFFSET*1 + 1];
        double m212 = matrices2[w + OFFSET*1 + 2];
        double m213 = matrices2[w + OFFSET*1 + 3];
        double m220 = matrices2[w + OFFSET*2 + 0];
        double m221 = matrices2[w + OFFSET*2 + 1];
        double m222 = matrices2[w + OFFSET*2 + 2];
        double m223 = matrices2[w + OFFSET*2 + 3];
        double m230 = matrices2[w + OFFSET*3 + 0];
        double m231 = matrices2[w + OFFSET*3 + 1];
        double m232 = matrices2[w + OFFSET*3 + 2];
        double m233 = matrices2[w + OFFSET*3 + 3];
        
        for (int k = 0; k < kPatternCount; k++) {
            
            const int state1 = states1[k];
            
            // Prefetch partials
            double p20 = partials2[v + 0];
            double p21 = partials2[v + 1];
            double p22 = partials2[v + 2];
            double p23 = partials2[v + 3];
            
            sum0  =  m200 * p20;
            sum1  =  m210 * p20;
            sum2  =  m220 * p20;
            sum3  =  m230 * p20;
            
            sum0 +=  m201 * p21;
            sum1 +=  m211 * p21;
            sum2 +=  m221 * p21;
            sum3 +=  m231 * p21;
            
            sum0 +=  m202 * p22;
            sum1 +=  m212 * p22;
            sum2 +=  m222 * p22;
            sum3 +=  m232 * p22;
            
            sum0 +=  m203 * p23;
            sum1 +=  m213 * p23;
            sum2 +=  m223 * p23;
            sum3 +=  m233 * p23;
                        
            destP[u    ] = matrices1[w            + state1] * sum0;
            destP[u + 1] = matrices1[w + OFFSET*1 + state1] * sum1;
            destP[u + 2] = matrices1[w + OFFSET*2 + state1] * sum2;
            destP[u + 3] = matrices1[w + OFFSET*3 + state1] * sum3;
            
            v += 4;
            u += 4;

        }
        w += OFFSET*4;
    }
}

void BeagleCPU4StateImpl::calcStatesPartialsFixedScaling(double* destP,
                                       const int* states1,
                                       const double* matrices1,
                                       const double* partials2,
                                       const double* matrices2,
                                       const double* scaleFactors) {
    
    double sum0, sum1, sum2, sum3;
    int u = 0;
    int v = 0;
    int w = 0;
    
    for (int l = 0; l < kCategoryCount; l++) {
        
        // Prefetch matrix 2
        double m200 = matrices2[w + OFFSET*0 + 0];
        double m201 = matrices2[w + OFFSET*0 + 1];
        double m202 = matrices2[w + OFFSET*0 + 2];
        double m203 = matrices2[w + OFFSET*0 + 3];
        double m210 = matrices2[w + OFFSET*1 + 0];
        double m211 = matrices2[w + OFFSET*1 + 1];
        double m212 = matrices2[w + OFFSET*1 + 2];
        double m213 = matrices2[w + OFFSET*1 + 3];
        double m220 = matrices2[w + OFFSET*2 + 0];
        double m221 = matrices2[w + OFFSET*2 + 1];
        double m222 = matrices2[w + OFFSET*2 + 2];
        double m223 = matrices2[w + OFFSET*2 + 3];
        double m230 = matrices2[w + OFFSET*3 + 0];
        double m231 = matrices2[w + OFFSET*3 + 1];
        double m232 = matrices2[w + OFFSET*3 + 2];
        double m233 = matrices2[w + OFFSET*3 + 3];
        
        for (int k = 0; k < kPatternCount; k++) {
            
            const int state1 = states1[k];
            const double scaleFactor = scaleFactors[k];
            
            // Prefetch partials
            double p20 = partials2[v + 0];
            double p21 = partials2[v + 1];
            double p22 = partials2[v + 2];
            double p23 = partials2[v + 3];
            
            sum0  =  m200 * p20;
            sum1  =  m210 * p20;
            sum2  =  m220 * p20;
            sum3  =  m230 * p20;
            
            sum0 +=  m201 * p21;
            sum1 +=  m211 * p21;
            sum2 +=  m221 * p21;
            sum3 +=  m231 * p21;
            
            sum0 +=  m202 * p22;
            sum1 +=  m212 * p22;
            sum2 +=  m222 * p22;
            sum3 +=  m232 * p22;
            
            sum0 +=  m203 * p23;
            sum1 +=  m213 * p23;
            sum2 +=  m223 * p23;
            sum3 +=  m233 * p23;
                    
            destP[u    ] = matrices1[w            + state1] * sum0 / scaleFactor;
            destP[u + 1] = matrices1[w + OFFSET*1 + state1] * sum1 / scaleFactor;
            destP[u + 2] = matrices1[w + OFFSET*2 + state1] * sum2 / scaleFactor;
            destP[u + 3] = matrices1[w + OFFSET*3 + state1] * sum3 / scaleFactor;
            
            v += 4;
            u += 4;
            
        }
        w += OFFSET*4;
    }   
}

void BeagleCPU4StateImpl::calcPartialsPartials(double* destP,
                                         const double* partials1,
                                         const double* matrices1,
                                         const double* partials2,
                                         const double* matrices2) {
    
    int u = 0;
    int v = 0;
    int w = 0;

    for (int l = 0; l < kCategoryCount; l++) {
        
        // Prefetch transition matrices
        double m100 = matrices1[w + OFFSET*0 + 0];
        double m101 = matrices1[w + OFFSET*0 + 1];
        double m102 = matrices1[w + OFFSET*0 + 2];
        double m103 = matrices1[w + OFFSET*0 + 3];
        double m110 = matrices1[w + OFFSET*1 + 0];
        double m111 = matrices1[w + OFFSET*1 + 1];
        double m112 = matrices1[w + OFFSET*1 + 2];
        double m113 = matrices1[w + OFFSET*1 + 3];
        double m120 = matrices1[w + OFFSET*2 + 0];
        double m121 = matrices1[w + OFFSET*2 + 1];
        double m122 = matrices1[w + OFFSET*2 + 2];
        double m123 = matrices1[w + OFFSET*2 + 3];
        double m130 = matrices1[w + OFFSET*3 + 0];
        double m131 = matrices1[w + OFFSET*3 + 1];
        double m132 = matrices1[w + OFFSET*3 + 2];
        double m133 = matrices1[w + OFFSET*3 + 3];
        
        double m200 = matrices2[w + OFFSET*0 + 0];
        double m201 = matrices2[w + OFFSET*0 + 1];
        double m202 = matrices2[w + OFFSET*0 + 2];
        double m203 = matrices2[w + OFFSET*0 + 3];
        double m210 = matrices2[w + OFFSET*1 + 0];
        double m211 = matrices2[w + OFFSET*1 + 1];
        double m212 = matrices2[w + OFFSET*1 + 2];
        double m213 = matrices2[w + OFFSET*1 + 3];
        double m220 = matrices2[w + OFFSET*2 + 0];
        double m221 = matrices2[w + OFFSET*2 + 1];
        double m222 = matrices2[w + OFFSET*2 + 2];
        double m223 = matrices2[w + OFFSET*2 + 3];
        double m230 = matrices2[w + OFFSET*3 + 0];
        double m231 = matrices2[w + OFFSET*3 + 1];
        double m232 = matrices2[w + OFFSET*3 + 2];
        double m233 = matrices2[w + OFFSET*3 + 3];
        
        for (int k = 0; k < kPatternCount; k++) {
            
            double sum10, sum20;
            double sum11, sum21;
            double sum12, sum22;
            double sum13, sum23;
            
            // Prefetch partials
            double p10 = partials1[v + 0];
            double p11 = partials1[v + 1];
            double p12 = partials1[v + 2];
            double p13 = partials1[v + 3];
            
            double p20 = partials2[v + 0];
            double p21 = partials2[v + 1];
            double p22 = partials2[v + 2];
            double p23 = partials2[v + 3];
            
            // Do integration
            sum10  = m100 * p10;
            sum11  = m110 * p10;
            sum12  = m120 * p10;
            sum13  = m130 * p10;
            
            sum10 += m101 * p11;
            sum11 += m111 * p11;
            sum12 += m121 * p11;
            sum13 += m131 * p11;
            
            sum10 += m102 * p12;
            sum11 += m112 * p12;
            sum12 += m122 * p12;
            sum13 += m132 * p12;
            
            sum10 += m103 * p13;
            sum11 += m113 * p13;
            sum12 += m123 * p13;
            sum13 += m133 * p13;
            
            sum20  = m200 * p20;
            sum21  = m210 * p20;
            sum22  = m220 * p20;
            sum23  = m230 * p20;
            
            sum20 += m201 * p21;
            sum21 += m211 * p21;
            sum22 += m221 * p21;
            sum23 += m231 * p21;
            
            sum20 += m202 * p22;
            sum21 += m212 * p22;
            sum22 += m222 * p22;
            sum23 += m232 * p22;
            
            sum20 += m203 * p23;
            sum21 += m213 * p23;
            sum22 += m223 * p23;
            sum23 += m233 * p23;
                        
            // Final results
            destP[u    ] = sum10 * sum20;
            destP[u + 1] = sum11 * sum21;
            destP[u + 2] = sum12 * sum22;
            destP[u + 3] = sum13 * sum23;

            u += 4;
            v += 4;

        }
        w += OFFSET*4;
    }
}

void BeagleCPU4StateImpl::calcPartialsPartialsFixedScaling(double* destP,
                                         const double* partials1,
                                         const double* matrices1,
                                         const double* partials2,
                                         const double* matrices2,
                                         const double* scaleFactors) {
    
    int u = 0;
    int v = 0;
    int w = 0;
    
    for (int l = 0; l < kCategoryCount; l++) {
        
        // Prefetch transition matrices
        double m100 = matrices1[w + OFFSET*0 + 0];
        double m101 = matrices1[w + OFFSET*0 + 1];
        double m102 = matrices1[w + OFFSET*0 + 2];
        double m103 = matrices1[w + OFFSET*0 + 3];
        double m110 = matrices1[w + OFFSET*1 + 0];
        double m111 = matrices1[w + OFFSET*1 + 1];
        double m112 = matrices1[w + OFFSET*1 + 2];
        double m113 = matrices1[w + OFFSET*1 + 3];
        double m120 = matrices1[w + OFFSET*2 + 0];
        double m121 = matrices1[w + OFFSET*2 + 1];
        double m122 = matrices1[w + OFFSET*2 + 2];
        double m123 = matrices1[w + OFFSET*2 + 3];
        double m130 = matrices1[w + OFFSET*3 + 0];
        double m131 = matrices1[w + OFFSET*3 + 1];
        double m132 = matrices1[w + OFFSET*3 + 2];
        double m133 = matrices1[w + OFFSET*3 + 3];
        
        double m200 = matrices2[w + OFFSET*0 + 0];
        double m201 = matrices2[w + OFFSET*0 + 1];
        double m202 = matrices2[w + OFFSET*0 + 2];
        double m203 = matrices2[w + OFFSET*0 + 3];
        double m210 = matrices2[w + OFFSET*1 + 0];
        double m211 = matrices2[w + OFFSET*1 + 1];
        double m212 = matrices2[w + OFFSET*1 + 2];
        double m213 = matrices2[w + OFFSET*1 + 3];
        double m220 = matrices2[w + OFFSET*2 + 0];
        double m221 = matrices2[w + OFFSET*2 + 1];
        double m222 = matrices2[w + OFFSET*2 + 2];
        double m223 = matrices2[w + OFFSET*2 + 3];
        double m230 = matrices2[w + OFFSET*3 + 0];
        double m231 = matrices2[w + OFFSET*3 + 1];
        double m232 = matrices2[w + OFFSET*3 + 2];
        double m233 = matrices2[w + OFFSET*3 + 3];
        
        for (int k = 0; k < kPatternCount; k++) {
            
            double sum10, sum20;
            double sum11, sum21;
            double sum12, sum22;
            double sum13, sum23;
            
            // Prefetch scale factor
            const double scaleFactor = scaleFactors[k];
            
            // Prefetch partials
            double p10 = partials1[v + 0];
            double p11 = partials1[v + 1];
            double p12 = partials1[v + 2];
            double p13 = partials1[v + 3];
            
            double p20 = partials2[v + 0];
            double p21 = partials2[v + 1];
            double p22 = partials2[v + 2];
            double p23 = partials2[v + 3];
            
            // Do integration
            sum10  = m100 * p10;
            sum11  = m110 * p10;
            sum12  = m120 * p10;
            sum13  = m130 * p10;
            
            sum10 += m101 * p11;
            sum11 += m111 * p11;
            sum12 += m121 * p11;
            sum13 += m131 * p11;
            
            sum10 += m102 * p12;
            sum11 += m112 * p12;
            sum12 += m122 * p12;
            sum13 += m132 * p12;
            
            sum10 += m103 * p13;
            sum11 += m113 * p13;
            sum12 += m123 * p13;
            sum13 += m133 * p13;
            
            sum20  = m200 * p20;
            sum21  = m210 * p20;
            sum22  = m220 * p20;
            sum23  = m230 * p20;
            
            sum20 += m201 * p21;
            sum21 += m211 * p21;
            sum22 += m221 * p21;
            sum23 += m231 * p21;
            
            sum20 += m202 * p22;
            sum21 += m212 * p22;
            sum22 += m222 * p22;
            sum23 += m232 * p22;
            
            sum20 += m203 * p23;
            sum21 += m213 * p23;
            sum22 += m223 * p23;
            sum23 += m233 * p23;
                        
            // Final results
            destP[u    ] = sum10 * sum20 / scaleFactor;
            destP[u + 1] = sum11 * sum21 / scaleFactor;
            destP[u + 2] = sum12 * sum22 / scaleFactor;
            destP[u + 3] = sum13 * sum23 / scaleFactor;
            
            u += 4;
            v += 4;
            
        }
        w += OFFSET*4;
    }    
}

void BeagleCPU4StateImpl::calcEdgeLogLikelihoods(const int parIndex,
                                                 const int childIndex,
                                                 const int probIndex,
                                                 const int firstDerivativeIndex,
                                                 const int secondDerivativeIndex,
                                                 const double* inWeights,
                                                 const double* inStateFrequencies,
                                                 const int scalingFactorsIndex,
                                                 double* outLogLikelihoods,
                                                 double* outFirstDerivatives,
                                                 double* outSecondDerivatives) {
    // TODO: implement derivatives for calculateEdgeLnL
    // TODO: implement rate categories for calculateEdgeLnL
    
    assert(parIndex >= kTipCount);
    
    const double* partialsParent = gPartials[parIndex];
    const double* transMatrix = gTransitionMatrices[probIndex];
    const double* wt = inWeights;
    
    memset(integrationTmp, 0, (kPatternCount * kStateCount)*sizeof(double));
    
    if (childIndex < kTipCount && gTipStates[childIndex]) { // Integrate against a state at the child
        
        const int* statesChild = gTipStates[childIndex];    
        int v = 0; // Index for parent partials
        
        for(int l = 0; l < kCategoryCount; l++) {
            int u = 0; // Index in resulting product-partials (summed over categories)
            const double weight = wt[l];
            for(int k = 0; k < kPatternCount; k++) {
                
                const int stateChild = statesChild[k];  // DISCUSSION PT: Does it make sense to change the order of the partials,
                // so we can interchange the patterCount and categoryCount loop order?
                int w =  l * kMatrixSize;
                for(int i = 0; i < kStateCount; i++) {
                    integrationTmp[u] += transMatrix[w + stateChild] * partialsParent[v + i] * weight;
                    u++;
#ifdef PAD_MATRICES
                    w += (kStateCount + 1);
#else
                    w += kStateCount;
#endif
                }
                v += kStateCount;
            }
        }
        
    } else { // Integrate against a partial at the child
        
        const double* partialsChild = gPartials[childIndex];
        int v = 0;
        int w = 0;
        for(int l = 0; l < kCategoryCount; l++) {            
            int u = 0;
            const double weight = wt[l];
            
            const double *matrices1 = transMatrix;
            
            double m100 = matrices1[w + OFFSET*0 + 0]; // Should we force these into registers?
            double m101 = matrices1[w + OFFSET*0 + 1];
            double m102 = matrices1[w + OFFSET*0 + 2];
            double m103 = matrices1[w + OFFSET*0 + 3];
            double m110 = matrices1[w + OFFSET*1 + 0];
            double m111 = matrices1[w + OFFSET*1 + 1];
            double m112 = matrices1[w + OFFSET*1 + 2];
            double m113 = matrices1[w + OFFSET*1 + 3];
            double m120 = matrices1[w + OFFSET*2 + 0];
            double m121 = matrices1[w + OFFSET*2 + 1];
            double m122 = matrices1[w + OFFSET*2 + 2];
            double m123 = matrices1[w + OFFSET*2 + 3];
            double m130 = matrices1[w + OFFSET*3 + 0];
            double m131 = matrices1[w + OFFSET*3 + 1];
            double m132 = matrices1[w + OFFSET*3 + 2];
            double m133 = matrices1[w + OFFSET*3 + 3];
            
            for(int k = 0; k < kPatternCount; k++) {                
                                 
                const double* partials1 = partialsChild;
                
                double p10 = partials1[v    ];
                double p11 = partials1[v + 1];
                double p12 = partials1[v + 2];
                double p13 = partials1[v + 3];
                
                register double sum10, sum11, sum12, sum13;
                
                // Do integration
                sum10  = m100 * p10;
                sum11  = m110 * p10;
                sum12  = m120 * p10;
                sum13  = m130 * p10;
                
                sum10 += m101 * p11;
                sum11 += m111 * p11;
                sum12 += m121 * p11;
                sum13 += m131 * p11;
                
                sum10 += m102 * p12;
                sum11 += m112 * p12;
                sum12 += m122 * p12;
                sum13 += m132 * p12;
                
                sum10 += m103 * p13;
                sum11 += m113 * p13;
                sum12 += m123 * p13;
                sum13 += m133 * p13;

                integrationTmp[u    ] += sum10 * partialsParent[v    ] * weight;
                integrationTmp[u + 1] += sum11 * partialsParent[v + 1] * weight;
                integrationTmp[u + 2] += sum12 * partialsParent[v + 2] * weight;
                integrationTmp[u + 3] += sum13 * partialsParent[v + 3] * weight;
                
                u += 4;
                v += 4;
            } 
            w += OFFSET*4;
        }
    }
    
    register double freq0 = inStateFrequencies[0]; // Is it a good idea to specify 'register'?  
    register double freq1 = inStateFrequencies[1];
    register double freq2 = inStateFrequencies[2];
    register double freq3 = inStateFrequencies[3];
    
    int u = 0;
    for(int k = 0; k < kPatternCount; k++) {
        double sumOverI =
            freq0 * integrationTmp[u    ] +
            freq1 * integrationTmp[u + 1] +
            freq2 * integrationTmp[u + 2] +
            freq3 * integrationTmp[u + 3];
        
        u += 4;        
        outLogLikelihoods[k] = log(sumOverI);
    }        
    
    
    if (scalingFactorsIndex != BEAGLE_OP_NONE) {
        const double* scalingFactors = gScaleBuffers[scalingFactorsIndex];
        for(int k=0; k < kPatternCount; k++)
            outLogLikelihoods[k] += scalingFactors[k];
    }
}

void BeagleCPU4StateImpl::calcRootLogLikelihoods(const int bufferIndex,
                                                const double* inWeights,
                                                const double* inStateFrequencies,
                                                const int scalingFactorsIndex,
                                                double* outLogLikelihoods) {

     // We treat this as a special case so that we don't have convoluted logic
     //      at the end of the loop over patterns
     const double* rootPartials = gPartials[bufferIndex];
     assert(rootPartials);
     const double* wt = inWeights;
     int u = 0;
     int v = 0;
     for (int k = 0; k < kPatternCount; k++) {
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
        integrationTmp[v] = rootPartials[v] * wt[0]; v++;
     }
     for (int l = 1; l < kCategoryCount; l++) {
         u = 0;
         for (int k = 0; k < kPatternCount; k++) {
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
             integrationTmp[u] += rootPartials[v] * wt[l]; u++; v++;
         }
     }
     u = 0;
     for (int k = 0; k < kPatternCount; k++) {
         double sum = inStateFrequencies[0] * integrationTmp[u]; u++;
         sum += inStateFrequencies[1] * integrationTmp[u]; u++;
         sum += inStateFrequencies[2] * integrationTmp[u]; u++;
         sum += inStateFrequencies[3] * integrationTmp[u]; u++;
         outLogLikelihoods[k] = log(sum);   // take the log
     }
     if (scalingFactorsIndex >=0) {
         const double *cumulativeScaleFactors = gScaleBuffers[scalingFactorsIndex];
         for(int k=0; k<kPatternCount; k++)
             outLogLikelihoods[k] += cumulativeScaleFactors[k];
     }
}

///////////////////////////////////////////////////////////////////////////////
// BeagleCPUImplFactory public methods

BeagleImpl* BeagleCPU4StateImplFactory::createImpl(int tipCount,
                                             int partialsBufferCount,
                                             int compactBufferCount,
                                             int stateCount,
                                             int patternCount,
                                             int eigenBufferCount,
                                             int matrixBufferCount,
                                             int categoryCount,
                                             int scaleBufferCount,
                                             long preferenceFlags,
                                             long requirementFlags) {

    if (stateCount != 4) {
        return NULL;
    }

    BeagleImpl* impl = new BeagleCPU4StateImpl();

    try {
        if (impl->createInstance(tipCount, partialsBufferCount, compactBufferCount, stateCount,
                                 patternCount, eigenBufferCount, matrixBufferCount,
                                 categoryCount,scaleBufferCount, preferenceFlags, requirementFlags) == 0)
            return impl;
    }
    catch(...) {
        if (DEBUGGING_OUTPUT)
            std::cerr << "exception in initialize\n";
        delete impl;
        throw;
    }

    delete impl;

    return NULL;
}

const char* BeagleCPU4StateImplFactory::getName() {
    return "CPU-4State";
}

const long BeagleCPU4StateImplFactory::getFlags() {
    return BEAGLE_FLAG_ASYNCH | BEAGLE_FLAG_CPU | BEAGLE_FLAG_DOUBLE;
}

