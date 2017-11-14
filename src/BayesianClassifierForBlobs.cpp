/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "BayesianClassifierForBlobs.h"



BayesianClassifierForBlobs::BayesianClassifierForBlobs(){
    countBgBlobs = 2091;
    countFluxBlobs = 4039;
    bgFrequency = 0.5;      /// WE ASSUME THAT THE FREQUENCIES OF THE CLASSES ARE THE SAME
    fluxFrequency = 0.5;
}



vector<pair<string,double> > BayesianClassifierForBlobs::classify(Blob* b){

    vector<pair<string,double> > prediction;

    //double pixelMean = b->getPixelsMean();
    //double area = b->getNumberOfPixels();
    double photons = b->getNumberOfPhotonsInBlob();
    double photonsCloseness = b->getPhotonsCloseness();
    /// ADD ATTRIBUTE

    //double bgPM = computeProbabilityFromDistribution(pixelMean,bgPixelMeanDistribution);
    //double bgA = computeProbabilityFromDistribution(area, bgAreaDistribution);
    double bgP = computeProbabilityFromDistribution(photons, bgPhotonsInBlobDistribution);
    double bgPC = computeProbabilityFromDistribution(photonsCloseness, bgPhotonsClosenessDistribution);
    /// ADD DISTR VALUE


    //double fluxPM = computeProbabilityFromDistribution(pixelMean,fluxPixelMeanDistribution);
    //double fluxA = computeProbabilityFromDistribution(area, fluxAreaDistribution);
    double fluxP = computeProbabilityFromDistribution(photons, fluxPhotonsInBlobDistribution);
    double fluxPC = computeProbabilityFromDistribution(photonsCloseness, fluxPhotonsClosenessDistribution);
    /// ADD DISTR VALUE






    /// THE LIKELYHOOD CALCULUS .. MODIFY THIS TO CHANGE ATTRIBUTES OF ANALYSIS
    double likelyHoodOfBackground = bgP*bgPC*bgFrequency;
    double likelyHoodOfFlux = fluxP*fluxPC*fluxFrequency;


    /// NORMALIZATION STEP TO GET A PROBABILITY
    double sum = likelyHoodOfBackground+likelyHoodOfFlux;

    double probabilityOfBg = likelyHoodOfBackground/sum;
    double probabilityOfFlux = likelyHoodOfFlux/sum;






    prediction.push_back(make_pair("Background",probabilityOfBg));
    prediction.push_back(make_pair("Flux",probabilityOfFlux));


   // cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is background " << " with probability: " << probabilityOfBg*100<<"%"<<endl;
  //  cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is flux " << " with probability: " << probabilityOfFlux*100<<"%"<<endl;


    return prediction;
}


double BayesianClassifierForBlobs::computeProbabilityFromDistribution(double x,normal_distribution<double> distribution){
    double mean = distribution.mean();
    double stddev = distribution.stddev();


    double probability = 0;

    double multiplier = 1 / ( sqrt(2*M_PI*pow(stddev,2))   );

    double exponent = -1 *( (pow(x-mean,2)) / (2*pow(stddev,2)) );

    double exponential = exp(exponent);

    probability = multiplier * exponential;

    /*
    cout << "mean: " << mean << endl;
    cout << "stddev: " << stddev << endl;
    cout << "multiplier: " << multiplier << endl;
    cout << "exponent: " << exponent << endl;
    cout << "exponential: " << exponential << endl;
    cout << "probability: " << probability << endl;
    cout << "\n";
    */
    return probability;
}

