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


#ifndef _Tribots_ConfigReader_h_
#define _Tribots_ConfigReader_h_

#include <deque>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <stack>

namespace Tribots {
  
  /** ConfigReader, Alternative zum ValueReder
   * liest mit append_from_stream bzw. append_from_file Konfig-Dateien ein
   * Eintrage der Form "+ FILENAME" fuehren zu rekursivem Auswerten von Konfigurationsdateien
   * in allen Eintraegen wird das Kuerzel "$PATH" durch den Pfadnamen ersetzt, in dem sich die aktuell gelesene Konfigurationsdatei befindet
   * in allen Eintraegen wird das Kuerzel "$HOME" durch den Inhalt der Environment-Variable $HOME ersetzt
   * Blockmarkierungen [Ueberschrift] kann man abfragen mit get ("Ueberschrift::Zeile", ...)
   * Leere Blockmarkierung [] fuehrt zu keinen Prefix 
   Verbosity:
   0 -> keine Ausgabe
   1 -> fehlerhafte Zeilen melden
   2 -> lesen von Konfig-Files melden (auf stderr)
   3 -> lesen von korrekten Zeilen melden
   4 -> korrekte Zeilen mit Werten melden
  */
  class ConfigReader {
  private:
    struct KVPair;
    std::deque<KVPair> key_value_table;        // lexikographisch sortierte Liste mit Schluessel-Wert-Paaren
    const unsigned int verbosity;              // ermoeglicht verschiedene Kontrollausgaben, siehe oben
    const char comment_char;                   // Zeichen, um Kommentarzeilen einzuleiten
    const char assign_char;                    // Zeichen, um Schluessel-Wert-Zuweisungen zu markieren
    std::stack<std::string> directory_stack;   // beim Einlesen verschachtelter Aufrufe zum Zwischenspeichern des aktuellen Verzeichnisses
    std::deque<std::string> files;             // Liste aller eingelesener Dateien
    std::string home_path;                     // der Wert der Environment-Variable $HOME

    bool find_first (std::string& value, const std::string& key) const;  ///< finde erstes Wort im Valueteil zum Schluessel key, liefere true bei Erfolg
    bool find_all (std::deque<std::string>& values, const std::string& key) const;  ///< finde alle Worte im Valueteil zum Schluessel key, liefere true bei Erfolg

  public:
    /** ConfigReader erzeugen, 
	arg1=verbosity
	arg2=Kommentarzeichen
	arg3=Trennzeichen */
    ConfigReader (unsigned int=0, char ='#', char ='=') throw ();
    /** Destruktor */
    ~ConfigReader () throw ();

    /** Information aus Stream einfuegen 
	Arg1: Eingabestream
	Arg2: sollen rekursiv andere Dateien gelesen werden (+ -Direktiven)?
	Ret: konnte aus allen Dateien gelesen werden? */
    bool append_from_stream (std::istream&, bool =true) throw (std::bad_alloc);

    /** Information aus Datei einfuegen 
	Arg1: Dateiname
	Arg2: sollen rekursiv andere Dateien gelesen werden (+ -Direktiven)?
	Ret: konnte aus allen Dateien gelesen werden? */
    bool append_from_file (const char*, bool=true) throw (std::bad_alloc);

    /** Liste aller gelesenen Dateien */
    const std::deque<std::string>& list_of_sources () const throw ();

    // Informationen holen: arg1=key, arg2=ret-value, 
    // ret=sucess bzw. Groesse des gelesenen Array
    // ret=0, falls keine Zeile gefunden wurdem
    // ret<0, falls Konvertierung des Strings fehlgeschlagen
    int get (const char*, int&) const throw (std::bad_alloc);                  ///< int lesen
    int get (const char*, unsigned int&) const throw (std::bad_alloc);         ///< unsigned int lesen
    int get (const char*, long int&) const throw (std::bad_alloc);             ///< long int lesen
    int get (const char*, unsigned long int&) const throw (std::bad_alloc);    ///< unsigned long int lesen
    int get (const char*, double&) const throw (std::bad_alloc);               ///< double lesen
    int get (const char*, float&) const throw (std::bad_alloc);                ///< double lesen
    int get (const char*, char&) const throw (std::bad_alloc);                 ///< char lesen
    int get (const char*, bool&) const throw (std::bad_alloc);                 ///< bool lesen (1/0/true/false)
    int get (const char*, std::string&) const throw (std::bad_alloc);          ///< string (ein Wort) lesen

    int get (const char*, std::vector<int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<unsigned int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<long int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<unsigned long int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<double>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<float>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<char>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<bool>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<std::string>&) const throw (std::bad_alloc);

    void set (const char*, int) throw (std::bad_alloc);                  ///< int schreiben
    void set (const char*, unsigned int) throw (std::bad_alloc);         ///< unsigned int schreiben
    void set (const char*, long int) throw (std::bad_alloc);             ///< long int schreiben
    void set (const char*, unsigned long int) throw (std::bad_alloc);    ///< unsigned long int schreiben
    void set (const char*, double) throw (std::bad_alloc);               ///< double schreiben
    void set (const char*, float) throw (std::bad_alloc);                ///< double schreiben
    void set (const char*, char) throw (std::bad_alloc);                 ///< char schreiben
    void set (const char*, bool) throw (std::bad_alloc);                 ///< bool schreiben (true/false)
    void set (const char*, const std::string&) throw (std::bad_alloc);   ///< string (ein Wort) schreiben
    void set (const char*, const char*) throw (std::bad_alloc);          ///< string (ein Wort) schreiben

    void set (const char*, const std::vector<int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<unsigned int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<long int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<unsigned long int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<double>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<float>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<char>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<bool>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<std::string>&) throw (std::bad_alloc);

    /** einen Eintrag in einen Stream schreiben; liefert true bei Erfolg, d.h. wenn Schluessel gefunden */
    bool write (std::ostream&, const char*) const throw ();

    /** liest einen Konfigurationsstream und schreibt einen anderen, wobei abweichende Werte zu 
	gelesenen Schluesseln ersetzt werden. Nicht gelesene Schluessel werden nicht beruecksichtigt
	Arg1: Zielstream
	Arg2: Quellstream
	Arg3: sollen rekursiv andere Dateien gelesen werden (+ -Direktiven)?
	Ret: konnten alle Dateien gelesen und geschrieben werden? */
    bool ConfigReader::replace_config_stream (std::ostream&, std::istream&, bool =true) throw (std::bad_alloc);

    /** ueberschreibt eine Konfigurationsdatei. Die Quellatei wird in eine .bak-Datei umbenannt und
	eine neue Datei geschrieben. Zu den in der Quelldatei auftretenden Schluesseln werden die
	gespeicherten Werte ausgegeben. Nicht auftretende Schluessel werden ignoriert.
	Arg1: Name der Konfigurationsdatei
	Arg2: sollen rekursiv andere Dateien gelesen werden (+ -Direktiven)?
	Ret: konnten alle Dateien gelesen und geschrieben werden? */	
    bool ConfigReader::replace_config_file (const char*, bool =true) throw (std::bad_alloc);

  };

}

#endif


