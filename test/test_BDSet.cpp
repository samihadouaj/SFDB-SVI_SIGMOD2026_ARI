/*
   Copyright 2023 University of Michigan-Dearborn

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "catch.hpp"
#include "../src/BDSet.h"

TEST_CASE( "BDSet::ctor" )
{
   const int NUMVARS = 2;      // 2 dices
   const int NUMOUTCOMES = 6;  // 6 faces per dice
   const int NUMOBS = 4;       // 4 observations (over all the dices)
   const double prior = 2.0;
   
   BDSet<NUMVARS,NUMOUTCOMES,NUMOBS> bdSet(prior);
   const int UNOBSERVED = bdSet.UNOBSERVED;

   for (int varId=0; varId<NUMVARS; varId++) {
      for (int obsId=0; obsId<NUMOBS; obsId++) {
         REQUIRE( UNOBSERVED == bdSet.getObservedValue(varId, obsId) );
      }
   }

   for (int varId=0; varId<NUMVARS; varId++) {
      REQUIRE( 0 == bdSet.countObservations(varId) );
   }

   for (int varId=0; varId<NUMVARS; varId++) {
      for (int valId=0; valId<NUMOUTCOMES; valId++) {
         REQUIRE( 0 == bdSet.countObservations(varId, valId) );
      }
   }

   for (int varId=0; varId<NUMVARS; varId++) {
      for (int valId=0; valId<NUMOUTCOMES; valId++) {
         REQUIRE( prior == bdSet.getDirichletHyperParam(varId, valId) );
      }
   }

   for (int varId=0; varId<NUMVARS; varId++) {
      REQUIRE( (prior*NUMOUTCOMES) == bdSet.getDirichletHyperParamSum(varId) );
   }

   for (int varId=0; varId<NUMVARS; varId++) {
      for (int valId=0; valId<NUMOUTCOMES; valId++) {
         REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(varId, valId) );
      }
   }

}


TEST_CASE( "BDSet::setDirichletHyperParam" )
{
   const int NUMVARS = 2;      // 2 dices
   const int NUMOUTCOMES = 6;  // 6 faces per dice
   const int NUMOBS = 4;       // 4 observations (over all the dices)
   const double oldPrior = 1.0;
   const double newPrior = 2.0;
   
   BDSet<NUMVARS,NUMOUTCOMES,NUMOBS> bdSet(oldPrior);

   const int varId = 0;
   const int valId = 0;
   const int otherVarId = 1;
   const int otherValId = 1;
   bdSet.setDirichletHyperParam(varId, valId, newPrior);
   REQUIRE( newPrior == bdSet.getDirichletHyperParam(varId, valId) );
   REQUIRE( oldPrior == bdSet.getDirichletHyperParam(otherVarId, valId) );
   REQUIRE( oldPrior == bdSet.getDirichletHyperParam(otherVarId, otherValId) );
   REQUIRE( oldPrior == bdSet.getDirichletHyperParam(varId, otherValId) );
   REQUIRE( ((oldPrior*(NUMOUTCOMES-1))+newPrior) == bdSet.getDirichletHyperParamSum(varId) );
   REQUIRE( (oldPrior*NUMOUTCOMES) == bdSet.getDirichletHyperParamSum(otherVarId) );
}

TEST_CASE( "BDSet::assertObservation/retractObservation" )
{
   const int NUMVARS = 2;      // 2 dices
   const int NUMOUTCOMES = 6;  // 6 faces per dice
   const int NUMOBS = 4;       // 4 observations (over all the dices)
   const double prior = 1.0;

   BDSet<NUMVARS,NUMOUTCOMES,NUMOBS> bdSet(prior);

   const int varId = 0;
   const int valId = 0;
   const int obsId = 0;
   const int otherObsId = 1;
   const int otherVarId = 1;
   const int otherValId = 1;
   const int UNOBSERVED = bdSet.UNOBSERVED;
   bdSet.assertObservation(varId, obsId, valId);
   
   REQUIRE( valId == bdSet.getObservedValue(varId, obsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, obsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(varId, otherObsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, otherObsId)); 

   REQUIRE( 1 == bdSet.countObservations(varId) );
   REQUIRE( 1 == bdSet.countObservations(varId, valId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId, valId) );
   REQUIRE( 0 == bdSet.countObservations(varId, otherValId) );

   REQUIRE( (2.0/(NUMOUTCOMES+1.0)) == bdSet.getPosteriorPredictive(varId, valId) );
   REQUIRE( (1.0/(NUMOUTCOMES+1.0)) == bdSet.getPosteriorPredictive(varId, otherValId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, valId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, otherValId) );
   
   bdSet.assertObservation(varId, otherObsId, valId);

   REQUIRE( valId == bdSet.getObservedValue(varId, obsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, obsId)); 
   REQUIRE( valId == bdSet.getObservedValue(varId, otherObsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, otherObsId)); 

   REQUIRE( 2 == bdSet.countObservations(varId) );
   REQUIRE( 2 == bdSet.countObservations(varId, valId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId, valId) );
   REQUIRE( 0 == bdSet.countObservations(varId, otherValId) );

   REQUIRE( (3.0/(NUMOUTCOMES+2.0)) == bdSet.getPosteriorPredictive(varId, valId) );
   REQUIRE( (1.0/(NUMOUTCOMES+2.0)) == bdSet.getPosteriorPredictive(varId, otherValId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, valId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, otherValId) );

   bdSet.retractObservation(varId, otherObsId);

   REQUIRE( valId == bdSet.getObservedValue(varId, obsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, obsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(varId, otherObsId)); 
   REQUIRE( UNOBSERVED == bdSet.getObservedValue(otherVarId, otherObsId)); 

   REQUIRE( 1 == bdSet.countObservations(varId) );
   REQUIRE( 1 == bdSet.countObservations(varId, valId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId) );
   REQUIRE( 0 == bdSet.countObservations(otherVarId, valId) );
   REQUIRE( 0 == bdSet.countObservations(varId, otherValId) );

   REQUIRE( (2.0/(NUMOUTCOMES+1.0)) == bdSet.getPosteriorPredictive(varId, valId) );
   REQUIRE( (1.0/(NUMOUTCOMES+1.0)) == bdSet.getPosteriorPredictive(varId, otherValId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, valId) );
   REQUIRE( (1.0/NUMOUTCOMES) == bdSet.getPosteriorPredictive(otherVarId, otherValId) );
}

