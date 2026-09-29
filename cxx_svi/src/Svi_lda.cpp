#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>
#include <ostream>
#include <sstream>
#include "Svi_lda.h"
#include "InternalState.h"
#include "Vocabulary.h"
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "ViUtils.h"
#include <cmath>
#include "SaveAsMallet.h"
#include <fstream>

// inline void softmaxRowsRange(std::vector<float>& rows,
//                              int K,
//                              std::size_t start_row,
//                              std::size_t end_row_excl)
// {
//     if (K <= 0) return;
//     const std::size_t total_rows = rows.size() / static_cast<std::size_t>(K);
//     if (total_rows == 0) return;

//     start_row    = std::min(start_row, total_rows);
//     end_row_excl = std::min(end_row_excl, total_rows);
//     if (start_row >= end_row_excl) return;

//     for (std::size_t r = start_row; r < end_row_excl; ++r) {
//         float* row = rows.data() + r * static_cast<std::size_t>(K);

//         float m = row[0];
//         for (int t = 1; t < K; ++t) m = std::max(m, row[t]);

//         float denom = 0.0f;
//         for (int t = 0; t < K; ++t) { row[t] = std::exp(row[t] - m); denom += row[t]; }

//         if (denom == 0.0f) {
//             const float u = 1.0f / static_cast<float>(K);
//             for (int t = 0; t < K; ++t) row[t] = u;
//         } else {
//             const float inv = 1.0f / denom;
//             for (int t = 0; t < K; ++t) row[t] *= inv;
//         }
//     }
// }

void updateLocalParmas(std::vector<float> &mu_blue_hat,
                       const std::vector<SparseDocument> &data,
                       Internal_State &internal,
                       int vi_iter_per_batch,
                       long long startIdx,
                       int batchSize,
                       int nb_sat_assig_per_constraint,
                       const std::vector<float> &prior,
                     double scaling_factor)
{
   const int K = nb_sat_assig_per_constraint; // number of topcis
   const int V = internal.blue.num_outcomes;  // number of distinct words in corpus

   // actual batch end (guard against tail batch)
   const long long endIdx = startIdx + batchSize;

   // Start blue-hat from the blue prior each batch
   std::fill(mu_blue_hat.begin(), mu_blue_hat.end(), prior[1]);

   // Count total distinct words in this batch to size local buffer once
   long long num_distinct_words_in_batch = 0;
   for (int i = startIdx; i < endIdx; ++i)
      num_distinct_words_in_batch += data[i].numDistinctWordsPerDoc;

   // lambdas per batch
   std::vector<float> local_lambdas(num_distinct_words_in_batch * K, 0.0f);

   // Ensure digammas are fresh for the first VI iteration
   // internal.prepare_red_die();

   // I am using vi_iter_per_batch iterations on each batch assuming it will converge after this ammount.
   for (int iter = 0; iter < vi_iter_per_batch; ++iter)
   {
      long long offset = 0; // row offset within batch buffer

      for (int docIdx = startIdx; docIdx < endIdx; ++docIdx)
      {
         const SparseDocument &doc = data[docIdx];
         const int W = doc.numDistinctWordsPerDoc;

         const float red_sum = internal.red.digamma_sum_mu[docIdx];

         for (int w = 0; w < W; ++w)
         {
            const int wordId = doc.tokenIndex[w];
            float *row = &local_lambdas[(long long)(offset + w) * K];

            // per-topic components for this word
            for (int t = 0; t < K; ++t)
            {
               const float red_mu = internal.red.digamma_mu[docIdx * internal.red.num_outcomes + t];
               const float blue_sum = internal.blue.digamma_sum_mu[t];
               const float blue_mu = internal.blue.digamma_mu[t * V + wordId];
               // row[t] = ((red_mu - red_sum) + (blue_mu - blue_sum));
               // local_lambdas[((w + offset) * nb_sat_assig_per_constraint) + t] = exp(row[t]);
               row[t] = fastExp((red_mu - red_sum) + (blue_mu - blue_sum));
            }
         }

         normalizeLambdasRange(local_lambdas, nb_sat_assig_per_constraint, offset, offset + W);

         // update mu_red
         for (int t = 0; t < K; ++t)
         {
            float upd = 0.0f;
            for (int w = 0; w < W; ++w)
            {
               upd += (float)doc.tokenCount[w] * local_lambdas[((long long)offset + w) * K + t];
            }
            // per-topic prior (use scalar prior[0] if that’s your design)
            const float alpha_k = internal.red.prior[0];
            internal.set_mu_red(docIdx * internal.red.num_outcomes + t, alpha_k + upd);
         }

         offset += W;
         internal.prepare_red_die_for_doc(docIdx);
      }

      // internal.prepare_red_die();
   }

   // Accumulate expected counts into mu_blue_hat (the intermediate param)
   long long offset = 0;
   for (int docIdx = startIdx; docIdx < endIdx; ++docIdx)
   {
      const SparseDocument &doc = data[docIdx];
      const int W = doc.numDistinctWordsPerDoc;

      for (int w = 0; w < W; ++w)
      {
         const int wordId = doc.tokenIndex[w];
         const int cnt = doc.tokenCount[w];
         const float *row = &local_lambdas[((long long)offset + w) * K];

         for (int t = 0; t < K; ++t)
         {
            mu_blue_hat[t * V + wordId] += cnt * row[t]*scaling_factor;
         }
      }
      offset += W;
   }
}

