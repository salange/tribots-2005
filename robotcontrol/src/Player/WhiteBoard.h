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

#ifndef _WHITEBOARD_H_
#define _WHITEBOARD_H_

#include "../Fundamental/Time.h"
#include "../Fundamental/Frame2D.h"
#include "../Fundamental/geometry.h"
#include "../Structures/PlayerRole.h"

namespace Tribots {
  
  /** Singleton, das Standardabfragen und Prädikate bereitstellt. 
   *  Rechenzeitaufwändige Anfragen sollen ihre Ergebnisse zwischenspeichern
   *  und nur neu berechnen, wenn Zykluszähler oder angefragter Zeitstempel
   *  abweichen.
   *
   *  Dieses WhiteBoard ist ersteinmal eine Minimalversion und bietet keinen
   *  einheitlichen Mechanismus um Berechnungsergebnisse zwischenzuspeichern 
   *  und zu verwalten. Könnte man in der Zukunft noch ausbauen...
   */
  class WhiteBoard {
  public:
    /** von dieser Klasse werden spezielle Datentypen abgeleitet, die
     *  verschiedene Werte mit Zeitstempel und Zyklus versehen 
     *  zwischenpeichern können. 
     */
    class Data {
    public:
      Time t;
      unsigned long cycle;

      virtual ~Data() {}
    };

    /** liefert einen Pointer auf die Instanz des WhiteBoards. */
    static WhiteBoard* getTheWhiteBoard();


    /** Liefert die Matrix zur Umrechnung von absoluten nach relativen 
     *  Koordinaten. 
     *  \attention Die Zeitstempel von umgerechneten Positionen und angefragter
     *             Umrechnungsmatrix sollten übereinstimmen!
     */
    const Frame2d& getAbs2RelFrame(const Time& t);
    /** Liefert die Matrix zur Umrechnung von relativen nach absoluten 
     *  Koordinaten. 
     *  \attention Die Zeitstempel von umgerechneten Positionen und angefragter
     *             Umrechnungsmatrix sollten übereinstimmen!
     */
    const Frame2d& getRel2AbsFrame(const Time& t);

    /** Test, ob ball im gegnerischen tor, wobei die tiefe des tores mit 2 
     *  multipliziert wird, um fehler zu verhindern */
    bool isBallInOppGoal(const Time& t);
    /** Prüft, ob der Roboter selbst im Ballbesitz ist. */
    bool doPossesBall(const Time& t);
    /**Nur true setzen wenn man den keinen Ballbesitz erhalten möchte obwohl man welchen haben könnte, unbedingt immer wieder nach Gebrauch ausschalten*/
    bool m_bDoNotGetBallPossession;
    /** gibt an, ob ein anderer Roboter des Teams im Ballbesitz ist */
    bool teamPossesBall();
    /** gibt an, ob ein anderer Roboter des Teams im erweiterten Ballbesitz ist */
    bool advancedTeamPossesBall();    

    // RCPlayer-spezifische Kommunikation //////////////////////////////

    /** KickOff-Position und  Orientierung abfragen (wird ueber MessageBoard/Kommunikation von aussen gesetzt). RCPlayer-spezifisch! */
    void kickOffPosition(Vec&, Angle&);
    /** liefert die aktuelle Rolle zurück. RCPlayer-spezifisch! */
    const PlayerRole getPlayerRole();
    /** aendert die Rolle. RCPlayer-spezifisch! */
    void changePlayerRole(PlayerRole);
    /** prueft das Messageboard auf für den RCPlayer interessante Inhalte */
    void checkMessageBoard();

    virtual ~WhiteBoard();
  
  protected:
    WhiteBoard();
    static WhiteBoard* instance;

    Data* ballInOppGoalData;
    Data* possesBallData;
    Data* abs2relData;
    Data* rel2absData;

    PlayerRole playerRole;

    int frames_ball_owned;
    unsigned int cycles_without_team_posses_ball;
    unsigned int cycles_without_advanced_team_posses_ball;    
    
    Vec kick_off_pos;
    Angle kick_off_heading;
    
    class BoolData : public Data {
    public:
      bool b;
    };
     
    class Frame2dData : public Data {
    public:
      Frame2d frame;
    };
  }; 

}

#define WBOARD Tribots::WhiteBoard::getTheWhiteBoard()
#endif
