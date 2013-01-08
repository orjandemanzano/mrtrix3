/*
   Copyright 2009 Brain Research Institute, Melbourne, Australia

   Written by J-Donald Tournier & David Raffelt, 17/12/12.

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __gui_mrview_tool_tractogram_h__
#define __gui_mrview_tool_tractogram_h__

#include <QAction>
#include <string>

// necessary to avoid conflict with Qt4's foreach macro:
#ifdef foreach
# undef foreach
#endif

#include "gui/mrview/displayable.h"
#include "gui/mrview/shader.h"
#include "dwi/tractography/properties.h"
#include "dwi/tractography/file.h"


namespace MR
{
  class ProgressBar;

  namespace GUI
  {
    class Projection;

    namespace MRView
    {
      class Window;

      namespace Tool
      {

        class Tractogram : public Displayable
        {
          Q_OBJECT

          public:
            Tractogram (const std::string& filename);
            Tractogram (Window& parent, const std::string& filename);

            ~Tractogram ();

            std::string& get_filename () {
              return filename;
            }

            const std::string& get_filename () const {
              return filename;
            }

            void render ();

          signals:
            void scalingChanged ();

          private:
            std::string filename;
            std::vector<GLuint> vertex_buffers;
            DWI::Tractography::Reader<float> file;
            DWI::Tractography::Properties properties;

            void set_color () {
            }

            void load_tracks();

        };

      }
    }
  }
}

#endif

