#include "ICPState.h"
#include "ICPContractionCandidate.h"
#include "ICPTree.h"
#include "ICPPDWModule.h"
#include "ICPUtil.h"
#include "../../logging.h"

namespace smtrat
{
  template class ICPState<ICPPDWSettingsDebug>;
  template class ICPState<ICPPDWSettingsProduction>;


  template<class Settings>
  ICPState<Settings>::ICPState(ICPTree<Settings>* correspondingTree) :
    mOriginalVariables(),
    mBounds(),
    mAppliedContractionCandidates(),
    mAppliedIntervalConstraints(),
    mCorrespondingTree(correspondingTree)
  {
  }

  template<class Settings>
  ICPState<Settings>::ICPState(std::set<carl::Variable>* originalVariables,ICPTree<Settings>* correspondingTree) :
    mOriginalVariables(originalVariables),
    mBounds(),
    mAppliedContractionCandidates(),
    mAppliedIntervalConstraints(),
    mCorrespondingTree(correspondingTree)
  {
  }

  template<class Settings>
  ICPState<Settings>::ICPState(const vb::VariableBounds<ConstraintT>& parentBounds,std::set<carl::Variable>* originalVariables,ICPTree<Settings>* correspondingTree) :
    mBounds(),
    mAppliedContractionCandidates(),
    mAppliedIntervalConstraints(),
    mOriginalVariables(originalVariables),
    mCorrespondingTree(correspondingTree)
  {
    // copy parentBounds to mBounds
    for (const auto& mapEntry : parentBounds.getIntervalMap()) {
      carl::Variable var = mapEntry.first;
      IntervalT interval = mapEntry.second;

      // if the interval is infinite, there is no point in setting it
      // (setInterval actually expects a bounded interval)
      if (!interval.isInfinite()) {
        setInterval(var, interval, ConstraintT());
      }
      else {
        // we also need to make sure the copied variable bounds object knows about unbounded variables
        // to this end, we will simply call "getDoubleInterval" on every unbounded variable once
        // this method call will emplace an unbounded interval for this variable in the variable bounds object
        mBounds.getDoubleInterval(var);
      }
    }
  }
  
  template<class Settings>
  void ICPState<Settings>::initVariables(std::set<carl::Variable> vars) {
    for (const auto& v : vars) {
      // this getter will create an unbounded interval for unknown vars
      // so essentially, it is an initializer
      mBounds.getDoubleInterval(v);
    }
  }

  template<class Settings>
  vb::VariableBounds<ConstraintT>& ICPState<Settings>::getBounds() {
    return mBounds;
  }

  template<class Settings>
  void ICPState<Settings>::applyContraction(ICPContractionCandidate<Settings>* cc, IntervalT interval) {
    OneOrTwo<ConstraintT> intervalConstraints = setInterval(cc->getVariable(), interval, cc->getConstraint());
    addAppliedIntervalConstraint(intervalConstraints);
    addAppliedContractionCandidate(cc);
  }

  template<class Settings>
  OneOrTwo<ConstraintT> ICPState<Settings>::setInterval(carl::Variable var, const IntervalT& interval, const ConstraintT& _origin) {
    // since we cannot directly set the interval for a variable,
    // we will need to add two constraints. one for the lower and one for the upper bound
    // one advantage of this approach is that we can easily revert a contraction
    // by removing those constraints from the variable bounds

    ConstraintT upperBound;
    bool hasUpper = false;
    ConstraintT lowerBound;
    bool hasLower = false;

    // if upper bound is infty, the constraint is useless
    if (interval.upperBoundType() != carl::BoundType::INFTY) {
      // x <= upper bound
      // x - upper bound <= 0
      Poly upperPoly;
      upperPoly += var;
      upperPoly -= interval.upper();
      carl::Relation upperRelation = (interval.upperBoundType() == carl::BoundType::WEAK) ? carl::Relation::LEQ : carl::Relation::LESS;
      upperBound = ConstraintT(upperPoly, upperRelation);
      hasUpper = true;
      mBounds.addBound(upperBound, _origin);
    }

    // if lower bound is infty, the constraint is useless
    if (interval.lowerBoundType() != carl::BoundType::INFTY) {
      // x >= lower bound
      // lower bound - x <= 0
      Poly lowerPoly;
      lowerPoly -= var;
      lowerPoly += interval.lower();
      carl::Relation lowerRelation = (interval.lowerBoundType() == carl::BoundType::WEAK) ? carl::Relation::LEQ : carl::Relation::LESS;
      lowerBound = ConstraintT(lowerPoly, lowerRelation);
      hasLower = true;
      mBounds.addBound(lowerBound, _origin);
    }

    if (hasUpper && hasLower) {
      return OneOrTwo<ConstraintT>(upperBound, lowerBound);
    }
    else if (hasUpper) {
      return OneOrTwo<ConstraintT>(upperBound, std::experimental::optional<ConstraintT>());
    }
    else /*if (hasLower)*/ {
      return OneOrTwo<ConstraintT>(lowerBound, std::experimental::optional<ConstraintT>());
    }
  }

