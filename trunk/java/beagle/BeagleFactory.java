/**
 *
 */
package beagle;

/**
 * @author Marc Suchard
 * @author Andrew Rambaut
 *
 */
public class BeagleFactory {

    public static ResourceDetails[] getResourceDetails() {
        if (BeagleJNIWrapper.INSTANCE == null) {
            try {
                BeagleJNIWrapper.loadBeagleLibrary();
                System.err.println("BEAGLE library loaded");

            } catch (UnsatisfiedLinkError ule) {
                System.err.println("Failed to load BEAGLE library: " + ule.getMessage());
            }
        }

        return BeagleJNIWrapper.INSTANCE.getResourceList();
    }

    public static Beagle loadBeagleInstance(
            int tipCount,
            int partialsBufferCount,
            int compactBufferCount,
            int stateCount,
            int patternCount,
            int eigenBufferCount,
            int matrixBufferCount,
            int categoryCount,
            int scaleBufferCount
    ) {

        boolean forceJava = Boolean.valueOf(System.getProperty("java_only"));

        if (BeagleJNIWrapper.INSTANCE == null) {
            try {
                BeagleJNIWrapper.loadBeagleLibrary();
                System.err.println("BEAGLE library loaded");

            } catch (UnsatisfiedLinkError ule) {
                System.err.println("Failed to load BEAGLE library: " + ule.getMessage());
            }
        }

        if (!forceJava && BeagleJNIWrapper.INSTANCE != null) {

            // 0 = CPU, > 0 are CUDA-devices
            // at the moment the order determines which device the instance is put on
            // because the preference flag is ignored
            int[] resourceList = new int[] { 1, 0 };

            return new BeagleJNIImpl(
                    tipCount,
                    partialsBufferCount,
                    compactBufferCount,
                    stateCount,
                    patternCount,
                    eigenBufferCount,
                    matrixBufferCount,
                    categoryCount,
                    scaleBufferCount,
                    resourceList,
                    BeagleFlag.GPU.getMask(),
                    0
            );
        }

        if (stateCount == 4) {
//            return new DependencyAwareBeagleImpl();
            return new FourStateBeagleImpl(
                    tipCount,
                    partialsBufferCount,
                    compactBufferCount,
                    patternCount,
                    eigenBufferCount,
                    matrixBufferCount,
                    categoryCount,
                    scaleBufferCount
            );
        }

        return new GeneralBeagleImpl(tipCount,
                partialsBufferCount,
                compactBufferCount,
                stateCount,
                patternCount,
                eigenBufferCount,
                matrixBufferCount,
                categoryCount,
                scaleBufferCount
        );
    }

    // Code and constants for test main()

