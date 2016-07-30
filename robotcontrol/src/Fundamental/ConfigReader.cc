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


#include "ConfigReader.h"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstdio>

using namespace std;
using Tribots::ConfigReader;

// Schluessel-Wert-Paare:
struct Tribots::ConfigReader::KVPair {
  string key;
  string value;

  KVPair () {;}
  KVPair (const KVPair& kv) : key (kv.key), value (kv.value) {;}
  const KVPair& operator= (const KVPair& kv) {
    key=kv.key;
    value=kv.value;
    return *this;
  }
  bool operator== (const KVPair& kv) const throw () { return (key==kv.key); }
  bool operator< (const KVPair& kv) const throw () { return (key<kv.key); }
};


namespace {
  // einige Hilfsfunktionen:
  
  // suche nach dem naechsten Wort in einem String
  // arg1= Ausgangsstring, arg2: erste Position nach dem Wort, arg3: Position, ab der gesucht werden soll
  string next_word (const string& str, unsigned int& endpos, unsigned int firstpos) {
    unsigned int first_non_white=firstpos;
    if (str.length()<=firstpos) {
      endpos=firstpos;
      return string("");
    } else {
      endpos = firstpos+1;
      bool was_whitespace = (isspace (str [firstpos]));
      while (endpos<str.length()) {
	if (isspace(str [endpos])) {
	  if (!was_whitespace) {
	    return str.substr(first_non_white,endpos-first_non_white);
	  }
	} else {
	  if (was_whitespace) {
	    was_whitespace=false;
	    first_non_white=endpos;
	  }
	}
	endpos++;
      }
      // Stringende erreicht
      return (str.substr(first_non_white,endpos-first_non_white));
    }
  }

  // pruefen, ob string nur aus whitespace besteht
  bool is_whitespace (const string& s) {
    unsigned int n=s.length();
    for (unsigned int i=0; i<n; i++)
      if (!isspace(s[i]))
	return false;
    return true;
  }

  // Whitespace am Anfang und Ende eines String entfernen
  string remove_trailing_whitespace (const string& src) {
    int n=src.length();
    int first_character=0;
    int last_character=n-1;
    for (int i=0; i<n; i++)
      if (isspace (src[i]))
	first_character++;
      else
	break;
    for (int i=n-1; i>=0; i--)
      if (isspace (src[i]))
	last_character--;
      else
	break;
    if (first_character>last_character)
      return string("");
    return src.substr (first_character, last_character-first_character+1);
  }

  // in einer sortierten container<element> suchen
  // Arg1: Position des Wertes (oder wo der Wert eingefuegt werden muesste),
  // Arg2: Die deque
  // Arg3: Der Schluessel
  // Ret: Element gefunden?
  template<class C, class T>
  bool find_sorted (unsigned int& rv, const C& container, const T& element) {
    unsigned int right=container.size();
    unsigned int left=0;
    unsigned int searchpos;
    
    if (right==0) {
      // empty container
      rv=0;
      return false;
    }
    while (true) {
      if (left+1>=right) {
	if (container [left]==element) {
	  // element found
	  rv=left;
	  return true;
	} else {
	  // element not in container
	  if (container[left]<element)
	    rv=left+1;
	  else
	    rv=left;
	  return false;
	}
      } else {
	searchpos=(left+right)/2;   // left>right => searchpos<left => container[searchpos] exists
	if (element<container[searchpos]) {
	  // search on the left hand side of searchpos
	  right=searchpos;
	} else {
	  // search on the right hand side of searchpos
	  left=searchpos;
	}
      }
    }
  }

  void extract_path_from_filename (std::string& path, std::string& filename, const std::string& line) {
    if (line.length()==0) {
      filename = path = "";
      return;
    }
    unsigned int i=line.length()-1;
    while (i>0)
      if (line[i]=='/')
	break;
      else
	i--;
    if (line[i]=='/') {
      path = line.substr (0,i+1);
      filename = line.substr (i+1, line.length()-i-1);
      return;
    }
    path = "";
    filename = line;
    return;
  }

  string replace_all (const string& s1, const string& s2, const string& s3) { // durchsucht s1 sequentiell nach Vorkommen von s2 und ersetzt diese durch s3
    string res = s1;
    unsigned int pos = 0;
    while ((pos = res.find (s2, pos))<res.length()) {
      res.replace (pos, s2.length(), s3);
      pos+=s3.length();
    }
     return res;
  }
    
} // namespace


ConfigReader::ConfigReader (unsigned int vb, char cc, char ac) throw () : verbosity (vb), comment_char (cc), assign_char (ac) {
  directory_stack.push (string("./"));
  home_path = getenv ("HOME");
}

ConfigReader::~ConfigReader () throw () {;}

