// // #include "Atom.h"

// #include "Tau.h"
// #include "ReadData.h"
// #include "Internal_State.h"
// #include "VIDie.h"
// #include "utils/TimeThis.h"
// #include "SavingMalletState.h"
// #include "utils/argparse.hpp"
// #include "utils/DebugHelper.h"
// #include <string>
// #include <signal.h>
// #include "ViUtils.h"
// // #include "Svi.h"
// #include "Svi_Template.h"

// // #include "utils/preprocessCSV2data.h"
// #include "lda_svi/LdaSVIPlanBuilder.h"
// #include "lda_svi/InMemorySVIPlanNode.h"
// #include "lda/MappingReader.h"

// // std::string generateLDAPattern(int die1_a, int die1_b, int die2_a, int die2_b)
// // {

// //    return "std::tuple<Tau<ATOM<0,0,2,3>,ATOM<1,2,1,3>>,VIDice<VIDie<" +
// //           std::to_string(die1_a) + "," + std::to_string(die1_b) + ">,VIDie<" +
// //           std::to_string(die2_a) + "," + std::to_string(die2_b) + ">>, Svi_template<2>>";
// // }

// std::string generateLDAPattern(int K, int vidie1_a, int vidie1_b, int vidie2_a, int vidie2_b)
// {
//    std::ostringstream oss;
//    oss << "std::tuple< SAT<std::tuple< ";
//    for (int i = 0; i < K; ++i)
//    {
//       if (i > 0)
//       {
//          oss << ", ";
//       }
//       oss << "Tau<ATOM3<0,0," << i << ",3>,ATOM2<1," << i << ",1,3>>";
//    }
//    oss << " >>, VIDice<VIDie<" << vidie1_a << "," << vidie1_b << ">,VIDie<"
//        << vidie2_a << "," << vidie2_b << ">>, Svi_template<2> >";
//    return oss.str();
// }

// int runMe(int argc, const char **argv)
// {
//    bool manual = false;
//    std::string vocabFilePath;
//    std::string malletVocabFilePath;
//    float redPrior = 0.2;
//    float bluePrior = 0.1;

//    int nb_sat_assig_per_constraint = -1;
//    // int num_iter_vi=20;
//    int reportEvery = -1;
//    std::string lda_variant = "vi";
//    std::string lda_datasetName = "KOS";
//    std::string lda_datasetNameSuffix = "";
//    int lda_numDocuments = -1;
//    int lda_vocabSize = -1;  // (cat /scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_vocab.csv | wc -l) minus 1
//    int lda_corpusSize = -1; // (cat /scratch/lda_datasets_2023/NYTIMES_train/csv2/NYTIMES_train.csv | wc -l) minus 2
//    float lda_alpha = 0.2;
//    float lda_beta = 0.1;
//    int lda_numTopics = -1;
//    int total_num_iter = 1;
//    int rndSeed = 12345;
//    int num_threads = 1;
//    int batchSize = -1;
//    int vi_iter_per_batch = 3;
//    std::string outputDir = getexepath_parent() + "/benchmarks";
//    std::string lda_vi_dataset_dir = getexepath_parent() + "/data";
//    std::string lda_malletStateDir = outputDir + "/gammapdb_data/VI/chain_states";
//    std::cout << "getexepath_parent() = " << getexepath_parent() << std::endl;
//    // declare arguments
//    const bool isOptionalArg = true;
//    ArgumentParser argsParser;

//    if (manual)
//    {

//       vocabFilePath = "/gammapdb_arrow/data/KOS_train/csv2/KOS_vocab.csv";
//       malletVocabFilePath = "/gammapdb_arrow/data/wiki08_train/csv2/wiki08_vocab_mallet.csv";

//       //  Vocabulary vocab(vocabFilePath);
//       Vocabulary malletVocab(malletVocabFilePath);

//       auto start_count = std::chrono::high_resolution_clock::now();

//       std::string inputCsvUri = "file:///gammapdb_arrow/data/wiki08_train/csv2/wiki08_train_svi.csv";
//       std::string testdatapath = "/gammapdb_arrow/data/wiki08_train/csv2/wiki08_train.csv";
//       std::string mappingPath = "/gammapdb_arrow/data/wiki08_train/csv2/wiki08_train_svi_mapping.csv";

//       MappingReader mapping(mappingPath);
//       std::vector<int> data = parseFileToFlatVector(testdatapath);

//       auto end_count = std::chrono::high_resolution_clock::now();
//       long count_timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end_count - start_count).count();

//       batchSize = 5000000;
//       total_num_iter = 110;
//       nb_sat_assig_per_constraint = 10;
//       //  vi_iter_per_batch=7;
//       float priors[] = {redPrior, bluePrior};

