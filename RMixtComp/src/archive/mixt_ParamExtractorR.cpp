/* MixtComp version 2.0  - 13 march 2017
 * Copyright (C) Inria - Lille 1 */
/*
 *  Project:    MixtComp
 *  Created on: July 7, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "mixt_ParamExtractorR.h"
#include "IO/mixt_IO.h"
#include "LinAlg/mixt_LinAlg.h"

namespace mixt {

void ParamExtractorR::setNbMixture(int nbMixture) {
	mixtureName_.resize(nbMixture);
	param_.resize(nbMixture);
}

void ParamExtractorR::exportParam(int indexMixture, const std::string& idName, const std::string& paramName, const Matrix<Real>& statStorage, const Matrix<Real>& logStorage,
		const std::vector<std::string>& paramNames, Real confidenceLevel, const std::string& paramStr) {
	Rcpp::CharacterVector rows(statStorage.rows()); // names of the parameters
	Rcpp::CharacterVector cols; // names for expectation and confidence interval values

	Real alpha = (1. - confidenceLevel) / 2.;

	Rcpp::NumericMatrix statR(statStorage.rows(), statStorage.cols());

	// values setting
	for (int i = 0; i < statStorage.rows(); ++i) {
		for (int j = 0; j < statStorage.cols(); ++j) {
			statR(i, j) = statStorage(i, j);
		}
	}

	// names setting for rows
	for (int i = 0; i < statStorage.rows(); ++i) {
		rows[i] = paramNames[i];
	}

	// names setting for cols
	if (statStorage.cols() == 1) {
		cols.push_back("value");
	} else if (statStorage.cols() == 3) {
		cols.push_back("median");
		cols.push_back(std::string("q ") + type2str(alpha * 100.) + std::string("%"));
		cols.push_back(std::string("q ") + type2str((1. - alpha) * 100.) + std::string("%"));
	}

	Rcpp::List dimnms = Rcpp::List::create(rows, cols);
	statR.attr("dimnames") = dimnms;

	Rcpp::NumericMatrix logR;

	if (logStorage.rows() > 0 && logStorage.cols()) { // only if log has taken place, for example not during predict
			// copy of the log data
		logR = Rcpp::NumericMatrix(logStorage.rows(), logStorage.cols());
		for (int i = 0; i < logStorage.rows(); ++i) {
			for (int j = 0; j < logStorage.cols(); ++j) {
				logR(i, j) = logStorage(i, j);
			}
		}
		Rcpp::CharacterVector colsLog(logStorage.cols());
		Rcpp::List dimnmsLog = Rcpp::List::create(rows, colsLog);
		logR.attr("dimnames") = dimnmsLog;
	}

	mixtureName_[indexMixture] = idName;
	param_[indexMixture][paramName] = Rcpp::List::create(Rcpp::Named("stat") = statR, Rcpp::Named("log") = logR, Rcpp::Named("paramStr") = paramStr);
}

/** Extractor for Rank data type */
void ParamExtractorR::exportParam(int indexMixture, const std::string& idName, const std::string& paramName, const std::vector<RankStat>& paramStat, const std::vector<std::string>& paramNames,
		Real confidenceLevel, const std::string& paramStr) {
	Rcpp::CharacterVector rowsNames = Rcpp::wrap(paramNames); // names of the classes, used for both parameters values and logs

	int nbClass = paramStat.size();
	int nbPos = paramStat[0].nbPos(); // get number of positions in rank

	std::list < Rcpp::List > statCPP; // global list, of param descriptor, each element is the descriptor of a class

	for (int k = 0; k < nbClass; ++k) { // loop to create the parameters descriptor
		const std::list<std::pair<RankVal, Real> >& statStorageMu = paramStat[k].statStorageMu(); // helper reference to point to current statStorage
		std::list < Rcpp::List > classProba; // list of pairs {vector representing rank, proba} for the current class

		for (std::list<std::pair<RankVal, Real> >::const_iterator it = statStorageMu.begin(), ite = statStorageMu.end(); it != ite; ++it) {
			const RankVal& rankCPP = it->first; // current rank in C++
			Rcpp::IntegerVector rankR(nbPos); // current rank in R
			for (int p = 0; p < nbPos; ++p) {
				rankR(p) = rankCPP.o()(p);
			}
			classProba.push_back(Rcpp::List::create(rankR, it->second));
		}
		statCPP.push_back(Rcpp::wrap(classProba));
	}

	Rcpp::List statR = Rcpp::wrap(statCPP);
	statR.attr("names") = rowsNames;

	std::list < Rcpp::List > logCPP; // global list, of param descriptor, each element is the descriptor of a class

	for (int k = 0; k < nbClass; ++k) { // loop to create the parameters log
		int nbPos = paramStat[k].nbPos(); // get number of positions in rank
		const Vector<RankVal>& logStorageMu = paramStat[k].logStorageMu(); // helper reference to point to current statStorage
		std::list < Rcpp::IntegerVector > classProba; // list of sampled mu for the current class

		for (int i = 0, ie = logStorageMu.size(); i < ie; ++i) {
			const RankVal& rankCPP = logStorageMu(i); // current rank in C++
			Rcpp::IntegerVector rankR(nbPos); // current rank in R
			for (int p = 0; p < nbPos; ++p) {
				rankR(p) = rankCPP.o()(p);
			}
			classProba.push_back(rankR);
		}
		logCPP.push_back(Rcpp::wrap(classProba));
	}

	Rcpp::List logR = Rcpp::wrap(logCPP);
	logR.attr("names") = rowsNames;

	mixtureName_[indexMixture] = idName;
	param_[indexMixture][paramName] = Rcpp::List::create(Rcpp::Named("stat") = statR, Rcpp::Named("log") = logR, Rcpp::Named("paramStr") = paramStr);
}

Rcpp::List ParamExtractorR::rcppReturnParam() const {
	Rcpp::CharacterVector mixtureNameR = Rcpp::wrap(mixtureName_);
	Rcpp::List paramR = Rcpp::wrap(param_);
	paramR.attr("names") = mixtureNameR;
	return paramR;
}

} // namespace mixt