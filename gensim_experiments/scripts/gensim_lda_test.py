import argparse
import csv
import math
import os
import gzip
from gensim import corpora, models
from tqdm import tqdm
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
import time
import pandas as pd
from collections import deque
import io
from multiprocessing import Process, Queue, cpu_count


def save_chain_state(model, corpus, dictionary, dataset_name, num_topics, alpha_prior, beta_prior, num_iterations, num_threads, rnd_seed, iteration, chain_state_filepath, chunk_size=1000):
    """Save the chain state of the model after a given iteration."""
    
    print(f"Saving chain state for iteration {iteration}")
    
    reverse_dict = defaultdict(lambda: None, {v: k for k, v in dictionary.items()})
    file_name = f"chain-state_{dataset_name}_{num_topics}topics_A{alpha_prior}_B{beta_prior}_NI{num_iterations}_NT{num_threads}_RND{rnd_seed}.gz.{iteration}"
    output_dir = os.path.abspath(chain_state_filepath)  # Use absolute path
    full_path = os.path.join(output_dir, file_name)
    os.makedirs(output_dir, exist_ok=True)  # Ensure the directory exists
    print(f"Saving chain state to: {full_path}")
    
    with gzip.open(full_path, 'wt', encoding='utf-8') as f:
        # Write Header
        f.write("#doc source pos mallet_word_id word topic\n")
        f.write(f"#alpha : {' '.join([str(alpha_prior)] * num_topics)}\n")
        f.write(f"#beta : {beta_prior}\n")
        
        buffer = io.StringIO()  # Use StringIO as a string buffer
        for doc_id, doc_bow in enumerate(corpus, start=1):
            if doc_id % 500 == 0:  # Report every 500 documents processed
                print(f"{doc_id} documents processed...")
                
            topic_dist = model.get_document_topics(doc_bow)
            topic_weights = np.array([prob for _, prob in topic_dist])
            topic_weights /= np.sum(topic_weights)
            
            # Constructing Cumulative Distribution Function (CDF)
            cdf = np.cumsum(topic_weights)
            
            pos = 0
            for word_id, freq in doc_bow:
                word = dictionary[word_id]
                word_id_str, word_str = word.split(' ')
                mallet_word_id = reverse_dict[word]
                
                for _ in range(freq):
                    u = np.random.uniform()  # Generate a random number between 0 and 1
                    sampled_topic_index = np.searchsorted(cdf, u, side='right') - 1  # Find the index where u would be inserted into the cdf
                    sampled_topic_index = max(0, min(len(topic_dist) - 1, sampled_topic_index))  # Clip to valid index range
                    
                    # Access topic from topic_dist using the index found
                    sampled_topic, _ = topic_dist[sampled_topic_index]
                    
                    buffer.write(f"{doc_id} NA {pos} {word_id_str} {word_str} {sampled_topic}\n")
                    pos += 1
                    
                    if buffer.tell() >= chunk_size:
                        f.write(buffer.getvalue())
                        buffer.seek(0)
                        buffer.truncate()
                        
        if buffer.tell() > 0:
            f.write(buffer.getvalue())
    
    print(f"Chain state saved successfully for iteration {iteration}")


def load_data(dataset_name, split='train'):
    """Load the dataset and prepare the corpus."""
    
    corpus = []
    mallet_dictionary = {}
    doc_freq = defaultdict(int)
    current_doc_id = None
    
    # Load document data
    with open(f'../data/{dataset_name}_{split}/csv2/{dataset_name}_{split}.csv', 'r',encoding='utf-8') as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            docId, pos, wordId = row
            if current_doc_id != docId:
                if current_doc_id is not None:
                    corpus.append(list(doc_freq.items()))
                doc_freq = defaultdict(int)
                current_doc_id = docId
            doc_freq[int(wordId)] += 1
            
        if doc_freq:
            corpus.append(list(doc_freq.items()))

    # Load vocabulary
    with open(f'../data/{dataset_name}_train/csv2/{dataset_name}_vocab_mallet.csv', 'r',encoding='utf-8') as f:
        reader = csv.reader(f)
        next(reader)
        for idx, row in enumerate(reader):
            word = row[0]
            mallet_dictionary[idx] = word
    
    # Create Gensim dictionary
    gensim_dictionary = corpora.Dictionary.from_corpus(corpus, id2word=mallet_dictionary)
    
    return corpus, gensim_dictionary


