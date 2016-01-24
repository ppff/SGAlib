#include "sga.hpp"

//Initialize the Random class
INIT_RANDOM();

//Choose the type of the gene
typedef myType Gene;

//Subclass the main class of the library
class GA : public SGA::GeneticAlgorithm<Gene>
{
	public :
	
		//Constructor (which needs to call the parent constructor)
		GA() : SGA::GeneticAlgorithm<Gene>() {}
	
		//Generate a random gene
		virtual Gene randomGene() const override
		{
			//your code here
			//think of SGA::Random::get(min, max) etc
		}
		
		//Compute the fitness score of a chromosome
		virtual SGA::Score score(SGA::Chromosome<Gene> const & chromosome) const override
		{
			//your code here
		}
		
		//OPTIONAL: chromosome to string (useful if you enable logging)
		virtual std::string print(SGA::Chromosome<Gene> const & chromosome) const override
		{
			//your code here, if you want/need
		}
};
