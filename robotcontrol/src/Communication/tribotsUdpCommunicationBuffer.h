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

/* TribotsUdpCommunicationBuffer class
 *
 * Verwalten der Buffer Daten fuer die UDP kommunikation.
 */

#ifndef _TRIBOTS_UDPCOMMUNICATION_BUFFER_H_
#define _TRIBOTS_UDPCOMMUNICATION_BUFFER_H_

#include <sstream>


#define MAX_NUTZDATENLAENGE 8192
// 0-400 Linien+hindernisse ~8KB

#define MAX_NUTZDATEN 100

#define _SENDEVERSUCHE_ 50
// Server und client sind unterschiedlich schnell!!

#define _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
#define _TRIBOTS_UDPCOMMUNICATION_LOG


using namespace std;

namespace Tribots {

class TribotsUdpCommunicationBuffer {

 protected:
	// 
	static const unsigned char art_leer			= 0;
	static const unsigned char art_ausgang_normal		= 1;
	static const unsigned char art_ausgang_rueckschein	= 2;
	static const unsigned char art_ausgang_rueckscheinantwort = 3;
	static const unsigned char art_eingang_normal		= 11;
	static const unsigned char art_version_info			= 20;

	static const unsigned char prio_wichtig			= 0;
	static const unsigned char prio_normal			= 1;
	static const unsigned char prio_optional		= 2;
	
	unsigned int max_eintraege;
	
	unsigned int versionsnummer_uebertragungen;
	
	// Aufgetretene Fehler zaehlen
	unsigned long e_send_buffer_full;
	unsigned long e_put_to_buffer_full;
	unsigned long e_put_to_buffer_tobig;
	unsigned long e_set_receive_buffer;
	unsigned long e_set_receive_buffer_tobig;
	unsigned long e_get_next_buffer_tobig;
	unsigned long e_receive_unknow_data;
	unsigned long e_receive_data_checksum;
	unsigned long w_receive_to_old;
	
#ifdef _TRIBOTS_UDPCOMMUNICATION_BUFFER_STAT_
	// Statistische Daten
	unsigned long stat_data_send;
	unsigned long stat_data_receive;
	unsigned int stat_max_nutzdatenlaenge_send;
	char stat_max_nutzdatenlaenge_send_art;
	unsigned int stat_max_bufferlaenge_send;
	unsigned long stat_data_send_prio[3];
#endif
	
	unsigned int fortlaufende_paketnummer;
	unsigned int fortlaufende_paketnummer_remote;
	
	struct eintraege {
		// Daten die versendet werden
		unsigned char art; // eingang/ausgang 
		unsigned int paketnummer;
		unsigned char checksum;
		unsigned int nutzdatenlaenge;
		char nutzdaten[MAX_NUTZDATENLAENGE];
		// Interne Daten
		unsigned char prio;
		unsigned int wiederholungen; // anzahl der versuche, bis es verworfen wird
	};

	eintraege * puffer[MAX_NUTZDATEN];

	unsigned int find_next_art(unsigned char art);
	void put_to_buffer_rueckscheinantwort(unsigned int paketnummer,unsigned char checksum,unsigned int nutzdatenlaenge);
	void del_from_buffer_rueckschein(unsigned int paketnummer,unsigned char checksum,unsigned int nutzdatenlaenge);
	unsigned char checksum(char * src,unsigned int size);
	void log_status();
	void print_inhalt(char * src,unsigned int size, unsigned short art);
	
public:
	TribotsUdpCommunicationBuffer(unsigned int eintraege);
	~TribotsUdpCommunicationBuffer();
	bool put_to_buffer(char * src,unsigned int size,bool rueckschein,unsigned char prio);
	unsigned int get_send_buffer(char * dest,unsigned int maxsize,unsigned int version); // Liefert die zu versendenden Daten
	bool set_receive_buffer(char * src,unsigned int src_size,unsigned int version); // Nimmt die empfangenen Daten entgegen
	unsigned int get_next_buffer(char * dest,unsigned int dest_maxsize); // Liefert ein Datenstueck
	
	// debug
	void print_hex(ostream * output,char * src,unsigned int size);
};
}

#endif