    private final static String human = "AGAAATATGTCTGATAAAAGAGTTACTTTGATAGAGTAAATAATAGGAGCTTAAACCCCCTTATTTCTACTAGGACTATGAGAATCGAACCCATCCCTGAGAATCCAAAATTCTCCGTGCCACCTATCACACCCCATCCTAAGTAAGGTCAGCTAAATAAGCTATCGGGCCCATACCCCGAAAATGTTGGTTATACCCTTCCCGTACTAAGAAATTTAGGTTAAATACAGACCAAGAGCCTTCAAAGCCCTCAGTAAGTTG-CAATACTTAATTTCTGTAAGGACTGCAAAACCCCACTCTGCATCAACTGAACGCAAATCAGCCACTTTAATTAAGCTAAGCCCTTCTAGACCAATGGGACTTAAACCCACAAACACTTAGTTAACAGCTAAGCACCCTAATCAAC-TGGCTTCAATCTAAAGCCCCGGCAGG-TTTGAAGCTGCTTCTTCGAATTTGCAATTCAATATGAAAA-TCACCTCGGAGCTTGGTAAAAAGAGGCCTAACCCCTGTCTTTAGATTTACAGTCCAATGCTTCA-CTCAGCCATTTTACCACAAAAAAGGAAGGAATCGAACCCCCCAAAGCTGGTTTCAAGCCAACCCCATGGCCTCCATGACTTTTTCAAAAGGTATTAGAAAAACCATTTCATAACTTTGTCAAAGTTAAATTATAGGCT-AAATCCTATATATCTTA-CACTGTAAAGCTAACTTAGCATTAACCTTTTAAGTTAAAGATTAAGAGAACCAACACCTCTTTACAGTGA";
    private final static String chimp = "AGAAATATGTCTGATAAAAGAATTACTTTGATAGAGTAAATAATAGGAGTTCAAATCCCCTTATTTCTACTAGGACTATAAGAATCGAACTCATCCCTGAGAATCCAAAATTCTCCGTGCCACCTATCACACCCCATCCTAAGTAAGGTCAGCTAAATAAGCTATCGGGCCCATACCCCGAAAATGTTGGTTACACCCTTCCCGTACTAAGAAATTTAGGTTAAGCACAGACCAAGAGCCTTCAAAGCCCTCAGCAAGTTA-CAATACTTAATTTCTGTAAGGACTGCAAAACCCCACTCTGCATCAACTGAACGCAAATCAGCCACTTTAATTAAGCTAAGCCCTTCTAGATTAATGGGACTTAAACCCACAAACATTTAGTTAACAGCTAAACACCCTAATCAAC-TGGCTTCAATCTAAAGCCCCGGCAGG-TTTGAAGCTGCTTCTTCGAATTTGCAATTCAATATGAAAA-TCACCTCAGAGCTTGGTAAAAAGAGGCTTAACCCCTGTCTTTAGATTTACAGTCCAATGCTTCA-CTCAGCCATTTTACCACAAAAAAGGAAGGAATCGAACCCCCTAAAGCTGGTTTCAAGCCAACCCCATGACCTCCATGACTTTTTCAAAAGATATTAGAAAAACTATTTCATAACTTTGTCAAAGTTAAATTACAGGTT-AACCCCCGTATATCTTA-CACTGTAAAGCTAACCTAGCATTAACCTTTTAAGTTAAAGATTAAGAGGACCGACACCTCTTTACAGTGA";
    private final static String gorilla = "AGAAATATGTCTGATAAAAGAGTTACTTTGATAGAGTAAATAATAGAGGTTTAAACCCCCTTATTTCTACTAGGACTATGAGAATTGAACCCATCCCTGAGAATCCAAAATTCTCCGTGCCACCTGTCACACCCCATCCTAAGTAAGGTCAGCTAAATAAGCTATCGGGCCCATACCCCGAAAATGTTGGTCACATCCTTCCCGTACTAAGAAATTTAGGTTAAACATAGACCAAGAGCCTTCAAAGCCCTTAGTAAGTTA-CAACACTTAATTTCTGTAAGGACTGCAAAACCCTACTCTGCATCAACTGAACGCAAATCAGCCACTTTAATTAAGCTAAGCCCTTCTAGATCAATGGGACTCAAACCCACAAACATTTAGTTAACAGCTAAACACCCTAGTCAAC-TGGCTTCAATCTAAAGCCCCGGCAGG-TTTGAAGCTGCTTCTTCGAATTTGCAATTCAATATGAAAT-TCACCTCGGAGCTTGGTAAAAAGAGGCCCAGCCTCTGTCTTTAGATTTACAGTCCAATGCCTTA-CTCAGCCATTTTACCACAAAAAAGGAAGGAATCGAACCCCCCAAAGCTGGTTTCAAGCCAACCCCATGACCTTCATGACTTTTTCAAAAGATATTAGAAAAACTATTTCATAACTTTGTCAAGGTTAAATTACGGGTT-AAACCCCGTATATCTTA-CACTGTAAAGCTAACCTAGCGTTAACCTTTTAAGTTAAAGATTAAGAGTATCGGCACCTCTTTGCAGTGA";

    private static int[] getStates(String sequence) {
        int[] states = new int[sequence.length()];

        for (int i = 0; i < sequence.length(); i++) {
            switch (sequence.charAt(i)) {
                case 'A':
                    states[i] = 0;
                    break;
                case 'C':
                    states[i] = 1;
                    break;
                case 'G':
                    states[i] = 2;
                    break;
                case 'T':
                    states[i] = 3;
                    break;
                default:
                    states[i] = 4;
                    break;
            }
        }
        return states;
    }

    private static double[] getPartials(String sequence) {
        double[] partials = new double[sequence.length() * 4];

        int k = 0;
        for (int i = 0; i < sequence.length(); i++) {
            switch (sequence.charAt(i)) {
                case 'A':
                    partials[k++] = 1;
                    partials[k++] = 0;
                    partials[k++] = 0;
                    partials[k++] = 0;
                    break;
                case 'C':
                    partials[k++] = 0;
                    partials[k++] = 1;
                    partials[k++] = 0;
                    partials[k++] = 0;
                    break;
                case 'G':
                    partials[k++] = 0;
                    partials[k++] = 0;
                    partials[k++] = 1;
                    partials[k++] = 0;
                    break;
                case 'T':
                    partials[k++] = 0;
                    partials[k++] = 0;
                    partials[k++] = 0;
                    partials[k++] = 1;
                    break;
                default:
                    partials[k++] = 1;
                    partials[k++] = 1;
                    partials[k++] = 1;
                    partials[k++] = 1;
                    break;
            }
        }
        return partials;
    }


