# SGAlib : Small Genetic Algorithm library
A simple and lightweight C++11 library to easily set up genetic algorithms.

## About the library

### Motivations

This library was written to provide a complete - yet easy-to-use - C++ solution to set up genetic algorithms. 

### Features

* Single C++11 header file without external dependencies
* Handles 3 types of selection: roulette wheel selection, stochastic universal sampling and tournament selection
* Handles chromosomes with varying lengths
* Can run the algorithm in a separate thread to allow the user to stop it whenever he wants to (can be useful with a GUI on top for example)

### Get started

You need to create a class inheriting  `SGA::GeneticAlgorithm` and implement the 2 pure virtual functions.

One way to do this is to follow these instructions:

1. Open *genetic_algorithm.cpp* (in *subclass_template*) and copy its contents to your code
2. Replace `myType` by the type of your genes (`int`, `float` or a custom class for instance)
3. Implement the 2 functions `randomType()` and `score()`
4. Optionally implement the `print()` function
5. Copy the library (*sga.hpp*) next to *genetic_algorithm.cpp* (or another include path)
6. Don't forget to enable C++11 support for your compiler (`-std=c++11` for GCC) and link to `-pthread` (if you use the non-blocking option)

### License

This libray is licensed under the Do What The Fuck You Want Public License.

### TODO

* Finish test
* Finish example 2
* Finish example 3
* Add novelty search
* Add algorithm clusters
* Add pictures to readme

## Understanding genetic algorithms

### Natural selection

One of the basic activities of living organisms is to reproduce.  
Reproducing (usually) requires two individuals. The couple will be formed depending on many parameters: whoever is here at that precise time, which one is the strongest, etc. We will call this process the **_selection_**.  
When reproducing, the two individuals share their **chromosomes**. They exchange **genes** which encode traits, creating a new and unique individual. This is the **_recombination_**.  
Also, during the reproduction, **_mutations_** can happen within the chromosomes and create completely new traits. 

Thanks to these two last steps, individuals with new abilities will join their siblings. These new abilities might be terrible and they will die quickly, or they might be very well suited to their environment and allow them to live longer. Hence, they will probably reproduce more and the new ability will spread inside the species. This is the very basics of natural selection.

### The main idea

Genetic algorithms mimic natural selection to find solutions to problems.

Let's say you have a problem you don't know how to solve, but you can however evaluate potential solutions. The most simple thing to do is to randomly try every solution until you find the best one (brute-force technique). Of course, it will be very computationally expensive, and there's certainly a way you can do better. Here come, among others, genetic algorithms.

Let's first define some terms:

* A population is a set of chromosomes
* A chromosome is a set of genes
* A gene is a value of a certain type, as chosen by the programmer (historically a gene is a bit and chromosomes are bit strings)

*NB: Here, what we call a gene should actually be a nucleotide but hell, we're not biologists.*

Here is what genetic algorithms do, on a high level of abstraction:

1. Randomly generate a population of chromosomes
2. Repeat until ending criterion is met (eg. a good enough chromosome was found):
 1. Evaluate all the individuals (compute a fitness score)
 2. Select two individuals
 3. Recombine them (cross their genes)
 4. Operate some mutations with a low probability

We will now detail the three main operations, also known as genetic operators.

### Selection

How do we select which individuals will mate?

**Fitness proportionate selection**  
This technique, also called roulette wheel selection, works by attributing the slots of a roulette wheel to the individuals of the population and then randomly picking one of them (just like if we spinned the wheel). However, the size of the slots are proportional to the fitness scores of the individuals. Hence, a better individual will have more chances to get picked.  
TL;DR: randomly pick an individual with more probability for better individuals.

**Stochastic universal sampling**  
This is the same thing as roulette wheel selection except that instead of having one selection point, we have multiple ones, and they are evenly spaced. Just imagine a roulette wheel with not one selection point but four, separated by 90 degrees each, for example. This gives more chances to weaker individuals to get selected.  
TL;DR: randomly pick multiple individuals at the same time but still with more probability for better individuals.

**Tournament selection**  
In tournament selection, we randomly choose a few individuals and keep the best one of them.  
TL;DR: really?  
Comment: note that if the tournament size is 1, it is actually not the same thing as roulette wheel selection because all the individuals have the same probability to be selected for the tournament.

The selected chromosomes then go to recombination and mutation and this process is repeated until a new population with the same size as the previous one is generated. In some cases, it can be interesting to select a certain percentage of the population only (this is called elitism) but then you will eventually end up with a population of one individual. For this reason, it is not very common.

### Recombination

Once we have our two chromosomes to cross (we can have more but the usual way is to cross two), how do we choose the genes to exchange?

The trivial way to do this is to cut the chromosomes at a single and constant point, then exchange the last two parts for example. You can also cut at multiple predefined points, but your chromosomes need to have a fixed length.

For chromosomes with variable lengths, one quite generic possibility is to randomly create a list of cut points and then exchange some of the segments created.

