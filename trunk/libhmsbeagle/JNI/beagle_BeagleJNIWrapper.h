/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class beagle_BeagleJNIWrapper */

#ifndef _Included_beagle_BeagleJNIWrapper
#define _Included_beagle_BeagleJNIWrapper
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    getResourceList
 * Signature: ()[Lbeagle/ResourceDetails;
 */
JNIEXPORT jobjectArray JNICALL Java_beagle_BeagleJNIWrapper_getResourceList
  (JNIEnv *, jobject);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    createInstance
 * Signature: (IIIIIIII[IIII)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_createInstance
  (JNIEnv *, jobject, jint, jint, jint, jint, jint, jint, jint, jint, jintArray, jint, jint, jint);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    initializeInstance
 * Signature: (I[Lbeagle/InstanceDetails;)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_initializeInstance
  (JNIEnv *, jobject, jint, jobjectArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    finalize
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_finalize
  (JNIEnv *, jobject, jint);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    setPartials
 * Signature: (II[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_setPartials
  (JNIEnv *, jobject, jint, jint, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    getPartials
 * Signature: (II[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_getPartials
  (JNIEnv *, jobject, jint, jint, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    setTipStates
 * Signature: (II[I)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_setTipStates
  (JNIEnv *, jobject, jint, jint, jintArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    setEigenDecomposition
 * Signature: (II[D[D[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_setEigenDecomposition
  (JNIEnv *, jobject, jint, jint, jdoubleArray, jdoubleArray, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    setCategoryRates
 * Signature: (I[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_setCategoryRates
  (JNIEnv *, jobject, jint, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    setTransitionMatrix
 * Signature: (II[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_setTransitionMatrix
  (JNIEnv *, jobject, jint, jint, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    updateTransitionMatrices
 * Signature: (II[I[I[I[DI)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_updateTransitionMatrices
  (JNIEnv *, jobject, jint, jint, jintArray, jintArray, jintArray, jdoubleArray, jint);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    updatePartials
 * Signature: ([II[III)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_updatePartials
  (JNIEnv *, jobject, jintArray, jint, jintArray, jint, jint);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    waitForPartials
 * Signature: ([II[II)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_waitForPartials
  (JNIEnv *, jobject, jintArray, jint, jintArray, jint);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    calculateRootLogLikelihoods
 * Signature: (I[I[D[D[I[II[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_calculateRootLogLikelihoods
  (JNIEnv *, jobject, jint, jintArray, jdoubleArray, jdoubleArray, jintArray, jintArray, jint, jdoubleArray);

/*
 * Class:     beagle_BeagleJNIWrapper
 * Method:    calculateEdgeLogLikelihoods
 * Signature: (I[I[I[I[I[I[D[D[I[II[D[D[D)I
 */
JNIEXPORT jint JNICALL Java_beagle_BeagleJNIWrapper_calculateEdgeLogLikelihoods
  (JNIEnv *, jobject, jint, jintArray, jintArray, jintArray, jintArray, jintArray, jdoubleArray, jdoubleArray, jintArray, jintArray, jint, jdoubleArray, jdoubleArray, jdoubleArray);

#ifdef __cplusplus
}
#endif
#endif