  template<class Settings>
  IntervalT ICPState<Settings>::getInterval(carl::Variable var) {
    mBounds.getInterval(var);
  }

  template<class Settings>
  vector<ICPContractionCandidate<Settings>*>& ICPState<Settings>::getAppliedContractionCandidates() {
    return mAppliedContractionCandidates;
  }

  template<class Settings>
  void ICPState<Settings>::addAppliedContractionCandidate(ICPContractionCandidate<Settings>* contractionCandidate) {
    mAppliedContractionCandidates.push_back(contractionCandidate);
  }

  template<class Settings>
  vector<OneOrTwo<ConstraintT> >& ICPState<Settings>::getAppliedIntervalConstraints() {
    return mAppliedIntervalConstraints;
  }

  template<class Settings>
  void ICPState<Settings>::addAppliedIntervalConstraint(const OneOrTwo<ConstraintT>& constraints) {
    mAppliedIntervalConstraints.push_back(constraints);
  }

  template<class Settings>
  bool ICPState<Settings>::removeConstraint(const ConstraintT& constraint) {
    // if we remove a constraint that has been used in this ICPState, all subsequent contractions become invalid
    // so we need to revert every applied contraction from the first application of the removed constraint
    int firstApplicationIndex = -1;
    bool isUsed = false;

    // a simple bound is not used as a contraction candidate, but simply applied to the bounds
    // thus, we assume that a simple bound is always used immediatly
    if (ICPUtil<Settings>::isSimpleBound(constraint)) {
      // basically, we need to revert all changes, since the very first contraction candidate
      // will already have made use of the simple bound
      firstApplicationIndex = 0;
      isUsed = true;
    }
    else {
      // we find the first usage of the removed constraint
      for (int i = 0; i < (int) mAppliedContractionCandidates.size(); i++) {
        if (mAppliedContractionCandidates[i]->getConstraint() == constraint) {
          firstApplicationIndex = i;
          isUsed = true;
          break;
        }
      }
    }

    // the constraint has been used, so we traverse the applied contraction from the end
    // until the first application index, and revert all bound constraints that were applied
    if (isUsed) {
      for (int i = mAppliedIntervalConstraints.size() - 1; i >= firstApplicationIndex; i--) {
        removeIntervalConstraints(mAppliedIntervalConstraints[i], mAppliedContractionCandidates[i]->getConstraint());
      }
      // actually delete those applied entries from the vectors
      mAppliedContractionCandidates.resize(firstApplicationIndex);
      mAppliedIntervalConstraints.resize(firstApplicationIndex);
    }

    return isUsed;
  }

  template<class Settings>
  void ICPState<Settings>::removeIntervalConstraints(const OneOrTwo<ConstraintT>& intervalConstraints, const ConstraintT& origin) {
    const ConstraintT& constraint1 = intervalConstraints.first;
    mBounds.removeBound(constraint1, origin);

    if (intervalConstraints.second) {
      const ConstraintT& constraint2 = *(intervalConstraints.second);
      mBounds.removeBound(constraint2, origin);
    }
  }

  template<class Settings>
  carl::Variable ICPState<Settings>::getConflictingVariable() {
    for (const auto& mapEntry : mBounds.getIntervalMap()) {
      if (mapEntry.second.isEmpty()) {
        return mapEntry.first;
      }
    }

    assert(false);
    return carl::freshRealVariable();
  }

  template<class Settings>
  bool ICPState<Settings>::isTerminationConditionReached() {
    if(mAppliedContractionCandidates.size() > Settings::maxContractions) {
#ifdef PDW_MODULE_DEBUG_1
      std::cout << "Termination reached by max iterations!" << std::endl;
#endif
      return true;
    }

    // check if maximum number of splits has been reached and terminate
    if(computeNumberOfSplits()>Settings::maxSplitNumber) {
#ifdef PDW_MODULE_DEBUG_1
      std::cout << "Termination reached by maximal number of splits!" << std::endl;
#endif
      return true;
    }

    //otherwise check if we have reached our desired interval
    //first check if all intervals are inside the desired one
    bool isTargetDiameterReached = true;
    for (auto key : (*mOriginalVariables) ) {
      if(mBounds.getDoubleInterval(key).isUnbounded() || mBounds.getDoubleInterval(key).diameter()>Settings::targetDiameter) {
        isTargetDiameterReached = false;
        break;
      }
    }
    if (isTargetDiameterReached) {
      //if all intervals are ok, just terminate
#ifdef PDW_MODULE_DEBUG_1
      std::cout << "Termination reached by desired interval diameter!" << std::endl;
#endif
      return true;
    }

    return false;
  }

