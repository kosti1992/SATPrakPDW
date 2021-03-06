/**
 * @file ICPPDWSettings.h
 * @author David
 *
 * @version 2017-04-27
 * Created on 2017-04-27.
 */

#pragma once

#include "../../Common.h"
#include <experimental/optional>
#include "../../solver/ModuleSettings.h"


//#define PDW_MODULE_DEBUG_0
//#define PDW_MODULE_DEBUG_1

#ifdef PDW_MODULE_DEBUG_1
#ifndef PDW_MODULE_DEBUG_0
#define PDW_MODULE_DEBUG_0
#endif
#endif


namespace smtrat
{
  typedef DoubleInterval IntervalT;

  //Typedef for one or two things
  template <class T>
    using OneOrTwo = std::pair<T,std::experimental::optional<T>>;

  struct ICPPDWSettingsDebug  : ModuleSettings
  {
    /// Name of the Module
    static constexpr auto moduleName = "ICPPDWModule<ICPPDWSettingsDebug>";

    // number of maximal contraction per ICP state
    static constexpr int maxContractions = 50;

    // desired interval
    static constexpr double targetDiameter = 0.0;

    // maximal number of splits allowed
    static constexpr int maxSplitNumber = 20;

    //we define a big M in order to be able to compute gain in case of inf intervals
    // it is defined as twice the max interval, since we have to consider an interval [-inf,0] to be better than [-1000,0]
    // since (in our examples), original vars can go from -1000 to 1000, in polynomials (e.g. x³) the value can get really big
    // so this is good enough for at most cubic polynomials, i.e. 10*1000³
    static constexpr double maxOriginalVarBound = 1000.0;
    static constexpr double bigM = 10 * maxOriginalVarBound * maxOriginalVarBound * maxOriginalVarBound;

    //gain threshold. Best gain in contraction lower than this will result in a manual split.
    static constexpr double gainThreshold = 0.01;

    //Small value
    static constexpr double epsilon = 0.001;

    //for a heuristic in the getBestContractionCandidate we need a lower and an upper bound
    static constexpr double lowerDelta = -0.01;//set the lower bound to a smaller value to make absolute reduction more relevant
    static constexpr double upperDelta = 0.03;//set the upper higher in order to make the difference in gain more relevant

    //alpha as used in reinforced learning, see chapter 8 slide 17
    static constexpr double alpha = 0.9;

    //Split intervals are multiplied by this value to prefer non-splitting contractions
    static constexpr double splitPenalty = 0.3;

    //an epsilon is required to distinguish between candidates with weights which are regarded and which are not
    static constexpr double weightEps = 0.01;

    //look at this many possible contraction candidates:
    static constexpr int minCandidates = 5;

    //this factor states how weights are updated after checkCore is called ones more
    static constexpr double updateFactor = 1.1;

    //this factor represents a scaling factor as used whenever trees are compared
    static constexpr double compAlpha = 0;

  };

  struct ICPPDWSettingsProduction  : ModuleSettings
  {
    /// Name of the Module
    static constexpr auto moduleName = "ICPPDWModule<ICPPDWSettingsProduction>";

    // number of maximal contraction per ICP state
    static constexpr int maxContractions = 1000;

    // desired interval relative to the initial interval
    static constexpr double targetDiameter = 0.1;

    // maximal number of splits allowed
    static constexpr int maxSplitNumber = 100;

    //we define a big M in order to be able to compute gain in case of inf intervals
    // it is defined as twice the max interval, since we have to consider an interval [-inf,0] to be better than [-1000,0]
    // since (in our examples), original vars can go from -1000 to 1000, in polynomials (e.g. x³) the value can get really big
    // so this is good enough for at most cubic polynomials, i.e. 10*1000³
    static constexpr double maxOriginalVarBound = 1000.0;
    static constexpr double bigM = 10 * maxOriginalVarBound * maxOriginalVarBound * maxOriginalVarBound;

    //gain threshold. Best gain in contraction lower than this will result in a manual split.
    static constexpr double gainThreshold = 0.01;

    //Small value
    static constexpr double epsilon = 0.001;

    //for a heuristic in the getBestContractionCandidate we need a lower and an upper bound
    static constexpr double lowerDelta = -0.1;//set the lower bound to a smaller value to make absolute reduction more relevant
    static constexpr double upperDelta = 0.03;//set the upper higher in order to make the difference in gain more relevant

    //alpha as used in reinforced learning, see chapter 8 slide 17
    static constexpr double alpha = 0.9;

    //Split intervals are multiplied by this value to prefer non-splitting contractions
    static constexpr double splitPenalty = 0.3;

    //an epsilon is required to distinguish between candidates with weights which are regarded and which are not
    static constexpr double weightEps = 0.01;

    //look at this many possible contraction candidates:
    static constexpr int minCandidates = 5;

    //this factor states how weights are updated after checkCore is called ones more
    static constexpr double updateFactor = 1.1;

    //this factor represents a scaling factor as used whenever trees are compared
    static constexpr double compAlpha = 0;

  };
}
