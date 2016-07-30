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

#include "tribotsUdpCommunicationBuffer.h"
#include "../Structures/Journal.h"
#include <iostream>

Tribots::TribotsUdpCommunicationBuffer::TribotsUdpCommunicationBuffer(unsigned int eintraege)
{
	versionsnummer_uebertragungen=10;
	
	max_eintraege=eintraege;
	if (max_eintraege>MAX_NUTZDATEN) max_eintraege=MAX_NUTZDATEN;
	
	for (unsigned int i=0;i<max_eintraege;i++)
	{
		puffer[i] = new struct eintraege;
		puffer[i]->art=art_leer;
		puffer[i]->paketnummer=0;
		puffer[i]->checksum=0;
		puffer[i]->nutzdatenlaenge=0;
		puffer[i]->prio=prio_normal;
		puffer[i]->wiederholungen=0;
	}
	
	e_send_buffer_full=0;
	e_put_to_buffer_full=0;
	e_put_to_buffer_tobig=0;
	e_set_receive_buffer=0;
	e_set_receive_buffer_tobig=0;
	e_get_next_buffer_tobig=0;
	e_receive_unknow_data=0;
	e_receive_data_checksum=0;
	w_receive_to_old=0;
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_	
	stat_data_send=0; // Anzahl der gesendeten Daten in Byte
	stat_data_receive=0;
	stat_max_nutzdatenlaenge_send=0;
	stat_max_nutzdatenlaenge_send_art=' ';
	stat_max_bufferlaenge_send=0;
	stat_data_send_prio[0]=0;
	stat_data_send_prio[1]=0;
	stat_data_send_prio[2]=0;	
#endif
	
	fortlaufende_paketnummer=0;
	fortlaufende_paketnummer_remote=0;
	
	struct eintraege {
		// Daten die versendet werden
		unsigned char art; // eingang/ausgang 
		unsigned int paketnummer;
		unsigned char checksum;
		unsigned int nutzdatenlaenge;
		char nutzdaten[MAX_NUTZDATENLAENGE];
		// Interne Daten
		unsigned char prio;
		// 0 wichtig
		// 1 normale
		// 2 optional
		unsigned int wiederholungen; // anzahl der versuche, bis es verworfen wird
	};
}

Tribots::TribotsUdpCommunicationBuffer::~TribotsUdpCommunicationBuffer()
{
	log_status();

	for (unsigned int i=0;i<max_eintraege;i++)
	{
		delete (puffer[i]);
	}

}

void Tribots::TribotsUdpCommunicationBuffer::print_hex(ostream * output,char * src,unsigned int size)
{
//	unsigned short _tmp;
	unsigned char _tmp_wert;
	for (unsigned int i=0;i<size;i++)
	{
//		_tmp=0;
//		_tmp=(unsigned short)src[i];
		_tmp_wert=(unsigned char)src[i];
		switch (_tmp_wert/16)
		{
			case 0:
				*output << "0"; break;
			case 1:
				*output << "1"; break;
			case 2:
				*output << "2"; break;
			case 3:
				*output << "3"; break;
			case 4:
				*output << "4"; break;
			case 5:
				*output << "5"; break;
			case 6:
				*output << "6"; break;
			case 7:
				*output << "7"; break;
			case 8:
				*output << "8"; break;
			case 9:
				*output << "9"; break;
			case 10:
				*output << "A"; break;
			case 11:
				*output << "B"; break;
			case 12:
				*output << "C"; break;
			case 13:
				*output << "D"; break;
			case 14:
				*output << "E"; break;
			case 15:
				*output << "F"; break;
		}
		switch (_tmp_wert%16)
		{
			case 0:
				*output << "0"; break;
			case 1:
				*output << "1"; break;
			case 2:
				*output << "2"; break;
			case 3:
				*output << "3"; break;
			case 4:
				*output << "4"; break;
			case 5:
				*output << "5"; break;
			case 6:
				*output << "6"; break;
			case 7:
				*output << "7"; break;
			case 8:
				*output << "8"; break;
			case 9:
				*output << "9"; break;
			case 10:
				*output << "A"; break;
			case 11:
				*output << "B"; break;
			case 12:
				*output << "C"; break;
			case 13:
				*output << "D"; break;
			case 14:
				*output << "E"; break;
			case 15:
				*output << "F"; break;
		}
				
		*output <<"|";
	}
	*output << endl;
}

