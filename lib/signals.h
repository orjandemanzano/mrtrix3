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

#ifndef __signals_h__
#define __signals_h__


#include <map>
#include <signal.h>
#include <string>

#include "exception.h"



class SignalHandler
{

  public:
    SignalHandler()
    {
      add (SIGHUP,  "(SIGHUP) Disconnection of terminal");
      add (SIGINT,  "(SIGINT) Program manually interrupted");
      add (SIGQUIT, "(SIGQUIT) Process terminated (generating core dump)");
      add (SIGILL,  "(SIGILL) Illegal instruction (corrupt binary command file?)");
      add (SIGBUS,  "(SIGBUS) Bus error: Accessing invalid address (out of storage space?)");
      add (SIGFPE,  "(SIGFPE) Floating-point arithmetic exception");
      add (SIGSEGV, "(SIGSEGV) Segmentation fault");
      add (SIGALRM, "(SIGALRM) Timer expiration");
      add (SIGTERM, "(SIGTERM) Terminated by kill command");
      add (SIGPWR,  "(SIGPWR) Power failure restart");
      add (SIGSYS,  "(SIGSYS) Bad system call");
    }

  private:
    void add (const int, const std::string&);
    static void handler (int);
    static std::map<int, std::string> messages;

};

std::map<int, std::string> SignalHandler::messages;



void SignalHandler::handler (int code)
{
  FAIL("[SYSTEM] " + messages[code]);
  throw code;
}



void SignalHandler::add (const int code, const std::string& msg)
{
  struct sigaction act;
  act.sa_handler = &handler;
  sigemptyset (&act.sa_mask);
  sigaddset (&act.sa_mask, SIGINT);
  sigaddset (&act.sa_mask, SIGQUIT);
  act.sa_flags = 0;
  struct sigaction* old_act = nullptr;
  sigaction (code, &act, old_act);
  if (old_act) {
    delete old_act;
    old_act = nullptr;
  }
  messages[code] = msg;
}



#endif


