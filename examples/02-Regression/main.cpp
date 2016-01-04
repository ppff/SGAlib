// Copyright © 2015 Pierre Schefler <schefler.pierre@gmail.com>
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE.md file for more details.

/* In this example, we set up a genetic algorithm to find a function.
 * A function will be represented as a string of operators (+, -, *, /) and numbers.
 * We will manually define the function and generate some values with it, to which we will add some noise.
 * The score of a chromosome (ie, a function) will be computed as the error between the values it generates and the one we had with our original function.
 * Ultimately, we want the algorithm to find the original function.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <map>
#include <limits>

#include "../../src/sga.hpp"

/* What a gene can be */

class Gene
{
	public :
	
		enum Type {Add, Sub, Mul, Div, Number, Input};
		
		//Random init in the c-tor
		Gene()
		{
			_type = SGA::Random::get(0, Input);
			if (_type == Number)
				_number = SGA::Random::get(0.0, 100.0);
		}
		
		//Construct with a specified type
		Gene(unsigned type, double number)
		{
			setType(type, number);
		}
		
		//Construct from a string
		Gene(std::string s)
		{
			fromString(s);
		}
		
		//Get the type
		unsigned type() const 
		{ 
			return _type; 
		}
		
		//Set the type
		void setType(unsigned type, double num) 
		{ 
			_type = type;
			if (type == Number)
				_number = num;
		}
		
		//Get the number
		double value() const
		{
			return _number;
		}
		
		//From string
		void fromString(std::string s)
		{
			if (s == std::string("+"))
			{
				_type = Add;
			}
			else if (s == std::string("-"))
			{
				_type = Sub;
			}
			else if (s == std::string("*"))
			{
				_type = Mul;
			}
			else if (s == std::string("/"))
			{
				_type = Div;
			}
			else if (s == std::string("x"))
			{
				_type = Input;
			}
			else
			{
				_type = Number;
				_number = std::stod(s);
			}
		}
		
		//To string
		std::string toString() const
		{
			switch (_type)
			{
				case Add : return std::string("+"); break;
				case Sub : return std::string("-"); break;
				case Mul : return std::string("*"); break;
				case Div : return std::string("/"); break;
				case Number : return std::to_string(_number); break;
				case Input : return std::string("x"); break;
			}
			
			return std::string("");
		}
	
	private :
	
		unsigned _type;
		double _number;
				
};

/* About chromosomes (functions)... */

std::string functionToString(SGA::Chromosome<Gene> const & function)
{
	std::stringstream ss;
	
	for (unsigned i=0 ; i<function.size() ; i++)
	{
		ss << function[i].toString();
		if (i != function.size()-1)
			ss << " ";
	}
	
	return ss.str();
}