bool ConfigReader::replace_config_file (const char* name, bool recursive) throw (std::bad_alloc) {
  string cpath = directory_stack.top();
  string fpath, fname, npath;
  extract_path_from_filename (fpath, fname, name);
  if (fpath.length()>0 && fpath[0]=='/')
    npath = fpath;
  else
    npath = cpath+fpath;
  directory_stack.push (npath);
 
  string new_fname = npath+fname;
  string old_fname = new_fname+string(".bak");
  bool filesuccess = (rename (new_fname.c_str(), old_fname.c_str())==0);
  ifstream src (old_fname.c_str());
  ofstream dest (new_fname.c_str());
  filesuccess &= (src!=NULL);
  filesuccess &= (dest!=NULL);
  if (!filesuccess) {
    if (verbosity>=1)
      cerr << "ConfigReader Error: cannot handle files " << new_fname << " and " << old_fname << '\n';
    directory_stack.pop();
    dest.flush();
    return false;
  }
  if (verbosity>=2)
    cerr << "ConfigReader: copy from file " << old_fname << " to " << new_fname << '\n';
  bool res = replace_config_stream (dest, src, recursive);
  directory_stack.pop();
  dest.flush();
  return res;
}

bool ConfigReader::append_from_file (const char* name, bool recursive) throw (std::bad_alloc) {
  string cpath = directory_stack.top();
  string fpath, fname, npath;
  extract_path_from_filename (fpath, fname, name);
  if (fpath.length()>0 && fpath[0]=='/')
    npath = fpath;
  else
    npath = cpath+fpath;
  directory_stack.push (npath);
  files.push_back (npath+fname);

  ifstream stream ((npath+fname).c_str());
  if (!stream) {
    if (verbosity>=1)
      cerr << "ConfigReader Error: cannot open file " << npath << fname << '\n';
    directory_stack.pop();    
    return false;
  }
  if (verbosity>=2)
    cerr << "ConfigReader: read from file " << npath << fname << '\n';
  bool res = append_from_stream (stream, recursive);
  directory_stack.pop();
  return res;
}

bool ConfigReader::replace_config_stream (std::ostream& dest, std::istream& src, bool recursive) throw (std::bad_alloc) {
  bool success = true;
  string prefix = "";
  while (!src.eof()) {
    string oline;
    getline (src, oline);
    if (src.eof() && oline.length()==0)
      break;
    unsigned int p=oline.find (comment_char);
    unsigned int n=oline.length();
    string line = oline;
    string comment = "";
    if (p<n) {
      // Kommentare abschneiden
      line.erase (p, n-p);
      comment = oline.substr (p, n-p);
      n=p;
    }
    if (n==0) {
      dest << comment << '\n';
      continue;
    }  // leere Zeilen oder Kommentarzeilen
    if (line[0]=='+') {  // pruefen, ob eine weitere Konfigurationsdatei eingebunden werden soll
      unsigned int dont_care;
      string fname = replace_all(replace_all(next_word (line, dont_care, 1),string("$PATH"),directory_stack.top()),string("$HOME"),home_path);
      if (recursive)
	success &= replace_config_file (fname.c_str(), true);
      dest << oline << '\n';
    } else if (line[0]=='[') {  // pruefen, ob ein neuer Block definiert wird
      p=line.find (']');
      if (p<n) {
	if (p==1) {
	  prefix = "";
	  if (verbosity>=3)
	    cerr << "ConfigReader: read from empty block\n";
	} else {
	  prefix = line.substr (1,p-1)+string("::");
	  if (verbosity>=3)
	    cerr << "ConfigReader: read from block " << line.substr (1,p-1) << '\n';
	}
	dest << oline << '\n';
      } else {
	success=false;
	if (verbosity>=1)
	  cerr << "ConfigReader Error: invalid block definition " << line << '\n';
	dest << oline << '\n';
      }
    } else {  // normale Zeile auswerten, falls moeglich
      p=line.find (assign_char);
      if (p<n) {
	KVPair new_pair;
	string pure_key = remove_trailing_whitespace (line.substr (0,p));
	new_pair.key = prefix+pure_key;
	new_pair.value = replace_all (replace_all (remove_trailing_whitespace (line.substr (p+1, n-p-1)),string("$PATH"),directory_stack.top()), string("$HOME"),home_path);
	unsigned int pos;
	if (verbosity==3) 
	  cerr << "ConfigReader: read line " << new_pair.key << '\n';
	if (find_sorted (pos, key_value_table, new_pair)) {
	  if (key_value_table[pos].value==new_pair.value) {
	    dest << oline << '\n';
	    if (verbosity>=3)
	      cerr << "ConfigReader: keep line " << new_pair.key << " | " << new_pair.value << '\n';
	  } else {
	    dest << pure_key << '\t' << assign_char << ' ' << key_value_table[pos].value << (comment.length()>0 ? " " : "") << comment << '\n';
	    if (verbosity>=1)
	      cerr << "ConfigReader: replace line " << new_pair.key << " | " << new_pair.value << '\n';
	  }
	} else {
	  if (verbosity>=3)
	    cerr << "ConfigReader: keep line " << new_pair.key << " | " << new_pair.value << '\n';
	  dest << oline << '\n';
	}
      } else
	dest << oline << '\n';
    }
  }
  return success;
}

