/***************************************************************************
                          tea_enc.c  -  description
                             -------------------
    begin                : Oct 1 2004
    copyright            : (C) 2004 by Peter 'Roxton' Semiletov
    email                : tea@list.ua
***************************************************************************/

/*
Tarot Osuji <tarot@sdf.lonestar.org> //Leafcode editor, http://tarot.freeshell.org/leafpad/ 


*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "griffon_config.h"
#include "rox_strings.h"



gboolean enc_is_finnish (const gchar *s)
{
  gchar *a[8] = {
                 " eri ",
                 " sen ",
                 " ovat ",
                 " voi ",
                 " mutta ",
                 " joka ",
                 " jotta ",
                 " sen "
                 };

  gint i;
  for (i = 0; i <= 7; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_koi8r (const gchar *s)
{
  gchar *a[13] = {
                  " ÎÁ ",
                  " ÎÅ ",
                  " ÂÅÚ ",
                  " ÔÕÔ ",
                  " ÖÅ ",
                  " ÄÏ ",
                  " ÅÝÅ ",
                  " ×Ù ",
                  " ÍÏÊ ",
                  " ÅÇÏ ",
                  " ÅÅ ",
                  " ×ÁÓ ",
                  " ÎÁÓ ",
                  " ÎÉÈ ",
                  " ÉÈ "
                 };

  gint i;
  
  for (i = 0; i <= 12; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_koi8u (const gchar *s)
{
  gchar *a[13] = {
                  " ×É ",
                  " ÝÅ ",
                  " ÑË¦",
                  " ÁÂÏ ",
                  " Î¦Ö ",
                  " ×¦Î ",
                  " ÝÏ ",  
                  " ÊÏÇÏ ",
                  " ¦Ú ",
                  " Í¦Ö ",
                  " ¦Í'Ñ ",
                  " Ð¦Ä ",
                  " ÃÀ ",
                  " ÒÅÞ¦ ",
                  " §È "
                  };

  gint i;
  
  for (i = 0; i <= 12; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_cp1251 (const gchar *s)
{
  gchar *a[13] = {
                  " íà ",
                  " íå ",
                  " áåç ",
                  " òóò ",
                  " æå ",
                  " äî ",
                  " åùå ",
                  " âû ",
                  " ìîé ",
                  " åãî ",
                  " åå ",
                  " âàñ ",
                  " íàñ ",
                  " íèõ ",
                  " èõ "
                  };

  gint i;
  for (i = 0; i <= 12; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_dos866 (const gchar *s)
{
  gchar *a[13] = {  
                  " ­  ",
                  " ­¥ ",
                  " ¡¥§ ",
                  " âãâ ",
                  " ¦¥ ",
                  " ¤® ",
                  " ¥é¥ ",
                  " ¢ë ",
                  " ¬®© ",  
                  " ¥£® ",
                  " ¥¥ ",
                  " ¢ á ",
                  " ­ á ",
                  " ­¨å ",
                  " ¨å "
                  };

  gint i;
  for (i = 0; i <= 12; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_serbian (const gchar *s)
{
  gchar *a[7] = {
                 " kako ",
                 " koji ",
                 " druge ",
                 " koja ",
                 " skup ",
                 " ako ",
                 " drugu "
                 };

  gint i;
  for (i = 0; i <= 6; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_german (const gchar *s)
{
  gchar *a[8] = {
                 " der ",
                 " das ",
                 " ich ",
                 " ist ",
                 " uns ",
                 " dem ",
                 " und ",
                 " auf "
                 };

  gint i;
  for (i = 0; i <= 7; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_polish (const gchar *s)
{
  gchar *a[7] = {
                 " nie ",
                 " gdzie ",
                 " sie ",
                 " wam ",
                 " jest ",
                 " mozna ",
                 " moze "
                 };

  gint i;
  for (i = 0; i <= 6; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_latvian (const gchar *s)
{
  gchar *a[6] = {
                 " lai ",
                 " visu ",
                 " visi ",
                 " arî ",
                 " ir ",
                 " vai "
                 };

  gint i;
  for (i = 0; i <= 5; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_slovak (const gchar *s)
{
  gchar *a[6] = {
                 " tomto ",
                 " sa ",
                 " jeho ",
                 " pre ",
                 " bolo ",
                 " ste "
                };

  gint i;
  for (i = 0; i <= 5; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_slovenian (const gchar *s)
{
  gchar *a[6] = {
                 " sta ",
                 " ki ",
                 " kot ",
                 " ter ",
                 " tudi ",
                 " vec "
                 };

  gint i;
  for (i = 0; i <= 5; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_spanish (const gchar *s)
{
  gchar *a[6] = {
                 " los ",
                 " las ",
                 " sobre ",
                 " dar ",
                 " del ",
                 " por "
                 };

  gint i;
  for (i = 0; i <= 5; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_turkish (const gchar *s)
{
  gchar *a[4] = {
                 " bir ",
                 " bu ",
                 " ile ",
                 " olan "
                };

  gint i;
  for (i = 0; i <= 5; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


gboolean enc_is_portuguese (const gchar *s)
{
  gchar *a[4] = {
                 " por ",
                 " pelo ",
                 " um ",
                 " uma "
                };

  gint i;
  for (i = 0; i <= 3; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


//JIS X 0208-1990
gboolean enc_is_iso_2022_jp (const gchar *s) 
{
  gchar *a[4] = {
                 "\x1b\x24\x40",
                 "\x1b\x24\x42",
                 "\x1b\x28\x42",
                 "\x1b\x28\x4a",
                };

  gint i;
  for (i = 0; i <= 3; i++)
      if (strstr (s, a[i]))
         return TRUE;

  return FALSE; 
}


/*
GB = Guo Biao = Guojia Biaojun = National Standard

JIS = Japanese Industrial Standard
JIS = ISO-2022-JP
Shift-JIS = SJIS or MS Kanji or CP_932
EUC-JP
GB2312 = Chinese Simplified
Big5 = Chinese Traditional
*/


