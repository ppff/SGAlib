// Copyright © 2015 Pierre Schefler <schefler.pierre@gmail.com>
// This work is free. You can redistribute it and/or modify it under the
// terms of the Do What The Fuck You Want To Public License, Version 2,
// as published by Sam Hocevar. See the LICENSE.md file for more details.

/* The algorithm is subclassed in algo.hpp and test functions are included. They are called in main().
 * The purpose of this test is to verify that the various genetic operators work as expected.
 */

#include <iostream>
#include "algo.hpp"

INIT_RANDOM();

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
