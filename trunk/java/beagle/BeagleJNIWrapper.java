/*
 * BeagleJNIjava
 *
 */

package beagle;

import java.util.Map;

/*
 * BeagleJNIjava
 *
 * @author Andrew Rambaut
 *
 */

public class BeagleJNIWrapper {
    public static final String LIBRARY_NAME = "hmsbeagle-jni";

    /**
     * private constructor to enforce singleton instance
     */
    private BeagleJNIWrapper() {
    }

    public native ResourceDetails[] getResourceList();

    public native int createInstance(
            int tipCount,
            int partialsBufferCount,
            int compactBufferCount,
            int stateCount,
            int patternCount,
            int eigenBufferCount,
            int matrixBufferCount,
            int categoryCount,
            final int[] resourceList,
            int resourceCount,
            int preferenceFlags,
            int requirementFlags);

    public native int initializeInstance(
						int instance,
						InstanceDetails[] returnInfo);

    public native int finalize(int instance);

    public native int setPartials(int instance, int bufferIndex, final double[] inPartials);

    public native int getPartials(int instance, int bufferIndex, final double[] outPartials);

    public native int setTipStates(int instance, int tipIndex, final int[] inStates);

    public native int setEigenDecomposition(int instance,
                                             int eigenIndex,
                                             final double[] eigenVectors,
                                             final double[] inverseEigenValues,
                                             final double[] eigenValues);

    public native int setCategoryRates(int instance, double[] inCategoryRates);

    public native int setTransitionMatrix(int instance, int matrixIndex, final double[] inMatrix);

    public native int updateTransitionMatrices(int instance, int eigenIndex,
                                               final int[] probabilityIndices,
                                               final int[] firstDerivativeIndices,
                                               final int[] secondDervativeIndices,
                                               final double[] edgeLengths,
                                               int count);

    public native int updatePartials(final int[] instance,
                                     int instanceCount,
                                     final int[] operations,
                                     int operationCount,
                                     int rescale);

    public native int waitForPartials(final int[] instance,
                                     int instanceCount,
                                     final int[] destinationPartials,
                                     int destinationPartialsCount);

    public native int calculateRootLogLikelihoods(int instance,
                                                  final int[] bufferIndices,
                                                  final double[] inWeights,
                                                  final double[] inStateFrequencies,
                                                  final int[] scalingFactorsIndices,
                                                  final int[] scalingFactorsCount,
                                                  int count,
                                                  final double[] outLogLikelihoods);

    public native int calculateEdgeLogLikelihoods(int instance,
                                    final int[] parentBufferIndices,
                                    final int[] childBufferIndices,
                                    final int[] probabilityIndices,
                                    final int[] firstDerivativeIndices,
                                    final int[] secondDerivativeIndices,
                                    final double[] inWeights,
                                    final double[] inStateFrequencies,
                                    final int[] scalingFactorsIndices,
                                    final int[] scalingFactorsCount,
                                    int count,
                                    final double[] outLogLikelihoods,
                                    final double[] outFirstDerivatives,
                                    final double[] outSecondDerivatives);
    /* Library loading routines */

    public static void loadBeagleLibrary() throws UnsatisfiedLinkError {
        System.loadLibrary(LIBRARY_NAME);
        INSTANCE = new BeagleJNIWrapper();
    }

    public static BeagleJNIWrapper INSTANCE;
}