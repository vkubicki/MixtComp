/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2013-2014

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
 *  Created on: August 25, 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>,
 **/

#ifndef MIXT_EXPORT_H_
#define MIXT_EXPORT_H_

#include <iostream>
#include <sstream>
#include <fstream>

namespace mixt {

template <typename Type>
Type str2type(std::string s) {
  std::istringstream i(s);
  Type x;
  i >> x;
  return x;
}

template<typename Type>
std::string type2str(const Type& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename Type>
void writeDataCsv(std::string fileName, const Type& data)
{
  std::cout << "writing: " << fileName << std::endl;
  std::ofstream stream;
  stream.open(fileName.c_str());
  for (int i = 0;
       i < data.rows();
       ++i)
  {
    for (int j = 0;
         j < data.cols();
         ++j)
    {
      stream << data(i,j);
      if (j < data.cols())
        stream << ";";
    }
    stream << std::endl;
  }
  stream.close();
}

/**
 * Sequentially output elements of iterable object to a string
 *
 * @param obj object to iterate on
 */
template<typename Type>
std::string itString(const Type& obj)
{
  if (obj.size() == 0)
  {
    return std::string();
  }

  std::stringstream sstm;
  typename Type::const_iterator it    = obj.begin();
  typename Type::const_iterator itEnd = obj.end();

  sstm << *it;
  ++it;

  for (;
       it != itEnd;
       ++it)
  {
    sstm << " " << *it;
  }

  return sstm.str();
}

} // namespace mixt

#endif /* MIXT_EXPORT_H_ */
