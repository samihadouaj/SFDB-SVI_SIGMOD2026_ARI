#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include "ReadData.h"
#include "time.h"
#include "chrono"
#include "MappingReader.h"
#include "Vocabulary.h"
#include "argparse.hpp"
#include <iomanip>
#include "Svi_lda.h"
#include "Svi_ldaP.h"

using namespace std;




static inline std::string joinPath(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    char last = a.back();
    if (last == '/' || last == '\\') return a + b;
    return a + "/" + b;
}

int main(int argc, const char** argv) {
    // ---------------- defaults matching your main ----------------
    std::string lda_datasetName       = "KOS";
    std::string lda_datasetNameSuffix = "";
    std::string lda_vi_dataset_dir    = ".";

    double alpha = 0.1;   // --lda_alpha
    double eta   = 0.2;  // --lda_beta
    int    numTopics      = 10;

    int    totalNumIter   = 3300; // --total_num_iter
    int    save_every     = 400;  // --reportEvery
    int    minibatchSize  = 200;  // --batchSize
    int    vi_iter_per_batch   = -1;    // --vi_iter_per_batch

    int    rndSeed        = 123;  

    // fixed schedule like your main
    // const double tau        = 1000.0;
    // const double kappa      = 0.9;
    // const double scale      = 10.0;

     int numDocuments = 3268;
     int numWords     = 6906;
    string outputDir = "";
    string malletStateDir="";
    string lda_outfileId="";
    int num_threads =1;
    const bool opt = true;
    ArgumentParser args;
    args.addArgument("-d", "--lda_datasetName", 1, opt);
    args.addArgument("-s", "--lda_datasetNameSuffix", 1, opt);
    args.addArgument("--lda_vi_dataset_dir", 1, opt);
    args.addArgument("-a", "--lda_alpha", 1, opt);
    args.addArgument("-b", "--lda_beta", 1, opt);
    args.addArgument("--lda_numTopics", 1, opt);
    args.addArgument("--total_num_iter", 1, opt);
    args.addArgument("--reportEvery", 1, opt);
    args.addArgument("--batchSize", 1, opt);
    args.addArgument("--vi_iter_per_batch", 1, opt);
    args.addArgument("--lda_numDocuments", 1);
    args.addArgument("--lda_vocabSize", 1);
    args.addArgument("--lda_outputDir", 1);
    args.addArgument("--malletStateDir", 1);
    args.addArgument("--lda_outfileId", 1);
    args.addArgument("--num_threads", 1, opt);

   args.parse(argc, argv);  
    auto has = [&](const char* k){ return args.count(k) > 0; };
    auto getS = [&](const char* k){ return args.retrieve<std::string>(k); };

    if (has("lda_datasetName"))        lda_datasetName       = getS("lda_datasetName");
    if (has("lda_datasetNameSuffix"))  lda_datasetNameSuffix = getS("lda_datasetNameSuffix");
    if (has("lda_vi_dataset_dir"))     lda_vi_dataset_dir    = getS("lda_vi_dataset_dir");

    if (has("lda_alpha"))              alpha        = std::stod(getS("lda_alpha"));
    if (has("lda_beta"))               eta          = std::stod(getS("lda_beta"));
    if (has("lda_numTopics"))          numTopics    = std::stoi(getS("lda_numTopics"));

    if (has("total_num_iter"))         totalNumIter = std::stoi(getS("total_num_iter"));
    if (has("reportEvery"))            save_every   = std::stoi(getS("reportEvery"));
    if (has("batchSize"))              minibatchSize= std::stoi(getS("batchSize"));
    if (has("vi_iter_per_batch"))      vi_iter_per_batch = std::stoi(getS("vi_iter_per_batch"));
    if (has("lda_numDocuments"))       numDocuments = std::stoi(getS("lda_numDocuments"));
    if (has("lda_vocabSize"))          numWords     = std::stoi(getS("lda_vocabSize"));
    if (has("lda_outputDir"))          outputDir    = getS("lda_outputDir");
    if (has("malletStateDir"))         malletStateDir = getS("malletStateDir");
    if (has("num_threads"))            num_threads  = std::stoi(getS("num_threads"));
    // if (has("lda_outfileId"))          lda_outfileId = getS("lda_outfileId");


    lda_outfileId = lda_datasetName+lda_datasetNameSuffix+"_"+std::to_string(numTopics)+"topics_A"+std::to_string(alpha)+"_B"+std::to_string(eta)+"_NI"+std::to_string(totalNumIter)+"_NT_"+std::to_string(num_threads)+"_RND"+std::to_string(rndSeed);
if (args.count("lda_outfileId")) {
    lda_outfileId = args.retrieve<std::string>("lda_outfileId");
}

    // ---------------- derive file paths exactly like your main ----------------
    const std::string csv2_dir   = joinPath(lda_vi_dataset_dir, "csv2");
    const std::string cxx_svi_csv2_dir = joinPath(joinPath(lda_vi_dataset_dir, "cxx_svi"), "csv2"); // C++SVI-format files (converter output)
    const std::string base       = lda_datasetName + lda_datasetNameSuffix;

    const std::string sparseDataFileName = joinPath(cxx_svi_csv2_dir, base + "_train_svi.csv");            // readDocsFast
    const std::string dictionaryFileName = joinPath(csv2_dir, base + "_vocab.csv");         
    const std::string evalTripletsCSV    = joinPath(csv2_dir, base + "_train.csv");                // parseFileToFlatVector
    const std::string malletVocabCSV     = joinPath(csv2_dir, base + "_vocab_mallet.csv");   // Vocabulary
    const std::string mappingCSV         = joinPath(cxx_svi_csv2_dir, base + "_train_svi_mapping.csv");    // MappingReader

    cout<<"mappingCSV "<<mappingCSV<<endl;
    cout<<"malletVocabCSV " <<malletVocabCSV<<endl;
    cout<<"evalTripletsCSV "<<evalTripletsCSV<<endl;
    cout<<"sparseDataFileName "<<sparseDataFileName<<endl;
    cout<<"dictionaryFileName "<<dictionaryFileName<<endl;

    std::cout << "Number of documents: " << numDocuments << "\n";

    // ---------------- load data ----------------
    
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<SparseDocument> documents = readDocsFast(sparseDataFileName, numDocuments);
    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Read sparse docs in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
              << " ms\n";

    std::cout << evalTripletsCSV << "\n";
    std::vector<int> data_eval = parseFileToFlatVector(evalTripletsCSV);
    std::cout << "Size of data (tokens): " << (data_eval.size()/3) << "\n";

    Vocabulary    malletVocab(malletVocabCSV);
    MappingReader mapping(mappingCSV);


// ---------- Pretty config print ----------
auto fmt = [](double v) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::setprecision(6) << v;
    return oss.str();
};

