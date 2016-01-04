// Copyright © 2015 Pierre Schefler <schefler.pierre@gmail.com>
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE.md file for more details.

/*---------*/
/* READ ME */

/* In this example, we set up a genetic algorithm to find a number.
 * A chromosome will be defined as a series of numbers between 0 and 9, effectively representing a number.
 * The score of a chromosome will be inversely proportional to its distance with the desired number.
 */
 
/* During the execution, if you enter a number high enough, the algorithm might be long to find it.
 * This is because, in the end, we need the best individual to change a tiny bit only, which is rather hard because it can only happen:
 *  - with a mutation (low probability)
 *  - with a successful crossover with another individual which has the missing part (even lower probability)
 * 
 * This is what we could do to improve this:
 *  - choose a better scoring function (but which one?)
 *  - give the score depending on the novelty of the invidual (this is a very different concept called Novelty Search which might be interesting to try with this example).
 */

#include <iostream>
#include <sstream>
#include <string>

#include "../../src/sga.hpp"

/* The number to find */

unsigned long long objective;
SGA::Chromosome<unsigned> objectiveChromosome;

/* Useful functions */

unsigned long long chromosomeToNumber(SGA::Chromosome<unsigned> const & chromosome)
{
	//Chromosome : {1, 6, 3} --> number: 163 = 3*10^0 + 6*10^1 + 1*10^2
	
	unsigned long long res = 0;
	
	const unsigned long long l = chromosome.size()-1;
	for (unsigned long long i=0 ; i<chromosome.size() ; i++)
		res += chromosome[l-i]*(unsigned long long)pow(10, i);
	
	return res;
}

SGA::Chromosome<unsigned> numberToChromosome(unsigned long long number)
{
	//Number: 163 --> string: "163" --> chars: '1', '6', '3' --> numbers: '1'-'0'=1, '6'-'0'=6 ... -> chromosome = {1, 6, 3}
	
	SGA::Chromosome<unsigned> res;
	
	std::stringstream ss;
	ss << number;
	std::string str = ss.str();
	
	for (unsigned i=0 ; i<str.size() ; i++)
		res.push_back((unsigned)str[i] - '0');
		
	return res;
}

/* The algorithm class */

INIT_RANDOM();

class GA : public SGA::GeneticAlgorithm<unsigned>
{
	public :
	
		GA() : SGA::GeneticAlgorithm<unsigned>() {}
	
		virtual unsigned randomGene() const override
		{
			//Return a random integer between 0 and 9
			return SGA::Random::get(0, 9);
		}
		
		virtual SGA::Score score(SGA::Chromosome<unsigned> const & chromosome) const override
		{
			//The score reflects how close each number is to the ones of the objective
			SGA::Score score = 0.0;
			for (unsigned i=0 ; i<chromosome.size() ; i++)
			{
				if (i >= objectiveChromosome.size()) //If the chromosome is longer than the objective, the score reduces
					score += -1.0;
				else
					score += chromosome[i] == objectiveChromosome[i] ? 1.0 : 0.0;
			}
			
			//If the chromosome is shorter, the score also reduces
			if (chromosome.size() < objectiveChromosome.size())
				score += -1.0 * (objectiveChromosome.size() - chromosome.size());
			
			return score;
		}
		
		virtual std::string print(SGA::Chromosome<unsigned> const & chromosome) const override
		{
			std::stringstream ss;
			ss << chromosomeToNumber(chromosome);
			return ss.str();
		}
};

/* The program */

int main()
{
	//Get the objective
	std::cout << "Enter a number to find: " << std::flush;
	std::cin >> objective;
	std::cout << std::endl;
	objectiveChromosome = numberToChromosome(objective);
	
	//Run the algorithm
	std::cout << "Starting the algorithm..." << std::endl;
	std::cout << std::endl;
	
	GA algorithm;
	algorithm.setMainParameters(100, 0.01);
	algorithm.setChromosomesSize(1, numberToChromosome(std::numeric_limits<unsigned long long>::max()).size());
	algorithm.setEndingCriterion(SGA::EndingCriterion::MaxScore, (double)objectiveChromosome.size()); //The best score is 1 * number of digits in the objective.
	algorithm.setSelectionType(SGA::SelectionType::Tournament, 10); //We use tournament because the negative scores behave badly with fitness proportionate selection.
	algorithm.run(true, true);
	std::cout << std::endl;
	
	//Verify the result
	unsigned long long result = chromosomeToNumber(algorithm.best());
	if (result == objective)
	{
		std::cout << "Great! The algorithm found our objective: " << objective << std::endl;
	}
	else
	{
		std::cout << "Well that's weird, the algorithm found " << result << ". This shouldn't have happened. If you're using SGA::EndingCriterion::BestScore, make sure you have enough generations without improvement before the algorithm stops." << std::endl;
	}
	
	std::cout << std::endl;
	
	return EXIT_SUCCESS;
}