void Tribots::TribotsUdpCommunicationBuffer::print_inhalt(char * src,unsigned int size,unsigned short art)
{
//	unsigned short _tmp;
	unsigned char _tmp_wert;
/*
  static const char PingTag            = 'a';
  static const char RequestSettingsTag = 'b';
  static const char RemoteCtrTag       = 'c';
  static const char GameStateTag       = 'd';
  static const char OwnHalfTag         = 'e';
  static const char FieldGeometryTag   = 'f';
  static const char BallLocationTag    = 'g';
  static const char RobotPropertiesTag = 'h';
  static const char RobotLocationTag   = 'i';
  static const char PlayerTypeTag      = 'j';
  static const char RobotDataTag       = 'k';
  static const char XmlStringTag       = 'x';
  */
	if (src[0]=='x' && ( src[8]=='P' || src[9]=='P'))
	 {
		stringstream output; 
		if (art==1)
		{
			output << "Senden";
		} else
		{
			output << "Empfangen";
		}
		output << endl << "communication tag:'";
		switch (src[0])
		{
			case 'a':
				output << "PingTag"; break;
			case 'b':
				output << "RequestSettingsTag"; break;
			case 'c':
				output << "RemoteCtrTag"; break;
			case 'd':
				output << "GameStateTag"; break;
			case 'e':
				output << "OwnHalfTag"; break;
			case 'f':
				output << "FieldGeometryTag"; break;
			case 'g':
				output << "BallLocationTag"; break;
			case 'h':
				output << "RobotPropertiesTag"; break;
			case 'i':
				output << "RobotLocationTag"; break;
			case 'j':
				output << "PlayerTypeTag"; break;
			case 'k':
				output << "RobotDataTag"; break;
			case 'x':
				output << "XmlStringTag"; break;
			
		}
		output << "'" << endl << "Inhalt(" << size-1 << "byte): >>";
		
		for (unsigned int i=1;i<size;i++)
		{
	//		_tmp=0;
	//		_tmp=(unsigned short)src[i];
			_tmp_wert=(unsigned char)src[i];
			if (_tmp_wert>32 && _tmp_wert<122)
			{
				output  << src[i];
			} else
			{
				output <<"|";
				switch (_tmp_wert/16)
				{
					case 0:
						output << "0"; break;
					case 1:
						output << "1"; break;
					case 2:
						output << "2"; break;
					case 3:
						output << "3"; break;
					case 4:
						output << "4"; break;
					case 5:
						output << "5"; break;
					case 6:
						output << "6"; break;
					case 7:
						output << "7"; break;
					case 8:
						output << "8"; break;
					case 9:
						output << "9"; break;
					case 10:
						output << "A"; break;
					case 11:
						output << "B"; break;
					case 12:
						output << "C"; break;
					case 13:
						output << "D"; break;
					case 14:
						output << "E"; break;
					case 15:
						output << "F"; break;
				}
				switch (_tmp_wert%16)
				{
					case 0:
						output << "0"; break;
					case 1:
						output << "1"; break;
					case 2:
						output << "2"; break;
					case 3:
						output << "3"; break;
					case 4:
						output << "4"; break;
					case 5:
						output << "5"; break;
					case 6:
						output << "6"; break;
					case 7:
						output << "7"; break;
					case 8:
						output << "8"; break;
					case 9:
						output << "9"; break;
					case 10:
						output << "A"; break;
					case 11:
						output << "B"; break;
					case 12:
						output << "C"; break;
					case 13:
						output << "D"; break;
					case 14:
						output << "E"; break;
					case 15:
						output << "F"; break;
				}
			output <<"|";
			}
		}
		output << "<<" << endl;
		JMESSAGE(output.str().c_str()); 
	}
	
}