//       //  std::string pattern = "std::tuple< SAT<std::tuple< Tau<ATOM3<0,0,0,3>,ATOM2<1,0,1,3>>, Tau<ATOM3<0,0,1,3>,ATOM2<1,1,1,3>>, Tau<ATOM3<0,0,2,3>,ATOM2<1,2,1,3>>, Tau<ATOM3<0,0,3,3>,ATOM2<1,3,1,3>>, Tau<ATOM3<0,0,4,3>,ATOM2<1,4,1,3>>, Tau<ATOM3<0,0,5,3>,ATOM2<1,5,1,3>>, Tau<ATOM3<0,0,6,3>,ATOM2<1,6,1,3>>, Tau<ATOM3<0,0,7,3>,ATOM2<1,7,1,3>>, Tau<ATOM3<0,0,8,3>,ATOM2<1,8,1,3>>, Tau<ATOM3<0,0,9,3>,ATOM2<1,9,1,3>>, Tau<ATOM3<0,0,10,3>,ATOM2<1,10,1,3>>, Tau<ATOM3<0,0,11,3>,ATOM2<1,11,1,3>>, Tau<ATOM3<0,0,12,3>,ATOM2<1,12,1,3>>, Tau<ATOM3<0,0,13,3>,ATOM2<1,13,1,3>>, Tau<ATOM3<0,0,14,3>,ATOM2<1,14,1,3>>, Tau<ATOM3<0,0,15,3>,ATOM2<1,15,1,3>>, Tau<ATOM3<0,0,16,3>,ATOM2<1,16,1,3>>, Tau<ATOM3<0,0,17,3>,ATOM2<1,17,1,3>>, Tau<ATOM3<0,0,18,3>,ATOM2<1,18,1,3>>, Tau<ATOM3<0,0,19,3>,ATOM2<1,19,1,3>> >>, VIDice<VIDie<3269,20>,VIDie<20,6906>>, Svi_template<2> >";

//       compileAndExecuteInMemoryLDAPlan<std::tuple<SAT<std::tuple<Tau<ATOM3<0, 0, 0, 3>, ATOM2<1, 0, 1, 3>>, Tau<ATOM3<0, 0, 1, 3>, ATOM2<1, 1, 1, 3>>, Tau<ATOM3<0, 0, 2, 3>, ATOM2<1, 2, 1, 3>>, Tau<ATOM3<0, 0, 3, 3>, ATOM2<1, 3, 1, 3>>, Tau<ATOM3<0, 0, 4, 3>, ATOM2<1, 4, 1, 3>>, Tau<ATOM3<0, 0, 5, 3>, ATOM2<1, 5, 1, 3>>, Tau<ATOM3<0, 0, 6, 3>, ATOM2<1, 6, 1, 3>>, Tau<ATOM3<0, 0, 7, 3>, ATOM2<1, 7, 1, 3>>, Tau<ATOM3<0, 0, 8, 3>, ATOM2<1, 8, 1, 3>>, Tau<ATOM3<0, 0, 9, 3>, ATOM2<1, 9, 1, 3>>>>, VIDice<VIDie<3799409, 10>, VIDie<10, 192095>>, Svi_template<2>>>(
//           inputCsvUri,
//           data, // this is not data for training, it's for eval later
//           nb_sat_assig_per_constraint,
//           total_num_iter,
//           priors,
//           malletVocab,
//           lda_malletStateDir,
//           "",
//           batchSize,
//           vi_iter_per_batch,
//           11,
//           num_threads,
//           mapping);

//       //  collapse_data_to_disk("/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_50.csv", "/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_collaps_50.csv");

//       //  collapse_data_to_disk("/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_100.csv", "/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_collaps_100.csv");

//       // preprocess("/gammapdb_arrow/data/KOS_train/csv2/KOS_train.csv","/gammapdb_arrow/data/KOS_train/csv2/KOS_train_svi_50.csv",50 );
//       //  preprocess("/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train.csv", "/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_50.csv",50);
//       //  preprocess("/gammapdb_arrow/data/KOS_train/csv2/KOS_train.csv", "/gammapdb_arrow/data/KOS_train/csv2/KOS_train_svi_50.csv",50);
//       //  preprocess("/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train.csv", "/gammapdb_arrow/data/NYTIMES_train/csv2/NYTIMES_train_svi_100.csv",100);
//    }
//    else
//    {