SGA::Chromosome<Gene> stringToFunction(std::string const & str)
{
	std::istringstream iss(str);
	std::vector<std::string> tokens {std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
	SGA::Chromosome<Gene> result;
	
	for (std::string const & s : tokens)
		result.push_back(Gene(s));
	
	return result;
}

bool isValid(SGA::Chromosome<Gene> const & function)
{
	//A function can only be valid if genes are an operator and a number alternatively (1 + x * 6 - 7) and not (1 3 - + 9 / x)

	if (function[0].type() != Gene::Number && function[0].type() != Gene::Input)
		return false;
		
	for (unsigned i=0 ; i<function.size() ; i++)
	{
		unsigned type = function[i].type();
		if (i%2 == 0) //Number
		{
			if (type != Gene::Number && type != Gene::Input)
			{
				return false;
			}
		}
		else //Operator
		{
			if (type == Gene::Number || type == Gene::Input)
			{
				return false;
			}
		}
	}
	
	return true;
}

double evaluate(SGA::Chromosome<Gene> const & function, double x)
{
	if (!isValid(function))
		return 0.0;
	
	double result;
	Gene lastGene = function[0];
	
	result = lastGene.type() == Gene::Number ? lastGene.value() : x; //If it's not a number, it's an abscissa.
	
	for (unsigned i=1 ; i<function.size() ; i++)
	{
		Gene gene = function[i];
		
		if (i%2 == 0) //Number
		{
			switch (lastGene.type())
			{
				case Gene::Add : result += gene.type() == Gene::Number ? gene.value() : x; break;
				case Gene::Sub : result -= gene.type() == Gene::Number ? gene.value() : x; break;
				case Gene::Mul : result *= gene.type() == Gene::Number ? gene.value() : x; break;
				case Gene::Div : result /= gene.type() == Gene::Number ? gene.value() : x; break;
			}
		}
		//If it's an operator, we don't have to do anything
		
		lastGene = gene;
	}
	
	return result;
}

/* The function to find */

SGA::Chromosome<Gene> objective = stringToFunction("3 * x - 8.5");
std::vector<double> coordinates; //For safety reasons, never evaluate a function on 0 (problems with /x)
std::vector<double> values;

/* The algorithm class */

INIT_RANDOM();

class GA : public SGA::GeneticAlgorithm<Gene>
{
	public :
	
		GA() : SGA::GeneticAlgorithm<Gene>() {}
	
		virtual Gene randomGene() const override
		{
			return Gene();
		}
		
		virtual SGA::Score score(SGA::Chromosome<Gene> const & chromosome) const override
		{
			if (!isValid(chromosome))
				return std::numeric_limits<SGA::Score>::min();
			
			double error = 0.0;
			
			for (unsigned i=0 ; i<coordinates.size() ; i++)
			{
				error += std::pow(evaluate(chromosome, coordinates[i]) - values[i], 2);
			}
			
			double score = 100.0 / (error + 1.0);
				
			if (std::isnan(score))
			{
				std::cout << "[FAIL] " << print(chromosome) << " -> " << error << " | " << score << std::endl;
			}
			
			return score;
		}
		
		virtual std::string print(SGA::Chromosome<Gene> const & chromosome) const override
		{
			return functionToString(chromosome);
		}
};

/* A test function to verify the above functions work */

void test()
{
	std::cout << "WARNING: Running tests" << std::endl << std::endl;
	
	//Verify we uniformly create all types of genes
	std::cout << "1. Randomly creating 1000 genes : " << std::endl;
	std::map<unsigned, unsigned> geneTypes;
	for (unsigned i=0 ; i<1000 ; i++)
	{
		Gene gene;
		geneTypes[gene.type()]++;
	}
	
	for (auto it = geneTypes.begin() ; it != geneTypes.end() ; it++)
	{
		std::cout << " - number of genes of type " << it->first << " : " << it->second << std::endl;
	}
	std::cout << std::endl;
	
	//Verify the conversion between function and string works
	std::cout << "2. Converting string to function to string" << std::endl;
	std::vector<std::string> functions { "0.0", "x", "2 * x", "x * x", "5 * x - 3", "2.5 * x / 10" };
	for (unsigned i=0 ; i<functions.size() ; i++)
	{
		std::cout << " - " << i << " : '" << functions[i] << "' --> '" << functionToString(stringToFunction(functions[i])) << "'" << std::endl;
	}
	std::cout << std::endl;
	
	//Verify some values
	std::cout << "3. We will now verify the functions with these inputs: -1, 0 and 1 :" << std::endl;
	for (unsigned i=0 ; i<functions.size() ; i++)
	{
		std::cout << " - f(x) = " << functions[i] << " : f(-1) = " << evaluate(stringToFunction(functions[i]), -1.0) << " ; f(0) = " << evaluate(stringToFunction(functions[i]), 0.0) << " ; f(1) = " << evaluate(stringToFunction(functions[i]), 1.0) << std::endl;
	}
	std::cout << "Please verify that the above values are good." << std::endl;
	std::cout << std::endl;
	
	std::cout << "End of tests, exiting." << std::endl << std::endl;
	exit(EXIT_SUCCESS);
}

/* The program */

int main()
{
	//Optional test (will then exit the program)
	//test();
	
	//Create inputs
	for (float x=-5 ; x<=5 ; x+=0.1)
	{
		if (x != 0.0)
			coordinates.push_back(x);
	}
	
	//Introduction
	std::cout << "We will try to find the following function: f(x) = " << functionToString(objective) << std::endl;
	std::cout << "Corresponding inputs and outputs:" << std::endl;
	for (unsigned i=0 ; i<coordinates.size() ; i++)
	{
		std::cout << " " << coordinates[i] << " -> " << evaluate(objective, coordinates[i]) << std::endl;
	}
	
	//Compute values
	for (unsigned i=0 ; i<coordinates.size() ; i++)
	{
		values.push_back(evaluate(objective, coordinates[i]));
	}
	const double min = * std::min_element(values.begin(), values.end());
	const double max = * std::max_element(values.begin(), values.end());

	//Get the amount of noise
	std::cout << "Enter a max amount of noise to add to the values (we will add +- your max value)." << std::endl;
	std::cout << "For your information, the minimum value of the function is " << min << " and the max one is " << max << "." << std::endl;
	std::cout << "Your choice (" << (std::abs(min)+std::abs(max))/20.0 << " could be a good value): " << std::flush;
	double noise;
	std::cin >> noise;
	std::cout << std::endl;
	
	//Add the noise
	for (double & value : values)
	{
		value += SGA::Random::get(-noise, noise);
	}
	
	std::cout << "Inputs and outputs after noise:" << std::endl;
	for (unsigned i=0 ; i<coordinates.size() ; i++)
	{
		std::cout << " " << coordinates[i] << " -> " << values[i] << std::endl;
	}
	
	//Run the algorithm
	std::cout << "Starting the algorithm." << std::endl;
	std::cout << std::endl;
	
	GA algorithm;
	algorithm.setMainParameters(300, 0.1);
	algorithm.setChromosomesSize(5, 5);
	algorithm.setEndingCriterion(SGA::EndingCriterion::MaxScore, 100, 40);
	algorithm.setSelectionType(SGA::SelectionType::RouletteWheel, 20);
	algorithm.run(true, true);
	std::cout << std::endl;
	
	//Verify the result
	std::string result = functionToString(algorithm.best());
	std::cout << "The algorithm found: " << result;
	std::cout << std::endl;
	
	return EXIT_SUCCESS;
}