For more details, please read the [wikipedia article](https://en.wikipedia.org/wiki/Crossover_%28genetic_algorithm%29#Crossover_techniques).

### Mutation

Finally, once the new chromosomes are generated, they have to deal with their fate (wow, deep).  
With a low probability (usually less than one percent), some of their genes will be randomly replaced by new genes, hence making sure we don't get stuck in a local minima of the solution.

### I want more

You might enjoy reading [this](http://www.ai-junkie.com/ga/intro/gat1.html).

## Understanding the library

### Types and structures

The whole library is enclosed in the **`SGA` namespace**.

**Fitness scores** are `double` but can easily be changed into anything you want (you should however keep a floating point type).

The algorithm is implemented in a single class called `GeneticAlgorithm`. This class is a template to which you have to give a certain type. This type will be **the type of your genes**. For instance, if you're working with binary strings, your genes will be booleans, hence, you will write `SGA::GeneticAlgorithm<bool>`. If you're working with graphs, a gene can for example be an edge: (2->4), (5->1), etc. The template will accept anything.

A **chromosome** is defined as a `std::vector` of genes. You can use the alias `SGA::Chromosome<Gene>` (where `Gene` can be `bool`, etc).

A **population** is defined as a `std::vector` of chromosomes, except the main population used inside the library which is a `std::multimap` because it always keeps its elements sorted.

Finally, there's a handy structure you should know about: the **random number generator**. Call `SGA::Random::get([type] min, [type] max);` and get a random number of type `[type]` between `min` and `max`. Works with `double`, `float`, `int` and `unsigned` (uniform distributions only).

### Parameters

There are a few parameters which allow you to easily customize the algorithm:

* The **size of the population** and the **mutation probability**: set them with `setMainParameters(unsigned populationSize, double mutationProbability)`
* The **size of the chromosomes**: set it with `setChromosomesSize(unsigned min, unsigned max)` (just enter the same number for `min` and `max` if you need chromosomes with a fixed length)
* The **selection operator**: set it with `setSelectionType(SelectionType type, unsigned numberOfChromosomesForTournament)`. There are 3 selection types: 
 * `SGA::SelectionType::RouletteWheel`
 * `SGA::SelectionType::StochasticUniversal` 
 * `SGA::SelectionType::Tournament`. Note that if you choose the tournament selection, you will have to provide numberOfChromosomesForTournament which will define the size of the tournament (the number of chromosomes that are selected for each tournament).
* The **ending criterion**: set it with `setEndingCriterion(EndingCriterion type, Score maxScoreForMaxScoreCriterion, unsigned numberOfGenerationsWithoutImprovementForBestScoreCriterion)`. There are 2 available criterions: 
 * `SGA::EndingCriterion::MaxScore`: the algorithm runs until it reaches the score given by `maxScoreForMaxScoreCriterion`
 * `SGA::EndingCriterion::BestScore` the algorithm stops when the score of the best indivual hasn't improved in `numberOfGenerationsWithoutImprovementForBestScoreCriterion` generations
 * `SGA::EndingCriterion::NeverStop`: the algorithm only stops when the user calls `stop()`

The defaults are:

 * a population of 100
 * a mutation rate of 0.01
 * a chromosome size between 1 and 100
 * tournament selection with 10 individuals
 * best score ending criterion with 10 maximum generations

### How to use the algorithm

**1) Subclass the algorithm**

Start by sublassing `SGA::GeneticAlgorithm`. You need to do this because the way a chromosome is randomly generated and evaluated is always very specific, depending on the problem. Hence, you need to implement 2 functions before being able to instantiate the class:

* `T randomGene() const` which generates a random gene
* `Score score(Chromosome<T> const & chromosome) const` which computes the fitness score of a chromosome

You may also want to rewrite `std::string print(Chromosome<T> const & chromosome) const` which converts a chromosome to a string if you wish to use logging features.

**2) Instantiate the algorithm and set the parameters**

Pretty clear. Check out the examples if you want more details.

**3) Start the algorithm!**

There are 3 function you need to know to use the library:

* `run()` which starts the algorithm
* `stop()` which stops the algorithm
* `best()` which returns the best chromosome so far

The run function actually takes some parameters:

* `bool blocking`: if true, the algorithm will run until it reaches the ending criterion, you will have no way to stop it unless killing its host process; if false, the algorithm will run in another thread, allowing you to stop it when you want (using the `stop` function)
* `bool enableLogging`: if true, some information like the score of the latest generation will be logged to the `outputStream`
* `std::ostream & outputStream`: the stream to which informations should be logged (can be std::cout or a file stream for example)

###  About the multi-threading option

The `blocking` option is made possible thanks to `std::thread`. On linux or os x, recent compilers probably support it very well. However, on Windows, if you're using mingw, you might have some trouble compiling. Find a version of mingw with std::thread support or, if you don't need it anyway, uncomment the line `#define DISABLE_NONBLOCKING_MODE` in *sga.hpp*.

## Questions and Answers

### How are you?
Pretty good.

### My program works!
Good for you. (That's not a question by the way.)

### Blabla
Blablabla.