  template<class Settings>
  int ICPState<Settings>::computeNumberOfSplits(){
    return mCorrespondingTree->getNumberOfSplits();
  }

  template<class Settings>
  std::experimental::optional<int> ICPState<Settings>::getBestContractionCandidate(vector<ICPContractionCandidate<Settings>*>& candidates){
    if(candidates.size()==0) {
      throw std::invalid_argument( "Candidates vector is empty!" );
    }
    //in the case that the list has only one element, return this one element
    if(candidates.size()==1) {
      return 0; //return the first element
    }


    //store the current best candidate index
    int currentBest = 0;
    std::experimental::optional<double> currentBestGain = (candidates[currentBest])->computeGain(mBounds);

     //store the new diameter in case two candidates with equal gain are regarded
    double currentBestAbsoluteReduction = (*currentBestGain)*(mBounds.getDoubleInterval((*(candidates[currentBest])).getVariable()).diameter());


    //first compute the new weighted gain according to W_new = W_old + alpha*(gain - W_old)
    double currentBestGainWeighted = (*(candidates[currentBest])).getWeight()+
              Settings::alpha*((*currentBestGain)-(*(candidates[currentBest])).getWeight());
    //update the weight
    //SMTRAT_LOG_INFO("smtrat.module","Old weight of " << (*(candidates[currentBest]))<< " is " << (*(candidates[currentBest])).getWeight());
    (*(candidates[currentBest])).setWeight(currentBestGainWeighted);
    //SMTRAT_LOG_INFO("smtrat.module","Weight of " << (*(candidates[currentBest]))<< " adjusted to " << currentBestGainWeighted);

    for (int it = 1; it < (int) candidates.size(); it++) {
      double currentGain = (candidates[it])->computeGain(mBounds);
      //compute the weighted gain
      double currentGainWeighted = (*(candidates[it])).getWeight()+
              Settings::alpha*(currentGain-(*(candidates[it])).getWeight());
      //update the weighted gain
      //SMTRAT_LOG_INFO("smtrat.module","Old weight of " << (*(candidates[it]))<< " is " << (*(candidates[it])).getWeight());
      (*(candidates[it])).setWeight(currentGainWeighted);
      //SMTRAT_LOG_INFO("smtrat.module","Weight of " << (*(candidates[it]))<< " adjusted to " << currentGainWeighted);

      //do not consider candidates with weight < weight_eps
      if(currentBestGainWeighted<Settings::weightEps&&currentGainWeighted>Settings::weightEps){
        //if the previous candidate have not at lease epsilon weight, we disregard it
        currentBestGainWeighted = currentGainWeighted;
        currentBestGain = currentGain;
        currentBest = it;
        //retrieve the new diameter by means of the formula D_old - (1-gain)*D_old = absolute reduction
        currentBestAbsoluteReduction = currentGain*(mBounds.getDoubleInterval((*(candidates[it])).getVariable()).diameter());
        continue;
      }
      //if the current candidate is below weightEps, we disregard it
      if(currentGainWeighted<Settings::weightEps){
        continue;
      }
      //now actual comparison can take place
      if(currentGainWeighted-currentBestGainWeighted>Settings::upperDelta) {//the delta states that between the old and the new gain
        //the difference has to be at least upperDelta, by using this behavior we are able to enforce that only candidates which
        //achieve a certain margin of gain are regarded as the new optimal
        //now set the new best candidate as current
        currentBestGainWeighted = currentGainWeighted;
        currentBestGain = currentGain;
        currentBest = it;
        //retrieve the new diameter by means of the formula D_old - (1-gain)*D_old = absolute reduction
        currentBestAbsoluteReduction = currentGain*(mBounds.getDoubleInterval((*(candidates[it])).getVariable()).diameter());

      }else if(currentGainWeighted-currentBestGainWeighted>Settings::lowerDelta){
        //if the gain is not high enough but still almost the same, it would be interesting to consider the absolute interval reduction
        double currentNewAbsoluteReduction = currentGain*(mBounds.getDoubleInterval((*(candidates[it])).getVariable()).diameter());
        //now check if the
        if(currentBestAbsoluteReduction<currentNewAbsoluteReduction){
           currentBestGainWeighted = currentGainWeighted;
           currentBestGain = currentGain;
           currentBest = it;
           //retrieve the new diameter by means of the formula D_old - (1-gain)*D_old = absolute reduction
           currentBestAbsoluteReduction = currentGain*(mBounds.getDoubleInterval((*(candidates[it])).getVariable()).diameter());
        }
      }
    }
#ifdef SMTRAT_DEVOPTION_Statistics
    if(currentBestGain){
      mCorrespondingTree->getCorrespondingModule()->
        getStatistics()->addContractionGain((*currentBestGain));
       mCorrespondingTree->getCorrespondingModule()->
        getStatistics()->increaseNumberOfContractions();
    }
#endif
    std::experimental::optional<int> ret;

    if(currentBestGain>Settings::gainThreshold) {
      //if the gain is beyond the threshold, return it
      ret = currentBest;
      return ret;
    }
    else{
      //otherwise return an optional.empty()
      return ret;
    }

  }