bool ConfigReader::append_from_stream (std::istream& stream, bool recursive) throw (std::bad_alloc) {
  bool success = true;
  string prefix = "";
  while (!stream.eof()) {
    string line;
    getline (stream, line);
    unsigned int p=line.find (comment_char);
    unsigned int n=line.length();
    if (p<n) {
      // Kommentare abschneiden
      line.erase (p, n-p);
      n=p;
    }
    if (n==0)
      continue;  // ueberspringe leere Zeilen

    if (line[0]=='+') {  // pruefen, ob eine weitere Konfigurationsdatei eingebunden werden soll
      unsigned int dont_care;
      string fname = replace_all(replace_all(next_word (line, dont_care, 1),string("$PATH"),directory_stack.top()),string("$HOME"),home_path);
      if (recursive)
	success &= append_from_file (fname.c_str(), true);
    } else if (line[0]=='[') {  // pruefen, ob ein neuer Block definiert wird
      p=line.find (']');
      if (p<n) {
	if (p==1) {
	  prefix = "";
	  if (verbosity>=3)
	    cerr << "ConfigReader: read from empty block\n";
	} else {
	  prefix = line.substr (1,p-1)+string("::");
	  if (verbosity>=3)
	    cerr << "ConfigReader: read from block " << line.substr (1,p-1) << '\n';
	}
      } else {
	success=false;
	if (verbosity>=1)
	  cerr << "ConfigReader Error: invalid block definition " << line << '\n';
      }
    } else {  // normale Zeile auswerten, falls moeglich
      p=line.find (assign_char);
      if (p<n) {
	KVPair new_pair;
	new_pair.key = prefix+remove_trailing_whitespace (line.substr (0,p));
	new_pair.value = replace_all(replace_all(remove_trailing_whitespace (line.substr (p+1, n-p-1)),string("$PATH"),directory_stack.top()),string("$HOME"),home_path);
	unsigned int pos;
	if (verbosity==3) 
	  cerr << "ConfigReader: read line " << new_pair.key << '\n';
	if (find_sorted (pos, key_value_table, new_pair)) {
	  key_value_table[pos]=new_pair;  // vorhandenen Eintrag ueberschreiben
	  if (verbosity>=4)
	    cerr << "ConfigReader: replace line " << new_pair.key << " | " << new_pair.value << '\n';
	} else {
	  if (verbosity>=4)
	    cerr << "ConfigReader: insert line " << new_pair.key << " | " << new_pair.value << '\n';
	  key_value_table.insert (key_value_table.begin()+pos, new_pair);
	}
      }
    }
  }
  return success;
}

bool ConfigReader::find_first (string& value, const string& key) const {
  unsigned int p;
  KVPair kv;
  kv.key=key;
  bool found = find_sorted (p, key_value_table, kv);
  if (found) {
    unsigned int dummy;
    value = next_word (key_value_table [p].value, dummy, 0);
  }
  if (!found && verbosity>=1)
    cerr << "ConfigReader Error: key " << key << " not found\n";
  return found;
}

bool ConfigReader::find_all (deque<string>& values, const string& key) const {
  values.clear();
  unsigned int p;
  KVPair kv;
  kv.key=key;
  bool found = find_sorted (p, key_value_table, kv);
  if (found) {
    unsigned int n = key_value_table [p].value.length();
    string word;
    unsigned int q=0;
    while (q<n) {
      word = next_word (key_value_table [p].value, q, q);
      if (!is_whitespace (word))
	values.push_back (word);
    }
  }
  if (!found && verbosity>=1)
    cerr << "ConfigReader Error: key " << key << " not found\n";
  return found;
}

namespace {
  
  // einige Konvertierungsfunktionen:
  bool seval (long int& d, const string& s) {
    char* end_char;
    d = strtol (s.c_str(), &end_char, 0);
    if ((*end_char)!='\0')
      return false;
    return true;
  }
  
  bool seval (unsigned long int& d, const string& s) {
    char* end_char;
    d = strtoul (s.c_str(), &end_char, 0);
    if ((*end_char)!='\0')
      return false;
    if ((s[0]=='-') && (d!=0))
      return false;
    return true;
  }