void Tribots::TribotsUdpCommunicationBuffer::log_status()
{
	unsigned int _ausgang=0;
	unsigned int _eingang=0;
	unsigned int _wichtig=0;
	unsigned int _normal=0;
	unsigned int _optional=0;
	unsigned int _frei=0;
	for (unsigned int _pos=0;_pos<max_eintraege;_pos++)
	{
		if (puffer[_pos]->art!=art_leer)
		{
			if (puffer[_pos]->art<=10) _ausgang++; else _eingang++;
			if (puffer[_pos]->prio==prio_wichtig) _wichtig++;
			if (puffer[_pos]->prio==prio_normal) _normal++;
			if (puffer[_pos]->prio==prio_optional) _optional++;
		} else _frei++;
	}
	
	stringstream _output;
	_output << "Status vom CommunicationBuffer :" << endl;
	_output << "e_send_buffer_full             :" << e_send_buffer_full << endl;
	_output << "e_put_to_buffer_full           :" << e_put_to_buffer_full << endl;
	_output << "e_put_to_buffer_tobig          :" << e_put_to_buffer_tobig << endl;
//	_output << "e_get_receive_buffer_dest_full :" << e_get_receive_buffer_dest_full << endl;
	_output << "e_set_receive_buffer           :" << e_set_receive_buffer << endl;
	_output << "e_set_receive_buffer_tobig     :" << e_set_receive_buffer_tobig << endl;
	_output << "e_get_next_buffer_tobig        :" << e_get_next_buffer_tobig << endl;
	_output << "e_receive_unknow_data          :" << e_receive_unknow_data << endl;
	_output << "e_receive_data_checksum        :" << e_receive_data_checksum << endl;
	_output << "w_receive_to_old       :" << w_receive_to_old << endl;
	_output << endl;	
	_output << "fortlaufende_paketnummer       :" << fortlaufende_paketnummer << endl;
	_output << "fortlaufende_paketnummer_remote:" << fortlaufende_paketnummer_remote << endl;
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_	
	_output << "stat_data_send (byte)          :" << stat_data_send << endl;
	_output << "stat_data_receive (byte)       :" << stat_data_receive << endl;
	_output << "stat_max_nutzdatenlaenge_send  :" << stat_max_nutzdatenlaenge_send << endl;
	_output << "stat_max_nutz..laenge_send_art :'" << stat_max_nutzdatenlaenge_send_art << "'" << endl;
	_output << "stat_max_bufferlaenge_send     :" << stat_max_bufferlaenge_send << endl;
	_output << "stat_data_send_prio_wichtig    :" << stat_data_send_prio[0] << endl;
	_output << "stat_data_send_prio_normal     :" << stat_data_send_prio[1] << endl;
	_output << "stat_data_send_prio_optional   :" << stat_data_send_prio[2] << endl;
#endif
	_output << endl;	
	_output << "pakete im _ausgang             :" << _ausgang << endl;
	_output << "pakete im _eingang             :" << _eingang << endl;
	_output << "plaetze die _frei sind         :" << _frei << endl;
	_output << "_wichtig                       :" << _wichtig << endl;
	_output << "_normal                        :" << _normal << endl;
	_output << "_optional                      :" << _optional << endl;
	
	_output << endl;	
	JERROR(_output.str().c_str());
}

unsigned int Tribots::TribotsUdpCommunicationBuffer::find_next_art(unsigned char art)
{
	unsigned int _pos=0;
	while (_pos<max_eintraege && puffer[_pos]->art!=art)
	{
		_pos++;
	}
	return _pos;
}

unsigned char Tribots::TribotsUdpCommunicationBuffer::checksum(char * src,unsigned int size)
{
	unsigned char _result=170;
	for (unsigned int _pos=0;_pos<size;_pos++)
	{
		_result^=src[_pos];
	}
	return _result;
}