    public static void main(String[] argv) {

        // is nucleotides...
        int stateCount = 4;

        // get the number of site patterns
        int nPatterns = human.length();

        BeagleInfo.printResourceList();

        // create an instance of the BEAGLE library
        Beagle instance = loadBeagleInstance(
                    3,				/**< Number of tip data elements (input) */
                    5,	            /**< Number of partials buffers to create (input) */
                    3,		        /**< Number of compact state representation buffers to create (input) */
                    stateCount,		/**< Number of states in the continuous-time Markov chain (input) */
                    nPatterns,		/**< Number of site patterns to be handled by the instance (input) */
                    1,		        /**< Number of rate matrix eigen-decomposition buffers to allocate (input) */
                    4,		        /**< Number of rate matrix buffers (input) */
                    1,              /**< Number of rate categories (input) */
                    3               /**< Number of scale buffers (input) */
                    );
        if (instance == null) {
            System.err.println("Failed to obtain BEAGLE instance");
            System.exit(1);
        }

        StringBuilder sb = new StringBuilder();
        for (BeagleFlag flag : BeagleFlag.values()) {
            if (flag.isSet(instance.getDetails().getFlags())) {
                sb.append(" ").append(flag.name());
            }
        }
        System.out.println("Instance on resource #" + instance.getDetails().getResourceNumber() + " flags:" + sb.toString());
        
        instance.setTipStates(0, getStates(human));
        instance.setTipStates(1, getStates(chimp));
        instance.setTipStates(2, getStates(gorilla));

        // set the sequences for each tip using partial likelihood arrays
//        instance.setPartials(0, getPartials(human));
//        instance.setPartials(1, getPartials(chimp));
//        instance.setPartials(2, getPartials(gorilla));

        final double[] rates = { 1.0 };
        instance.setCategoryRates(rates);

        // create base frequency array
        final double[] freqs = { 0.25, 0.25, 0.25, 0.25 };

        // create an array containing site category weights
        final double[] weights = { 1.0 };

        // an eigen decomposition for the JC69 model
        final double[] evec = {
             1.0,  2.0,  0.0,  0.5,
             1.0,  -2.0,  0.5,  0.0,
             1.0,  2.0, 0.0,  -0.5,
             1.0,  -2.0,  -0.5,  0.0
        };

        final double[] ivec = {
             0.25,  0.25,  0.25,  0.25,
             0.125,  -0.125,  0.125,  -0.125,
             0.0,  1.0,  0.0,  -1.0,
             1.0,  0.0,  -1.0,  0.0
        };

        double[] eval = { 0.0, -1.3333333333333333, -1.3333333333333333, -1.3333333333333333 };

        // set the Eigen decomposition
        instance.setEigenDecomposition(0, evec, ivec, eval);

        // a list of indices and edge lengths
        int[] nodeIndices = { 0, 1, 2, 3 };
        double[] edgeLengths = { 0.1, 0.1, 0.2, 0.1 };

        // tell BEAGLE to populate the transition matrices for the above edge lengths
        instance.updateTransitionMatrices(
                                 0,             // eigenIndex
                                 nodeIndices,   // probabilityIndices
                                 null,          // firstDerivativeIndices
                                 null,          // secondDervativeIndices
                                 edgeLengths,   // edgeLengths
                                 4);            // count

        instance.resetScaleFactors(2);

        // create a list of partial likelihood update operations
        // the order is [dest, writeScale, readScale, source1, matrix1, source2, matrix2]
        int[] operations = {
            3, 0, 0, 0, 0, 1, 1,
            4, 1, 1, 2, 2, 3, 3
        };
        int[] rootIndices = { 4 };

        // update the partials
        instance.updatePartials(
                        operations,     // eigenIndex
                        2,              // operationCount
                        2);             // rescale ?

        double[] patternLogLik = new double[nPatterns];

        int[] scalingFactorsIndices = {2}; // internal nodes

        // TODO Need to call accumulateScaleFactors if scaling is enabled

        // calculate the site likelihoods at the root node
        instance.calculateRootLogLikelihoods(
                                    rootIndices,            // bufferIndices
                                    weights,                // weights
                                    freqs,                 // stateFrequencies
                                    scalingFactorsIndices,
                                    1,
                                    patternLogLik);         // outLogLikelihoods

        double logL = 0.0;
        for (int i = 0; i < nPatterns; i++) {
//            System.out.println("site lnL[" + i + "] = " + patternLogLik[i]);
            logL += patternLogLik[i];
        }

        System.out.println("logL = " + logL + " (PAUP logL = -1574.63623)");
    }

}
