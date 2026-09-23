#ifndef _SAMPLEREXECUTIONCONTEXT_H_
#define _SAMPLEREXECUTIONCONTEXT_H_

template<typename EXPR_T>
class SamplerExecutionContext
{
public:

   typedef EXPR_T expr_t;

   int *args;
   int *argsBase;
   int argsRecordLength;

   double *probs;
   double *probsBase;
   int probsRecordLength;

   int *retracted;
   int *retractedBase;
   int retractedRecordLength;

   uint64_t *rndSeed;

   SamplerExecutionContext(int *args, int *argsBase, int argsRecordLength, 
                           double *probs, double *probsBase, int probsRecordLength, 
                           int *retracted, int *retractedBase, int retractedRecordLength,
                           uint64_t *rndSeed):
      args(args), argsBase(argsBase), argsRecordLength(argsRecordLength), 
      probs(probs), probsBase(probsBase), probsRecordLength(probsRecordLength),
      retracted(retracted), retractedBase(retractedBase), retractedRecordLength(retractedRecordLength),
      rndSeed(rndSeed)
      {}

   void shift(int to)
   {
      //if (to==0)
      //{
      //   throw new std::runtime_error("bad shift");
      //}
      //std::cout << "shifting: " << to << std::endl; 
      args -= (to*argsRecordLength);
      probs -= (to*probsRecordLength);
      retracted -= (to*retractedRecordLength);
   };

}; // SamplerExecutionContext

#endif  // _SAMPLEREXECUTIONCONTEXT_H_