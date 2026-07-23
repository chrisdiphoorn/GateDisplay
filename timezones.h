// https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv

#include <Arduino.h>
#include <pgmspace.h>

struct TimezoneEntry {
  const char* zoneName;
  const char* posixRule;
};

const char z000[] PROGMEM = "Africa/Abidjan"; const char r000[] PROGMEM = "GMT0";
const char z001[] PROGMEM = "Africa/Accra"; const char r001[] PROGMEM = "GMT0";
const char z002[] PROGMEM = "Africa/Addis_Ababa"; const char r002[] PROGMEM = "EAT-3";
const char z003[] PROGMEM = "Africa/Algiers"; const char r003[] PROGMEM = "CET-1";
const char z004[] PROGMEM = "Africa/Asmara"; const char r004[] PROGMEM = "EAT-3";
const char z005[] PROGMEM = "Africa/Bamako"; const char r005[] PROGMEM = "GMT0";
const char z006[] PROGMEM = "Africa/Bangui"; const char r006[] PROGMEM = "WAT-1";
const char z007[] PROGMEM = "Africa/Banjul"; const char r007[] PROGMEM = "GMT0";
const char z008[] PROGMEM = "Africa/Bissau"; const char r008[] PROGMEM = "GMT0";
const char z009[] PROGMEM = "Africa/Blantyre"; const char r009[] PROGMEM = "CAT-2";
const char z010[] PROGMEM = "Africa/Brazzaville"; const char r010[] PROGMEM = "WAT-1";
const char z011[] PROGMEM = "Africa/Bujumbura"; const char r011[] PROGMEM = "CAT-2";
const char z012[] PROGMEM = "Africa/Cairo"; const char r012[] PROGMEM = "EET-2EEST,M4.5.5/0,M10.5.4/24";
const char z013[] PROGMEM = "Africa/Casablanca"; const char r013[] PROGMEM = "<+01>-1";
const char z014[] PROGMEM = "Africa/Ceuta"; const char r014[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z015[] PROGMEM = "Africa/Conakry"; const char r015[] PROGMEM = "GMT0";
const char z016[] PROGMEM = "Africa/Dakar"; const char r016[] PROGMEM = "GMT0";
const char z017[] PROGMEM = "Africa/Dar_es_Salaam"; const char r017[] PROGMEM = "EAT-3";
const char z018[] PROGMEM = "Africa/Djibouti"; const char r018[] PROGMEM = "EAT-3";
const char z019[] PROGMEM = "Africa/Douala"; const char r019[] PROGMEM = "WAT-1";
const char z020[] PROGMEM = "Africa/El_Aaiun"; const char r020[] PROGMEM = "<+01>-1";
const char z021[] PROGMEM = "Africa/Freetown"; const char r021[] PROGMEM = "GMT0";
const char z022[] PROGMEM = "Africa/Gaborone"; const char r022[] PROGMEM = "CAT-2";
const char z023[] PROGMEM = "Africa/Harare"; const char r023[] PROGMEM = "CAT-2";
const char z024[] PROGMEM = "Africa/Johannesburg"; const char r024[] PROGMEM = "SAST-2";
const char z025[] PROGMEM = "Africa/Juba"; const char r025[] PROGMEM = "CAT-2";
const char z026[] PROGMEM = "Africa/Kampala"; const char r026[] PROGMEM = "EAT-3";
const char z027[] PROGMEM = "Africa/Khartoum"; const char r027[] PROGMEM = "CAT-2";
const char z028[] PROGMEM = "Africa/Kigali"; const char r028[] PROGMEM = "CAT-2";
const char z029[] PROGMEM = "Africa/Kinshasa"; const char r029[] PROGMEM = "WAT-1";
const char z030[] PROGMEM = "Africa/Lagos"; const char r030[] PROGMEM = "WAT-1";
const char z031[] PROGMEM = "Africa/Libreville"; const char r031[] PROGMEM = "WAT-1";
const char z032[] PROGMEM = "Africa/Lome"; const char r032[] PROGMEM = "GMT0";
const char z033[] PROGMEM = "Africa/Luanda"; const char r033[] PROGMEM = "WAT-1";
const char z034[] PROGMEM = "Africa/Lubumbashi"; const char r034[] PROGMEM = "CAT-2";
const char z035[] PROGMEM = "Africa/Lusaka"; const char r035[] PROGMEM = "CAT-2";
const char z036[] PROGMEM = "Africa/Malabo"; const char r036[] PROGMEM = "WAT-1";
const char z037[] PROGMEM = "Africa/Maputo"; const char r037[] PROGMEM = "CAT-2";
const char z038[] PROGMEM = "Africa/Maseru"; const char r038[] PROGMEM = "SAST-2";
const char z039[] PROGMEM = "Africa/Mbabane"; const char r039[] PROGMEM = "SAST-2";
const char z040[] PROGMEM = "Africa/Mogadishu"; const char r040[] PROGMEM = "EAT-3";
const char z041[] PROGMEM = "Africa/Monrovia"; const char r041[] PROGMEM = "GMT0";
const char z042[] PROGMEM = "Africa/Nairobi"; const char r042[] PROGMEM = "EAT-3";
const char z043[] PROGMEM = "Africa/Ndjamena"; const char r043[] PROGMEM = "WAT-1";
const char z044[] PROGMEM = "Africa/Niamey"; const char r044[] PROGMEM = "WAT-1";
const char z045[] PROGMEM = "Africa/Nouakchott"; const char r045[] PROGMEM = "GMT0";
const char z046[] PROGMEM = "Africa/Ouagadougou"; const char r046[] PROGMEM = "GMT0";
const char z047[] PROGMEM = "Africa/Porto-Novo"; const char r047[] PROGMEM = "WAT-1";
const char z048[] PROGMEM = "Africa/Sao_Tome"; const char r048[] PROGMEM = "GMT0";
const char z049[] PROGMEM = "Africa/Tripoli"; const char r049[] PROGMEM = "EET-2";
const char z050[] PROGMEM = "Africa/Tunis"; const char r050[] PROGMEM = "CET-1";
const char z051[] PROGMEM = "Africa/Windhoek"; const char r051[] PROGMEM = "CAT-2";
const char z052[] PROGMEM = "America/Adak"; const char r052[] PROGMEM = "HST10HDT,M3.2.0,M11.1.0";
const char z053[] PROGMEM = "America/Anchorage"; const char r053[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z054[] PROGMEM = "America/Anguilla"; const char r054[] PROGMEM = "AST4";
const char z055[] PROGMEM = "America/Antigua"; const char r055[] PROGMEM = "AST4";
const char z056[] PROGMEM = "America/Araguaina"; const char r056[] PROGMEM = "<-03>3";
const char z057[] PROGMEM = "America/Argentina/Buenos_Aires"; const char r057[] PROGMEM = "<-03>3";
const char z058[] PROGMEM = "America/Argentina/Catamarca"; const char r058[] PROGMEM = "<-03>3";
const char z059[] PROGMEM = "America/Argentina/Cordoba"; const char r059[] PROGMEM = "<-03>3";
const char z060[] PROGMEM = "America/Argentina/Jujuy"; const char r060[] PROGMEM = "<-03>3";
const char z061[] PROGMEM = "America/Argentina/La_Rioja"; const char r061[] PROGMEM = "<-03>3";
const char z062[] PROGMEM = "America/Argentina/Mendoza"; const char r062[] PROGMEM = "<-03>3";
const char z063[] PROGMEM = "America/Argentina/Rio_Gallegos"; const char r063[] PROGMEM = "<-03>3";
const char z064[] PROGMEM = "America/Argentina/Salta"; const char r064[] PROGMEM = "<-03>3";
const char z065[] PROGMEM = "America/Argentina/San_Juan"; const char r065[] PROGMEM = "<-03>3";
const char z066[] PROGMEM = "America/Argentina/San_Luis"; const char r066[] PROGMEM = "<-03>3";
const char z067[] PROGMEM = "America/Argentina/Tucuman"; const char r067[] PROGMEM = "<-03>3";
const char z068[] PROGMEM = "America/Argentina/Ushuaia"; const char r068[] PROGMEM = "<-03>3";
const char z069[] PROGMEM = "America/Aruba"; const char r069[] PROGMEM = "AST4";
const char z070[] PROGMEM = "America/Asuncion"; const char r070[] PROGMEM = "<-03>3";
const char z071[] PROGMEM = "America/Atikokan"; const char r071[] PROGMEM = "EST5";
const char z072[] PROGMEM = "America/Bahia"; const char r072[] PROGMEM = "<-03>3";
const char z073[] PROGMEM = "America/Bahia_Banderas"; const char r073[] PROGMEM = "CST6";
const char z074[] PROGMEM = "America/Barbados"; const char r074[] PROGMEM = "AST4";
const char z075[] PROGMEM = "America/Belem"; const char r075[] PROGMEM = "<-03>3";
const char z076[] PROGMEM = "America/Belize"; const char r076[] PROGMEM = "CST6";
const char z077[] PROGMEM = "America/Blanc-Sablon"; const char r077[] PROGMEM = "AST4";
const char z078[] PROGMEM = "America/Boa_Vista"; const char r078[] PROGMEM = "<-04>4";
const char z079[] PROGMEM = "America/Bogota"; const char r079[] PROGMEM = "<-05>5";
const char z080[] PROGMEM = "America/Boise"; const char r080[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z081[] PROGMEM = "America/Cambridge_Bay"; const char r081[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z082[] PROGMEM = "America/Campo_Grande"; const char r082[] PROGMEM = "<-04>4";
const char z083[] PROGMEM = "America/Cancun"; const char r083[] PROGMEM = "EST5";
const char z084[] PROGMEM = "America/Caracas"; const char r084[] PROGMEM = "<-04>4";
const char z085[] PROGMEM = "America/Cayenne"; const char r085[] PROGMEM = "<-03>3";
const char z086[] PROGMEM = "America/Cayman"; const char r086[] PROGMEM = "EST5";
const char z087[] PROGMEM = "America/Chicago"; const char r087[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z088[] PROGMEM = "America/Chihuahua"; const char r088[] PROGMEM = "CST6";
const char z089[] PROGMEM = "America/Costa_Rica"; const char r089[] PROGMEM = "CST6";
const char z090[] PROGMEM = "America/Creston"; const char r090[] PROGMEM = "MST7";
const char z091[] PROGMEM = "America/Cuiaba"; const char r091[] PROGMEM = "<-04>4";
const char z092[] PROGMEM = "America/Curacao"; const char r092[] PROGMEM = "AST4";
const char z093[] PROGMEM = "America/Danmarkshavn"; const char r093[] PROGMEM = "GMT0";
const char z094[] PROGMEM = "America/Dawson"; const char r094[] PROGMEM = "MST7";
const char z095[] PROGMEM = "America/Dawson_Creek"; const char r095[] PROGMEM = "MST7";
const char z096[] PROGMEM = "America/Denver"; const char r096[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z097[] PROGMEM = "America/Detroit"; const char r097[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z098[] PROGMEM = "America/Dominica"; const char r098[] PROGMEM = "AST4";
const char z099[] PROGMEM = "America/Edmonton"; const char r099[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z100[] PROGMEM = "America/Eirunepe"; const char r100[] PROGMEM = "<-05>5";
const char z101[] PROGMEM = "America/El_Salvador"; const char r101[] PROGMEM = "CST6";
const char z102[] PROGMEM = "America/Fortaleza"; const char r102[] PROGMEM = "<-03>3";
const char z103[] PROGMEM = "America/Fort_Nelson"; const char r103[] PROGMEM = "MST7";
const char z104[] PROGMEM = "America/Glace_Bay"; const char r104[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z105[] PROGMEM = "America/Godthab"; const char r105[] PROGMEM = "<-02>2<-01>,M3.5.0/-1,M10.5.0/0";
const char z106[] PROGMEM = "America/Goose_Bay"; const char r106[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z107[] PROGMEM = "America/Grand_Turk"; const char r107[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z108[] PROGMEM = "America/Grenada"; const char r108[] PROGMEM = "AST4";
const char z109[] PROGMEM = "America/Guadeloupe"; const char r109[] PROGMEM = "AST4";
const char z110[] PROGMEM = "America/Guatemala"; const char r110[] PROGMEM = "CST6";
const char z111[] PROGMEM = "America/Guayaquil"; const char r111[] PROGMEM = "<-05>5";
const char z112[] PROGMEM = "America/Guyana"; const char r112[] PROGMEM = "<-04>4";
const char z113[] PROGMEM = "America/Halifax"; const char r113[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z114[] PROGMEM = "America/Havana"; const char r114[] PROGMEM = "CST5CDT,M3.2.0/0,M11.1.0/1";
const char z115[] PROGMEM = "America/Hermosillo"; const char r115[] PROGMEM = "MST7";
const char z116[] PROGMEM = "America/Indiana/Indianapolis"; const char r116[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z117[] PROGMEM = "America/Indiana/Knox"; const char r117[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z118[] PROGMEM = "America/Indiana/Marengo"; const char r118[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z119[] PROGMEM = "America/Indiana/Petersburg"; const char r119[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z120[] PROGMEM = "America/Indiana/Tell_City"; const char r120[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z121[] PROGMEM = "America/Indiana/Vevay"; const char r121[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z122[] PROGMEM = "America/Indiana/Vincennes"; const char r122[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z123[] PROGMEM = "America/Indiana/Winamac"; const char r123[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z124[] PROGMEM = "America/Inuvik"; const char r124[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z125[] PROGMEM = "America/Iqaluit"; const char r125[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z126[] PROGMEM = "America/Jamaica"; const char r126[] PROGMEM = "EST5";
const char z127[] PROGMEM = "America/Juneau"; const char r127[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z128[] PROGMEM = "America/Kentucky/Louisville"; const char r128[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z129[] PROGMEM = "America/Kentucky/Monticello"; const char r129[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z130[] PROGMEM = "America/Kralendijk"; const char r130[] PROGMEM = "AST4";
const char z131[] PROGMEM = "America/La_Paz"; const char r131[] PROGMEM = "<-04>4";
const char z132[] PROGMEM = "America/Lima"; const char r132[] PROGMEM = "<-05>5";
const char z133[] PROGMEM = "America/Los_Angeles"; const char r133[] PROGMEM = "PST8PDT,M3.2.0,M11.1.0";
const char z134[] PROGMEM = "America/Lower_Princes"; const char r134[] PROGMEM = "AST4";
const char z135[] PROGMEM = "America/Maceio"; const char r135[] PROGMEM = "<-03>3";
const char z136[] PROGMEM = "America/Managua"; const char r136[] PROGMEM = "CST6";
const char z137[] PROGMEM = "America/Manaus"; const char r137[] PROGMEM = "<-04>4";
const char z138[] PROGMEM = "America/Marigot"; const char r138[] PROGMEM = "AST4";
const char z139[] PROGMEM = "America/Martinique"; const char r139[] PROGMEM = "AST4";
const char z140[] PROGMEM = "America/Matamoros"; const char r140[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z141[] PROGMEM = "America/Mazatlan"; const char r141[] PROGMEM = "MST7";
const char z142[] PROGMEM = "America/Menominee"; const char r142[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z143[] PROGMEM = "America/Merida"; const char r143[] PROGMEM = "CST6";
const char z144[] PROGMEM = "America/Metlakatla"; const char r144[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z145[] PROGMEM = "America/Mexico_City"; const char r145[] PROGMEM = "CST6";
const char z146[] PROGMEM = "America/Miquelon"; const char r146[] PROGMEM = "<-03>3<-02>,M3.2.0,M11.1.0";
const char z147[] PROGMEM = "America/Moncton"; const char r147[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z148[] PROGMEM = "America/Monterrey"; const char r148[] PROGMEM = "CST6";
const char z149[] PROGMEM = "America/Montevideo"; const char r149[] PROGMEM = "<-03>3";
const char z150[] PROGMEM = "America/Montreal"; const char r150[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z151[] PROGMEM = "America/Montserrat"; const char r151[] PROGMEM = "AST4";
const char z152[] PROGMEM = "America/Nassau"; const char r152[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z153[] PROGMEM = "America/New_York"; const char r153[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z154[] PROGMEM = "America/Nipigon"; const char r154[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z155[] PROGMEM = "America/Nome"; const char r155[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z156[] PROGMEM = "America/Noronha"; const char r156[] PROGMEM = "<-02>2";
const char z157[] PROGMEM = "America/North_Dakota/Beulah"; const char r157[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z158[] PROGMEM = "America/North_Dakota/Center"; const char r158[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z159[] PROGMEM = "America/North_Dakota/New_Salem"; const char r159[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z160[] PROGMEM = "America/Nuuk"; const char r160[] PROGMEM = "<-02>2<-01>,M3.5.0/-1,M10.5.0/0";
const char z161[] PROGMEM = "America/Ojinaga"; const char r161[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z162[] PROGMEM = "America/Panama"; const char r162[] PROGMEM = "EST5";
const char z163[] PROGMEM = "America/Pangnirtung"; const char r163[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z164[] PROGMEM = "America/Paramaribo"; const char r164[] PROGMEM = "<-03>3";
const char z165[] PROGMEM = "America/Phoenix"; const char r165[] PROGMEM = "MST7";
const char z166[] PROGMEM = "America/Port-au-Prince"; const char r166[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z167[] PROGMEM = "America/Port_of_Spain"; const char r167[] PROGMEM = "AST4";
const char z168[] PROGMEM = "America/Porto_Velho"; const char r168[] PROGMEM = "<-04>4";
const char z169[] PROGMEM = "America/Puerto_Rico"; const char r169[] PROGMEM = "AST4";
const char z170[] PROGMEM = "America/Punta_Arenas"; const char r170[] PROGMEM = "<-03>3";
const char z171[] PROGMEM = "America/Rainy_River"; const char r171[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z172[] PROGMEM = "America/Rankin_Inlet"; const char r172[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z173[] PROGMEM = "America/Recife"; const char r173[] PROGMEM = "<-03>3";
const char z174[] PROGMEM = "America/Regina"; const char r174[] PROGMEM = "CST6";
const char z175[] PROGMEM = "America/Resolute"; const char r175[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z176[] PROGMEM = "America/Rio_Branco"; const char r176[] PROGMEM = "<-05>5";
const char z177[] PROGMEM = "America/Santarem"; const char r177[] PROGMEM = "<-03>3";
const char z178[] PROGMEM = "America/Santiago"; const char r178[] PROGMEM = "<-04>4<-03>,M9.1.6/24,M4.1.6/24";
const char z179[] PROGMEM = "America/Santo_Domingo"; const char r179[] PROGMEM = "AST4";
const char z180[] PROGMEM = "America/Sao_Paulo"; const char r180[] PROGMEM = "<-03>3";
const char z181[] PROGMEM = "America/Scoresbysund"; const char r181[] PROGMEM = "<-02>2<-01>,M3.5.0/-1,M10.5.0/0";
const char z182[] PROGMEM = "America/Sitka"; const char r182[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z183[] PROGMEM = "America/St_Barthelemy"; const char r183[] PROGMEM = "AST4";
const char z184[] PROGMEM = "America/St_Johns"; const char r184[] PROGMEM = "NST3:30NDT,M3.2.0,M11.1.0";
const char z185[] PROGMEM = "America/St_Kitts"; const char r185[] PROGMEM = "AST4";
const char z186[] PROGMEM = "America/St_Lucia"; const char r186[] PROGMEM = "AST4";
const char z187[] PROGMEM = "America/St_Thomas"; const char r187[] PROGMEM = "AST4";
const char z188[] PROGMEM = "America/St_Vincent"; const char r188[] PROGMEM = "AST4";
const char z189[] PROGMEM = "America/Swift_Current"; const char r189[] PROGMEM = "CST6";
const char z190[] PROGMEM = "America/Tegucigalpa"; const char r190[] PROGMEM = "CST6";
const char z191[] PROGMEM = "America/Thule"; const char r191[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z192[] PROGMEM = "America/Thunder_Bay"; const char r192[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z193[] PROGMEM = "America/Tijuana"; const char r193[] PROGMEM = "PST8PDT,M3.2.0,M11.1.0";
const char z194[] PROGMEM = "America/Toronto"; const char r194[] PROGMEM = "EST5EDT,M3.2.0,M11.1.0";
const char z195[] PROGMEM = "America/Tortola"; const char r195[] PROGMEM = "AST4";
const char z196[] PROGMEM = "America/Vancouver"; const char r196[] PROGMEM = "PST8PDT,M3.2.0,M11.1.0";
const char z197[] PROGMEM = "America/Whitehorse"; const char r197[] PROGMEM = "MST7";
const char z198[] PROGMEM = "America/Winnipeg"; const char r198[] PROGMEM = "CST6CDT,M3.2.0,M11.1.0";
const char z199[] PROGMEM = "America/Yakutat"; const char r199[] PROGMEM = "AKST9AKDT,M3.2.0,M11.1.0";
const char z200[] PROGMEM = "America/Yellowknife"; const char r200[] PROGMEM = "MST7MDT,M3.2.0,M11.1.0";
const char z201[] PROGMEM = "Antarctica/Casey"; const char r201[] PROGMEM = "<+08>-8";
const char z202[] PROGMEM = "Antarctica/Davis"; const char r202[] PROGMEM = "<+07>-7";
const char z203[] PROGMEM = "Antarctica/DumontDUrville"; const char r203[] PROGMEM = "<+10>-10";
const char z204[] PROGMEM = "Antarctica/Macquarie"; const char r204[] PROGMEM = "AEST-10AEDT,M10.1.0,M4.1.0/3";
const char z205[] PROGMEM = "Antarctica/Mawson"; const char r205[] PROGMEM = "<+05>-5";
const char z206[] PROGMEM = "Antarctica/McMurdo"; const char r206[] PROGMEM = "NZST-12NZDT,M9.5.0,M4.1.0/3";
const char z207[] PROGMEM = "Antarctica/Palmer"; const char r207[] PROGMEM = "<-03>3";
const char z208[] PROGMEM = "Antarctica/Rothera"; const char r208[] PROGMEM = "<-03>3";
const char z209[] PROGMEM = "Antarctica/Syowa"; const char r209[] PROGMEM = "<+03>-3";
const char z210[] PROGMEM = "Antarctica/Troll"; const char r210[] PROGMEM = "<+00>0<+02>-2,M3.5.0/1,M10.5.0/3";
const char z211[] PROGMEM = "Antarctica/Vostok"; const char r211[] PROGMEM = "<+05>-5";
const char z212[] PROGMEM = "Arctic/Longyearbyen"; const char r212[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z213[] PROGMEM = "Asia/Aden"; const char r213[] PROGMEM = "<+03>-3";
const char z214[] PROGMEM = "Asia/Almaty"; const char r214[] PROGMEM = "<+05>-5";
const char z215[] PROGMEM = "Asia/Amman"; const char r215[] PROGMEM = "<+03>-3";
const char z216[] PROGMEM = "Asia/Anadyr"; const char r216[] PROGMEM = "<+12>-12";
const char z217[] PROGMEM = "Asia/Aqtau"; const char r217[] PROGMEM = "<+05>-5";
const char z218[] PROGMEM = "Asia/Aqtobe"; const char r218[] PROGMEM = "<+05>-5";
const char z219[] PROGMEM = "Asia/Ashgabat"; const char r219[] PROGMEM = "<+05>-5";
const char z220[] PROGMEM = "Asia/Atyrau"; const char r220[] PROGMEM = "<+05>-5";
const char z221[] PROGMEM = "Asia/Baghdad"; const char r221[] PROGMEM = "<+03>-3";
const char z222[] PROGMEM = "Asia/Bahrain"; const char r222[] PROGMEM = "<+03>-3";
const char z223[] PROGMEM = "Asia/Baku"; const char r223[] PROGMEM = "<+04>-4";
const char z224[] PROGMEM = "Asia/Bangkok"; const char r224[] PROGMEM = "<+07>-7";
const char z225[] PROGMEM = "Asia/Barnaul"; const char r225[] PROGMEM = "<+07>-7";
const char z226[] PROGMEM = "Asia/Beirut"; const char r226[] PROGMEM = "EET-2EEST,M3.5.0/0,M10.5.0/0";
const char z227[] PROGMEM = "Asia/Bishkek"; const char r227[] PROGMEM = "<+06>-6";
const char z228[] PROGMEM = "Asia/Brunei"; const char r228[] PROGMEM = "<+08>-8";
const char z229[] PROGMEM = "Asia/Chita"; const char r229[] PROGMEM = "<+09>-9";
const char z230[] PROGMEM = "Asia/Choibalsan"; const char r230[] PROGMEM = "<+08>-8";
const char z231[] PROGMEM = "Asia/Colombo"; const char r231[] PROGMEM = "<+0530>-5:30";
const char z232[] PROGMEM = "Asia/Damascus"; const char r232[] PROGMEM = "<+03>-3";
const char z233[] PROGMEM = "Asia/Dhaka"; const char r233[] PROGMEM = "<+06>-6";
const char z234[] PROGMEM = "Asia/Dili"; const char r234[] PROGMEM = "<+09>-9";
const char z235[] PROGMEM = "Asia/Dubai"; const char r235[] PROGMEM = "<+04>-4";
const char z236[] PROGMEM = "Asia/Dushanbe"; const char r236[] PROGMEM = "<+05>-5";
const char z237[] PROGMEM = "Asia/Famagusta"; const char r237[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z238[] PROGMEM = "Asia/Gaza"; const char r238[] PROGMEM = "EET-2EEST,M3.4.4/50,M10.4.4/50";
const char z239[] PROGMEM = "Asia/Hebron"; const char r239[] PROGMEM = "EET-2EEST,M3.4.4/50,M10.4.4/50";
const char z240[] PROGMEM = "Asia/Ho_Chi_Minh"; const char r240[] PROGMEM = "<+07>-7";
const char z241[] PROGMEM = "Asia/Hong_Kong"; const char r241[] PROGMEM = "HKT-8";
const char z242[] PROGMEM = "Asia/Hovd"; const char r242[] PROGMEM = "<+07>-7";
const char z243[] PROGMEM = "Asia/Irkutsk"; const char r243[] PROGMEM = "<+08>-8";
const char z244[] PROGMEM = "Asia/Jakarta"; const char r244[] PROGMEM = "WIB-7";
const char z245[] PROGMEM = "Asia/Jayapura"; const char r245[] PROGMEM = "WIT-9";
const char z246[] PROGMEM = "Asia/Jerusalem"; const char r246[] PROGMEM = "IST-2IDT,M3.4.4/26,M10.5.0";
const char z247[] PROGMEM = "Asia/Kabul"; const char r247[] PROGMEM = "<+0430>-4:30";
const char z248[] PROGMEM = "Asia/Kamchatka"; const char r248[] PROGMEM = "<+12>-12";
const char z249[] PROGMEM = "Asia/Karachi"; const char r249[] PROGMEM = "PKT-5";
const char z250[] PROGMEM = "Asia/Kathmandu"; const char r250[] PROGMEM = "<+0545>-5:45";
const char z251[] PROGMEM = "Asia/Khandyga"; const char r251[] PROGMEM = "<+09>-9";
const char z252[] PROGMEM = "Asia/Kolkata"; const char r252[] PROGMEM = "IST-5:30";
const char z253[] PROGMEM = "Asia/Krasnoyarsk"; const char r253[] PROGMEM = "<+07>-7";
const char z254[] PROGMEM = "Asia/Kuala_Lumpur"; const char r254[] PROGMEM = "<+08>-8";
const char z255[] PROGMEM = "Asia/Kuching"; const char r255[] PROGMEM = "<+08>-8";
const char z256[] PROGMEM = "Asia/Kuwait"; const char r256[] PROGMEM = "<+03>-3";
const char z257[] PROGMEM = "Asia/Macau"; const char r257[] PROGMEM = "CST-8";
const char z258[] PROGMEM = "Asia/Magadan"; const char r258[] PROGMEM = "<+11>-11";
const char z259[] PROGMEM = "Asia/Makassar"; const char r259[] PROGMEM = "WITA-8";
const char z260[] PROGMEM = "Asia/Manila"; const char r260[] PROGMEM = "PST-8";
const char z261[] PROGMEM = "Asia/Muscat"; const char r261[] PROGMEM = "<+04>-4";
const char z262[] PROGMEM = "Asia/Nicosia"; const char r262[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z263[] PROGMEM = "Asia/Novokuznetsk"; const char r263[] PROGMEM = "<+07>-7";
const char z264[] PROGMEM = "Asia/Novosibirsk"; const char r264[] PROGMEM = "<+07>-7";
const char z265[] PROGMEM = "Asia/Omsk"; const char r265[] PROGMEM = "<+06>-6";
const char z266[] PROGMEM = "Asia/Oral"; const char r266[] PROGMEM = "<+05>-5";
const char z267[] PROGMEM = "Asia/Phnom_Penh"; const char r267[] PROGMEM = "<+07>-7";
const char z268[] PROGMEM = "Asia/Pontianak"; const char r268[] PROGMEM = "WIB-7";
const char z269[] PROGMEM = "Asia/Pyongyang"; const char r269[] PROGMEM = "KST-9";
const char z270[] PROGMEM = "Asia/Qatar"; const char r270[] PROGMEM = "<+03>-3";
const char z271[] PROGMEM = "Asia/Qyzylorda"; const char r271[] PROGMEM = "<+05>-5";
const char z272[] PROGMEM = "Asia/Riyadh"; const char r272[] PROGMEM = "<+03>-3";
const char z273[] PROGMEM = "Asia/Sakhalin"; const char r273[] PROGMEM = "<+11>-11";
const char z274[] PROGMEM = "Asia/Samarkand"; const char r274[] PROGMEM = "<+05>-5";
const char z275[] PROGMEM = "Asia/Seoul"; const char r275[] PROGMEM = "KST-9";
const char z276[] PROGMEM = "Asia/Shanghai"; const char r276[] PROGMEM = "CST-8";
const char z277[] PROGMEM = "Asia/Singapore"; const char r277[] PROGMEM = "<+08>-8";
const char z278[] PROGMEM = "Asia/Srednekolymsk"; const char r278[] PROGMEM = "<+11>-11";
const char z279[] PROGMEM = "Asia/Taipei"; const char r279[] PROGMEM = "CST-8";
const char z280[] PROGMEM = "Asia/Tashkent"; const char r280[] PROGMEM = "<+05>-5";
const char z281[] PROGMEM = "Asia/Tbilisi"; const char r281[] PROGMEM = "<+04>-4";
const char z282[] PROGMEM = "Asia/Tehran"; const char r282[] PROGMEM = "<+0330>-3:30";
const char z283[] PROGMEM = "Asia/Thimphu"; const char r283[] PROGMEM = "<+06>-6";
const char z284[] PROGMEM = "Asia/Tokyo"; const char r284[] PROGMEM = "JST-9";
const char z285[] PROGMEM = "Asia/Tomsk"; const char r285[] PROGMEM = "<+07>-7";
const char z286[] PROGMEM = "Asia/Ulaanbaatar"; const char r286[] PROGMEM = "<+08>-8";
const char z287[] PROGMEM = "Asia/Urumqi"; const char r287[] PROGMEM = "<+06>-6";
const char z288[] PROGMEM = "Asia/Ust-Nera"; const char r288[] PROGMEM = "<+10>-10";
const char z289[] PROGMEM = "Asia/Vientiane"; const char r289[] PROGMEM = "<+07>-7";
const char z290[] PROGMEM = "Asia/Vladivostok"; const char r290[] PROGMEM = "<+10>-10";
const char z291[] PROGMEM = "Asia/Yakutsk"; const char r291[] PROGMEM = "<+09>-9";
const char z292[] PROGMEM = "Asia/Yangon"; const char r292[] PROGMEM = "<+0630>-6:30";
const char z293[] PROGMEM = "Asia/Yekaterinburg"; const char r293[] PROGMEM = "<+05>-5";
const char z294[] PROGMEM = "Asia/Yerevan"; const char r294[] PROGMEM = "<+04>-4";
const char z295[] PROGMEM = "Atlantic/Azores"; const char r295[] PROGMEM = "<-01>1<+00>,M3.5.0/0,M10.5.0/1";
const char z296[] PROGMEM = "Atlantic/Bermuda"; const char r296[] PROGMEM = "AST4ADT,M3.2.0,M11.1.0";
const char z297[] PROGMEM = "Atlantic/Canary"; const char r297[] PROGMEM = "WET0WEST,M3.5.0/1,M10.5.0";
const char z298[] PROGMEM = "Atlantic/Cape_Verde"; const char r298[] PROGMEM = "<-01>1";
const char z299[] PROGMEM = "Atlantic/Faroe"; const char r299[] PROGMEM = "WET0WEST,M3.5.0/1,M10.5.0";
const char z300[] PROGMEM = "Atlantic/Madeira"; const char r300[] PROGMEM = "WET0WEST,M3.5.0/1,M10.5.0";
const char z301[] PROGMEM = "Atlantic/Reykjavik"; const char r301[] PROGMEM = "GMT0";
const char z302[] PROGMEM = "Atlantic/South_Georgia"; const char r302[] PROGMEM = "<-02>2";
const char z303[] PROGMEM = "Atlantic/Stanley"; const char r303[] PROGMEM = "<-03>3";
const char z304[] PROGMEM = "Atlantic/St_Helena"; const char r304[] PROGMEM = "GMT0";
const char z305[] PROGMEM = "Australia/Adelaide"; const char r305[] PROGMEM = "ACST-9:30ACDT,M10.1.0,M4.1.0/3";
const char z306[] PROGMEM = "Australia/Brisbane"; const char r306[] PROGMEM = "AEST-10";
const char z307[] PROGMEM = "Australia/Broken_Hill"; const char r307[] PROGMEM = "ACST-9:30ACDT,M10.1.0,M4.1.0/3";
const char z308[] PROGMEM = "Australia/Currie"; const char r308[] PROGMEM = "AEST-10AEDT,M10.1.0,M4.1.0/3";
const char z309[] PROGMEM = "Australia/Darwin"; const char r309[] PROGMEM = "ACST-9:30";
const char z310[] PROGMEM = "Australia/Eucla"; const char r310[] PROGMEM = "<+0845>-8:45";
const char z311[] PROGMEM = "Australia/Hobart"; const char r311[] PROGMEM = "AEST-10AEDT,M10.1.0,M4.1.0/3";
const char z312[] PROGMEM = "Australia/Lindeman"; const char r312[] PROGMEM = "AEST-10";
const char z313[] PROGMEM = "Australia/Lord_Howe"; const char r313[] PROGMEM = "<+1030>-10:30<+11>-11,M10.1.0,M4.1.0";
const char z314[] PROGMEM = "Australia/Melbourne"; const char r314[] PROGMEM = "AEST-10AEDT,M10.1.0,M4.1.0/3";
const char z315[] PROGMEM = "Australia/Perth"; const char r315[] PROGMEM = "AWST-8";
const char z316[] PROGMEM = "Australia/Sydney"; const char r316[] PROGMEM = "AEST-10AEDT,M10.1.0,M4.1.0/3";
const char z317[] PROGMEM = "Europe/Amsterdam"; const char r317[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z318[] PROGMEM = "Europe/Andorra"; const char r318[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z319[] PROGMEM = "Europe/Astrakhan"; const char r319[] PROGMEM = "<+04>-4";
const char z320[] PROGMEM = "Europe/Athens"; const char r320[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z321[] PROGMEM = "Europe/Belgrade"; const char r321[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z322[] PROGMEM = "Europe/Berlin"; const char r322[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z323[] PROGMEM = "Europe/Bratislava"; const char r323[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z324[] PROGMEM = "Europe/Brussels"; const char r324[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z325[] PROGMEM = "Europe/Bucharest"; const char r325[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z326[] PROGMEM = "Europe/Budapest"; const char r326[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z327[] PROGMEM = "Europe/Busingen"; const char r327[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z328[] PROGMEM = "Europe/Chisinau"; const char r328[] PROGMEM = "EET-2EEST,M3.5.0,M10.5.0/3";
const char z329[] PROGMEM = "Europe/Copenhagen"; const char r329[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z330[] PROGMEM = "Europe/Dublin"; const char r330[] PROGMEM = "IST-1GMT0,M10.5.0,M3.5.0/1";
const char z331[] PROGMEM = "Europe/Gibraltar"; const char r331[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z332[] PROGMEM = "Europe/Guernsey"; const char r332[] PROGMEM = "GMT0BST,M3.5.0/1,M10.5.0";
const char z333[] PROGMEM = "Europe/Helsinki"; const char r333[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z334[] PROGMEM = "Europe/Isle_of_Man"; const char r334[] PROGMEM = "GMT0BST,M3.5.0/1,M10.5.0";
const char z335[] PROGMEM = "Europe/Istanbul"; const char r335[] PROGMEM = "<+03>-3";
const char z336[] PROGMEM = "Europe/Jersey"; const char r336[] PROGMEM = "GMT0BST,M3.5.0/1,M10.5.0";
const char z337[] PROGMEM = "Europe/Kaliningrad"; const char r337[] PROGMEM = "EET-2";
const char z338[] PROGMEM = "Europe/Kiev"; const char r338[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z339[] PROGMEM = "Europe/Kirov"; const char r339[] PROGMEM = "MSK-3";
const char z340[] PROGMEM = "Europe/Lisbon"; const char r340[] PROGMEM = "WET0WEST,M3.5.0/1,M10.5.0";
const char z341[] PROGMEM = "Europe/Ljubljana"; const char r341[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z342[] PROGMEM = "Europe/London"; const char r342[] PROGMEM = "GMT0BST,M3.5.0/1,M10.5.0";
const char z343[] PROGMEM = "Europe/Luxembourg"; const char r343[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z344[] PROGMEM = "Europe/Madrid"; const char r344[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z345[] PROGMEM = "Europe/Malta"; const char r345[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z346[] PROGMEM = "Europe/Mariehamn"; const char r346[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z347[] PROGMEM = "Europe/Minsk"; const char r347[] PROGMEM = "<+03>-3";
const char z348[] PROGMEM = "Europe/Monaco"; const char r348[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z349[] PROGMEM = "Europe/Moscow"; const char r349[] PROGMEM = "MSK-3";
const char z350[] PROGMEM = "Europe/Oslo"; const char r350[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z351[] PROGMEM = "Europe/Paris"; const char r351[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z352[] PROGMEM = "Europe/Podgorica"; const char r352[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z353[] PROGMEM = "Europe/Prague"; const char r353[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z354[] PROGMEM = "Europe/Riga"; const char r354[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z355[] PROGMEM = "Europe/Rome"; const char r355[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z356[] PROGMEM = "Europe/Samara"; const char r356[] PROGMEM = "<+04>-4";
const char z357[] PROGMEM = "Europe/San_Marino"; const char r357[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z358[] PROGMEM = "Europe/Sarajevo"; const char r358[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z359[] PROGMEM = "Europe/Saratov"; const char r359[] PROGMEM = "<+04>-4";
const char z360[] PROGMEM = "Europe/Simferopol"; const char r360[] PROGMEM = "MSK-3";
const char z361[] PROGMEM = "Europe/Skopje"; const char r361[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z362[] PROGMEM = "Europe/Sofia"; const char r362[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z363[] PROGMEM = "Europe/Stockholm"; const char r363[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z364[] PROGMEM = "Europe/Tallinn"; const char r364[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z365[] PROGMEM = "Europe/Tirane"; const char r365[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z366[] PROGMEM = "Europe/Ulyanovsk"; const char r366[] PROGMEM = "<+04>-4";
const char z367[] PROGMEM = "Europe/Uzhgorod"; const char r367[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z368[] PROGMEM = "Europe/Vaduz"; const char r368[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z369[] PROGMEM = "Europe/Vatican"; const char r369[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z370[] PROGMEM = "Europe/Vienna"; const char r370[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z371[] PROGMEM = "Europe/Vilnius"; const char r371[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z372[] PROGMEM = "Europe/Volgograd"; const char r372[] PROGMEM = "MSK-3";
const char z373[] PROGMEM = "Europe/Warsaw"; const char r373[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z374[] PROGMEM = "Europe/Zagreb"; const char r374[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z375[] PROGMEM = "Europe/Zaporozhye"; const char r375[] PROGMEM = "EET-2EEST,M3.5.0/3,M10.5.0/4";
const char z376[] PROGMEM = "Europe/Zurich"; const char r376[] PROGMEM = "CET-1CEST,M3.5.0,M10.5.0/3";
const char z377[] PROGMEM = "Indian/Antananarivo"; const char r377[] PROGMEM = "EAT-3";
const char z378[] PROGMEM = "Indian/Chagos"; const char r378[] PROGMEM = "<+06>-6";
const char z379[] PROGMEM = "Indian/Christmas"; const char r379[] PROGMEM = "<+07>-7";
const char z380[] PROGMEM = "Indian/Cocos"; const char r380[] PROGMEM = "<+0630>-6:30";
const char z381[] PROGMEM = "Indian/Comoro"; const char r381[] PROGMEM = "EAT-3";
const char z382[] PROGMEM = "Indian/Kerguelen"; const char r382[] PROGMEM = "<+05>-5";
const char z383[] PROGMEM = "Indian/Mahe"; const char r383[] PROGMEM = "<+04>-4";
const char z384[] PROGMEM = "Indian/Maldives"; const char r384[] PROGMEM = "<+05>-5";
const char z385[] PROGMEM = "Indian/Mauritius"; const char r385[] PROGMEM = "<+04>-4";
const char z386[] PROGMEM = "Indian/Mayotte"; const char r386[] PROGMEM = "EAT-3";
const char z387[] PROGMEM = "Indian/Reunion"; const char r387[] PROGMEM = "<+04>-4";
const char z388[] PROGMEM = "Pacific/Apia"; const char r388[] PROGMEM = "<+13>-13";
const char z389[] PROGMEM = "Pacific/Auckland"; const char r389[] PROGMEM = "NZST-12NZDT,M9.5.0,M4.1.0/3";
const char z390[] PROGMEM = "Pacific/Bougainville"; const char r390[] PROGMEM = "<+11>-11";
const char z391[] PROGMEM = "Pacific/Chatham"; const char r391[] PROGMEM = "<+1245>-12:45<+1345>,M9.5.0/2:45,M4.1.0/3:45";
const char z392[] PROGMEM = "Pacific/Chuuk"; const char r392[] PROGMEM = "<+10>-10";
const char z393[] PROGMEM = "Pacific/Easter"; const char r393[] PROGMEM = "<-06>6<-05>,M9.1.6/22,M4.1.6/22";
const char z394[] PROGMEM = "Pacific/Efate"; const char r394[] PROGMEM = "<+11>-11";
const char z395[] PROGMEM = "Pacific/Enderbury"; const char r395[] PROGMEM = "<+13>-13";
const char z396[] PROGMEM = "Pacific/Fakaofo"; const char r396[] PROGMEM = "<+13>-13";
const char z397[] PROGMEM = "Pacific/Fiji"; const char r397[] PROGMEM = "<+12>-12";
const char z398[] PROGMEM = "Pacific/Funafuti"; const char r398[] PROGMEM = "<+12>-12";
const char z399[] PROGMEM = "Pacific/Galapagos"; const char r399[] PROGMEM = "<-06>6";
const char z400[] PROGMEM = "Pacific/Gambier"; const char r400[] PROGMEM = "<-09>9";
const char z401[] PROGMEM = "Pacific/Guadalcanal"; const char r401[] PROGMEM = "<+11>-11";
const char z402[] PROGMEM = "Pacific/Guam"; const char r402[] PROGMEM = "ChST-10";
const char z403[] PROGMEM = "Pacific/Honolulu"; const char r403[] PROGMEM = "HST10";
const char z404[] PROGMEM = "Pacific/Kiritimati"; const char r404[] PROGMEM = "<+14>-14";
const char z405[] PROGMEM = "Pacific/Kosrae"; const char r405[] PROGMEM = "<+11>-11";
const char z406[] PROGMEM = "Pacific/Kwajalein"; const char r406[] PROGMEM = "<+12>-12";
const char z407[] PROGMEM = "Pacific/Majuro"; const char r407[] PROGMEM = "<+12>-12";
const char z408[] PROGMEM = "Pacific/Marquesas"; const char r408[] PROGMEM = "<-0930>9:30";
const char z409[] PROGMEM = "Pacific/Midway"; const char r409[] PROGMEM = "SST11";
const char z410[] PROGMEM = "Pacific/Nauru"; const char r410[] PROGMEM = "<+12>-12";
const char z411[] PROGMEM = "Pacific/Niue"; const char r411[] PROGMEM = "<-11>11";
const char z412[] PROGMEM = "Pacific/Norfolk"; const char r412[] PROGMEM = "<+11>-11<+12>,M10.1.0,M4.1.0/3";
const char z413[] PROGMEM = "Pacific/Noumea"; const char r413[] PROGMEM = "<+11>-11";
const char z414[] PROGMEM = "Pacific/Pago_Pago"; const char r414[] PROGMEM = "SST11";
const char z415[] PROGMEM = "Pacific/Palau"; const char r415[] PROGMEM = "<+09>-9";
const char z416[] PROGMEM = "Pacific/Pitcairn"; const char r416[] PROGMEM = "<-08>8";
const char z417[] PROGMEM = "Pacific/Pohnpei"; const char r417[] PROGMEM = "<+11>-11";
const char z418[] PROGMEM = "Pacific/Port_Moresby"; const char r418[] PROGMEM = "<+10>-10";
const char z419[] PROGMEM = "Pacific/Rarotonga"; const char r419[] PROGMEM = "<-10>10";
const char z420[] PROGMEM = "Pacific/Saipan"; const char r420[] PROGMEM = "ChST-10";
const char z421[] PROGMEM = "Pacific/Tahiti"; const char r421[] PROGMEM = "<-10>10";
const char z422[] PROGMEM = "Pacific/Tarawa"; const char r422[] PROGMEM = "<+12>-12";
const char z423[] PROGMEM = "Pacific/Tongatapu"; const char r423[] PROGMEM = "<+13>-13";
const char z424[] PROGMEM = "Pacific/Wake"; const char r424[] PROGMEM = "<+12>-12";
const char z425[] PROGMEM = "Pacific/Wallis"; const char r425[] PROGMEM = "<+12>-12";
const char z426[] PROGMEM = "Etc/GMT"; const char r426[] PROGMEM = "GMT0";
const char z427[] PROGMEM = "Etc/GMT-0"; const char r427[] PROGMEM = "GMT0";
const char z428[] PROGMEM = "Etc/GMT-1"; const char r428[] PROGMEM = "<+01>-1";
const char z429[] PROGMEM = "Etc/GMT-2"; const char r429[] PROGMEM = "<+02>-2";
const char z430[] PROGMEM = "Etc/GMT-3"; const char r430[] PROGMEM = "<+03>-3";
const char z431[] PROGMEM = "Etc/GMT-4"; const char r431[] PROGMEM = "<+04>-4";
const char z432[] PROGMEM = "Etc/GMT-5"; const char r432[] PROGMEM = "<+05>-5";
const char z433[] PROGMEM = "Etc/GMT-6"; const char r433[] PROGMEM = "<+06>-6";
const char z434[] PROGMEM = "Etc/GMT-7"; const char r434[] PROGMEM = "<+07>-7";
const char z435[] PROGMEM = "Etc/GMT-8"; const char r435[] PROGMEM = "<+08>-8";
const char z436[] PROGMEM = "Etc/GMT-9"; const char r436[] PROGMEM = "<+09>-9";
const char z437[] PROGMEM = "Etc/GMT-10"; const char r437[] PROGMEM = "<+10>-10";
const char z438[] PROGMEM = "Etc/GMT-11"; const char r438[] PROGMEM = "<+11>-11";
const char z439[] PROGMEM = "Etc/GMT-12"; const char r439[] PROGMEM = "<+12>-12";
const char z440[] PROGMEM = "Etc/GMT-13"; const char r440[] PROGMEM = "<+13>-13";
const char z441[] PROGMEM = "Etc/GMT-14"; const char r441[] PROGMEM = "<+14>-14";
const char z442[] PROGMEM = "Etc/GMT0"; const char r442[] PROGMEM = "GMT0";
const char z443[] PROGMEM = "Etc/GMT+0"; const char r443[] PROGMEM = "GMT0";
const char z444[] PROGMEM = "Etc/GMT+1"; const char r444[] PROGMEM = "<-01>1";
const char z445[] PROGMEM = "Etc/GMT+2"; const char r445[] PROGMEM = "<-02>2";
const char z446[] PROGMEM = "Etc/GMT+3"; const char r446[] PROGMEM = "<-03>3";
const char z447[] PROGMEM = "Etc/GMT+4"; const char r447[] PROGMEM = "<-04>4";
const char z448[] PROGMEM = "Etc/GMT+5"; const char r448[] PROGMEM = "<-05>5";
const char z449[] PROGMEM = "Etc/GMT+6"; const char r449[] PROGMEM = "<-06>6";
const char z450[] PROGMEM = "Etc/GMT+7"; const char r450[] PROGMEM = "<-07>7";
const char z451[] PROGMEM = "Etc/GMT+8"; const char r451[] PROGMEM = "<-08>8";
const char z452[] PROGMEM = "Etc/GMT+9"; const char r452[] PROGMEM = "<-09>9";
const char z453[] PROGMEM = "Etc/GMT+10"; const char r453[] PROGMEM = "<-10>10";
const char z454[] PROGMEM = "Etc/GMT+11"; const char r454[] PROGMEM = "<-11>11";
const char z455[] PROGMEM = "Etc/GMT+12"; const char r455[] PROGMEM = "<-12>12";
const char z456[] PROGMEM = "Etc/UCT"; const char r456[] PROGMEM = "UTC0";
const char z457[] PROGMEM = "Etc/UTC"; const char r457[] PROGMEM = "UTC0";
const char z458[] PROGMEM = "Etc/Greenwich"; const char r458[] PROGMEM = "GMT0";
const char z459[] PROGMEM = "Etc/Universal"; const char r459[] PROGMEM = "UTC0";
const char z460[] PROGMEM = "Etc/Zulu"; const char r460[] PROGMEM = "UTC0";

const TimezoneEntry tzTable[] PROGMEM = {
  {z000, r000},
  {z001, r001},
  {z002, r002},
  {z003, r003},
  {z004, r004},
  {z005, r005},
  {z006, r006},
  {z007, r007},
  {z008, r008},
  {z009, r009},
  {z010, r010},
  {z011, r011},
  {z012, r012},
  {z013, r013},
  {z014, r014},
  {z015, r015},
  {z016, r016},
  {z017, r017},
  {z018, r018},
  {z019, r019},
  {z020, r020},
  {z021, r021},
  {z022, r022},
  {z023, r023},
  {z024, r024},
  {z025, r025},
  {z026, r026},
  {z027, r027},
  {z028, r028},
  {z029, r029},
  {z030, r030},
  {z031, r031},
  {z032, r032},
  {z033, r033},
  {z034, r034},
  {z035, r035},
  {z036, r036},
  {z037, r037},
  {z038, r038},
  {z039, r039},
  {z040, r040},
  {z041, r041},
  {z042, r042},
  {z043, r043},
  {z044, r044},
  {z045, r045},
  {z046, r046},
  {z047, r047},
  {z048, r048},
  {z049, r049},
  {z050, r050},
  {z051, r051},
  {z052, r052},
  {z053, r053},
  {z054, r054},
  {z055, r055},
  {z056, r056},
  {z057, r057},
  {z058, r058},
  {z059, r059},
  {z060, r060},
  {z061, r061},
  {z062, r062},
  {z063, r063},
  {z064, r064},
  {z065, r065},
  {z066, r066},
  {z067, r067},
  {z068, r068},
  {z069, r069},
  {z070, r070},
  {z071, r071},
  {z072, r072},
  {z073, r073},
  {z074, r074},
  {z075, r075},
  {z076, r076},
  {z077, r077},
  {z078, r078},
  {z079, r079},
  {z080, r080},
  {z081, r081},
  {z082, r082},
  {z083, r083},
  {z084, r084},
  {z085, r085},
  {z086, r086},
  {z087, r087},
  {z088, r088},
  {z089, r089},
  {z090, r090},
  {z091, r091},
  {z092, r092},
  {z093, r093},
  {z094, r094},
  {z095, r095},
  {z096, r096},
  {z097, r097},
  {z098, r098},
  {z099, r099},
  {z100, r100},
  {z101, r101},
  {z102, r102},
  {z103, r103},
  {z104, r104},
  {z105, r105},
  {z106, r106},
  {z107, r107},
  {z108, r108},
  {z109, r109},
  {z110, r110},
  {z111, r111},
  {z112, r112},
  {z113, r113},
  {z114, r114},
  {z115, r115},
  {z116, r116},
  {z117, r117},
  {z118, r118},
  {z119, r119},
  {z120, r120},
  {z121, r121},
  {z122, r122},
  {z123, r123},
  {z124, r124},
  {z125, r125},
  {z126, r126},
  {z127, r127},
  {z128, r128},
  {z129, r129},
  {z130, r130},
  {z131, r131},
  {z132, r132},
  {z133, r133},
  {z134, r134},
  {z135, r135},
  {z136, r136},
  {z137, r137},
  {z138, r138},
  {z139, r139},
  {z140, r140},
  {z141, r141},
  {z142, r142},
  {z143, r143},
  {z144, r144},
  {z145, r145},
  {z146, r146},
  {z147, r147},
  {z148, r148},
  {z149, r149},
  {z150, r150},
  {z151, r151},
  {z152, r152},
  {z153, r153},
  {z154, r154},
  {z155, r155},
  {z156, r156},
  {z157, r157},
  {z158, r158},
  {z159, r159},
  {z160, r160},
  {z161, r161},
  {z162, r162},
  {z163, r163},
  {z164, r164},
  {z165, r165},
  {z166, r166},
  {z167, r167},
  {z168, r168},
  {z169, r169},
  {z170, r170},
  {z171, r171},
  {z172, r172},
  {z173, r173},
  {z174, r174},
  {z175, r175},
  {z176, r176},
  {z177, r177},
  {z178, r178},
  {z179, r179},
  {z180, r180},
  {z181, r181},
  {z182, r182},
  {z183, r183},
  {z184, r184},
  {z185, r185},
  {z186, r186},
  {z187, r187},
  {z188, r188},
  {z189, r189},
  {z190, r190},
  {z191, r191},
  {z192, r192},
  {z193, r193},
  {z194, r194},
  {z195, r195},
  {z196, r196},
  {z197, r197},
  {z198, r198},
  {z199, r199},
  {z200, r200},
  {z201, r201},
  {z202, r202},
  {z203, r203},
  {z204, r204},
  {z205, r205},
  {z206, r206},
  {z207, r207},
  {z208, r208},
  {z209, r209},
  {z210, r210},
  {z211, r211},
  {z212, r212},
  {z213, r213},
  {z214, r214},
  {z215, r215},
  {z216, r216},
  {z217, r217},
  {z218, r218},
  {z219, r219},
  {z220, r220},
  {z221, r221},
  {z222, r222},
  {z223, r223},
  {z224, r224},
  {z225, r225},
  {z226, r226},
  {z227, r227},
  {z228, r228},
  {z229, r229},
  {z230, r230},
  {z231, r231},
  {z232, r232},
  {z233, r233},
  {z234, r234},
  {z235, r235},
  {z236, r236},
  {z237, r237},
  {z238, r238},
  {z239, r239},
  {z240, r240},
  {z241, r241},
  {z242, r242},
  {z243, r243},
  {z244, r244},
  {z245, r245},
  {z246, r246},
  {z247, r247},
  {z248, r248},
  {z249, r249},
  {z250, r250},
  {z251, r251},
  {z252, r252},
  {z253, r253},
  {z254, r254},
  {z255, r255},
  {z256, r256},
  {z257, r257},
  {z258, r258},
  {z259, r259},
  {z260, r260},
  {z261, r261},
  {z262, r262},
  {z263, r263},
  {z264, r264},
  {z265, r265},
  {z266, r266},
  {z267, r267},
  {z268, r268},
  {z269, r269},
  {z270, r270},
  {z271, r271},
  {z272, r272},
  {z273, r273},
  {z274, r274},
  {z275, r275},
  {z276, r276},
  {z277, r277},
  {z278, r278},
  {z279, r279},
  {z280, r280},
  {z281, r281},
  {z282, r282},
  {z283, r283},
  {z284, r284},
  {z285, r285},
  {z286, r286},
  {z287, r287},
  {z288, r288},
  {z289, r289},
  {z290, r290},
  {z291, r291},
  {z292, r292},
  {z293, r293},
  {z294, r294},
  {z295, r295},
  {z296, r296},
  {z297, r297},
  {z298, r298},
  {z299, r299},
  {z300, r300},
  {z301, r301},
  {z302, r302},
  {z303, r303},
  {z304, r304},
  {z305, r305},
  {z306, r306},
  {z307, r307},
  {z308, r308},
  {z309, r309},
  {z310, r310},
  {z311, r311},
  {z312, r312},
  {z313, r313},
  {z314, r314},
  {z315, r315},
  {z316, r316},
  {z317, r317},
  {z318, r318},
  {z319, r319},
  {z320, r320},
  {z321, r321},
  {z322, r322},
  {z323, r323},
  {z324, r324},
  {z325, r325},
  {z326, r326},
  {z327, r327},
  {z328, r328},
  {z329, r329},
  {z330, r330},
  {z331, r331},
  {z332, r332},
  {z333, r333},
  {z334, r334},
  {z335, r335},
  {z336, r336},
  {z337, r337},
  {z338, r338},
  {z339, r339},
  {z340, r340},
  {z341, r341},
  {z342, r342},
  {z343, r343},
  {z344, r344},
  {z345, r345},
  {z346, r346},
  {z347, r347},
  {z348, r348},
  {z349, r349},
  {z350, r350},
  {z351, r351},
  {z352, r352},
  {z353, r353},
  {z354, r354},
  {z355, r355},
  {z356, r356},
  {z357, r357},
  {z358, r358},
  {z359, r359},
  {z360, r360},
  {z361, r361},
  {z362, r362},
  {z363, r363},
  {z364, r364},
  {z365, r365},
  {z366, r366},
  {z367, r367},
  {z368, r368},
  {z369, r369},
  {z370, r370},
  {z371, r371},
  {z372, r372},
  {z373, r373},
  {z374, r374},
  {z375, r375},
  {z376, r376},
  {z377, r377},
  {z378, r378},
  {z379, r379},
  {z380, r380},
  {z381, r381},
  {z382, r382},
  {z383, r383},
  {z384, r384},
  {z385, r385},
  {z386, r386},
  {z387, r387},
  {z388, r388},
  {z389, r389},
  {z390, r390},
  {z391, r391},
  {z392, r392},
  {z393, r393},
  {z394, r394},
  {z395, r395},
  {z396, r396},
  {z397, r397},
  {z398, r398},
  {z399, r399},
  {z400, r400},
  {z401, r401},
  {z402, r402},
  {z403, r403},
  {z404, r404},
  {z405, r405},
  {z406, r406},
  {z407, r407},
  {z408, r408},
  {z409, r409},
  {z410, r410},
  {z411, r411},
  {z412, r412},
  {z413, r413},
  {z414, r414},
  {z415, r415},
  {z416, r416},
  {z417, r417},
  {z418, r418},
  {z419, r419},
  {z420, r420},
  {z421, r421},
  {z422, r422},
  {z423, r423},
  {z424, r424},
  {z425, r425},
  {z426, r426},
  {z427, r427},
  {z428, r428},
  {z429, r429},
  {z430, r430},
  {z431, r431},
  {z432, r432},
  {z433, r433},
  {z434, r434},
  {z435, r435},
  {z436, r436},
  {z437, r437},
  {z438, r438},
  {z439, r439},
  {z440, r440},
  {z441, r441},
  {z442, r442},
  {z443, r443},
  {z444, r444},
  {z445, r445},
  {z446, r446},
  {z447, r447},
  {z448, r448},
  {z449, r449},
  {z450, r450},
  {z451, r451},
  {z452, r452},
  {z453, r453},
  {z454, r454},
  {z455, r455},
  {z456, r456},
  {z457, r457},
  {z458, r458},
  {z459, r459},
  {z460, r460},
};

const int tzCount = 461;

//const int tzCount = sizeof(tzTable) / sizeof(tzTable[0]);

const char* findTzRule(const char* searchName) {
  int low = 0;
  int high = tzCount - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;
    
    // Get pointer to the name at the midpoint from Flash
    const char* midName = (const char*)pgm_read_ptr(&(tzTable[mid].zoneName));
    
    int cmp = strcmp(searchName, midName);

    if (cmp == 0) {
      // Found it! Return the pointer to the rule
      return (const char*)pgm_read_ptr(&(tzTable[mid].posixRule));
    } else if (cmp < 0) {
      high = mid - 1;
    } else {
      low = mid + 1;
    }
  }
  return NULL; // Not found
}