  template<class Settings>
  map<carl::Variable,double> ICPState<Settings>::guessSolution(){
    map<carl::Variable,double> ret;
    //TODO: This only checks the first of possibly several intervals?
    const EvalDoubleIntervalMap& bounds = mBounds.getIntervalMap();
    for(auto& bound : bounds) {
      double mid = 0; //Default if something goes wrong
      double epsilon = Settings::epsilon;
      if(bound.second.isEmpty()) { //No values in interval, should not happen
        mid = 0;
      } else if (bound.second.isInfinite()) { //Both are INFTY
        mid = 0;
      } else if (bound.second.isUnbounded()) { //Only one is INFTY
        if(bound.second.lowerBoundType() == carl::BoundType::INFTY) { //Has upper bound
          if(bound.second.upperBoundType() == carl::BoundType::STRICT) { //Upper bound is strict
            mid = bound.second.upper() - epsilon; //Subtract some small epsilon in this case
          } else {
            mid = bound.second.upper();
          }
        } else if(bound.second.upperBoundType() == carl::BoundType::INFTY) { //Has lower bound
          if(bound.second.lowerBoundType() == carl::BoundType::STRICT) { //lower bound is strict
            mid = bound.second.lower() - epsilon; //Subtract some small epsilon in this case
          } else {
            mid = bound.second.lower();
          }
        } else { //Should never happen
          mid = 0;
        }
      } else { //No INFTY
        mid = (bound.second.diameter()/ 2.0) + bound.second.lower();
      }
      ret.insert(std::pair<carl::Variable,double>(bound.first,mid));
    };

    return ret;

  }

  template<class Settings>
  carl::Variable ICPState<Settings>::getBestSplitVariable(){
    OneOrTwo<IntervalT> intervals;
    double currentInterval = 0;
    double bestSplitInterval = 0;
    std::vector<carl::Variable> unsatVars;
    Model model;

    //first collect all variables which are located in a unsat clause
    map<carl::Variable,double> sol(guessSolution());
     for(auto& clause : sol) {
        Rational val = carl::rationalize<Rational>(clause.second);
        model.emplace(clause.first, val);
      }
     for( const auto& rf : mCorrespondingTree->getCorrespondingModule()->rReceivedFormula()) {
       //fist check if this formula is sat
       unsigned isSatisfied = carl::model::satisfiedBy(rf.formula().constraint(), model);

       assert(isSatisfied != 2);
       if(isSatisfied == 0) {
          //if it is not sat, get the corresponding variables from the initial constraints
          for(const auto& var:rf.formula().constraint().variables()){
            if(std::find(unsatVars.begin(), unsatVars.end(), var) == unsatVars.end()) {
             //this var is not yet in the vector, add it
              unsatVars.push_back(var);
            }
          }
        }
      }
      carl::Variable bestSplitVariable = unsatVars[0];
    // now finally we can iterate over all variables which are part of an unsat clause
    for (carl::Variable var : unsatVars) {
      //first compute the diameter of a variable
      currentInterval = mBounds.getDoubleInterval(var).diameter();
      //now check if the new interval is "bigger"
      if(bestSplitInterval<currentInterval) {
        bestSplitVariable = var;
        bestSplitInterval = currentInterval;
      }
    }
    //finally return the variable of the biggest interval
    return bestSplitVariable;
  }


};
