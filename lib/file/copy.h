/*
 * Copyright (c) 2008-2016 the MRtrix3 contributors
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 * 
 * MRtrix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * For more details, see www.mrtrix.org
 * 
 */

#ifndef __file_copy_h__
#define __file_copy_h__

#include "file/utils.h"
#include "file/mmap.h"


namespace MR
{
  namespace File
  {


    inline void copy (const std::string& source, const std::string& destination)
    {
      DEBUG ("copying file \"" + source + "\" to \"" + destination + "\"...");
      MMap input (source);
      create (destination, input.size());
      MMap output (destination, true);
      ::memcpy (output.address(), input.address(), input.size());
    }


  }
}

#endif