bool Tribots::TribotsUdpCommunicationBuffer::put_to_buffer(char * src,unsigned int size,bool rueckschein,unsigned char prio)
{
//log_status();
	// Suche freie Position
	unsigned int _pos;
/*	
	stringstream _output;
	_output << "put_to_buffer(src," << size<<","<< (unsigned short) signal<<")"<<endl;
	_output << " pos:" << _pos << endl;;
	print_hex(&_output,src,size);
//	JERROR(_output.str().c_str());
*/
	
	_pos=find_next_art(art_leer);
	if (_pos>=max_eintraege)
	{
		e_put_to_buffer_full++;
		return false; // Puffer voll
	}
	if (size>MAX_NUTZDATENLAENGE)
	{
		e_put_to_buffer_tobig++;
		return false; // Nutzdaten zu lang
	}
//std::cerr << "put_to_buffer() size:" << size << " _pos:" << _pos << " nutzdatenlaenge:" << size << endl;

	memcpy(&puffer[_pos]->nutzdaten[0], src,size);
		puffer[_pos]->nutzdatenlaenge=size;

#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
		if (size>stat_max_nutzdatenlaenge_send)
		{
			stat_max_nutzdatenlaenge_send=size;
			stat_max_nutzdatenlaenge_send_art=src[0];
		}
#endif
		
	puffer[_pos]->checksum=checksum(src,size);

	if (rueckschein==true)
	{
		puffer[_pos]->art=art_ausgang_rueckschein;
		puffer[_pos]->wiederholungen=_SENDEVERSUCHE_;
	} else
	{
		puffer[_pos]->art=art_ausgang_normal;
		puffer[_pos]->wiederholungen=0;
	}

	if (prio<=prio_optional) puffer[_pos]->prio=prio; else puffer[_pos]->prio=prio_optional;
	
	puffer[_pos]->paketnummer=fortlaufende_paketnummer;
		fortlaufende_paketnummer++;
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
	stat_data_send_prio[puffer[_pos]->prio]++;
#endif
/*	
	if (puffer[_pos]->prio<=2)
	{  
		stringstream _output;
		_output << "put_to_buffer(src,size=" << size<<",prio="<< (unsigned short) prio<<")"<<endl;
		if (puffer[_pos]->nutzdaten[0] == 'a') _output << "PingTag";
		if (puffer[_pos]->nutzdaten[0] == 'b') _output << "RequestSettingsTag";
		if (puffer[_pos]->nutzdaten[0] == 'c') _output << "RemoteCtrTag";
		if (puffer[_pos]->nutzdaten[0] == 'd') _output << "GameStateTag";
		if (puffer[_pos]->nutzdaten[0] == 'e') _output << "OwnHalfTag";
		if (puffer[_pos]->nutzdaten[0] == 'f') _output << "FieldGeometryTag";
		if (puffer[_pos]->nutzdaten[0] == 'g') _output << "BallLocationTag";
		if (puffer[_pos]->nutzdaten[0] == 'h') _output << "RobotPropertiesTag";
		if (puffer[_pos]->nutzdaten[0] == 'i') _output << "RobotLocationTag";
		if (puffer[_pos]->nutzdaten[0] == 'j') _output << "PlayerTypeTag";
		if (puffer[_pos]->nutzdaten[0] == 'k') _output << "RobotDataTag";
		if (puffer[_pos]->nutzdaten[0] == 'x') _output << "XmlStringTag";
		if (rueckschein) _output << " mit rueckschein ";
		_output << "(" << puffer[_pos]->nutzdaten[0] << ")" << endl;
		JWARNING(_output.str().c_str());
	}
*/
	
//	if (fortlaufende_paketnummer%200==0) log_status(); //DEBUG
//	print_inhalt(src,size,1); // DEBUG
	return true;
}


void Tribots::TribotsUdpCommunicationBuffer::put_to_buffer_rueckscheinantwort(unsigned int paketnummer,unsigned char checksum,unsigned int nutzdatenlaenge)
{
	//std::cerr << "put_to_buffer_rueckscheinantwort" << paketnummer << endl;
	unsigned int _pos;
	_pos=find_next_art(art_leer);
	if (_pos<max_eintraege)
	{
		puffer[_pos]->art=art_ausgang_rueckscheinantwort;
		puffer[_pos]->paketnummer=paketnummer;
		puffer[_pos]->checksum=checksum;
		puffer[_pos]->nutzdatenlaenge=nutzdatenlaenge;
		puffer[_pos]->prio=prio_wichtig;
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
		stat_data_send_prio[puffer[_pos]->prio]++;
#endif
		puffer[_pos]->wiederholungen=0;
	} else
	{
		e_put_to_buffer_full++;
	}
}

