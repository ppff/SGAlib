// Copyright © 2015 Pierre Schefler <schefler.pierre@gmail.com>
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE.md file for more details.

#pragma once

#include <iostream>
#include <random>
#include <exception>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <deque>
#include <string>

//#define DISABLE_NONBLOCKING_MODE //Use this to remove the dependecy to std::thread

#ifndef DISABLE_NONBLOCKING_MODE
	#include <thread>
#endif

namespace SGA
{

/******************/
/** Introduction **/
/******************/

//Alias for a Chromosome which is actually a vector
template <typename T>
using Chromosome = std::vector<T>;

//Alias for a Population of Chromosomes which is a vector of Chromosomes
template <typename T>
using Population = std::vector< Chromosome<T> >;

//Typedef for Score which is a double
typedef double Score;

//Useful macro to log infos
#define LOG(...) \
do \
{ \
	if (_logEnable) \
		_logStream.get() << "[SGA] " << __VA_ARGS__ << std::endl; \
} while(0)

//Enums

/* How to end the algorithm:
 *  - MaxScore: when the best chromosome from the population has a fitness score above _bestScore, the algorithm ends;
 *  - BestScore (default): when the best score doesn't get better during _steadyGenerations generations, the algorithm ends.
 */
enum class EndingCriterion { MaxScore, BestScore };

/* How to select chromosomes for the recombination operation:
 *  - RouletteWheel: randomly pick up a chromosome with a probability proportional to its fitness score (just like a roulette wheel but with bigger slots for better individuals);
 *  - StochasticUniversal: same idea as RouletteWheel but multiple individuals are selected at the same time;
 *  - Tournament (default): randomly pick _tournamentSize chromosomes and keep the best one.
 * NB: find more about the selection types inside the function implementation.
 */
enum class SelectionType { RouletteWheel, StochasticUniversal, Tournament };

//Handy random number generator
struct Random
{
	static std::random_device   _seed;
	static std::mt19937         _engine;
	
	static double get(double min, double max)
	{
		std::uniform_real_distribution<double> distribution(min, max);
		return distribution(_engine);
	}

	static float get(float min, float max)
	{
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(_engine);
	}

	static int get(int min, int max)
	{
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(_engine);
	}

	static unsigned get(unsigned min, unsigned max)
	{
		std::uniform_int_distribution<unsigned> distribution(min, max);
		return distribution(_engine);
	}
};

//Handy macro to init the random number generator
#define INIT_RANDOM() \
std::random_device   SGA::Random::_seed; \
std::mt19937         SGA::Random::_engine(SGA::Random::_seed());

/****************************/
/** Algorithm declarations **/
/****************************/

template <typename T>
class GeneticAlgorithm
{
	//Everything the user needs should be here, in the public section
	public :
		
		/*-----------------------*/
		/* Constructor and stuff */
		/*-----------------------*/
		
		//Init default values, etc
		GeneticAlgorithm();
		
		/*-----------------------------*/
		/* Main functions for the user */
		/*-----------------------------*/
		
		//Run the algorithm until it ends or until the user stops it
		void run(bool blocking, bool enableLogging = false, std::ostream & outputStream = std::cout);
		
		//Stop the algorithm
		void stop();
		
		//Get the best chromosome
		Chromosome<T> best();
		
		//Set the ending criterion (with optional parameter if the user chooses MaxScore)
		void setEndingCriterion(EndingCriterion type, Score maxScoreForMaxScoreCriterion = 0.0, unsigned numberOfGenerationsWithoutImprovementForBestScoreCriterion = 10);
		
		//Set the selection type (with optional parameter if the user chooses Tournament)
		void setSelectionType(SelectionType type, unsigned numberOfChromosomesForTournament = 0);
		
		//Set the population size and mutation probability
		void setMainParameters(unsigned populationSize, double mutationProbability);
		
		//Set the chromosomes size
		void setChromosomesSize(unsigned min, unsigned max); //Just enter the same number on min and max for a constant length
		
