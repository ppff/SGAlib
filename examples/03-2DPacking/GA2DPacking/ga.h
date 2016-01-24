#ifndef GA_H
#define GA_H

#include <QPoint>
#include <QPair>
#include <QRect>

#include "sga.hpp"

#define MAX_WIDTH 800
#define MAX_HEIGHT 500

//Choose the type of the gene
typedef QPoint Gene;

//A rectangle (width x height)
typedef QPair<int, int> Rect;

//The bounding box
typedef QPair<QPoint, QPoint> BB;

//Subclass the main class of the library
class GA : public SGA::GeneticAlgorithm<Gene>
{
    public :

        //Constructor (which needs to call the parent constructor)
        GA() : SGA::GeneticAlgorithm<Gene>()
        {
            //Generate a random cluster of rectangles
            const int number = SGA::Random::get(50, 100);

            //Generate the rectangles
            for (int i=0 ; i<number ; i++)
                rectangles.append(Rect(SGA::Random::get(10, 60), SGA::Random::get(10, 60)));
        }

        //Generate a random gene
        virtual Gene randomGene() const override
        {
            return QPoint(SGA::Random::get(0, MAX_WIDTH), SGA::Random::get(0, MAX_HEIGHT));
        }

        //Compute the fitness score of a chromosome
        virtual SGA::Score score(SGA::Chromosome<Gene> const & chromosome) const override
        {
            //The score needs to reflect two things:
            // - have the smallest bounding box
            // - no rectangles are overlapping

            const BB bb = boundingBox(chromosome);
            const int c = collisions(chromosome);

            //if (c > 0)
            //    return 0.0;

            return 10000.0 - ((double)((bb.second.x() - bb.first.x())+(bb.second.y() - bb.first.y()))) - 10.0 * c;
            return /*10000000.0*/1000.0/((double)((bb.second.x() - bb.first.x())+(bb.second.y() - bb.first.y())) + 0.1);// - 1.0 * c;
        }

        //Not using the print function
        //virtual std::string print(SGA::Chromosome<Gene> const & chromosome) const override;

        //Compute the bounding box
        BB boundingBox(SGA::Chromosome<Gene> const & chromosome) const
        {
            int xmin = MAX_WIDTH, xmax = 0, ymin = MAX_HEIGHT, ymax = 0;
            for (unsigned i=0 ; i<chromosome.size() ; i++)
            {
                if (chromosome[i].x() < xmin)
                    xmin = chromosome[i].x();
                if (chromosome[i].y() < ymin)
                    ymin = chromosome[i].y();
                if (chromosome[i].x() + rectangles[i].first > xmax)
                    xmax = chromosome[i].x() + rectangles[i].first;
                if (chromosome[i].y() + rectangles[i].second > ymax)
                    ymax = chromosome[i].y() + rectangles[i].second;
            }

            return BB(QPoint(xmin, ymin), QPoint(xmax, ymax));
        }

        //Compute the number of collisions
        int collisions(SGA::Chromosome<Gene> const & chromosome) const
        {
            int collisions = 0;

            for (unsigned i=0 ; i<chromosome.size()-1 ; i++)
            {
                for (unsigned j=i+1 ; j<chromosome.size() ; j++)
                {
                    if (        chromosome[i].x() < chromosome[j].x() + rectangles[j].first     //RectA.X1 < RectB.X2
                            &&  chromosome[i].x() + rectangles[i].first > chromosome[j].x()     //RectA.X2 > RectB.X1
                            &&  chromosome[i].y() < chromosome[j].y() + rectangles[j].second    //RectA.Y1 < RectB.Y2
                            &&  chromosome[i].y() + rectangles[i].second > chromosome[j].y()    //RectA.Y2 > RectB.Y1
                       )
                    {
                        collisions ++;
                    }
                }
            }

            return collisions;
        }

        //Get a list of QRect to draw based on a chromosome
        QList<QRect> getRectangles(SGA::Chromosome<Gene> const & chromosome) const
        {
            QList<QRect> result;

            for (unsigned i=0 ; i<chromosome.size() ; i++)
            {
                result.append(QRect(chromosome[i].x(), chromosome[i].y(), rectangles[i].first, rectangles[i].second));
            }

            return result;
        }

        //Get the number of rectangles
        int numberOfRectangles() const
        {
            return rectangles.size();
        }

    private :

        //Cluster of rectangles we want to pack
        QList<Rect> rectangles;
};

#endif // GA_H
