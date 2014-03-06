#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
ISJ projekt - Automaticke stahovani a srovnavani titulku

    Projekt neni dokonceny

@author: Roman Blanco
@contact: xblanc01(at)stud.fit.vutbr.cz
@version: 1.0
'''

import sys
import xmlrpc.client
import re
import base64
import gzip

class Application:
  def __init__(self, argv):

    if len(argv) != 2: 
      print("- Byl zadan nespravny pocet argumentu")
      return
    else:
      link = argv[1]
    
    # zjiskani jazyka, nazvu filmu, a id cisla z odkazu
    #---------------------------------------------------------
    self.titles_lang = re.match(".*-(..)", link).group(1)
    self.titles_name = re.match(".*/(.*)-..", link).group(1)
    self.titles_id = re.match(".*/([0-9]+)/.*", link).group(1)
    #---------------------------------------------------------
  
    self.server = xmlrpc.client.ServerProxy(
        "http://api.opensubtitles.org/xml-rpc")
  
    # prihlaseni na opensubtitles, pokud == 1, nebylo uspesne
    self.loginToken = self.login()
    if self.loginToken == 1:
      return
  
    # vyhledani filmu na IMDB, funkce vrati jeho ID
    self.searchIMDB = self.searchOnIMDB()
    if self.searchIMDB == 1:
      return
  
    # ziskani informaci o filmu z opensubtitles (ziskani IDSubtitleFile)
    self.searchOS = self.searchOnOS()
    if self.searchOS == 1:
      return
  
    self.searchEqOS = self.searchEquivalentOS()
    if self.searchOS == False:
      return
  
    # nacteni titulku do pameti
    self.download = self.DownloadSubs()
    if self.download == 1:
      return
  
    # odhlaseni z opensubtitles, pokud == 1, nebylo uspesne
    self.logoutToken = self.logout()
    if self.logout == 1:
      return
  
  def login(self):
    '''
    @summary: Prihlaseni na server opensubtitles
    @return: 0 - uspesne prihlaseni
    @return: 1 - neuspesne prihlaseni
    '''
  
    login = self.server.LogIn("","","ISO639", "OS Test User Agent")
    if login['status'] == "200 OK":
      print("+ Pripojeni uspesne")
      return login['token']
    else:
      print("- Pripojeni selhalo")
      return 1
  
  def logout(self):
    '''
    @summary: Odhlaseni ze serveru opensubtitles.org
    @return: 0 - uspesne odhlaseni
    @return: 1 - neuspesne odhlaseni
    '''
  
    logout = self.server.LogOut(self.loginToken)
    if logout['status'] == "200 OK":
      print("+ Odhlaseni uspesne")
      return 0
    else:
      print("- Odhlaseni selhalo")
      return 1
  
  def searchOnIMDB(self):
    '''
    @summary: Vyhleda film na IMDB, podle jehoz ID se budou vyhledavat
              dale titulky na opensubtitles.org
    @return: 0 - nalezeni filmu na IMDB
    @return: 1 - nenalezeni filmu na IMDB
    '''
  
    search = self.server.SearchMoviesOnIMDB(
             self.loginToken, self.titles_name)
    if search['status'] == "200 OK":
      print("+ Film na IMDB nalezen")
      return search['data'][0]['id']
    else:
      print("- Film na IMDB nenalezen")
      return 1
  
  def searchOnOS(self):
    '''
    @summary: Podle ID filmu na IMDB vyhleda na opensubtitles.org film,
              jehoz titulky byly zadane. Spravnost nalezeneho filmu je
              overovana pomoci ID titulku, kterou pomoci RE ziskam z 
              puvodniho odkazu
    @return: ID hledaneho souboru s titulky k filmu
    @return: 1 - nenalezeni filmu na IMDB
    '''
  
    # bude vyhledavat bud en/cs titulky, dle jazyka z odkazu v parametru
    if self.titles_lang == "cs":
      SubtLanguageID = "cze"
    else:
      SubtLanguageID = "eng"
    # nalezeni titulku na OS podle ID z IMDB
    self.found = self.server.SearchSubtitles(self.loginToken, 
                 [{'sublanguageid': SubtLanguageID,
                 'imdbid': self.searchIMDB}])
    i = 0
    loop = True
    while loop:
      try:
        # overeni, ze byly nalezeny spravne titulky
        if self.found['data'][i]['IDSubtitle'] == self.titles_id:
          print("+ Odpovidajici titulky na OS nalezeny")
          loop = False
      except:
        print("- Odpovidajici titulky na OS nenalezeny")
        return 1
      i += 1
    # ulozeni si informaci o poctu cd, nazvu a velikosti titulku
    #---------------------------------------------------------
    self.titles_cd = self.found['data'][i]['SubSumCD']
    self.titles_size = self.found['data'][i]['SubSize']
    self.titles_moviename = self.found['data'][i]['MovieName']
    self.titles_actualcd= self.found['data'][i]['SubActualCD']
    #---------------------------------------------------------
    return self.found['data'][i]['IDSubtitleFile']
  
  def searchEquivalentOS(self):
    # vyhledavam titulky k opacnemu jazyku
    if self.titles_lang == "cs":
      SubtLanguageID = "eng"
    else:
      SubtLanguageID = "cze"
    # docasne pole pro ulozeni menicich se hodnot, k porovnavani
    temp = dict()
    temp['IDSubtitleFile'] = False
    temp['SubDownloadsCnt'] = 0
    temp['SubSize'] = 0
    # vyhledavani stejne jako pri hledani puvodnich titulku,jen opacny jazyk
    findEquiv = self.server.SearchSubtitles(self.loginToken,
                [{'sublanguageid': SubtLanguageID,
                'imdbid': self.searchIMDB}])
    i = 0
    # Prohledava nalezene titulky a hleda nejodopovidajici
    while True:
      try:
        if findEquiv['data'][i]['MovieName'] == self.titles_moviename and findEquiv['data'][i]['SubSumCD'] == self.titles_cd and findEquiv['data'][i]['SubActualCD'] == self.titles_actualcd:
          if int(findEquiv['data'][i]['SubDownloadsCnt']) >= int(temp['SubDownloadsCnt']):
            # nahraju si do pole 'temp' udaje o nejvyhovujicich titulcich
            temp['SubDownloadsCnt'] = findEquiv['data'][i]['SubDownloadsCnt']
            temp['SubSize'] = findEquiv['data'][i]['SubSize']
            temp['IDSubtitleFile'] = findEquiv['data'][i]['IDSubtitleFile']
            # TODO
          elif temp['SubDownloadsCnt'] == findEquiv['data'][i]['SubDownloadsCnt']:
            if abs(self.titles_size - int(findEquiv['data'][i]['SubSize'])) > temp['SubSize']:
              # nahraju si do pole 'temp' udaje o nejvyhovujicich titulcich
              temp['SubDownloadsCnt'] = findEquiv['data'][i]['SubDownloadsCnt']
              temp['SubSize'] = findEquiv['data'][i]['SubSize']
              temp['IDSubtitleFile'] = findEquiv['data'][i]['IDSubtitleFile']
      except:
        break
      i += 1
    if temp['IDSubtitleFile'] == False:
      print("- Odpovidajici titulky nenalezeny")
    else:
      print("+ Odpovidajici titulky nalezeny")
    return temp['IDSubtitleFile']
  
  def DownloadSubs(self):
    encoded_l1 = self.server.DownloadSubtitles(self.loginToken, [self.searchOS])
    decoded_l1 = base64.b64decode(bytes(encoded_l1['data'][0]['data'], "cp1250"))
    decompresed_l1 = str(gzip.decompress(decoded_l1), "cp1250")

    encoded_l2 = self.server.DownloadSubtitles(self.loginToken, [self.searchEqOS])
    decoded_l2 = base64.b64decode(bytes(encoded_l2['data'][0]['data'], "cp1250"))
    decompresed_l2 = str(gzip.decompress(decoded_l2), "cp1250")
    
if  __name__ == '__main__':
  app = Application(sys.argv)
