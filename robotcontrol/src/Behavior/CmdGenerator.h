/*
 * Copyright (c) 2002-2005, Neuroinformatics research group, 
 * University of Osnabrueck <tribots@informatik.uni-osnabrueck.de>
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _CMDGENERATOR_H_
#define _CMDGENERATOR_H_

#include "../Structures/DriveVector.h"
#include "../Structures/TribotsException.h"
#include "../Fundamental/Time.h"
#include <string>

namespace Tribots {

  /** reactive or deliberative policy to generate drivevectors (actions) */
  class CmdGenerator {
  public:
    virtual ~CmdGenerator() throw () {};

    /** 
     * true, if it is possible to continue this action in the current
     * state of the worldmodel 
     * \param Time the expected execution time of the present command cycle
     * \returns true if the commitment condition is fullfilled. default
     *          implementation returns always true.
     */
    virtual bool checkCommitmentCondition(const Time&) throw()
    { return true; }
    /**
     * generates a drivevector 
     * \param dv drivevector that will be filled by the method
     * \param Time the expected execution time of the present command cycle */
    virtual void getCmd(DriveVector *dv, const Time&) throw(TribotsException)=0;
    
    /**
     * notifies the command generator, that it is to gain control during the
     * present control cycle. May be implemented by deliberative command 
     * generators. May throw an exception, if it's not able to gain control.
     * In such a case checkCommitmentCondition should return false.
     * \param Time the expected execution time of the present control cycle */
    virtual void gainControl(const Time&) throw(TribotsException) {}
    /**
     * notifies the command generator, that it is to loose control in the
     * present control cycle. May be implemented by deliberative command
     * generators. The behavior must accept to loose control in every possible
     * time step, if it was active.
     * \param Time the expected execution time of the present control cycle */
    virtual void looseControl(const Time&) throw(TribotsException) {}

    virtual const char* getName() const { return name.c_str(); }

  protected:
    CmdGenerator(const char* name1="CmdGenerator") : name(name1) {} ///< standard constructor
    std::string name;
  };

}

#endif