void Tribots::TribotsUdpCommunicationBuffer::del_from_buffer_rueckschein(unsigned int paketnummer,unsigned char checksum,unsigned int nutzdatenlaenge)
{
//	std::cerr << "del_from_buffer_rueckschein" << paketnummer; // DEBUG
	for (unsigned int _pos=0;_pos<max_eintraege;_pos++)
	{
		if (	puffer[_pos]->art==art_ausgang_rueckschein && 
			puffer[_pos]->paketnummer==paketnummer &&
			puffer[_pos]->checksum==checksum &&
			puffer[_pos]->nutzdatenlaenge==nutzdatenlaenge )
		{
			puffer[_pos]->wiederholungen=0;
			puffer[_pos]->art=art_leer;
			//std::cerr << "gefunden"; // DEBUG

		}
	}
	//std::cerr << endl; // DEBUG
}


unsigned int Tribots::TribotsUdpCommunicationBuffer::get_send_buffer(char * dest,unsigned int maxsize, unsigned int version)
{
//log_status();
//std::cerr << "get_send_buffer() maxsize:" << maxsize << endl;
// Den zu sendenden Puffer zusammenbauen
//	stringstream _output;
//	_output << "get_send_buffer(dest," << maxsize<<")"<<endl;
	
	unsigned int _dest_pos=0;
	
	unsigned int _size_head;
		_size_head=sizeof(puffer[0]->art);
		_size_head+=sizeof(puffer[0]->paketnummer);
		_size_head+=sizeof(puffer[0]->checksum);
		_size_head+=sizeof(puffer[0]->nutzdatenlaenge);
	
	char * _ziel=dest;
	
	// Versionsnummer mit versenden.
	if (versionsnummer_uebertragungen>0)
	{
		versionsnummer_uebertragungen--;
		eintraege _tmp_eintrag;
			_tmp_eintrag.art=art_version_info;
			_tmp_eintrag.paketnummer=0;
			_tmp_eintrag.checksum=0;
			_tmp_eintrag.nutzdatenlaenge=version;
		
		if (_dest_pos+_size_head<maxsize)
		{
			memcpy(_ziel,&_tmp_eintrag.art,sizeof(_tmp_eintrag.art));
				_dest_pos+=sizeof(_tmp_eintrag.art);
				_ziel=dest+_dest_pos;
				
			memcpy(_ziel,&_tmp_eintrag.paketnummer,sizeof(_tmp_eintrag.paketnummer));
				_dest_pos+=sizeof(_tmp_eintrag.paketnummer);
				_ziel=dest+_dest_pos;
				
			memcpy(_ziel,&_tmp_eintrag.checksum,sizeof(_tmp_eintrag.checksum));
				_dest_pos+=sizeof(_tmp_eintrag.checksum);
				_ziel=dest+_dest_pos;
				
			memcpy(_ziel,&_tmp_eintrag.nutzdatenlaenge,sizeof(_tmp_eintrag.nutzdatenlaenge));
				_dest_pos+=sizeof(_tmp_eintrag.nutzdatenlaenge);
				_ziel=dest+_dest_pos;
	
		} else e_send_buffer_full++; // Fehler zaehlen
	}
		
	// Inhalte mit groesserer Wichtigkeit zuerst in den Puffer legen.
	for (unsigned char _prio=prio_wichtig;_prio<=prio_optional;_prio++)
	for (unsigned int _pos=0;_pos<max_eintraege;_pos++)
	{
		if (_prio==puffer[_pos]->prio)
		{
			if (puffer[_pos]->art==art_ausgang_normal || puffer[_pos]->art==art_ausgang_rueckschein)
			{// normale Postausgang-Daten in den Puffer legen.
				if (_dest_pos+_size_head+puffer[_pos]->nutzdatenlaenge<maxsize)
				{
/*					std::cerr << "copy" <<
						"  puffer["<< _pos << "]->prio:" << (unsigned short)puffer[_pos]->prio << 
						"  _prio:" << (unsigned short)_prio << endl; //DEBUG
*/	
					memcpy(_ziel,&puffer[_pos]->art,sizeof(puffer[_pos]->art));
						_dest_pos+=sizeof(puffer[_pos]->art);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->paketnummer,sizeof(puffer[_pos]->paketnummer));
						_dest_pos+=sizeof(puffer[_pos]->paketnummer);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->checksum,sizeof(puffer[_pos]->checksum));
						_dest_pos+=sizeof(puffer[_pos]->checksum);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->nutzdatenlaenge,sizeof(puffer[_pos]->nutzdatenlaenge));
						_dest_pos+=sizeof(puffer[_pos]->nutzdatenlaenge);
						_ziel=dest+_dest_pos;
					
					//Nutzdaten kopieren
					memcpy(_ziel,&puffer[_pos]->nutzdaten[0],puffer[_pos]->nutzdatenlaenge);
						_dest_pos+=puffer[_pos]->nutzdatenlaenge;
						_ziel=dest+_dest_pos;
					
					// Versendete Nachricht sofort loeschen?
					if (puffer[_pos]->art==art_ausgang_rueckschein && puffer[_pos]->wiederholungen>0)
						puffer[_pos]->wiederholungen--; // Nachricht behalten, bis timeout oder rueckantwort
					else	puffer[_pos]->art=art_leer; // Versendete Nachricht loeschen
					
				} else e_send_buffer_full++; // Fehler zaehlen
			} else if (puffer[_pos]->art==art_ausgang_rueckscheinantwort) 
			{// rueckscheinantwort in den Puffer legen.
				if (_dest_pos+_size_head<maxsize)
				{
					memcpy(_ziel,&puffer[_pos]->art,sizeof(puffer[_pos]->art));
						_dest_pos+=sizeof(puffer[_pos]->art);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->paketnummer,sizeof(puffer[_pos]->paketnummer));
						_dest_pos+=sizeof(puffer[_pos]->paketnummer);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->checksum,sizeof(puffer[_pos]->checksum));
						_dest_pos+=sizeof(puffer[_pos]->checksum);
						_ziel=dest+_dest_pos;
						
					memcpy(_ziel,&puffer[_pos]->nutzdatenlaenge,sizeof(puffer[_pos]->nutzdatenlaenge));
						_dest_pos+=sizeof(puffer[_pos]->nutzdatenlaenge);
						_ziel=dest+_dest_pos;
						
					puffer[_pos]->art=art_leer;  // Versendete Nachricht loeschen
					
				} else e_send_buffer_full++; // Fehler zaehlen
			}
		}
	}
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
	if (_dest_pos>stat_max_bufferlaenge_send) stat_max_bufferlaenge_send=_dest_pos;
	stat_data_send+=_dest_pos; // Ausgehende Bytes zaehlen.