//       // declare arguments
//       argsParser.addArgument("-d", "--lda_datasetName", 1, isOptionalArg);
//       argsParser.addArgument("-s", "--lda_datasetNameSuffix", 1, isOptionalArg);
//       argsParser.addArgument("--lda_numDocuments", 1, isOptionalArg);
//       argsParser.addArgument("--lda_vocabSize", 1, isOptionalArg);
//       argsParser.addArgument("--lda_corpusSize", 1, isOptionalArg);
//       argsParser.addArgument("-a", "--lda_alpha", 1, isOptionalArg);
//       argsParser.addArgument("-b", "--lda_beta", 1, isOptionalArg);
//       argsParser.addArgument("--lda_numTopics", 1, isOptionalArg);
//       argsParser.addArgument("-n", "--total_num_iter", 1, isOptionalArg);
//       argsParser.addArgument("-r", "--rndSeed", 1, isOptionalArg);
//       argsParser.addArgument("--reportEvery", 1, isOptionalArg);
//       argsParser.addArgument("-o", "--outputDir", 1, isOptionalArg);
//       argsParser.addArgument("--lda_vi_dataset_dir", 1, isOptionalArg);
//       argsParser.addArgument("--lda_malletStateDir", 1, isOptionalArg);
//       argsParser.addArgument("--lda_outfileId", 1, isOptionalArg);
//       argsParser.addArgument("--numThreads", 1, isOptionalArg);
//       argsParser.addArgument("--batchSize", 1, isOptionalArg);
//       argsParser.addArgument("--vi_iter_per_batch", 1, isOptionalArg);

//       // parse arguments
//       argsParser.parse(argc, argv);

//       if (argsParser.count("lda_datasetName"))
//       {
//          lda_datasetName = argsParser.retrieve<std::string>("lda_datasetName");
//       }
//       if (argsParser.count("lda_datasetNameSuffix"))
//       {
//          lda_datasetNameSuffix = argsParser.retrieve<std::string>("lda_datasetNameSuffix");
//       }
//       if (argsParser.count("lda_numDocuments"))
//       {
//          lda_numDocuments = std::stoi(argsParser.retrieve<std::string>("lda_numDocuments"));
//       }
//       if (argsParser.count("lda_vocabSize"))
//       {
//          lda_vocabSize = std::stoi(argsParser.retrieve<std::string>("lda_vocabSize"));
//       }
//       if (argsParser.count("lda_corpusSize"))
//       {
//          lda_corpusSize = std::stoi(argsParser.retrieve<std::string>("lda_corpusSize"));
//       }
//       if (argsParser.count("lda_alpha"))
//       {
//          lda_alpha = std::stod(argsParser.retrieve<std::string>("lda_alpha"));
//       }
//       if (argsParser.count("lda_beta"))
//       {
//          lda_beta = std::stod(argsParser.retrieve<std::string>("lda_beta"));
//       }
//       if (argsParser.count("lda_numTopics"))
//       {
//          lda_numTopics = std::stoi(argsParser.retrieve<std::string>("lda_numTopics"));
//       }
//       if (argsParser.count("total_num_iter"))
//       {
//          total_num_iter = std::stoi(argsParser.retrieve<std::string>("total_num_iter"));
//       }
//       if (argsParser.count("rndSeed"))
//       {
//          rndSeed = std::stoi(argsParser.retrieve<std::string>("rndSeed"));
//       }
//       if (argsParser.count("reportEvery"))
//       {
//          reportEvery = std::stoi(argsParser.retrieve<std::string>("reportEvery"));
//       }
//       if (argsParser.count("outputDir"))
//       {
//          outputDir = argsParser.retrieve<std::string>("outputDir");
//       }
//       if (argsParser.count("lda_vi_dataset_dir"))
//       {
//          lda_vi_dataset_dir = argsParser.retrieve<std::string>("lda_vi_dataset_dir");
//       }
//       if (argsParser.count("lda_malletStateDir"))
//       {
//          lda_malletStateDir = argsParser.retrieve<std::string>("lda_malletStateDir");
//       }
//       if (argsParser.count("numThreads"))
//       {
//          num_threads = std::stoi(argsParser.retrieve<std::string>("numThreads"));
//       }
//       if (argsParser.count("batchSize"))
//       {
//          batchSize = std::stoi(argsParser.retrieve<std::string>("batchSize"));
//       }
//       if (argsParser.count("vi_iter_per_batch"))
//       {
//          vi_iter_per_batch = std::stoi(argsParser.retrieve<std::string>("vi_iter_per_batch"));
//       }

