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



#include <iostream>

#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include "TribotsSyntax.h"

TribotsSyntax::TribotsSyntax(QTextEdit *textEdit) 
  : QSyntaxHighlighter(textEdit) {
  return;
}

TribotsSyntax::~TribotsSyntax(void) {
  return;
}

int TribotsSyntax::highlightParagraph(const QString &text, 
                                          int endStateOfLastPara) {
  
  int start=0;
  QStringList lines = QStringList::split(';', text, false);
  for(unsigned int i=0; i<lines.count(); i++) {
    int commandLength = 0;

    QString line = lines[i];
    int lineLength = line.length() + 1;

    if(line.isNull() || line.isEmpty()) {
      std::cout << "is null or empty" << std::endl;
      setFormat(start, lineLength, QColor(255, 0, 0));
      start += lineLength;
      continue;
    }

    for(int i=0; i<lineLength-1; i++) {
      QChar a = line[i];
      
      if(a == "#") {
        setFormat(start, lineLength, QColor(150, 150, 150));
      
	break;
      } 
      
      if(a == "=") {
        setFormat(start, lineLength, QColor(0, 0, 155));
      
	break;
      } 
      
      if(a == "[") {
        setFormat(start, lineLength, QColor(155, 0, 0));
      
	break;
      }
      if(a == "+") {
        setFormat(start, lineLength, QColor(0,155, 0));
      
	break;
      }  
      }
      
      
    
}       

  return 0;
}