#endif

/*	
stringstream _output;
print_hex(&_output,dest,_dest_pos);	
JERROR(_output.str().c_str());
*/
	return _dest_pos;
}

bool Tribots::TribotsUdpCommunicationBuffer::set_receive_buffer(char * src,unsigned int src_size,unsigned int version)
{  // Nimmt die empfangenen Daten entgegen und speichert sie Intern
	unsigned int _src_pos=0;
	unsigned int _eintrag_pos;
	eintraege _tmp_eintrag;
	
	unsigned int _size_head;
		_size_head=sizeof(puffer[0]->art);
		_size_head+=sizeof(puffer[0]->paketnummer);
		_size_head+=sizeof(puffer[0]->checksum);
		_size_head+=sizeof(puffer[0]->nutzdatenlaenge);
	
	char * _quelle=src+_src_pos;

	while (_src_pos+_size_head<=src_size)
	{ // genug Daten vorhanden?
		//JERROR("while (_src_pos+_size_head<src_size)");
		memcpy(&_tmp_eintrag.art,_quelle,sizeof(_tmp_eintrag.art));
			_src_pos+=sizeof(_tmp_eintrag.art);
			_quelle=src+_src_pos;
			
		memcpy(&_tmp_eintrag.paketnummer,_quelle,sizeof(_tmp_eintrag.paketnummer));
			_src_pos+=sizeof(_tmp_eintrag.paketnummer);
			_quelle=src+_src_pos;
			
		memcpy(&_tmp_eintrag.checksum,_quelle,sizeof(_tmp_eintrag.checksum));
			_src_pos+=sizeof(_tmp_eintrag.checksum);
			_quelle=src+_src_pos;
			
		memcpy(&_tmp_eintrag.nutzdatenlaenge,_quelle,sizeof(_tmp_eintrag.nutzdatenlaenge));
			_src_pos+=sizeof(_tmp_eintrag.nutzdatenlaenge);
			_quelle=src+_src_pos;
		
		if (_tmp_eintrag.art==art_ausgang_normal || _tmp_eintrag.art==art_ausgang_rueckschein)
		{
			//JERROR("_tmp_eintrag.art==art_ausgang_normal");
			// Erstmal nen freien Platz suchen
			_eintrag_pos=find_next_art(art_leer);
			if (_eintrag_pos>=max_eintraege) { e_put_to_buffer_full++; return false; }
			
			// Schauen ob die Daten in den freien Platz passen
			if (_src_pos+_tmp_eintrag.nutzdatenlaenge<=src_size)
			{ // genug Daten vorhanden?
				//JERROR("_src_pos+_tmp_eintrag.nutzdatenlaenge<src_size");
				puffer[_eintrag_pos]->art=art_eingang_normal; // Neues Paket
				puffer[_eintrag_pos]->paketnummer=_tmp_eintrag.paketnummer;
				puffer[_eintrag_pos]->checksum=_tmp_eintrag.checksum;
				puffer[_eintrag_pos]->nutzdatenlaenge=_tmp_eintrag.nutzdatenlaenge;
				
				if (_tmp_eintrag.nutzdatenlaenge<=MAX_NUTZDATENLAENGE)
				{ // nutzdatenlaenge im erlaubten bereich?
					//JERROR("_tmp_eintrag.nutzdatenlaenge<=MAX_NUTZDATENLAENGE");
					memcpy(&puffer[_eintrag_pos]->nutzdaten[0],_quelle,puffer[_eintrag_pos]->nutzdatenlaenge);
						//print_inhalt(_quelle,puffer[_eintrag_pos]->nutzdatenlaenge,2); // DEBUG
						_src_pos+=puffer[_eintrag_pos]->nutzdatenlaenge;
						_quelle=src+_src_pos;
						
						
					// Debug
					/*
					stringstream _output;
					_output << "Empfangenes Paket wird im Posteingang gespeichert:" << endl;
					_output << "pos:" << _eintrag_pos;
					_output << " art:" << (unsigned short)puffer[_eintrag_pos]->art;
					_output << " paketnummer:" << puffer[_eintrag_pos]->paketnummer;
					_output << " checksum:" << (unsigned short)puffer[_eintrag_pos]->checksum;
					_output << " nutzdatenlaenge:" << puffer[_eintrag_pos]->nutzdatenlaenge << endl;
					print_hex(&_output,&puffer[_eintrag_pos]->nutzdaten[0],puffer[_eintrag_pos]->nutzdatenlaenge);
					_output << endl;
					JWARNING(_output.str().c_str());
					*/
				} else { e_set_receive_buffer_tobig++; return false; }
				
				// Alte Daten angekommen? Wegwerfen, weil es koennten alte REFBOX sachen sein.
				if (_tmp_eintrag.paketnummer<fortlaufende_paketnummer_remote)
				 {
				 	//FIXME
					//puffer[_eintrag_pos]->art=art_leer; // Paket wieder loeschen
					w_receive_to_old++;
				 } else
				 { // annette 0160 2422498
				 	fortlaufende_paketnummer_remote=_tmp_eintrag.paketnummer;
				 }
				
				// Stimmt die checksumme mit den Daten ueberein?
				if (puffer[_eintrag_pos]->checksum !=checksum(&puffer[_eintrag_pos]->nutzdaten[0],puffer[_eintrag_pos]->nutzdatenlaenge))
				{
					std::cerr << "  set_receive_buffer: Datenstueck mit falscher checksum.  " << endl;
					puffer[_eintrag_pos]->art=art_leer; // Paket wieder loeschen
					e_receive_data_checksum++;
				} else 
				{ // Daten Korrekt
					if (_tmp_eintrag.art==art_ausgang_rueckschein)
					{	// Das emfangene Datenstueck bestaetigen
						put_to_buffer_rueckscheinantwort(
							puffer[_eintrag_pos]->paketnummer,
							puffer[_eintrag_pos]->checksum,
							puffer[_eintrag_pos]->nutzdatenlaenge);
					}
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
					stat_data_receive+=_size_head+puffer[_eintrag_pos]->nutzdatenlaenge;
#endif
				}
			} else
			{ // Daten abgeschnitten?
				stringstream _output;
				_output << "Daten abgeschnitten?" << endl;
				_output << "_src_pos+_tmp_eintrag.nutzdatenlaenge<=src_size" << endl;
				_output << _src_pos << "+" << _tmp_eintrag.nutzdatenlaenge << " <=" << src_size << endl;
				_output << "restlaenge:" << src_size-_src_pos << endl;
				_output << endl;	
				JERROR(_output.str().c_str());
				print_inhalt(_quelle,src_size-_src_pos,2);

				e_set_receive_buffer++;
				return false;
			}
		} else if (_tmp_eintrag.art==art_ausgang_rueckscheinantwort)
		{ // Erhaltene rueckscheinantwort verarbeiten: Das wiederholte senden anhalten
			 del_from_buffer_rueckschein(_tmp_eintrag.paketnummer,_tmp_eintrag.checksum,_tmp_eintrag.nutzdatenlaenge);
		} else if (_tmp_eintrag.art==art_version_info)
		{
			if (_tmp_eintrag.nutzdatenlaenge!=version)
			{
				stringstream _output;
				_output << "Versionsfehler in der Kommunikation! EigeneVersion(" << version << ")!=(" << _tmp_eintrag.nutzdatenlaenge << ")" << endl;
				JERROR(_output.str().c_str());
				cerr <<_output.str().c_str() ; cerr.flush();
			}
		
		} else e_receive_unknow_data++; // Komischer Inhalt.
	
	}
//	log_status();
	return (_src_pos==src_size);
}

