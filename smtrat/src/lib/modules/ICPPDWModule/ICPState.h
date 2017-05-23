/*
 * File:   ICPState.h
 * Author: David
 */

#pragma once

#include "../../Common.h"
#include "ICPContractionCandidate.h"
#include "ICPPDWSettings.h"
#include <map>
#include <math.h>
#include <stdexcept>


namespace smtrat
{

  class ICPTree;//in order to avoid circular dependencies
  /**
   * Represents a state of the ICP algorithm.
   *
   * A state stores the current search box,
   * all the contraction candidates that have been applied, and
   * all the constraints that were added as a result of contraction (lower & upper bounds).
   * In case of a split, the state stores the dimension (i.e. variable) in which the split occurred.
   * In case of unsatisfiability, this state stores the reasons (i.e. constraints).
   */
  class ICPState
  {
    private:
      /**in order to find out when to terminate we need to check if the diameter has
       * been reached. for this purpose a pointer to the initial set of variables is required
       */
      std::set<carl::Variable>* mOriginalVariables;

      /**In order to get the number of already performed splittings it is mandatory to store
       * a pointer to the current tree.
       */
      ICPTree* mCorrespondingTree;

      /**
       * The current search box.
       * VariableBounds internally stores a map from variables to intervals.
       * It also keeps track of which constraints were the reasons for the
       * intervals. This way, we can automatically revert applied constraints.
       */
      vb::VariableBounds<ConstraintT> mBounds;

      /**
       * The contraction candidates that have been applied.
       * They will be stored in the order that they have been applied.
       */
      vector<ICPContractionCandidate*> mAppliedContractionCandidates;

      /**
       * After we apply a contraction candidate, we add the lower and upper
       * bounds of the contracted interval as constraints. Those constraints
       * are stored in this vector.
       *
       * We choose a vector to store constraint pairs instead of std::pair
       * because we might only add one constraint if the interval was unbounded.
       *
       * The index of applied interval constraints coincides with
       * the index of its contraction candidate in mContractionCandidates.
       */
      vector<OneOrTwo<ConstraintT>> mAppliedIntervalConstraints;

    public:
      /**
       * Default constructor will create an empty state with no variable bounds.
       */
      ICPState(ICPTree* correspondingTree);

      /**
       * Behaves the samme way as the default custuctor, but sets the point to the set of variables
       */
      ICPState(std::set<carl::Variable>* originalVariables,ICPTree* correspondingTree);

      /**
       * This constructor will initialize the variable bounds with a copy of parentBounds.
       */
      ICPState(const vb::VariableBounds<ConstraintT>& parentBounds,std::set<carl::Variable>* originalVariables,ICPTree* correspondingTree);

      /**
       * Returns the current search box as VariableBounds.
       *
       * @return reference to the search box
       */
      vb::VariableBounds<ConstraintT>& getBounds();

      /**
       * Applies a contraction to this state.
       *
       * The bounds of the contraction candidate variable will be updated to the given inteval.
       * Internally, mAppliedContractionCandidates and mAppliedIntervalConstraints will be filled.
       *
       * @param cc The contraction candidate that has been applied
       * @param interval The contracted interval that should be applied
       */
      void applyContraction(ICPContractionCandidate* cc, IntervalT interval);

      /**
       * Updates the current interval bound for a specific variable.
       * Internally it adds two new constraints for the lower and upper bound.
       *
       * @param var The variable of which the interval should be updated
       * @param interval The new interval for that variable
       * @param _origin The constraint which caused the new interval
       * @return the constraints that were added to the bounds
       */
      OneOrTwo<ConstraintT> setInterval(carl::Variable var, const IntervalT& interval, const ConstraintT& _origin);

      /**
       * Returns the current interval bound for a specific variable.
       *
       * @param var The variable
       * @return The interval
       */
      IntervalT getInterval(carl::Variable var);

      vector<ICPContractionCandidate*>& getAppliedContractionCandidates();
      void addAppliedContractionCandidate(ICPContractionCandidate* contractionCandidate);

      vector<OneOrTwo<ConstraintT>>& getAppliedIntervalConstraints();
      void addAppliedIntervalConstraint(const OneOrTwo<ConstraintT>& constraints);

      /**
       * @return the variable that has an empty interval.
       */
      carl::Variable getConflictingVariable();

      /**
       * For a given contraction candidate compute the new interval, subsequently the gain by the formula 1- D_new/D_old
       */
      double computeGain(smtrat::ICPContractionCandidate& candidate,vb::VariableBounds<ConstraintT>& _bounds);

      /**
       * Chooses the best contraction candidate.
       *
       * @param contractionCandidates A list of available contraction candidates
       * @return the best contraction candidate
       */
      std::experimental::optional<int> getBestContractionCandidate(vector<ICPContractionCandidate*>& contractionCandidates);

      /**
       * Determines whether we should stop contracting.
       * (e.g. because the target diameter was reached)
       *
       * @return whether the termination condition was reached
       */
      bool isTerminationConditionReached();

      /**
       * Guesses a solution for the mBonds by choosing some values out of them.
       * @return The solution in the form of a map <carl::Variable,double>
       */
      map<carl::Variable,double> guessSolution();

      /**
       * For all stored variables and intervals, find the best original variable
       * for manual splitting.
       */
      carl::Variable getBestSplitVariable();

      /**
       * Return the number of splits performed.
       */
      int computeNumberOfSplits();

  };
}