// ------------------------------- runSVI -------------------------------
void runSVI(
    std::vector<SparseDocument> &data,
    int num_vars_red,
    int num_outcomes_red,
    int num_vars_blue,
    int num_outcomes_blue,
    int nb_sat_assig_per_constraint, // K
    int batchSize,
    int total_num_iter,
    int save_every,
    const std::vector<float> &priors, // [0]=red prior (scalar or per-topic), [1]=blue prior (scalar)
    const std::string &malletStateDir,
    Vocabulary &malletVocab,
    MappingReader &mapping,
    const std::string &outfileId,
    int vi_iter_per_batch,
    std::vector<int> &data_eval // the data from the csv used for evaluation
)
{
   // ------------------------------------------- init -------------------------------------------
   long long nb_unique_constraints = 0;
   for (const auto &doc : data)
      nb_unique_constraints += doc.numDistinctWordsPerDoc;
   std::cout << "Number of unique constraints in the dataset: " << nb_unique_constraints << std::endl;

   const long long total_unique_nb_sat_assig = nb_unique_constraints * nb_sat_assig_per_constraint;
   const int numDocuments = data.size();

   Internal_State interna_state(total_unique_nb_sat_assig,
                                nb_sat_assig_per_constraint,
                                num_vars_red, num_outcomes_red,
                                num_vars_blue, num_outcomes_blue);

   interna_state.initmuGamma();
   interna_state.setRedPrior(priors[0]);
   interna_state.setBluePrior(priors[1]);
   interna_state.prepare_blue_die();
   interna_state.prepare_red_die();


   const int num_batches = (numDocuments + batchSize - 1) / batchSize;
   std::vector<float> all_rhots = precompute_rhot(total_num_iter);

   std::vector<float> mu_blue_hat((std::size_t)num_vars_blue * num_outcomes_blue, priors[1]);
   std::vector<float> lambda_eval(static_cast<size_t>(nb_sat_assig_per_constraint) * static_cast<size_t>(nb_unique_constraints), 0.0f); // This will be used to create the mallet state for the evaluation. This is not used in training.
   std::vector<int> tokenTopic(nb_unique_constraints, 0);                                                                               // This will be used to create the mallet state for the evaluation. This is not used in training.

   // --- main SVI loop ------------------------------------------------------
   for (int it = 0; it < total_num_iter; ++it)
   {
      const auto t0 = std::chrono::high_resolution_clock::now();

      const double rhot = all_rhots[it];
      const double one_minus_rhot = 1.0 - rhot;

      const long long b = it % num_batches;
      const long long startIx = b * batchSize;
      const long long currentBatchSize = std::min<long long>(batchSize, numDocuments - startIx);




      const double scaling_factor =
          static_cast<double>(numDocuments) / static_cast<double>(currentBatchSize);
      std::cout << " scaling_factor =" << scaling_factor << endl;
      updateLocalParmas(mu_blue_hat, data, interna_state,
                        vi_iter_per_batch, startIx, currentBatchSize,
                        nb_sat_assig_per_constraint, priors,scaling_factor);

      
      // Merging the intermediate parameters with the global ones
      const int Nblue = num_vars_blue * num_outcomes_blue;
      for (int i = 0; i < Nblue; ++i)
      {
         interna_state.blue.mu[i] =
             (float)(one_minus_rhot * interna_state.blue.mu[i] +
                     rhot * ( mu_blue_hat[i]));
      }

      // compute the digammas for the blue die since it was updated
      interna_state.prepare_blue_die();

      const auto t1 = std::chrono::high_resolution_clock::now();

      auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
      std::cout << "#### " << it + 1 << "," << timeMs << std::endl;

      // --- save state every 'save_every' iterations -------------------------
      size_t write = 0;
      if (((it + 1) % save_every) == 0 && save_every != -1)
      {
         write = 0;
         size_t offset = 0;

         for (size_t docId = 0; docId < numDocuments; docId++)
         {
            size_t numDistinctTokens = data[docId].numDistinctWordsPerDoc;
            const SparseDocument &doc = data[docId];
            const size_t W = doc.numDistinctWordsPerDoc;
            const size_t V = interna_state.blue.num_outcomes;
            const float red_sum = interna_state.red.digamma_sum_mu[docId];
            for (size_t w = 0; w < W; ++w)
            {
               const size_t wordId = doc.tokenIndex[w];
               float *row = &lambda_eval[(std::size_t)(offset + w) * nb_sat_assig_per_constraint];
               // per-topic components for this word
               for (size_t t = 0; t < nb_sat_assig_per_constraint; ++t)
               {
                  const float red_mu = interna_state.red.digamma_mu[docId * interna_state.red.num_outcomes + t];
                  const float blue_sum = interna_state.blue.digamma_sum_mu[t];

                  const float blue_mu = interna_state.blue.digamma_mu[t * V + wordId];
                  write++;
                  const float log_phi = (red_mu - red_sum) + (blue_mu - blue_sum);
                  row[t] = exp(log_phi);
               }
            }
            offset += W;
         }
         std::cout << "write= " << write << ",size(lambda_eval)= " << size(lambda_eval) << std::endl;
         // Now you run  getTokenTopic(tokenTopic, allProbsEval, numTopics) to get the topic assignment for each token in the corpus out of allProbsEval

         getTokenTopic(tokenTopic, lambda_eval, nb_sat_assig_per_constraint);
         std::cout << "size of tokenTopic= " << size(tokenTopic) << std::endl;
         // Now run saveAsMalletBatchVI(malletStateOFS, numTopics, &malletVocab, data, tokenTopic, mapping);
         // printMatrixRowMajor(allProbsEval, totalDistinctWordsInCorpus, numTopics);

         std::ofstream malletStateOFS;
         std::string snapshotFilePath = malletStateDir + "/chain-state_" + outfileId + ".txt." + std::to_string(it + 1);
         std::cout << "writing in file: " << snapshotFilePath << std::endl;
         malletStateOFS.open(snapshotFilePath);
         if (!malletStateOFS.is_open())
         {
            std::cerr << "Failed to open file: " << snapshotFilePath << std::endl;
         }
         saveAsMalletBatchVI(malletStateOFS, nb_sat_assig_per_constraint, &malletVocab, data_eval, tokenTopic, mapping);
         malletStateOFS.flush();
         malletStateOFS.close();
         // printTopics(interna_state.blue.mu, nb_sat_assig_per_constraint, malletVocab);
      }
   }
}