//       std::string lda_outfileId = lda_datasetName + lda_datasetNameSuffix + "_" + std::to_string(lda_numTopics) + "topics_batchS" + std::to_string(batchSize) + "_totalNumIter" + std::to_string(total_num_iter) + "_NT" + std::to_string(num_threads) + "_RND" + std::to_string(rndSeed);
//       if (argsParser.count("lda_outfileId"))
//       {
//          lda_outfileId = argsParser.retrieve<std::string>("lda_outfileId");
//       }

//       // print arguments
//       std::cout << "------------------------------------------------------------------------------------" << std::endl;
//       std::cout << "GammaPDB Arrow Engine " << std::endl;
//       std::cout << "------------------------------------------------------------------------------------" << std::endl;
//       std::cout << "current time         = " << getCurrentTimeStampAsString() << std::endl;

//       std::cout << "lda_datasetName      = " << lda_datasetName << lda_datasetNameSuffix << std::endl;

//       std::cout << "lda_numDocuments     = " << lda_numDocuments << std::endl;
//       std::cout << "lda_vocabSize        = " << lda_vocabSize << std::endl;
//       std::cout << "lda_corpusSize       = " << lda_corpusSize << std::endl;

//       std::cout << "lda_alpha            = " << lda_alpha << std::endl;
//       std::cout << "lda_beta             = " << lda_beta << std::endl;
//       std::cout << "lda_numTopics        = " << lda_numTopics << std::endl;
//       std::cout << "total_num_iter       = " << total_num_iter << std::endl;
//       std::cout << "rndSeed              = " << rndSeed << std::endl;
//       std::cout << "reportEvery          = " << reportEvery << std::endl;
//       std::cout << "outputDir            = " << outputDir << std::endl;
//       std::cout << "lda_vi_dataset_dir   = " << lda_vi_dataset_dir << std::endl;
//       std::cout << "lda_malletStateDir   = " << lda_malletStateDir << std::endl;
//       std::cout << "lda_outfileId        = " << lda_outfileId << std::endl;
//       std::cout << "num_threads          = " << num_threads << std::endl;
//       std::cout << "batchSize            = " << batchSize << std::endl;
//       std::cout << "vi_iter_per_batch    = " << vi_iter_per_batch << std::endl;
//       std::cout << "------------------------------------------------------------------------------------" << std::endl;

//       nb_sat_assig_per_constraint = lda_numTopics;

//       std::string inputCsvUri = "file://" + lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train_svi.csv";
//       std::cout << "inputCsvUri = " << inputCsvUri << std::endl;
//       redPrior = lda_alpha;
//       bluePrior = lda_beta;
//       std::cout << "redPrior = " << redPrior << std::endl;
//       std::cout << "bluePrior = " << bluePrior << std::endl;

//       auto data = parseFileToFlatVector(lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train.csv");
//       std::cout << "Size of data: " << data.size() << std::endl;

//       vocabFilePath = lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_vocab.csv";
//       malletVocabFilePath = lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_vocab_mallet.csv";

//       std::string mappingFilePath = lda_vi_dataset_dir + "/csv2/" + lda_datasetName + "_train_svi_mapping.csv";

//       //  Vocabulary vocab(vocabFilePath);
//       Vocabulary malletVocab(malletVocabFilePath);
//       MappingReader mapping(mappingFilePath);

//       std::string pattern = "";

//       // pattern= generateLDAPattern(lda_numDocuments,lda_numTopics,lda_numTopics,lda_vocabSize);
//       pattern = generateLDAPattern(lda_numTopics, lda_numDocuments, lda_numTopics, lda_numTopics, lda_vocabSize);

//       std::cout << pattern << std::endl;

//       //  runsvi<pattern>(batch_pointers,data,collapsed_data,total_unique_nb_sat_assig,nb_sat_assig_per_constr,total_num_iter,redPrior,bluePrior,malletVocab,lda_malletStateDir,lda_outfileId, batchSize,vi_iter_per_batch,reportEvery,num_threads);
//       setenv("OMP_PROC_BIND", "true", 1);
//       setenv("OMP_PLACES", "cores", 1);

//       float priors[] = {redPrior, bluePrior};
//       compileAndExecuteInMemorySviLDAPlan<pattern>(
//             inputCsvUri,
//             data, //this is not data for training, it's for eval later
//             nb_sat_assig_per_constraint,
//             total_num_iter,
//             priors,
//             malletVocab,
//             lda_malletStateDir,
//             lda_outfileId,
//             batchSize,
//             vi_iter_per_batch,
//             reportEvery,
//             num_threads,
//             mapping);
//    }
//    return 0;
// }

// int main(int argc, const char **argv)
// {
//    signal(SIGSEGV, signal_handler);
//    std::set_terminate(uncaught_exception_handler);
//    return runMe(argc, argv);
// };
