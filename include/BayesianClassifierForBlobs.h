/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#ifndef BAYESIANCLASSIFIERFORBLOBS_H
#define BAYESIANCLASSIFIERFORBLOBS_H

#include <Blob.h>
#include <random>

using namespace std;

class BayesianClassifierForBlobs
{
    public:

        /**
            Uses already computed distribution values, hard-coded as class attributes.
        */
        BayesianClassifierForBlobs();


        /**
            Compute the probability for a Blob to be background and flux. Returns [ ("background", x%) , ("flux",1-x%) ]
        */
        vector<pair<string,double> > classify(Blob* b);



    private:


        /**
            Compute the probability of an attribute value, given a distribution, from the Gaussian Equation.
            This probability will be inserted in the Bayesian Theorem in the classify() method.
        */
        double computeProbabilityFromDistribution(double x,normal_distribution<double> distribution);


         /// Big Dataset

        normal_distribution<double> bgPixelMeanDistribution = normal_distribution<double>( 1.220504, 0.119515);
        normal_distribution<double> fluxPixelMeanDistribution = normal_distribution<double>( 1.81847, 0.458405);

        normal_distribution<double> bgAreaDistribution = normal_distribution<double>( 143.977, 66.2592 );
        normal_distribution<double> fluxAreaDistribution = normal_distribution<double>( 401.355, 143.053 );

        normal_distribution<double> bgPhotonsInBlobDistribution = normal_distribution<double>( 1.13719, 0.594528 );
        normal_distribution<double> fluxPhotonsInBlobDistribution = normal_distribution<double>( 4.15919, 1.96208 );

        normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 1.3744, 2.30439 );
        normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 5.51344, 2.43346 );
	

	//normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 15.1198, 7.05518 ); //degrees
	//normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 1.92534, 1.00237 ); // degrees


        /// ADD DISTRIBUTION IF NEEDED

        int countBgBlobs;
        int countFluxBlobs;
        double bgFrequency;
        double fluxFrequency;

};

#endif // BAYESIANCLASSIFIERFORBLOBS_H