void printTopics(const std::vector<float> &mu_blue,
                 int K, // number of topics
                 const Vocabulary &vocab,
                 int topN,       // how many words per topic
                 bool normalize, // show probabilities (row-normalized)
                 std::ostream &os)
{

   int V = vocab.getSize();
   if (K <= 0 || V <= 0 || (int)mu_blue.size() != K * V)
   {
      os << "printTopicsSimple: invalid sizes (K=" << K << ", V=" << V
         << ", mu_blue.size()=" << mu_blue.size() << ")\n";
      return;
   }
   topN = std::max(1, std::min(topN, V));
   os << std::fixed << std::setprecision(6);

   for (int k = 0; k < K; ++k)
   {
      const float *row = mu_blue.data() + (std::size_t)k * V;

      // Optional normalization to probabilities per topic
      double rowsum = 0.0;
      if (normalize)
      {
         for (int j = 0; j < V; ++j)
         {
            float x = row[j];
            if (std::isfinite(x) && x > 0.f)
               rowsum += x;
         }
      }

      auto score = [&](int j) -> double
      {
         float x = row[j];
         if (!std::isfinite(x) || x <= 0.f)
            return 0.0;
         return normalize && rowsum > 0.0 ? (double)x / rowsum : (double)x;
      };

      // Build index list 0..V-1
      std::vector<int> idx(V);
      std::iota(idx.begin(), idx.end(), 0);

      // Select topN by value using nth_element, then sort those topN
      auto cmp = [&](int a, int b)
      { return score(a) > score(b); };
      std::nth_element(idx.begin(), idx.begin() + topN, idx.end(), cmp);
      std::sort(idx.begin(), idx.begin() + topN, cmp);

      // Header
      os << "\nTopic " << k << " (top " << topN
         << (normalize ? ", normalized" : ", raw") << ")\n";
      os << "--------------------------------------------------------\n";
      os << std::left << std::setw(4) << "#"
         << std::setw(10) << "code"
         << std::setw(30) << "word"
         << std::right << std::setw(12) << (normalize ? "prob" : "weight") << "\n";

      // Rows
      for (int r = 0; r < topN; ++r)
      {
         int code = idx[r]; // <-- word code = column index
         const std::string word = vocab.getWordByCode(code);
         double val = score(code);
         os << std::left << std::setw(4) << (r + 1)
            << std::setw(10) << code
            << std::setw(30) << word.substr(0, 29)
            << std::right << std::setw(12) << val << "\n";
      }
   }
}