  bool seval (int& d, const string& s) {
    long int l;
    bool success = seval (l, s);
    d=l;
    return success;
  }

  bool seval (unsigned int& d, const string& s) {
    unsigned long int l;
    bool success = seval (l, s);
    d=l;
    return success;
  }

  bool seval (double& d, const string& s) {
    char* end_char;
    d = strtod (s.c_str(), &end_char);
    if ((*end_char)!='\0')
      return false;
    return true;
  }

  bool seval (float& d, const string& s) {
    double l;
    bool success = seval (l, s);
    d=l;
    return success;
  }

  bool seval (char& d, const string& s) {
    if (s.length()==0)
      return false;
    d=s[0];
    return (s.length()==1);
  }

  bool seval (bool& d, const string& s) {
    if (s==string("0")) {
      d=false;
      return true;
    }
    if (s==string("1")) {
      d=true;
      return true;
    }
    if (s==string("true")) {
      d=true;
      return true;
    }
    if (s==string("false")) {
      d=false;
      return true;
    }
    return false;
  }

  bool seval (string& d, const string& s) {
    unsigned int p=0;
    d=next_word (s, p, p);
    return true;
  }


  template<class T>
  int get_intern (const string& src, T& dest) {
    bool success = seval (dest, src);
    if (success)
      return 1;
    else
      return -1;
  }

  template<class U>
  int get_intern (const deque<string>& src, vector<U>& dest) {
    bool success = true;
    int n=src.size();
    dest.resize (n);
    for (int i=0; i<n; i++) 
      success &= seval (dest[i], src[i]);
    if (success)
      return n;
    else
      return -n;
  }
}

int ConfigReader::get (const char* key, int& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, unsigned int& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, long int& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, unsigned long int& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, double& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, float& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, char& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, bool& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::string& value) const throw (std::bad_alloc) {
  string sv;
  if (!find_first (sv, key)) 
    return 0;
  return get_intern (sv, value);
}


int ConfigReader::get (const char* key, std::vector<int>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<unsigned int>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<long int>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<unsigned long int>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<double>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<float>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<char>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<string>& value) const throw (std::bad_alloc) {
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<bool>& value) const throw (std::bad_alloc) {
  // Sonderbehandlung, da value<bool> anders gespeichert wird als value<T>
  deque<string> sv;
  if (!find_all (sv, key))
    return 0;
  bool success = true;
  int n=sv.size();
  value.resize (n);
  for (int i=0; i<n; i++) {
    bool b;
    success = success && seval (b, sv[i]);
    value[i]=b;
  }
  if (success)
    return n;
  else
    return -n;
}

bool ConfigReader::write (std::ostream& os, const char* key) const throw () {
  unsigned int p;
  KVPair kv;
  kv.key=key;
  bool found = find_sorted (p, key_value_table, kv);
  if (found) {
    os << key << ' ' << assign_char << ' ' << key_value_table[p].value << '\n';
  }
  if (!found && verbosity>=1)
    cerr << "ConfigReader Error: key " << key << " not found\n";
  return found;
}

void ConfigReader::set (const char* key, const std::string& value) throw (std::bad_alloc) {
  unsigned int p;
  KVPair kv;
  kv.key=key;
  bool found = find_sorted (p, key_value_table, kv);
  if (found)
    key_value_table[p].value=value;   // ersetzen eines bestehenden Wertes
  else {
    kv.value=value;
    key_value_table.insert (key_value_table.begin()+p, kv);  // neuen Wert einfuegen
  }
}

void ConfigReader::set (const char* key, const char* value) throw (std::bad_alloc) {
  string s (value);
  set (key, s);
}

void ConfigReader::set (const char* key, int value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, unsigned int value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, long int value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, unsigned long int value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, double value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, float value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  io << value;
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, char value) throw (std::bad_alloc) {
  string sval = " ";
  sval[0]=value;
  set (key, sval);
}

void ConfigReader::set (const char* key, bool value) throw (std::bad_alloc) {
  string sval = ( value ? "true" : "false" );
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<int>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<unsigned int>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<long int>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<unsigned long int>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<double>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<float>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<char>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << value[i] << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<bool>& value) throw (std::bad_alloc) {
  stringstream io;
  string sval;
  for (unsigned int i=0; i<value.size(); i++)
    io << (value[i] ? "true" : "false" ) << ' ';
  getline (io, sval);
  set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<std::string>& value) throw (std::bad_alloc) {
  string sval = "";
  for (unsigned int i=0; i<value.size(); i++)
    sval += value[i] + ' ';
  set (key, sval);
}

const std::deque<std::string>& ConfigReader::list_of_sources () const throw () {
  return files;
}
