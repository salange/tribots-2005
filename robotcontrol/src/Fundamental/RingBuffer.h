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


#ifndef Tribots_RingBuffer_h_
#define Tribots_RingBuffer_h_

#include <vector>

namespace Tribots {

  /** Klasse RingBuffer implementiert einen Ringpuffer fester Groesse */
  template<class T> class RingBuffer {
  private:
    int n;                    ///< Groesse des Puffers
    std::vector<T> elem;      ///< Speicherstruktur
    int anchor;               ///< Index des momentan ersten Elements

  public:
    /** Konstruktor, uebergibt die Groesse des Puffers */
    RingBuffer (unsigned int) throw (std::bad_alloc);
    /** Destruktor */
    ~RingBuffer () throw ();

    /** Groesse des Puffers liefern */
    unsigned int size () const throw ();
    /** Groesse des Puffers veraendern */
    void resize (unsigned int) throw (std::bad_alloc);
    /** Weiterbewegen der Verankerung um arg1 Elemente */
    void step (int =1) throw ();
    /** liefert das Ankerelement */
    const T& get () const throw ();
    /** liefert das Ankerelement */
    T& get () throw ();
    /** liefert das Element an Position Anker+arg */
    const T& operator[] (int) const throw ();
     /** liefert das Element an Position Anker+arg */
    T& operator[] (int) throw ();
  };

}





// Implementierung, wegen template-Deklaration:
template<class T> Tribots::RingBuffer<T>::RingBuffer (unsigned int n1) throw (std::bad_alloc) : n(n1), elem(n1), anchor(0) {;}

template<class T> Tribots::RingBuffer<T>::~RingBuffer () throw () {;}

template<class T> unsigned int Tribots::RingBuffer<T>::size () const throw () { return n; }

template<class T> void Tribots::RingBuffer<T>::resize (unsigned int n1) throw (std::bad_alloc) { 
  n=n1;
  elem.resize (n);
  if (anchor>=n)
    anchor=0;
}

template<class T> void Tribots::RingBuffer<T>::step (int n1) throw () {
  anchor+=n1;
  while (anchor<0)
    anchor+=n;
  while (anchor>=n)
    anchor-=n;
}

template<class T> const T& Tribots::RingBuffer<T>::get () const throw () { return elem[anchor]; }

template<class T> T& Tribots::RingBuffer<T>::get () throw () { return elem[anchor]; }

template<class T> const T& Tribots::RingBuffer<T>::operator[] (int i) const throw () {
  i+=anchor;
  while (i<0)
    i+=n;
  while (i>=n)
    i-=n;
  return elem[i];
}

template<class T> T& Tribots::RingBuffer<T>::operator[] (int i) throw () {
  i+=anchor;
  while (i<0)
    i+=n;
  while (i>=n)
    i-=n;
  return elem[i];
}


#endif

