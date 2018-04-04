# AG_utils

## Classi

Le seguenti classi sono utilizzate da più progetti AGILE.

* BayesianClassifierForBlobs
* Blob
* BlobsFinder
* ExpRatioEvaluator
* FileWriter
* FolderManager
* MapConverter

## BayesianClassifierForBlobs
The BayesianClassifierForBlobs implements a Naive Bayes classifier, applying the Bayes Theorem in order to make a fuzzy, binary classification (assigning to an instance a probability for each class type). In this case, it is used to classify blobs (i.e. connected components extracted from the AGILE's sky images by the BlobsFinder class): it a assings to each blob a 'source' and 'background' probability.

The Bayes Theorem works on conditional probability. Conditional probability is the probability that something will happen, given that something else has already occurred. Using the conditional probability, we can calculate the probability of an event using its prior knowledge. 

    P(H|E)=P(E|H)\*P(H)/P(E)
    	P(H) is the probability of hypothesis H being true. This is known as the prior probability.
    	P(E) is the probability of the evidence(regardless of the hypothesis).
    	P(E|H) is the probability of the evidence given that hypothesis is true (the prior knowledge).
    	P(H|E) is the probability of the hypothesis given that the evidence is there.

The prior knowledge si given by the BayesianModelEvaluator class that computes, given a training set of blobs, the distributions of two blob's features for each class type: the number of photons in each blob and the photon closeness. The latter is defined as the sum of the distance of each photon from the centroid of the blob, divided by the number of photons.

Naive Bayes classifier assumes that all the features are unrelated to each other. Unfortunatly, when a blob contains only one photon, the latter is the blob's centroid and this cause the photon closeness to be 0. That's why all the blobs used for training set and for classification, contains a number of photons greater or equal than 2.

The class expose a single public method:

	vector<pair<string,double> > classify(Blob* b);

The method accepts a blob, and it returns a vector like: \["background":<probability>, "source":<probability>\]. 
* It obtains the two blob's features calling the blob's getter functions.
* TODO

## BlobsFinder
The BlobsFinder static class depends on OpenCv version ??. It exposes a single public static function:
	
	vector<Blob*> findBlobs(string fitsfilePath, double PSF, double CDELT1, double CDELT2).

This method returns a list of Blobs intended as connected component regions in the image. It is independent from the image's resolution since it accepts the CDELT1 and CDELT2 parameters that specify the pixels width and height in terms of degrees. The algorithm is described in the following lines:

* It uses the MapConverter class to convert the fits file data in a IntMatrixCustomMap custom type.
* It applies a Gaussian Smoothing:
    * It converts the IntMatrixCustomMap in the Mat CV_32FC1 OpenCv image type (32 bits float).
    * It computes the Kernel size by [ (2 * PSF/CDELT2) + 1  ,  (2 * PSF/CDELT2) + 1 ] and uses the PSF as the sigma.
    * It smooths the image using the OpenCv's GaussianBlur() function.
    * It converts the CV_32FC1 smoothed image to CV_8UC1 (8bits uchar 0~255) smoothed image with a OpenCv's linear stretching.
* It applies a threshold in order to separate sticked blobs after the smoothing.
    * It computes the image histogram with the OpenCv's CalcHist function.
    * It computes the threshold as the mean value of the histogram's grey levels.
    * It thresholds the image, setting to 0 the gray levels that are below the threshold value.
* It add a 1x1 padding to the image before the next operations: we don't want to loose photons that are positioned on the image's border.
* It calls the OpenCv's findContours() function to that returns a list of contours in a hierarchy fashion. Each contour is a list of points belonging to a blob. The findContours() does not take in account the border of the image.
* It extracts the blobs iterating over the contours:
    * It checks if the contour is not contained in another contour.
    * It computes the pixels and the photons inside the contour using the OpenCv's pointPolygonTest() function.
    * It removes the padding from the contour coordinates and convert the OpenCv's Point type to the CustomPoint custom type.
    * It creates a new blob if the number of photons inside it is greater or equal than 2 (it is possible that after thresholding some blobs can form between two other blobs with 0 photons inside: see https://drive.google.com/file/d/17gV9M_4MVTqa4DqnbnQc2jnsg2XsgLr7/view?usp=sharing. Furthermore we don't want to have blobs containing only 1 photon: see 'BayesianClassifierForBlobs' detail section). 
    * It pushes the blob in a blob list.
* It returns the blob list.


## ExpRatioEvaluator

La routine exp-ratio permette di valutare quando una “detection” o “spot” è troppo vicina ai bordi dell’esposure AGILE.
Tutte le valutazioni vengono fatte su di una mappa exp NORMALIZZATA (se la mappa in input NON è normalizzata, il software provvederà a normalizzarla).
Restituisce un numero compreso tra 0 e 100 (0 bad).

In particolare si definisce exp-ratio come:

    exp-ratio = (1 -  nBad/nTot) * 100;

    nBad -> numero di pixels in una regione rettangolare i cui valori non sono compresi in un certo range dato in input alla routine
    nTot -> numero di pixels della regione rettangolare

La mappa viene normalizzata per renderla indipendente dal tempo di esposizione e dalla risoluzione, dividendo ogni pixel per un normalizationFactor.

    normalizationFactor = timeFactor*spatialFactor

    timeFactor = tStop - tStart
    spatialFactor = 0.0003046174197867085688996857673060958405 * cdelt2 * cdelt2 * Alikesinaa(0.0174532925199432954743716805978692718782 * distanceFromCenter);

### Utilizzo

La classe ExpRatioEvaluator deve essere instanziata chiamando i seguenti costruttori:


	ExpRatioEvaluator(const char * expPath, bool isExpMapNormalized, bool createExpNormalizedMap, bool createExpRatioMap, double minThreshold, double maxThreshold, int squareSize);


	ExpRatioEvaluator(AgileMap agileMap, bool isExpMapNormalized, bool createExpNormalizedMap, bool createExpRatioMap, double minThreshold, double maxThreshold, int squareSize);


### Parametri del costruttore

    expPath : il path alla mappa (.exp, .cts, .exp.gz, .cts.gz)

    agileMap : la mappa di tipo AgileMap

    isExpMapNormalized : se il valore è false, si afferma che la mappa exp in input NON è normalizzata. Il software provvederà a normalizzarla.

    createExpNormalizedMap : se il valore è true verrà scritta su file la mappa normalizzata.

    createExpRatioMap : se il valore è true verrà scritta su file la mappa exp-ratio.

    minThreshold : la soglia minima per le valutazioni della mappa exp-ratio. (default = 100)

    maxThreshold : la soglia minima per le valutazioni della mappa exp-ratio. (default = 140)

    squareSize : la dimensione in pixel del lato del quadrato dell'area di analisi exp-ratio. (default = 20)


Per calcolare exp-ratio si deve chiamare il metodo:

    double computeExpRatioValues(double l, double b);

    l : longitudine galattica

    b : latitudine galattica

### Output costruttore

Il costruttore crea e scrive su file due mappe a seconda dei parametri di input:

	Se createExpNormalizedMap == true, crea la mappa normalizzata il cui filename sarà:  expPath + "_norm.exp.gz", apribile con ds9

	Se createExpRatioMap == true, crea la mappa exp-ratio (in cui ogni pixel è un exp-ratio centrato sul pixel stesso) il cui filename sarà: expPath + "_norm_exp.gz" con ds9

### Output metodo computeExpRatioValues(double l, double b);

	il valore exp-ratio


### Altre funzioni:

Versione overload di computeExpRatioValues(..): si utilizza per non passare dalla conversione (l,b) -> (x,y) ma applicare direttamente la valutazione exp-ratio su (x,y). Type può essere una stringa qualsiasi.

    double* computeExpRatioValues(int x, int y, string type);


### Dipendenze:

   "FitsUtil.h"
   Per l'apertura e la creazione dei file Fits.

    AgileMap: "AgileMap.h"
    Si utilizzano le funzione di conversione pixel->coordinate galattiche e viceversa

### Esempio Normalizzazione (prima riga della matrice di valori della mappa exp):

    NON normalizzata: 8.55229 8.48367 8.41506 8.43352 8.45197 8.46707 8.48217 8.492 8.50182 8.50961 8.51739 8.52386 8.53033 8.53547 8.54062 8.54394 8.54726 8.54008 8.5329 8.52273 8.51255 8.54708 8.58161 8.61895 8.65628 8.5777 8.49913 8.50764 8.51615 8.523 8.52986 8.5352 8.54053 8.54302 8.54551 8.542 8.5385 8.52689 8.51528 8.49808 8.48089 8.46036 8.43983 8.41705 8.39428 8.36798 8.34167 8.23662 8.13157 8.16113 8.19069 8.22094 8.2512 8.24105 8.23091 8.21738 8.20384 8.18152 8.1592 8.13268 8.10616 8.07796 8.04976 8.01948 7.9892 7.95574 7.92228 7.84201 7.76173 7.69158 7.62142 7.59228 7.56314 7.53232 7.50151 7.46681 7.43211 7.43764 7.44317 7.44721 7.45125 7.41352 7.3758 7.33472 7.29365 7.25003 7.20641 7.15902 7.11163 6.99235 6.87308 6.83646 6.79983 6.75876 6.71769 6.67216 6.62662 6.57739 6.52816 6.47713


    normalizzata: 112.302 111.401 110.5 110.742 110.985 111.183 111.381 111.51 111.639 111.742 111.844 111.929 112.014 112.081 112.149 112.192 112.236 112.142 112.047 111.914 111.78 112.234 112.687 113.177 113.668 112.636 111.604 111.716 111.827 111.917 112.007 112.078 112.148 112.18 112.213 112.167 112.121 111.968 111.816 111.59 111.364 111.095 110.825 110.526 110.227 109.882 109.536 108.157 106.777 107.166 107.554 107.951 108.348 108.215 108.082 107.904 107.727 107.433 107.14 106.792 106.444 106.074 105.703 105.306 104.908 104.469 104.029 102.975 101.921 101 100.079 99.6959 99.3132 98.9086 98.504 98.0483 97.5927 97.6653 97.738 97.791 97.844 97.3487 96.8533 96.3139 95.7745 95.2018 94.629 94.0067 93.3844 91.8182 90.252 89.7711 89.2902 88.7508 88.2115 87.6136 87.0157 86.3692 85.7228 85.0527

    mappa exp-ratio: -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 70.4938 69.7799 69.1255 68.7091 68.5901 68.5901 68.6496 68.7091 68.7091 68.8281 69.0065 69.185 69.423 69.6014 69.7799 69.8394 69.8989 69.8989 69.6014 68.9471 68.2927 67.6383 66.9839 66.3296 65.7347 65.3778 65.0208 64.7829 64.6639 64.4854 64.307 64.1285 63.95 63.8311 63.7121 63.6526 63.5931 63.5336 63.4741 63.3551 63.2362 63.1172 62.8792 62.5818 62.0464 61.1541 60.2023 59.2504 58.3581 57.5253 56.633 55.6217 54.4319 53.1826 51.9929 50.7436 49.5538 48.3046 47.0553 45.8061 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1

### Esempi di output:

    computeExpRatioValues(double l, double b);

	Mappa non normalizzata, minTheshold: 10, maxThreshold: 11
    exp.computeExpRatioValues(30,30);
    exp-ratio: 47.6502
    nBad, totCount, greyLevelsMean   880 1681 9.93831

	Mappa non normalizzata, minTheshold: 0, maxThreshold: 100
    exp.computeExpRatioValues(30,30);
    exp-ratio: 100 0
    nBad, totCount, greyLevelsMean 1681 9.93831

    Mappa non normalizzata, minTheshold: 0, maxThreshold: 5
    exp.computeExpRatioValues(30,30);
    exp-ratio: 0
    nBad, totCount, greyLevelsMean 1681 1681 9.93831


	Mappa normalizzata, minTheshold: 50, maxThreshold: 150
    exp.computeExpRatioValues(30,30);
    exp-ratio: 100
    nBad, totCount, greyLevelsMean 0 1681 130.502

	Mappa normalizzata, minTheshold: 129, maxThreshold: 131
    exp.computeExpRatioValues(30,30);
    exp-ratio: 16.2403
    nBad, totCount, greyLevelsMean 1408 1681 130.502

	Mappa normalizzata, minTheshold: 0, maxThreshold: 50
    exp.computeExpRatioValues(30,30);
    exp-ratio: 0
    nBad, totCount, greyLevelsMean 1681 1681 130.502

### Codici di errore

    -1  :  l'area di valutazione (rettangolo) esce fuori dalla mappa.
    -2  :  le coordinate (l,b) su cui si centra l'area di valutazione escono fuori dalla mappa. Può essere causato dal fatto che le coordinate (l,b) sono il centroide di un blob estratto da una mappa il cui centro non coincide con la relativa mappa di esposizione.
