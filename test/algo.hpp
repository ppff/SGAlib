#pragma once

#include "../src/sga.hpp"

typedef bool Gene;
unsigned chromosomeSize = 16;

class GAtest : public SGA::GeneticAlgorithm<Gene>
{
	public :
	
		/* Basic stuff */
	
		GAtest() : SGA::GeneticAlgorithm<Gene>() {}
	
		virtual Gene randomGene() const override
		{
			return (bool)SGA::Random::get(0, 1);
		}
		
		virtual SGA::Chromosome<Gene> randomChromosome() const override
		{
			SGA::Chromosome<unsigned> result;
			for (unsigned i=0 ; i<chromosomeSize ; i++)
				result.push_back(randomGene());
			return result;
		}
		
		virtual SGA::Score score(SGA::Chromosome<Gene> const & chromosome) const override
		{
			//Dumb score
			SGA::Score score = 0.0;
			for (unsigned i=0 ; i<chromosome.size() ; i++)
				score += chromosome[i] ? : 1.0 : 0.0;
			return score;
		}
		
		virtual std::string print(SGA::Chromosome<Gene> const & chromosome) const override
		{
			std::stringstream ss;
			for (bool b : chromosome)
				ss << b ? "1" : "0";
			return ss.str();
		}
		
		/* Test functions (inside the class to have access to protected members) */
		
		bool selectionTest()
		{
			
};