def train_lda(corpus, dictionary, num_topics, num_iterations, alpha, beta, num_threads, rnd_seed):
    """Train the LDA model using Gensim."""
    
    lda_model = models.LdaMulticore(
        corpus=corpus,
        id2word=dictionary,
        num_topics=num_topics,
        alpha=alpha,
        eta=beta,
        iterations=num_iterations,
        passes=1,
        eval_every=None,
        workers=num_threads,  # Number of extra processes to use for parallelization
        random_state=rnd_seed  # Set random seed
    )
    return lda_model


def plot_perplexity(perplexities, iterations_list):
    """Plot perplexity over iterations."""
    
    plt.plot(iterations_list, perplexities)
    plt.xlabel('Iterations')
    plt.ylabel('Perplexity')
    plt.title('Perplexity vs Iterations')
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--dataset_name', default='KOS', type=str)
    parser.add_argument('--num_topics', default=5, type=int)
    parser.add_argument('--alpha_prior', default=0.2, type=float)
    parser.add_argument('--beta_prior', default=0.1, type=float)
    parser.add_argument('--num_iterations', default=30, type=int)
    parser.add_argument('--num_threads', default=1, type=int)
    parser.add_argument('--rnd_seed', default=42, type=int)
    parser.add_argument('--SAVE_EVERY', default=5, type=int)
    parser.add_argument('--profiling', default=True, type=lambda x: (str(x).lower() == 'true'), help='Enable or disable profiling mode')
    parser.add_argument('--chain_state_filepath', default='../benchmarks/gensim/chain_states/', type=str, help='Filepath to save chain states')
    parser.add_argument('--time_filepath', default='../benchmarks/gensim/csv/execution_time.csv', type=str, help='Filepath to save execution time')
    args = parser.parse_args()

    print(f"Profiling mode: {args.profiling}")
    print(f"Chain state filepath: {args.chain_state_filepath}")
    print(f"Time filepath: {args.time_filepath}")

    if args.profiling:
        os.makedirs(os.path.dirname(args.time_filepath), exist_ok=True)
        with open(args.time_filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            #writer.writerow(['RunId', 'IterationId', 'AvgTimeLast5Iterations(ms)'])

    runId = 1

    # Load data
    corpus_train, dictionary_train = load_data(args.dataset_name, split='train')
    corpus_test, _ = load_data(args.dataset_name, split='test')

    perplexities = []
    iterations_list = []
    last_N_times = deque(maxlen=args.SAVE_EVERY)

    # Training loop
    for i in tqdm(range(1, args.num_iterations + 1)):
        start_time = time.time()
        lda_train = train_lda(corpus_train, dictionary_train, args.num_topics, i, args.alpha_prior, args.beta_prior, args.num_threads, args.rnd_seed)
        end_time = time.time()
        
        elapsed_time = (end_time - start_time) * 1000  # Execution time in milliseconds
        last_N_times.append(elapsed_time)
        avg_time_last_N = sum(last_N_times) / len(last_N_times)
        
        print(f"Iterations: {i}, Time: {avg_time_last_N:.2f} ms")
        
        if args.profiling and (i % args.SAVE_EVERY == 0 or i == args.num_iterations):
            with open(args.time_filepath, 'a', newline='') as f:
                writer = csv.writer(f)
                writer.writerow([runId, i, avg_time_last_N])

        # Save chain state every SAVE_EVERY iterations
        if i % args.SAVE_EVERY == 0 or i == args.num_iterations:
            save_chain_state(lda_train, corpus_train, dictionary_train, args.dataset_name, args.num_topics, args.alpha_prior, args.beta_prior, args.num_iterations, args.num_threads, args.rnd_seed, i, args.chain_state_filepath)

        # Compute perplexity
        log_perplexity = lda_train.log_perplexity(corpus_test)
        real_perplexity = math.exp(-log_perplexity)
        #print(f"Iterations: {i}, Real Perplexity: {real_perplexity}")

        perplexities.append(real_perplexity)
        iterations_list.append(i)

    print("Training completed. Chain states have been saved.")