std::vector<std::pair<std::string, std::string>> rows = {
    {"lda_datasetName",        lda_datasetName},
    {"lda_datasetNameSuffix",  lda_datasetNameSuffix},
    {"lda_vi_dataset_dir",     lda_vi_dataset_dir},
    {"lda_alpha",              fmt(alpha)},
    {"lda_beta",               fmt(eta)},
    {"lda_numTopics",          std::to_string(numTopics)},
    {"total_num_iter",         std::to_string(totalNumIter)},
    {"reportEvery",            std::to_string(save_every)},
    {"batchSize",              std::to_string(minibatchSize)},
    {"vi_iter_per_batch",      std::to_string(vi_iter_per_batch)},
    {"rndSeed",                std::to_string(rndSeed)},
    {"num_threads",            std::to_string(num_threads)},
    {"lda_numDocuments",       std::to_string(numDocuments)},
    {"lda_vocabSize",          std::to_string(numWords)},
    {"lda_outputDir",          outputDir},
    {"malletStateDir",         malletStateDir},
    {"lda_outfileId",          lda_outfileId},
    // Derived file paths
    {"sparseDataFileName",     sparseDataFileName},
    {"dictionaryFileName",     dictionaryFileName},
    {"evalTripletsCSV",        evalTripletsCSV},
    {"malletVocabCSV",         malletVocabCSV},
    {"mappingCSV",             mappingCSV},
};



int num_vars_red = numDocuments;
int num_outcomes_red = numTopics;
int num_vars_blue = numTopics;
int num_outcomes_blue = numWords;
int nb_sat_assig_per_constraint = numTopics; // this is K

// compute column width for neat alignment
size_t labelw = 0;
for (const auto& r : rows) labelw = std::max(labelw, r.first.size());

std::cout << "\n====================== LDA CONFIG ======================\n";
for (const auto& r : rows) {
    std::cout << std::left << std::setw(static_cast<int>(labelw) + 2)
              << r.first << ": " << r.second << '\n';
}
std::cout << "=======================================================\n\n";
// ---------- end pretty config print ----------
    // ---------------- run SCVB0 with your exact signature ----------------
   

//  if(num_threads ==1)
//  {
// runSVI(documents,
//            num_vars_red,
//            num_outcomes_red,
//            num_vars_blue,
//            num_outcomes_blue, nb_sat_assig_per_constraint,minibatchSize,totalNumIter,save_every,
//            {static_cast<float>(alpha), static_cast<float>(eta)},malletStateDir,malletVocab,mapping,lda_outfileId,vi_iter_per_batch,data_eval);
//  }   

//  else{
    
runSVIP(documents,
           num_vars_red,
           num_outcomes_red,
           num_vars_blue,
           num_outcomes_blue, nb_sat_assig_per_constraint,minibatchSize,totalNumIter,save_every,
           {static_cast<float>(alpha), static_cast<float>(eta)},malletStateDir,malletVocab,mapping,lda_outfileId,vi_iter_per_batch,data_eval,num_threads);
//  }

    return 0;
}