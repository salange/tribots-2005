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


#ifndef _Tribots_DynamicRingBuffer_h_
#define _Tribots_DynamicRingBuffer_h_

#include <stdexcept>

namespace Tribots {

  /** Klasse DynamicRingBuffer realisiert einen Ringspeicher dynamisch 
      veraenderbarer Groesse; Zugriff erfolgt relativ zu einer Ankerposition 
      ACHTUNG: Mindestgroesse des Speichers muss 1 sein! */
  template<class T> class DynamicRingBuffer {
    /** Interne Knotenstruktur des Ringspeichers */
    struct Node {
      T elem;
      Tribots::DynamicRingBuffer<T>::Node* next;
    };
  private:
    unsigned int cur_size;       ///< aktuelle Groesse

    Node* anchor;                ///< Verankerung
    Node* pred_anchor;           ///< Vorgaengerknoten zur Verankerung
    Node* empty_nodes;           ///< Liste der leeren Knoten, die zur Wiederverwendung aufbewahrt werden

  public:
    /** Konstruktor; arg1= Speichergroesse (mindestens 1) */
    DynamicRingBuffer (unsigned int =1) throw (std::bad_alloc);
    /** Destruktor */
    ~DynamicRingBuffer () throw ();

    /** liefert die momentane Groesse des Speichers */
    unsigned int size () const throw ();
    /** Weiterbewegen der Verankerung um arg1 Elemente */
    void step (unsigned int =1) throw ();
    /** liefert das Element an Position Anker+arg1 */
    const T& get (unsigned int =0) const throw ();
    /** setzt das Element an Position Anker+arg1; wirft selbst keine Ausnahmen */
    void set (const T&, unsigned int =0);
    /** fuegt ein Element an der Ankerposition ein, Anker steht danach auf neuem Element;
        wirft ggf. eine bad_alloc Ausnahme */
    void insert (const T&);
    /** Element an der Ankerposition entfernen, wenn dies nicht das letzte Element ist;
	Anker steht danach auf Element Anker+1 */
    void erase () throw ();
  };

}








// Implementierung, wegen template-Deklaration:

template<class T> Tribots::DynamicRingBuffer<T>::DynamicRingBuffer (unsigned int n) throw (std::bad_alloc) : empty_nodes(NULL) {
  if (n==0) 
    n=1;
  cur_size = n;
  anchor = new Tribots::DynamicRingBuffer<T>::Node;
  pred_anchor=anchor;
  while ((--n)>0) {
    Tribots::DynamicRingBuffer<T>::Node* new_node = new Tribots::DynamicRingBuffer<T>::Node;
    new_node->next=anchor;
    anchor=new_node;
  }
  pred_anchor->next=anchor;
}

template<class T> Tribots::DynamicRingBuffer<T>::~DynamicRingBuffer () throw () {
  pred_anchor->next=NULL;
  while (anchor->next) {
    Tribots::DynamicRingBuffer<T>::Node* new_node = anchor;
    anchor = anchor->next;
    delete new_node;
  }
  while (empty_nodes) {
    Tribots::DynamicRingBuffer<T>::Node* new_node = empty_nodes;
    empty_nodes=empty_nodes->next;
    delete new_node;
  }
}

template<class T> unsigned int Tribots::DynamicRingBuffer<T>::size () const throw () {
  return cur_size;
}

template<class T> void Tribots::DynamicRingBuffer<T>::step (unsigned int n) throw () {
  while (n-->0)
    pred_anchor=pred_anchor->next;
  anchor=pred_anchor->next;
}

template<class T> const T&  Tribots::DynamicRingBuffer<T>::get (unsigned int n) const throw () {
  Tribots::DynamicRingBuffer<T>::Node* ptr = anchor;
  while ((n--)>0)
    ptr=ptr->next;
  return ptr->elem;
}

template<class T> void Tribots::DynamicRingBuffer<T>::set (const T& e, unsigned int n) {
  Tribots::DynamicRingBuffer<T>::Node* ptr = anchor;
  while ((n--)>0)
    ptr=ptr->next;
  ptr->elem = e;
}

template<class T> void Tribots::DynamicRingBuffer<T>::insert (const T& e) {
  if (empty_nodes) {
    pred_anchor->next=empty_nodes;
    empty_nodes=empty_nodes->next;
  } else 
    pred_anchor->next=new Tribots::DynamicRingBuffer<T>::Node;   // hier wird ggf. bad_alloc geworfen
  cur_size++;
  pred_anchor->next->next = anchor;
  pred_anchor->next->elem = e;
  anchor=pred_anchor->next;
}

template<class T> void Tribots::DynamicRingBuffer<T>::erase () throw () {
  if (cur_size>1) {
    cur_size--;
    pred_anchor->next=anchor->next;
    anchor->next=empty_nodes;
    empty_nodes=anchor;
    anchor=pred_anchor->next;
  }
}

#endif