		/*----------------------------------------------------*/
		/* Functions which have to be implemented by the user */
		/*----------------------------------------------------*/
		
		//Generate a random value of type T
		virtual T randomGene() const = 0;
		
		//Compute the score of a chromosome
		virtual Score score(Chromosome<T> const & chromosome) const = 0;
		
		/*----------------------------------------*/
		/* Functions the user may want to rewrite */
		/*----------------------------------------*/
		
		//Chromosome to string (returns empty string by default)
		virtual std::string print(Chromosome<T> const & chromosome) const {return std::string();}
	
	
	//The protected section contains the magic
	protected :
	
		/*------------*/
		/* Attributes */
		/*------------*/
		
		/* Parameters */

		unsigned 		_populationSize;		//Size of the population (default is 100)
		EndingCriterion _endCriterion;			//Criterion to end algorithm (default is BestScore)
		SelectionType 	_selectionType;			//Selection type (default is Tournament)
		double 			_mutationProbability;	//Mutation probabiliy (default is 0.01)
		Score 			_maxEndScore;			//Maximum score to reach (only used with MaxScore)
		unsigned		_steadyGenerations;		//The number of generations without improvement before the algorithm stops (only used with BestScore)
		unsigned 		_tournamentSize;		//Size for tournament selection (default is 10)
		unsigned		_minChromosomeSize;		//Minimum size for a chromosome (default is 1)
		unsigned		_maxChromosomeSize;		//Maximum size for a chromosome (default is 100)

		/* Things the algorithm needs for reasons */

		std::multimap<Score, Chromosome<T> > 	_population;	//The actual population (not a vector as the others because maps are automatically sorted)
		std::deque<Score> 						_lastScores;	//Buffer used for BestScore ending criterion
		bool 									_run;			//Boolean used to stop the algorithm if needed
		unsigned								_generation;	//To keep track of the number of generations
		
		/* Logging variables */
		
		bool 									_logEnable;		//Enable or disable logging to _logStream.
		std::reference_wrapper<std::ostream> 	_logStream;		//The output stream to write messages to.
		
		/*----------------*/
		/* Core functions */
		/*----------------*/
		
		//Make the population evolve until an ending criterion is reached or the user stops the algorithm
		void evolve(Population<T> population);
		
		//Check if an ending criterion is reached
		bool isEvolutionOver(); //Non-const for a minor reason
		
		//Select a population of chromosomes to be crossed (selection)
		Population<T> select() const;
		
		//Cross chromosomes between them (recombination): is implemented for a population of 2 chromosomes
		Population<T> cross(Population<T> const & chromosomes) const;
		
		//Make a chromosome change with a user-defined probability (mutation)
		void mutate(Chromosome<T> & chromosome) const;
				
		/*--------------------------------*/
		/* Useful stuff for the algorithm */
		/*--------------------------------*/
		
		//Generate a random chromosome
		Chromosome<T> randomChromosome() const;
		
		//Get the score of the chromosome at position index in the _population
		Score score(unsigned index) const;
		
		//Get the accumulation of all scores of the _population
		Score totalScore() const;
		
		//Get the chromosome whose accumulated fitness score is inferior to the required fitness
		Chromosome<T> chromosome(Score fitness) const;
		
		//Get the chromosome at position index in the _population
		Chromosome<T> chromosome(unsigned index) const;
		