unsigned int Tribots::TribotsUdpCommunicationBuffer::get_next_buffer(char * dest,unsigned int dest_maxsize)
{
	unsigned int _pos;
	_pos=find_next_art(art_eingang_normal);
	if (_pos>=max_eintraege) return 0; // kein Daten im Eingang gefunden
	
	if (puffer[_pos]->nutzdatenlaenge>dest_maxsize) // Passen die Daten in den Zielpuffer?
	{
		JWARNING("puffer[_pos]->nutzdatenlaenge>dest_maxsize");
		e_get_next_buffer_tobig++;
		puffer[_pos]->art=art_leer; // Eintrag loeschen
		return get_next_buffer(dest,dest_maxsize); // naechten Eintrag versuchen
	}

	// Debug
	/*
	stringstream _output;
	_output << "Datenstueck an Programm ueberreichen:" << endl;
	_output << "pos:" << _pos;
	_output << " paketnummer:" << puffer[_pos]->paketnummer;
	_output << " checksum:" << (unsigned short)puffer[_pos]->checksum;
	_output << " nutzdatenlaenge:" << puffer[_pos]->nutzdatenlaenge << endl;
	print_hex(&_output,&puffer[_pos]->nutzdaten[0],puffer[_pos]->nutzdatenlaenge);
	_output << endl;
	JERROR(_output.str().c_str());
	*/
	
	memcpy(dest,&puffer[_pos]->nutzdaten[0],puffer[_pos]->nutzdatenlaenge);
	puffer[_pos]->art=art_leer; // Eintrag loeschen

	return puffer[_pos]->nutzdatenlaenge;
}