//Leafpad code by Tarot Osuji <tarot@sdf.lonestar.org>
//modified by roxton
gchar *detect_charset_zh(const gchar *text)
{
	guint8 c = *text;
	gchar *charset = g_strdup ("GB2312");
	
	while ((c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x87) {
			charset = ch_str (charset, "GB18030");
			break;
		}
		else if (c >= 0x88 && c <= 0xA0) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = ch_str (charset, "GB18030");
				break;
			} //else GBK/Big5-HKSCS cannot determine
		}
		else if ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9)) {
			c = *text++;
			if (c >= 0x40 && c <= 0x7E)
				charset = ch_str ("Big5", charset);
			else if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = ch_str (charset, "GB18030");
				break;
			}
		}
		else if (c >= 0xC7) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = ch_str (charset, "GB18030");
				break;
			}
		}
	}
	
	return charset;
}

gchar *detect_charset_ja(const gchar *text)
{
   if (enc_is_iso_2022_jp (text))
      return g_strdup ("ISO-2022-JP-2");

   if ( g_utf8_validate (text, -1, 0) )
      return g_strdup ("UTF-8");

	guint8 c = *text;
	gchar *charset = NULL;
	
	while (charset == NULL && (c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x9F) {
			if (c == 0x8E) /* SS2 */ {
				c = *text++;
				if ((c >= 0x40 && c <= 0xA0) || (c >= 0xE0 && c <= 0xFC))
					charset = ch_str (charset, "Shift_JIS");
			}
			else if (c == 0x8F) /* SS3 */ {
				c = *text++;
				if (c >= 0x40 && c <= 0xA0)
					charset = ch_str (charset, "Shift_JIS");
				else if (c >= 0xFD)
					break;
			}
			else
				charset = ch_str (charset, "Shift_JIS");
		}
		else if (c >= 0xA1 && c <= 0xDF) {
			c = *text++;
			if (c <= 0x9F)
				charset = ch_str (charset, "Shift_JIS");
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xE0 && c <= 0xEF) {
			c = *text++;
			if (c >= 0x40 && c <= 0xA0)
				charset = ch_str (charset, "Shift_JIS");
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xF0)
			break;
	}
	
	if (! charset)
		charset = ch_str (charset, "EUC-JP");
	
	return charset;
}

//end of the code from Leafpad