		//Get the last element of the population
		std::pair<Score, Chromosome<T> > lastElement() const;
	
};

/******************************/
/** Algorithm implementation **/
/******************************/

/*-----------------------*/
/* Constructor and stuff */
/*-----------------------*/

template <typename T>
GeneticAlgorithm<T>::GeneticAlgorithm()
 : _logStream(std::cout)
{
	//Default parameters
	_populationSize = 100;
	_mutationProbability = 0.01;
	_endCriterion = EndingCriterion::BestScore;
	_selectionType = SelectionType::Tournament;
	_tournamentSize = 10;
	_run = false;
	_minChromosomeSize = 1;
	_maxChromosomeSize = 100;
	
	//Other
	_logEnable = false;
}

/*-----------------------------*/
/* Main functions for the user */
/*-----------------------------*/

template <typename T>
void GeneticAlgorithm<T>::run(bool blocking, bool enableLogging, std::ostream & outputStream)
{
	//Logging
	_logEnable = enableLogging;
	_logStream = std::ref(outputStream);
	
	//Safety check
	if (_selectionType == SelectionType::Tournament && _tournamentSize > _populationSize)
	{
		throw std::runtime_error("The tournament size cannot be greater than the population size");
	}
	
	//Reset stuff
	_lastScores.clear();
	_run = true;
	_generation = 0;
	
	//Create population
	Population<T> population;
	population.reserve(_populationSize);
	
	for (unsigned i=0 ; i<_populationSize ; i++)
	{
		population.push_back(randomChromosome());
	}
	
	if (blocking)
	{
		//Just run evolve
		evolve(population);
	}
	else
	{
		#ifdef DISABLE_NONBLOCKING_MODE
		
		throw std::runtime_error("Cannot run in nonblocking mode because it is disabled");
		
		#else
		
		//Make the population evolve in a new thread
		std::thread evolution(&GeneticAlgorithm<T>::evolve, this, population);
		
		//Detach the thread from its parent thread
		evolution.detach();
		
		#endif
	}
}

template <typename T>
void GeneticAlgorithm<T>::stop()
{
	LOG("User stopped the algorithm");
	_run = false;
	_logEnable = false;
}

template <typename T>
Chromosome<T> GeneticAlgorithm<T>::best()
{
	return lastElement().second;
}

template <typename T>
void GeneticAlgorithm<T>::setEndingCriterion(EndingCriterion type, Score maxScoreForMaxScoreCriterion, unsigned numberOfGenerationsWithoutImprovementForBestScoreCriterion)
{
	if (type == EndingCriterion::MaxScore)
	{
		_maxEndScore = maxScoreForMaxScoreCriterion;
	}
	else if (type == EndingCriterion::BestScore)
	{
		_steadyGenerations = numberOfGenerationsWithoutImprovementForBestScoreCriterion;
	}
	
	_endCriterion = type;
}	

template <typename T>
void GeneticAlgorithm<T>::setSelectionType(SelectionType type, unsigned numberOfChromosomesForTournament)
{
	if (type == SelectionType::Tournament)
	{
		_tournamentSize = numberOfChromosomesForTournament;
	}
	
	_selectionType = type;
}

template <typename T>
void GeneticAlgorithm<T>::setMainParameters(unsigned populationSize, double mutationProbability)
{
	_populationSize = populationSize;
	_mutationProbability = mutationProbability;
}

template <typename T>
void GeneticAlgorithm<T>::setChromosomesSize(unsigned min, unsigned max)
{
	_minChromosomeSize = min;
	_maxChromosomeSize = max;
}

/*----------------*/
/* Core functions */
/*----------------*/

template <typename T>
void GeneticAlgorithm<T>::evolve(Population<T> population)
{
	//While ending criterion not reached and user stop command not sent, do classic genetic algorithms stuff
	while (_run)
	{
		/* 1. Verify we're not good enough */
		
		//Compute fitness
		_population.clear();
		for (Chromosome<T> const & chromosome : population)
		{
			_population.insert( std::pair<Score, Chromosome<T> >(score(chromosome), chromosome) );
		}
		
		//Log results
		LOG("Generation " << _generation << ": best fitness score is " << lastElement().first << " (" << print(lastElement().second) << ")");
		
		//Check ending criterion
		if (isEvolutionOver())
		{
			LOG("The ending criterion was matched.");
			break;
		}
		
		/* 2. Make it evolve */
		
		//Prepare new population
		population.clear();
		population.reserve(_populationSize);
		
		//Fill the new population until it has reached the wanted size
		while (population.size() < _populationSize)
		{
			//A] Selection
			Population<T> selection;
			while (selection.size() < 2)
			{
				Population<T> tmp = select();
				selection.insert(selection.end(), tmp.begin(), tmp.end());
			}
			
			//B] Recombination
			for (unsigned i=0 ; i<selection.size()/2 ; i++) //If the selection size is not even, ignore the last individual
			{
				Population<T> newChromosomes = cross({selection[2*i], selection[2*i+1]});
				population.push_back(newChromosomes.front());
				population.push_back(newChromosomes.back());
			}
		}
		
		//C] Mutation
		for (Chromosome<T> & chromosome : population)
		{
			mutate(chromosome);
		}
		
		//We've evolved!
		_generation++;
	}
	
	LOG("The algorithm is over. The best individual has a fitness score of " << lastElement().first << " (" << print(lastElement().second) << ").");
	_run = false;
	_logEnable = false;
}

template <typename T>
bool GeneticAlgorithm<T>::isEvolutionOver()
{
	if (_endCriterion == EndingCriterion::MaxScore)
	{
		//Just check if the best chromosome has a score high enough
		return lastElement().first >= _maxEndScore;
	}
	else if (_endCriterion == EndingCriterion::BestScore)
	{	
		//Append the score of the best chromosome to the buffer
		_lastScores.push_back(lastElement().first);
		
		//If we don't have enough generations, exit, otherwise, remove the oldest best score
		if (_lastScores.size() <= 10)
		{
			return false;
		}
		else
		{
			_lastScores.pop_front();
		}
		
		//Run through the buffer to see if the best score has gotten any better during the last generations
		bool better = false;
		Score first = _lastScores.front();
		for (Score s : _lastScores)
		{
			if (s > first)
			{
				better = true;
				break;
			}
		}
		
		//If yes, continue, otherwise, we have reached the end of the evolution
		return !better;
	}
	else
	{
		throw std::runtime_error("Unknown ending criterion");
	}
}

template <typename T>
Population<T> GeneticAlgorithm<T>::select() const
{
	if (_selectionType == SelectionType::RouletteWheel)
	{
		/* The roulette wheel selection (also known asp FPS: fitness proportionate selection) works by randomly choosing a chromosome inside the population. However, the probability is measured as a fitness score. Hence, the highest score a chromosome has, the best chance it will have to be selected. */
		
		//Generate a random number between 0 and total fitness of the population (make the wheel spin)
		const double probability = Random::get(0.0, totalScore());
		
		//Return the corresponding chromosome (say where it stopped)
		return { chromosome(probability) };
	}
	else if (_selectionType == SelectionType::StochasticUniversal)
	{
		/* In stochastic universal sampling (SUS), we choose a random number of chromosomes to be selected. The thing is, these chromosomes will have their fitness scores evenly spaced inside the fitness distribution. */
		
		//Choose a number of chromosomes to select
		unsigned toSelect = Random::get(1, _population.size()/10);
		Score distanceBetweenScores = totalScore() / (Score)toSelect;
		
		//Generate the first score whose chromosome will be selected
		Score firstScore = Random::get(0.0, distanceBetweenScores);
		
		//Select chromosomes at equidistant fitness scores starting at firstScore
		Population<T> selected;
		for (Score score = firstScore ; score <= totalScore() ; score += distanceBetweenScores)
		{
			selected.push_back(chromosome(score));
		}
		return selected;
	}
	else if (_selectionType == SelectionType::Tournament)
	{
		/* In tournament selection, we randomly pick a fixed number of chromosomes and keep the best among them. The size of the tournament is defined by the user.  */
		
		//Populate the tournament (we use indices only)
		std::vector<unsigned> tournament;
		for (unsigned i=0 ; i<_tournamentSize ; i++)
		{
			tournament.push_back(Random::get(0, _population.size()-1));
		}
		
		//Find the best chromosome
		unsigned bestIndex = tournament[0];
		Score bestScore = score(bestIndex);
		
		for (unsigned i=0 ; i<_tournamentSize ; i++)
		{
			if (score(tournament[i]) > bestScore)
			{
				bestIndex = tournament[i];
				bestScore = score(bestIndex);
			}
		}
		
		//Return the best chromosome
		return { chromosome(bestIndex) };
	}
	else
	{
		throw std::runtime_error("Unknown selection type");
	}
}

template <typename T>
Population<T> GeneticAlgorithm<T>::cross(Population<T> const & chromosomes) const
{
	/* NB: this function was written for recombination between 2 chromosomes only but can easily be rewritten for more (without changing its prototype). */
	
	//Get the size of the smallest chromosome
	unsigned sizeOfSmallest = std::min(chromosomes[0].size(), chromosomes[1].size());

	//Select genes to exchange
	std::vector<unsigned> genesToExchange; 
	unsigned index = 0;
	bool add = true;
	while(index < sizeOfSmallest)
	{
		//Generate a final gene index
		unsigned next = Random::get(index, sizeOfSmallest);
		
		//Add all the indices in-between, half the time
		//We will get something like: {0 -> 2}, {5 -> 9}, etc...
		if (add)
		{
			for (unsigned i=index ; i<next ; i++)
				genesToExchange.push_back(i);
			add = false;
		}
		else
			add = true;
			
		index = next;
	}
	
	//Exchange the genes
	Population<T> result = chromosomes;
	for (unsigned i=0 ; i<genesToExchange.size() ; i++)
	{
		unsigned index = genesToExchange[i];
		Chromosome<T> tmp = result[0];
		result[0][index] = result[1][index];
		result[1][index] = tmp[index];
	}
	
	return result;
}

template <typename T>
void GeneticAlgorithm<T>::mutate(Chromosome<T> & chromosome) const
{
	//Activate mutation only if we have a number low enough
	if (Random::get(0.0, 1.0) <= _mutationProbability)
	{
		//Choose genes from begin to end-1
		const unsigned begin = Random::get(0, chromosome.size()-1);
		const unsigned end = Random::get(begin, chromosome.size());
		
		//Replace them with random values
		for (unsigned i=begin ; i<end ; i++)
			chromosome[i] = randomGene();
	}
}	

/*--------------------------------*/
/* Useful stuff for the algorithm */
/*--------------------------------*/

template <typename T>
Chromosome<T> GeneticAlgorithm<T>::randomChromosome() const
{
	const unsigned size = Random::get(_minChromosomeSize, _maxChromosomeSize);
	
	Chromosome<T> result(size);
	
	std::generate(result.begin(), result.end(), [&](){ return this->randomGene(); }); //god bless lambda functions
	
	return result;
}

template <typename T>
Score GeneticAlgorithm<T>::score(unsigned index) const
{
	//Safety check
	if (index >= _population.size())
		return 0.0;
		
	//Move an iterator to the corresponding index and return the score
	auto it = _population.begin();
	std::advance(it, index);
	
	return it->first;
}

template <typename T>
Score GeneticAlgorithm<T>::totalScore() const
{
	double scoreSum = 0.0;
	
	for (auto it=_population.begin() ; it!=_population.end() ; it++)
	{
		scoreSum += it->first;
	}
	
	return scoreSum;
}

template <typename T>
Chromosome<T> GeneticAlgorithm<T>::chromosome(Score fitness) const
{
	//Thanks to the (multi)map, the elements will always be sorted by ascending score.
	
	double cumulativeFitness = 0.0;
	
	for (auto it=_population.begin() ; it!=_population.end() ; it++)
	{
		cumulativeFitness += it->first;
		if (fitness <= cumulativeFitness)
			return {it->second};
	}
	
	//In case we didn't find anything, just return the best element.
	return lastElement().second;
}

template <typename T>
Chromosome<T> GeneticAlgorithm<T>::chromosome(unsigned index) const
{
	//Safety check
	if (index >= _population.size())
		return lastElement().second;
	
	//Move an iterator to the corresponding index and return the chromosome
	auto it = _population.begin();
	std::advance(it, index);
	
	return it->second;
}

template <typename T>
std::pair<Score, Chromosome<T> > GeneticAlgorithm<T>::lastElement() const
{
	return * std::prev(_population.end());
}

} //namespace
