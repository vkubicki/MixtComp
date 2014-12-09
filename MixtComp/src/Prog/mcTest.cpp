/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2014

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *  Project:    MixtComp
 *  Created on: Nov 14, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include <boost/math/distributions/normal.hpp>

template<class T>
double gen_normal_3(T &generator)
{
  return generator();
}

// Version that fills a vector
template<class T>
void gen_normal_3(T &generator,
              std::vector<double> &res)
{
  for(size_t i=0; i<res.size(); ++i)
    res[i]=generator();
}

int main(void)
{
  boost::variate_generator<boost::mt19937, boost::normal_distribution<> >
    generator(boost::mt19937(time(0)),
              boost::normal_distribution<>());

  for(size_t i=0; i<10; ++i)
    std::cout<<gen_normal_3(generator)
             <<std::endl;
}

int main()
{

}
