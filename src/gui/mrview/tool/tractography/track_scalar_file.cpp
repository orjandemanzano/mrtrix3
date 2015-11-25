/*
    Copyright 2013 Brain Research Institute, Melbourne, Australia

    Written by David Raffelt, 01/02/13.

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

#include "gui/mrview/tool/tractography/track_scalar_file.h"
#include "gui/mrview/colourmap.h"

namespace MR
{
  namespace GUI
  {
    namespace MRView
    {
      namespace Tool
      {

        TrackScalarFile::TrackScalarFile (Dock* parent) :
            Base (parent)
        {
          main_box = new VBoxLayout (this);
          main_box->setContentsMargins (5, 5, 5, 5);
          main_box->setSpacing (5);

          HBoxLayout* hlayout = new HBoxLayout;
          hlayout->setContentsMargins (0, 0, 0, 0);
          hlayout->setSpacing (0);

          intensity_file_button = new QPushButton (this);
          intensity_file_button->setToolTip (tr ("Open scalar track file"));
          connect (intensity_file_button, SIGNAL (clicked()), this, SLOT (open_intensity_track_scalar_file_slot ()));
          hlayout->addWidget (intensity_file_button);

          // Colourmap menu:
          colourmap_menu = new QMenu (tr ("Colourmap menu"), this);

          ColourMap::create_menu (this, colourmap_group, colourmap_menu, colourmap_actions, false, false);
          connect (colourmap_group, SIGNAL (triggered (QAction*)), this, SLOT (select_colourmap_slot()));
          colourmap_actions[1]->setChecked (true);

          colourmap_menu->addSeparator();

          show_colour_bar = colourmap_menu->addAction (tr ("Show colour bar"), this, SLOT (show_colour_bar_slot()));
          show_colour_bar->setCheckable (true);
          show_colour_bar->setChecked (true);
          addAction (show_colour_bar);

          invert_scale = colourmap_menu->addAction (tr ("Invert"), this, SLOT (invert_colourmap_slot()));
          invert_scale->setCheckable (true);
          addAction (invert_scale);

          scalarfile_by_direction = colourmap_menu->addAction (tr ("Colour by direction"), this, SLOT (scalarfile_by_direction_slot()));
          scalarfile_by_direction->setCheckable (true);
          addAction (scalarfile_by_direction);

          colourmap_menu->addSeparator();

          QAction* reset_intensity = colourmap_menu->addAction (tr ("Reset intensity"), this, SLOT (reset_intensity_slot()));
          addAction (reset_intensity);

          colourmap_button = new QToolButton (this);
          colourmap_button->setToolTip (tr ("Colourmap menu"));
          colourmap_button->setIcon (QIcon (":/colourmap.svg"));
          colourmap_button->setPopupMode (QToolButton::InstantPopup);
          colourmap_button->setMenu (colourmap_menu);
          hlayout->addWidget (colourmap_button);

          main_box->addLayout (hlayout);

          QGroupBox* group_box = new QGroupBox ("Intensity scaling");
          main_box->addWidget (group_box);
          hlayout = new HBoxLayout;
          group_box->setLayout (hlayout);

          min_entry = new AdjustButton (this);
          connect (min_entry, SIGNAL (valueChanged()), this, SLOT (on_set_scaling_slot()));
          hlayout->addWidget (min_entry);

          max_entry = new AdjustButton (this);
          connect (max_entry, SIGNAL (valueChanged()), this, SLOT (on_set_scaling_slot()));
          hlayout->addWidget (max_entry);


          QGroupBox* threshold_box = new QGroupBox ("Thresholds");
          main_box->addWidget (threshold_box);
          hlayout = new HBoxLayout;
          threshold_box->setLayout (hlayout);

          threshold_lower_box = new QCheckBox (this);
          connect (threshold_lower_box, SIGNAL (stateChanged(int)), this, SLOT (threshold_lower_changed(int)));
          hlayout->addWidget (threshold_lower_box);
          threshold_lower = new AdjustButton (this, 0.1);
          connect (threshold_lower, SIGNAL (valueChanged()), this, SLOT (threshold_lower_value_changed()));
          hlayout->addWidget (threshold_lower);

          threshold_upper_box = new QCheckBox (this);
          hlayout->addWidget (threshold_upper_box);
          threshold_upper = new AdjustButton (this, 0.1);
          connect (threshold_upper_box, SIGNAL (stateChanged(int)), this, SLOT (threshold_upper_changed(int)));
          connect (threshold_upper, SIGNAL (valueChanged()), this, SLOT (threshold_upper_value_changed()));
          hlayout->addWidget (threshold_upper);

          hlayout = new HBoxLayout;

          threshold_file_button = new QPushButton (this);
          threshold_file_button->setToolTip (tr ("Open scalar track file"));
          connect (threshold_file_button, SIGNAL (clicked()), this, SLOT (open_threshold_track_scalar_file_slot ()));
          hlayout->addWidget (threshold_file_button);

          main_box->addLayout (hlayout);

          main_box->addStretch ();
          setMinimumSize (main_box->minimumSize());
        }


        void TrackScalarFile::set_tractogram (Tractogram* selected_tractogram) {
          tractogram = selected_tractogram;
          update_tool_display();
        }



        void TrackScalarFile::render_tractogram_colourbar(const Tractogram &tractogram) {
          float min_value = tractogram.use_discard_lower() ?
            tractogram.scaling_min_thresholded() :
            tractogram.scaling_min();

          float max_value = tractogram.use_discard_upper() ?
            tractogram.scaling_max_thresholded() :
            tractogram.scaling_max();

          window().colourbar_renderer.render (tractogram.colourmap, tractogram.scale_inverted(),
                                              min_value, max_value,
                                              tractogram.scaling_min(), tractogram.display_range, tractogram.colour);
        }



        void TrackScalarFile::clear_tool_display () {
          intensity_file_button->setText ("");
          intensity_file_button->setEnabled (false);
          threshold_file_button->setText ("");
          threshold_file_button->setEnabled (false);
          min_entry->setEnabled (false);
          max_entry->setEnabled (false);
          min_entry->clear();
          max_entry->clear();
          threshold_lower_box->setChecked(false);
          threshold_upper_box->setChecked(false);
          threshold_lower_box->setEnabled (false);
          threshold_upper_box->setEnabled (false);
          threshold_lower->setEnabled (false);
          threshold_upper->setEnabled (false);
          threshold_lower->clear();
          threshold_upper->clear();
          colourmap_menu->setEnabled (false);
        }


        void TrackScalarFile::update_tool_display () {

          if (!tractogram) {
            clear_tool_display ();
            return;
          }

          if (tractogram->intensity_scalar_filename.size()) {
            intensity_file_button->setEnabled (true);
            threshold_file_button->setEnabled (true);
            min_entry->setEnabled (true);
            max_entry->setEnabled (true);
            min_entry->setRate (tractogram->scaling_rate());
            max_entry->setRate (tractogram->scaling_rate());

            threshold_lower_box->setEnabled (true);
            if (tractogram->use_discard_lower()) {
              threshold_lower_box->setChecked(true);
              threshold_lower->setEnabled (true);
            } else {
              threshold_lower->setEnabled (false);
              threshold_lower_box->setChecked(false);
            }
            threshold_upper_box->setEnabled (true);
            if (tractogram->use_discard_upper()) {
              threshold_upper_box->setChecked (true);
              threshold_upper->setEnabled (true);
            } else {
              threshold_upper->setEnabled (false);
              threshold_upper_box->setChecked (false);
            }
            threshold_lower->setRate (tractogram->scaling_rate());
            threshold_lower->setValue (tractogram->lessthan);
            threshold_upper->setRate (tractogram->scaling_rate());
            threshold_upper->setValue (tractogram->greaterthan);
            colourmap_menu->setEnabled (true);
            colourmap_actions[tractogram->colourmap]->setChecked (true);
            show_colour_bar->setChecked (tractogram->show_colour_bar);
            invert_scale->setChecked (tractogram->scale_inverted());
            scalarfile_by_direction->setChecked (tractogram->scalarfile_by_direction);
            if (tractogram->intensity_scalar_filename.length()) {
              intensity_file_button->setText (shorten (tractogram->intensity_scalar_filename, 35, 0).c_str());
              min_entry->setValue (tractogram->scaling_min());
              max_entry->setValue (tractogram->scaling_max());
            } else {
              intensity_file_button->setText (tr("Open File"));
            }

            if (tractogram->threshold_scalar_filename.length()) {
              threshold_file_button->setText (shorten (tractogram->threshold_scalar_filename, 35, 0).c_str());
            } else {
              threshold_file_button->setText (tr("Open File"));
            }

          } else {
            clear_tool_display ();
            intensity_file_button->setText (tr("Open File"));
            intensity_file_button->setEnabled (true);

            threshold_file_button->setText (tr("Open File"));
            threshold_file_button->setEnabled (true);
          }
        }


        bool TrackScalarFile::open_intensity_track_scalar_file_slot ()
        {
          std::string scalar_file = Dialog::File::get_file (this, "Select scalar text file or Track Scalar file (.tsf) to open", "");
          if (scalar_file.empty())
            return false;

          try {
            tractogram->load_intensity_track_scalars (scalar_file);
            tractogram->color_type = ScalarFile;
            set_tractogram (tractogram);
          } 
          catch (Exception& E) {
            E.display();
            return false;
          }
          return true;
        }

        bool TrackScalarFile::open_threshold_track_scalar_file_slot ()
        {
          std::string scalar_file = Dialog::File::get_file (this, "Select scalar text file or Track Scalar file (.tsf) to open", "");
          if (scalar_file.empty())
            return false;

          try {
            tractogram->load_threshold_track_scalars (scalar_file);
            tractogram->color_type = ScalarFile;
            set_tractogram (tractogram);
          }
          catch (Exception& E) {
            E.display();
            return false;
          }
          return true;
        }


        void TrackScalarFile::show_colour_bar_slot ()
        {
          if (tractogram) {
            tractogram->show_colour_bar = show_colour_bar->isChecked();
            window().updateGL();
          }
        }


        void TrackScalarFile::select_colourmap_slot ()
        {
          if (tractogram) {
            QAction* action = colourmap_group->checkedAction();
            size_t n = 0;
            while (action != colourmap_actions[n])
              ++n;
            tractogram->colourmap = n;
            window().updateGL();
          }
        }


        void TrackScalarFile::on_set_scaling_slot ()
        {
          if (tractogram) {
            tractogram->set_windowing (min_entry->value(), max_entry->value());
            window().updateGL();
          }
        }


        void TrackScalarFile::threshold_lower_changed (int)
        {
          if (tractogram) {
            threshold_lower->setEnabled (threshold_lower_box->isChecked());
            tractogram->set_use_discard_lower (threshold_lower_box->isChecked());
            window().updateGL();
          }
        }


        void TrackScalarFile::threshold_upper_changed (int)
        {
          if (tractogram) {
            threshold_upper->setEnabled (threshold_upper_box->isChecked());
            tractogram->set_use_discard_upper (threshold_upper_box->isChecked());
            window().updateGL();
          }
        }



        void TrackScalarFile::threshold_lower_value_changed ()
        {
          if (tractogram && threshold_lower_box->isChecked()) {
            tractogram->lessthan = threshold_lower->value();
            window().updateGL();
          }
        }



        void TrackScalarFile::threshold_upper_value_changed ()
        {
          if (tractogram && threshold_upper_box->isChecked()) {
            tractogram->greaterthan = threshold_upper->value();
            window().updateGL();
          }
        }


        void TrackScalarFile::scalarfile_by_direction_slot ()
        {
          if (tractogram) {
            tractogram->scalarfile_by_direction = scalarfile_by_direction->isChecked();
            window().updateGL();
          }
        }

        void TrackScalarFile::reset_intensity_slot ()
        {
          if (tractogram) {
            tractogram->reset_windowing();
            update_tool_display ();
            window().updateGL();
          }
        }


        void TrackScalarFile::invert_colourmap_slot ()
        {
          if (tractogram) {
            tractogram->set_invert_scale (invert_scale->isChecked());
            window().updateGL();
          }
        }

      }
    }
  }
}
