#ifndef __GBA_SLOT_H
#define __GBA_SLOT_H

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


/* PCB */
enum
{
	GBA_STD = 0,
	GBA_SRAM,
	GBA_EEPROM,
	GBA_EEPROM4,
	GBA_EEPROM64,
	GBA_FLASH,
	GBA_FLASH512,
	GBA_FLASH1M
};


// ======================> device_gba_cart_interface

class device_gba_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_gba_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_gba_cart_interface();

	// reading and writing
	virtual DECLARE_READ32_MEMBER(read_rom) { return 0xffffffff; }
	virtual DECLARE_READ32_MEMBER(read_ram) { return 0xffffffff; }
	virtual DECLARE_WRITE32_MEMBER(write_ram) {};

	void rom_alloc(UINT32 size, const char *tag);
	void nvram_alloc(UINT32 size);
	UINT32* get_rom_base() { return m_rom; }
	UINT32* get_nvram_base() { return m_nvram; }
	UINT32 get_rom_size() { return m_rom_size; }
	UINT32 get_nvram_size() { return m_nvram.bytes(); }
	void set_rom_size(UINT32 val) { m_rom_size = val; }

	void save_nvram()   { device().save_item(NAME(m_nvram)); }

	// internal state
	UINT32 *m_rom;  // this points to the cart rom region
	UINT32 m_rom_size;  // this is the actual game size, not the rom region size!
	dynamic_array<UINT32> m_nvram;
};


// ======================> gba_cart_slot_device

class gba_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	gba_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~gba_cart_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete();

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);

	int get_type() { return m_type; }
	int get_cart_type(UINT8 *ROM, UINT32 len);

	void setup_ram(UINT8 banks);
	void internal_header_logging(UINT8 *ROM, UINT32 len);

	void save_nvram()   { if (m_cart && m_cart->get_nvram_size()) m_cart->save_nvram(); }

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return "gba_cart"; }
	virtual const char *file_extensions() const { return "gba,bin"; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	// reading and writing
	virtual DECLARE_READ32_MEMBER(read_rom);
	virtual DECLARE_READ32_MEMBER(read_ram);
	virtual DECLARE_WRITE32_MEMBER(write_ram);


protected:

	int m_type;
	device_gba_cart_interface*       m_cart;
};



// device type definition
extern const device_type GBA_CART_SLOT;


/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define GBASLOT_ROM_REGION_TAG ":cart:rom"

#define MCFG_GBA_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, GBA_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)



//------------------------------------------------------------------------
//
// Misc structs to attempt NVRAM identification when loading from fullpath
//
//------------------------------------------------------------------------


#define GBA_CHIP_EEPROM     (1 << 0)
#define GBA_CHIP_SRAM       (1 << 1)
#define GBA_CHIP_FLASH      (1 << 2)
#define GBA_CHIP_FLASH_1M   (1 << 3)
#define GBA_CHIP_RTC        (1 << 4)
#define GBA_CHIP_FLASH_512  (1 << 5)
#define GBA_CHIP_EEPROM_64K (1 << 6)
#define GBA_CHIP_EEPROM_4K  (1 << 7)


struct gba_chip_fix_conflict_item
{
	char game_code[5];
	UINT32 chip;
};

static const gba_chip_fix_conflict_item gba_chip_fix_conflict_list[] =
{
	{ "ABFJ", GBA_CHIP_SRAM       }, // 0059 - Breath of Fire - Ryuu no Senshi (JPN)
	{ "AHMJ", GBA_CHIP_EEPROM_4K  }, // 0364 - Dai-Mahjong (JPN)
	{ "A2GJ", GBA_CHIP_EEPROM_64K }, // 0399 - Advance GT2 (JPN)
	{ "AK9E", GBA_CHIP_EEPROM_4K  }, // 0479 - Medabots AX - Rokusho Version (USA)
	{ "AK8E", GBA_CHIP_EEPROM_4K  }, // 0480 - Medabots AX - Metabee Version (USA)
	{ "AK9P", GBA_CHIP_EEPROM_4K  }, // 0515 - Medabots AX - Rokusho Version (EUR)
	{ "AGIJ", GBA_CHIP_EEPROM_4K  }, // 0548 - Medarot G - Kuwagata Version (JPN)
	{ "A3DJ", GBA_CHIP_EEPROM_4K  }, // 0567 - Disney Sports - American Football (JPN)
	{ "AF7J", GBA_CHIP_EEPROM_64K }, // 0605 - Tokimeki Yume Series 1 - Ohanaya-san ni Narou! (JPN)
	{ "AH7J", GBA_CHIP_EEPROM_64K }, // 0617 - Nakayoshi Pet Advance Series 1 - Kawaii Hamster (JPN)
	{ "AGHJ", GBA_CHIP_EEPROM_4K  }, // 0620 - Medarot G - Kabuto Version (JPN)
	{ "AR8E", GBA_CHIP_SRAM       }, // 0727 - Rocky (USA)
	{ "ALUE", GBA_CHIP_EEPROM_4K  }, // 0751 - Super Monkey Ball Jr. (USA)
	{ "A3DE", GBA_CHIP_EEPROM_4K  }, // 0800 - Disney Sports - Football (USA)
	{ "A87J", GBA_CHIP_EEPROM_64K }, // 0817 - Ohanaya-San Monogatari GBA (JPN)
	{ "A56J", GBA_CHIP_EEPROM_64K }, // 0827 - DokiDoki Cooking Series 1 - Komugi-chan no Happy Cake (JPN)
	{ "AUSJ", GBA_CHIP_FLASH      }, // 0906 - One Piece - Mezase! King of Berries (JPN)
	{ "ANTJ", GBA_CHIP_SRAM       }, // 0950 - Nippon Pro Mahjong Renmei Kounin - Tetsuman Advance (JPN)
	{ "A8OJ", GBA_CHIP_EEPROM_64K }, // 0988 - DokiDoki Cooking Series 2 - Gourmet Kitchen - Suteki na Obentou (JPN)
	{ "AK8P", GBA_CHIP_EEPROM_4K  }, // 1022 - Medabots AX - Metabee Version (EUR)
	{ "A6OJ", GBA_CHIP_EEPROM_64K }, // 1092 - Onimusha Tactics (JPN)
	{ "A6OE", GBA_CHIP_EEPROM_64K }, // 1241 - Onimusha Tactics (USA)
	{ "A6OP", GBA_CHIP_EEPROM_64K }, // 1288 - Onimusha Tactics (EUR)
	{ "BKME", GBA_CHIP_EEPROM_4K  }, // 1545 - Kim Possible 2 - Drakken's Demise (USA)
	{ "BDKJ", GBA_CHIP_EEPROM_64K }, // 1555 - Digi Communication 2 in 1 Datou! Black Gemagema Dan (JPN)
	{ "BR4J", GBA_CHIP_FLASH      }, // 1586 - Rockman EXE 4.5 - Real Operation (JPN)
	{ "BG8J", GBA_CHIP_EEPROM_64K }, // 1853 - Ganbare! Dodge Fighters (JPN)
	{ "AROP", GBA_CHIP_EEPROM_4K  }, // 1862 - Rocky (EUR)
//  "A2YE" - 1906 - Top Gun - Combat Zones (USA) - multiple NVRAM chips detected, but none present (protection against emu?)
	{ "BKMJ", GBA_CHIP_EEPROM_4K  }, // 2039 - Kim Possible (JPN)
	{ "BKEJ", GBA_CHIP_EEPROM_64K }, // 2047 - Konjiki no Gashbell - The Card Battle for GBA (JPN)
	{ "BKMP", GBA_CHIP_EEPROM_4K  }, // 2297 - Kim Possible 2 - Drakken's Demise (EUR)
	{ "BUHJ", GBA_CHIP_EEPROM_4K  }, // 2311 - Ueki no Housoku Shinki Sakuretsu! Nouryokumono Battle (JPN)
	{ "BYUJ", GBA_CHIP_EEPROM_64K }, // 2322 - Yggdra Union (JPN)
};

struct gba_chip_fix_eeprom_item
{
	char game_code[5];
};

static const gba_chip_fix_eeprom_item gba_chip_fix_eeprom_list[] =
{
	// gba scan no. 7
	{ "AKTJ" }, // 0145 - Hello Kitty Collection - Miracle Fashion Maker (JPN)
	{ "AISP" }, // 0185 - International Superstar Soccer (EUR)
	{ "AKGP" }, // 0204 - Mech Platoon (EUR)
	{ "AX2E" }, // 0207 - Dave Mirra Freestyle BMX 2 (USA)
	{ "AASJ" }, // 0234 - World Advance Soccer - Shouri heno Michi (JPN)
	{ "AA2J" }, // 0237 - Super Mario World - Super Mario Advance 2 (JPN)
	{ "AJWJ" }, // 0242 - Jikkyou World Soccer Pocket (JPN)
	{ "AABE" }, // 0244 - American Bass Challenge (USA)
	{ "AWXJ" }, // 0254 - ESPN Winter X-Games Snowboarding 2002 (JPN)
	{ "AALJ" }, // 0259 - Kidou Tenshi Angelic Layer - Misaki to Yume no Tenshi-tachi (JPN)
	{ "AKGE" }, // 0263 - Mech Platoon (USA)
	{ "AGLJ" }, // 0273 - Tomato Adventure (JPN)
	{ "AWIJ" }, // 0274 - Hyper Sports 2002 Winter (JPN)
	{ "APNJ" }, // 0286 - Pinky Monkey Town (JPN)
	{ "AA2E" }, // 0288 - Super Mario World - Super Mario Advance 2 (USA)
	{ "AX2P" }, // 0293 - Dave Mirra Freestyle BMX 2 (EUR)
	{ "AMGP" }, // 0296 - ESPN Great Outdoor Games - Bass Tournament (EUR)
	{ "AMHJ" }, // 0308 - Bomberman Max 2 - Bomberman Version (JPN)
	{ "AGNJ" }, // 0311 - Goemon - New Age Shutsudou! (JPN)
	{ "AMYJ" }, // 0324 - Bomberman Max 2 - Max Version (JPN)
	{ "AT3E" }, // 0326 - Tony Hawk's Pro Skater 3 (USA)
	{ "AHHE" }, // 0327 - High Heat - Major League Baseball 2003 (USA)
	{ "ANLE" }, // 0328 - NHL 2002 (USA)
	{ "AAGJ" }, // 0345 - Angelique (JPN)
	{ "ABJP" }, // 0351 - Broken Sword - The Shadow of the Templars (EUR)
	{ "AKVJ" }, // 0357 - K-1 Pocket Grand Prix (JPN)
	{ "AKGJ" }, // 0361 - Kikaika Guntai - Mech Platoon (JPN)
	{ "ADDJ" }, // 0362 - Diadroids World - Evil Teikoku no Yabou (JPN)
	{ "ABJE" }, // 0365 - Broken Sword - The Shadow of the Templars (USA)
	{ "AABP" }, // 0380 - Super Black Bass Advance (EUR)
	{ "AA2P" }, // 0390 - Super Mario World - Super Mario Advance 2 (EUR)
	{ "A2GJ" }, // 0399 - Advance GT2 (JPN)
	{ "AEWJ" }, // 0400 - Ui-Ire - World Soccer Winning Eleven (JPN)
	{ "ADPJ" }, // 0417 - Doraemon - Dokodemo Walker (JPN)
	{ "AN5J" }, // 0420 - Kawa no Nushi Tsuri 5 - Fushigi no Mori Kara (JPN)
	{ "ACBJ" }, // 0421 - Gekitou! Car Battler Go!! (JPN)
	{ "AHIJ" }, // 0426 - Hitsuji no Kimochi (JPN)
	{ "ATFP" }, // 0429 - Alex Ferguson's Player Manager 2002 (EUR)
	{ "AFUJ" }, // 0431 - Youkaidou (JPN)
	{ "AEPP" }, // 0435 - Sheep (EUR)
	{ "AMHE" }, // 0442 - Bomberman Max 2 - Blue Advance (USA)
	{ "AMYE" }, // 0443 - Bomberman Max 2 - Red Advance (USA)
	{ "AT3F" }, // 0457 - Tony Hawk's Pro Skater 3 (FRA)
	{ "ARJJ" }, // 0497 - Custom Robo GX (JPN)
	{ "AFCJ" }, // 0521 - RockMan & Forte (JPN)
	{ "ANJE" }, // 0528 - Madden NFL 2003 (USA)
	{ "AN7J" }, // 0533 - Famista Advance (JPN)
	{ "ATYJ" }, // 0540 - Gambler Densetsu Tetsuya - Yomigaeru Densetsu (JPN)
	{ "AXBJ" }, // 0551 - Black Matrix Zero (JPN)
	{ "A3AE" }, // 0580 - Yoshi's Island - Super Mario Advance 3 (USA)
	{ "A3AJ" }, // 0582 - Super Mario Advance 3 (JPN)
	{ "AZUJ" }, // 0595 - Street Fighter Zero 3 - Upper (JPN)
	{ "ALOE" }, // 0600 - The Lord of the Rings - The Fellowship of the Ring (USA)
	{ "A2SE" }, // 0602 - Spyro 2 - Season of Flame (USA)
	{ "AF7J" }, // 0605 - Tokimeki Yume Series 1 - Ohanaya-san ni Narou! (JPN)
	{ "A3AP" }, // 0610 - Yoshi's Island - Super Mario Advance 3 (EUR)
	{ "AH7J" }, // 0617 - Nakayoshi Pet Advance Series 1 - Kawaii Hamster (JPN)
	{ "AI7J" }, // 0618 - Nakayoshi Pet Advance Series 2 - Kawaii Koinu (JPN)
	{ "AN3J" }, // 0619 - Nakayoshi Pet Advance Series 3 - Kawaii Koneko (JPN)
	{ "AAPJ" }, // 0632 - Metalgun Slinger (JPN)
	{ "A2JJ" }, // 0643 - J.League - Winning Eleven Advance 2002 (JPN)
	{ "AHXJ" }, // 0649 - High Heat - Major League Baseball 2003 (JPN)
	{ "AHAJ" }, // 0651 - Hamster Paradise Advance (JPN)
	{ "APUJ" }, // 0653 - PukuPuku Tennen Kairanban (JPN)
	{ "A2SP" }, // 0673 - Spyro 2 - Season of Flame (EUR)
	{ "AN9J" }, // 0675 - Tales of the World - Narikiri Dungeon 2 (JPN)
	{ "ACBE" }, // 0683 - Car Battler Joe (USA)
	{ "AT6E" }, // 0693 - Tony Hawk's Pro Skater 4 (USA)
	{ "ALOP" }, // 0702 - The Lord of the Rings - The Fellowship of the Ring (EUR)
	{ "A63J" }, // 0710 - Kawaii Pet Shop Monogatari 3 (JPN)
	{ "AAXJ" }, // 0748 - Fantastic Maerchen - Cake-yasan Monogatari (JPN)
	{ "AZLE" }, // 0763 - The Legend of Zelda - A Link to the Past & Four Swords (USA)
	{ "AZUP" }, // 0765 - Street Fighter Alpha 3 - Upper (EUR)
	{ "AJKJ" }, // 0769 - Jikkyou World Soccer Pocket 2 (JPN)
	{ "AB3E" }, // 0781 - Dave Mirra Freestyle BMX 3 (USA)
	{ "A2IJ" }, // 0791 - Magi Nation (JPN)
	{ "AK7J" }, // 0792 - Klonoa Heroes - Densetsu no Star Medal (JPN)
	{ "A2HJ" }, // 0794 - Hajime no Ippo - The Fighting! (JPN)
	{ "ALNE" }, // 0795 - Lunar Legend (USA)
	{ "AUCJ" }, // 0808 - Uchuu Daisakusen Choco Vader - Uchuu Kara no Shinryakusha (JPN)
	{ "A59J" }, // 0809 - Toukon Heat (JPN)
	{ "ALJE" }, // 0815 - Sea Trader - Rise of Taipan (USA)
	{ "A87J" }, // 0817 - Ohanaya-San Monogatari GBA (JPN)
	{ "A56J" }, // 0827 - DokiDoki Cooking Series 1 - Komugi-chan no Happy Cake (JPN)
	{ "AHZJ" }, // 0830 - Higanbana (JPN)
	{ "A8BP" }, // 0832 - Medabots - Metabee Version (EUR)
	{ "A2OJ" }, // 0833 - K-1 Pocket Grand Prix 2 (JPN)
	{ "AY2P" }, // 0843 - International Superstar Soccer Advance (EUR)
	{ "ANSJ" }, // 0845 - Marie, Elie & Anis no Atelier - Soyokaze Kara no Dengon (JPN)
	{ "ACOJ" }, // 0865 - Manga-ka Debut Monogatari (JPN)
	{ "AZLP" }, // 0870 - The Legend of Zelda - A Link to the Past & Four Swords (EUR)
	{ "AWKJ" }, // 0879 - Wagamama Fairy Mirumo de Pon! - Ougon Maracas no Densetsu (JPN)
	{ "AZUE" }, // 0886 - Street Fighter Alpha 3 - Upper (USA)
	{ "AZLJ" }, // 0887 - Zelda no Densetsu - Kamigami no Triforce & 4tsu no Tsurugi (JPN)
	{ "A6ME" }, // 0889 - MegaMan & Bass (USA)
	{ "A64J" }, // 0915 - Shimura Ken no Baka Tonosama (JPN)
	{ "A9HJ" }, // 0917 - Dragon Quest Monsters - Caravan Heart (JPN)
	{ "AMHP" }, // 0929 - Bomberman Max 2 - Blue Advance (EUR)
	{ "AMYP" }, // 0930 - Bomberman Max 2 - Red Advance (EUR)
	{ "AMGJ" }, // 0943 - Exciting Bass (JPN)
	{ "A5KJ" }, // 0946 - Medarot 2 Core - Kabuto Version (JPN)
	{ "A4LJ" }, // 0949 - Sylvania Family 4 - Meguru Kisetsu no Tapestry (JPN)
	{ "A2VJ" }, // 0955 - Kisekko Gurumi - Chesty to Nuigurumi-tachi no Mahou no Bouken (JPN)
	{ "A5QJ" }, // 0956 - Medarot 2 Core - Kuwagata Version (JPN)
	{ "AZBJ" }, // 0958 - Bass Tsuri Shiyouze! (JPN)
	{ "AO2J" }, // 0961 - Oshare Princess 2 (JPN)
	{ "AB4J" }, // 0965 - Summon Night - Craft Sword Monogatari (JPN)
	{ "AZAJ" }, // 0971 - Azumanga Daiou Advance (JPN)
	{ "AF3J" }, // 0974 - Zero One (JPN)
	{ "A8OJ" }, // 0988 - DokiDoki Cooking Series 2 - Gourmet Kitchen - Suteki na Obentou (JPN)
	{ "AT3D" }, // 1016 - Tony Hawk's Pro Skater 3 (GER)
	{ "A6MP" }, // 1031 - MegaMan & Bass (EUR)
	{ "ANNJ" }, // 1032 - Gekitou Densetsu Noah - Dream Management (JPN)
	{ "AFNJ" }, // 1036 - Angel Collection - Mezase! Gakuen no Fashion Leader (JPN)
	{ "ALFP" }, // 1041 - Dragon Ball Z - The Legacy of Goku II (EUR)
	{ "A9TJ" }, // 1055 - Metal Max 2 Kai (JPN)
	{ "ALFE" }, // 1070 - Dragon Ball Z - The Legacy of Goku II (USA)
	{ "BHCJ" }, // 1074 - Hamster Monogatari Collection (JPN)
	{ "BKKJ" }, // 1075 - Minna no Shiiku Series - Boku no Kabuto-Kuwagata (JPN)
	{ "BKIJ" }, // 1083 - Nakayoshi Pet Advance Series 4 - Kawaii Koinu Kogata Inu (JPN)
	{ "BGBJ" }, // 1084 - Get! - Boku no Mushi Tsukamaete (JPN)
	{ "A82J" }, // 1085 - Hamster Paradise - Pure Heart (JPN)
	{ "U3IJ" }, // 1087 - Bokura no Taiyou - Taiyou Action RPG (JPN)
	{ "A6OJ" }, // 1092 - Onimusha Tactics (JPN)
	{ "AN8J" }, // 1102 - Tales of Phantasia (JPN)
	{ "AC4J" }, // 1104 - Meitantei Conan - Nerawareta Tantei (JPN)
	{ "A8ZJ" }, // 1108 - Shin Megami Tensei Devil Children - Puzzle de Call! (JPN)
	{ "BGMJ" }, // 1113 - Gensou Maden Saiyuuki - Hangyaku no Toushin-taishi (JPN)
	{ "BMDE" }, // 1115 - Madden NFL 2004 (USA)
	{ "BO3J" }, // 1141 - Oshare Princess 3 (JPN)
	{ "U3IE" }, // 1145 - Boktai - The Sun is in Your Hand (USA)
	{ "BMRJ" }, // 1194 - Matantei Loki Ragnarok - Gensou no Labyrinth (JPN)
	{ "BLME" }, // 1204 - Lizzie McGuire (USA)
	{ "AOWE" }, // 1208 - Spyro - Attack of the Rhynocs (USA)
	{ "BTOE" }, // 1209 - Tony Hawk's Underground (USA)
	{ "BFJE" }, // 1212 - Frogger's Journey - The Forgotten Relic (USA)
	{ "A88P" }, // 1229 - Mario & Luigi - Superstar Saga (EUR)
	{ "BEYP" }, // 1236 - Beyblade VForce - Ultimate Blader Jam (EUR)
	{ "A85J" }, // 1239 - Sanrio Puroland All Characters (JPN)
	{ "BMZJ" }, // 1240 - Zooo (JPN)
	{ "A6OE" }, // 1241 - Onimusha Tactics (USA)
	{ "AOWP" }, // 1253 - Spyro Adventure (EUR)
	{ "A88E" }, // 1260 - Mario & Luigi - Superstar Saga (USA)
	{ "BEYE" }, // 1262 - Beyblade VForce - Ultimate Blader Jam (USA)
	{ "BCME" }, // 1264 - CIMA - The Enemy (USA)
	{ "A88J" }, // 1266 - Mario & Luigi RPG (JPN)
	{ "A5CP" }, // 1269 - Sim City 2000 (EUR)
	{ "BGAJ" }, // 1277 - SD Gundam G Generation (JPN)
	{ "A6OP" }, // 1288 - Onimusha Tactics (EUR)
	{ "BLMP" }, // 1289 - Lizzie McGuire (EUR)
	{ "ASIE" }, // 1295 - The Sims - Bustin' Out (USA)
	{ "BISJ" }, // 1299 - Koinu-Chan no Hajimete no Osanpo - Koinu no Kokoro Ikusei Game (JPN)
	{ "BK3J" }, // 1305 - Card Captor Sakura - Sakura Card de Mini Game (JPN)
	{ "A4GJ" }, // 1306 - Konjiki no Gashbell!! - Unare! Yuujou no Zakeru (JPN)
	{ "BTAJ" }, // 1315 - Astro Boy - Tetsuwan Atom (JPN)
	{ "BS5J" }, // 1322 - Sylvanian Family - Yousei no Stick to Fushigi no Ki (JPN)
	{ "A5CE" }, // 1326 - Sim City 2000 (USA)
	{ "B4PJ" }, // 1342 - The Sims (JPN)
	{ "BDTJ" }, // 1383 - Downtown - Nekketsu Monogatari EX (JPN)
	{ "B08J" }, // 1391 - One Piece - Going Baseball (JPN)
	{ "AWUP" }, // 1394 - Sabre Wulf (EUR)
	{ "BRPJ" }, // 1421 - Liliput Oukoku (JPN)
	{ "BPNJ" }, // 1435 - Pika Pika Nurse Monogatari - Nurse Ikusei Game (JPN)
	{ "BP3J" }, // 1446 - Pia Carrot he Youkoso!! 3.3 (JPN)
	{ "BKCJ" }, // 1461 - Crayon Shin-Chan - Arashi no Yobu Cinema-Land no Daibouken! (JPN)
	{ "BGNJ" }, // 1464 - Battle Suit Gundam Seed - Battle Assault (JPN)
	{ "U3IP" }, // 1465 - Boktai - The Sun is in Your Hand (EUR)
	{ "BDTE" }, // 1484 - River City Ransom EX (USA)
	{ "BHTE" }, // 1485 - Harry Potter and the Prisoner of Azkaban (USA)
	{ "FZLE" }, // 1494 - Classic NES Series - The Legend of Zelda (USA)
	{ "FEBE" }, // 1499 - Classic NES Series - ExciteBike (USA)
	{ "BUCE" }, // 1505 - Ultimate Card Games (USA)
	{ "AWUE" }, // 1511 - Sabre Wulf (USA)
	{ "B2DP" }, // 1522 - Donkey Kong Country 2 (EUR)
	{ "BHTJ" }, // 1528 - Harry Potter to Azkaban no Shuujin (JPN)
	{ "A5SJ" }, // 1534 - Oshare Wanko (JPN)
	{ "B2DJ" }, // 1541 - Super Donkey Kong 2 (JPN)
	{ "BTAE" }, // 1551 - Astro Boy - Omega Factor (USA)
	{ "BKOJ" }, // 1553 - Kaiketsu Zorori to Mahou no Yuuenchi (JPN)
	{ "BDKJ" }, // 1555 - Digi Communication 2 in 1 Datou! Black Gemagema Dan (JPN)
	{ "U32J" }, // 1567 - Zoku Bokura no Taiyou - Taiyou Shounen Django (JPN)
	{ "ALFJ" }, // 1573 - Dragon Ball Z - The Legacy of Goku II - International (JPN)
	{ "BGHJ" }, // 1575 - Gakkou no Kaidan - Hyakuyobako no Fuuin (JPN)
	{ "BZOJ" }, // 1576 - Zero One SP (JPN)
	{ "BDXJ" }, // 1587 - B-Densetsu! Battle B-Daman Moero! B-Kon (JPN)
	{ "BNBJ" }, // 1589 - Himawari Doubutsu Byouin Pet no Oishasan (JPN)
	{ "BMFE" }, // 1595 - Madden NFL 2005 (USA)
	{ "FMRJ" }, // 1599 - Famicom Mini Series 23 - Metroid (JPN)
	{ "FPTJ" }, // 1600 - Famicom Mini Series 24 - Hikari Shinwa - Palutena no Kagame (JPN)
	{ "FLBJ" }, // 1601 - Famicom Mini Series 25 - The Legend of Zelda 2 - Link no Bouken (JPN)
	{ "FSDJ" }, // 1606 - Famicom Mini Series 30 - SD Gundam World - Gachapon Senshi Scramble Wars (JPN)
	{ "BSKJ" }, // 1611 - Summon Night - Craft Sword Monogatari 2 (JPN)
	{ "BG3E" }, // 1628 - Dragon Ball Z - Buu's Fury (USA)
	{ "BECJ" }, // 1644 - Angel Collection 2 - Pichimo ni Narou (JPN)
	{ "B2TE" }, // 1672 - Tony Hawk's Underground 2 (USA)
	{ "BTYE" }, // 1689 - Ty the Tasmanian Tiger 2 - Bush Rescue (USA)
	{ "BT2E" }, // 1695 - Teenage Mutant Ninja Turtles 2 - Battlenexus (USA)
	{ "U32E" }, // 1697 - Boktai 2 - Solar Boy Django (USA)
	{ "BFDJ" }, // 1708 - Fruit Mura no Doubutsu Tachi (JPN)
	{ "BPQJ" }, // 1717 - PukuPuku Tennen Kairanban - Koi no Cupid Daisakusen (JPN)
	{ "BZMJ" }, // 1721 - The Legend of Zelda - Fushigi no Boushi (JPN)
	{ "FLBE" }, // 1723 - Classic NES Series - Zelda II - The Adventure of Link (USA)
	{ "BZMP" }, // 1736 - The Legend of Zelda - The Minish Cap (EUR)
	{ "B2DE" }, // 1754 - Donkey Kong Country 2 (USA)
	{ "BT2P" }, // 1758 - Teenage Mutant Ninja Turtles 2 - Battle Nexus (EUR)
	{ "BB2E" }, // 1759 - Beyblade G-Revolution (USA)
	{ "BRGE" }, // 1761 - Yu-Yu-Hakusho - Tournament Tactics (USA)
	{ "BFJJ" }, // 1766 - Frogger - Kodaibunmei no Nazo (JPN)
	{ "BB2P" }, // 1776 - Beyblade G-Revolution (EUR)
	{ "BSFJ" }, // 1791 - Sylvania Family - Fashion Designer ni Naritai (JPN)
	{ "BPIE" }, // 1798 - It's Mr Pants (USA)
	{ "B3PJ" }, // 1809 - Pukupuku Tennen Kairanban Youkoso Illusion Land (JPN)
	{ "BHDJ" }, // 1812 - Hello Idol Debut (JPN)
	{ "BKUJ" }, // 1823 - Shingata Medarot - Kuwagata Version (JPN)
	{ "BKVJ" }, // 1824 - Shingata Medarot - Kabuto Version (JPN)
	{ "BLIJ" }, // 1825 - Little Patissier Cake no Oshiro (JPN)
	{ "B3TJ" }, // 1833 - Tales of the World - Narikiri Dungeon 3 (JPN)
	{ "B2KJ" }, // 1836 - Kiss x Kiss - Seirei Gakuen (JPN)
	{ "A9BE" }, // 1837 - Medabots - Rokusho Version (USA)
	{ "BZME" }, // 1842 - The Legend of Zelda - The Minish Cap (USA)
	{ "B8MJ" }, // 1845 - Mario Party Advance (JPN)
	{ "A8BE" }, // 1871 - Medabots - Metabee Version (USA)
	{ "BTAP" }, // 1879 - Astro Boy - Omega Factor (EUR)
	{ "FSRJ" }, // 1916 - Famicom Mini Series - Dai 2 Ji Super Robot Taisen (JPN)
	{ "B8ME" }, // 1931 - Mario Party Advance (USA)
	{ "BO8K" }, // 1938 - One Piece - Going Baseball Haejeok Yaku (KOR)
	{ "B4ZJ" }, // 1941 - Rockman Zero 4 (JPN)
	{ "BQAJ" }, // 1953 - Meitantei Conan Akatsuki no Monument (JPN)
	{ "BIPJ" }, // 1956 - One Piece - Dragon Dream (JPN)
	{ "BQBJ" }, // 1970 - Konchu Monster Battle Master (JPN)
	{ "BQSJ" }, // 1971 - Konchu Monster Battle Stadium (JPN)
	{ "BWXJ" }, // 1982 - Wanko Mix Chiwanko World (JPN)
	{ "A9TJ" }, // 1984 - Metal Max 2 - Kai Version (JPN)
	{ "U32P" }, // 1992 - Boktai 2 - Solar Boy Django (EUR)
	{ "B4RJ" }, // 2005 - Shikakui Atama wo Marukusuru Advance - Kokugo Sansu Rika Shakai (JPN)
	{ "B4KJ" }, // 2007 - Shikakui Atama wo Marukusuru Advance - Kanji Keisan (JPN)
	{ "BFCJ" }, // 2019 - Fantasic Children (JPN)
	{ "BCSP" }, // 2020 - 2 in 1 - V-Rally 3 - Stuntman (EUR)
	{ "BM2J" }, // 2024 - Momotarou Densetsu G Gold Deck wo Tsukure! (JPN)
	{ "BEJJ" }, // 2026 - Erementar Gerad (JPN)
	{ "B5AP" }, // 2034 - Crash & Spyro - Super Pack Volume 1 (EUR)
	{ "B52P" }, // 2035 - Crash & Spyro - Super Pack Volume 2 (EUR)
	{ "BFMJ" }, // 2046 - Futari wa Precure Max Heart Maji! Maji! Fight de IN Janai (JPN)
	{ "BKEJ" }, // 2047 - Konjiki no Gashbell - The Card Battle for GBA (JPN)
	{ "U33J" }, // 2048 - Shin Bokura no Taiyou - Gyakushuu no Sabata (JPN)
	{ "BHFJ" }, // 2050 - Twin Series 4 - Ham Ham Monster EX + Fantasy Puzzle Hamster Monogatari (JPN)
	{ "BMWJ" }, // 2051 - Twin Series 5 - Wan Wan Meitantei EX + Mahou no Kuni no Keaki-Okusan Monogatari (JPN)
	{ "BMZP" }, // 2055 - Zooo (EUR)
	{ "B6ME" }, // 2057 - Madden NFL 06 (USA)
	{ "BT4E" }, // 2058 - Dragon Ball GT - Transformation (USA)
	{ "B2OJ" }, // 2071 - Pro Mahjong - Tsuwamono GBA (JPN)
	{ "BX4E" }, // 2079 - 2 in 1 - Tony Hawk's Underground + Kelly Slater's Pro Surfer (USA)
	{ "BRLE" }, // 2097 - Rebelstar - Tactical Command (USA)
	{ "BX5P" }, // 2100 - Rayman - 10th Anniversary (EUR)
	{ "B4ZP" }, // 2108 - MegaMan Zero 4 (EUR)
	{ "BX5E" }, // 2123 - Rayman - 10th Anniversary (USA)
	{ "BGXJ" }, // 2143 - Gunstar Super Heroes (JPN)
	{ "B4ZE" }, // 2144 - Megaman Zero 4 (USA)
	{ "B53P" }, // 2164 - Crash & Spyro - Super Pack Volume 3 (EUR)
	{ "B26E" }, // 2169 - World Poker Tour (USA)
	{ "BH9E" }, // 2172 - Tony Hawk's American Sk8land (USA)
	{ "BHGE" }, // 2177 - Gunstar Super Heroes (USA)
	{ "BCMJ" }, // 2178 - Frontier Stories (JPN)
	{ "BUZE" }, // 2182 - Ultimate Arcade Games (USA)
	{ "BTVE" }, // 2198 - Ty the Tasmanian Tiger 3 - Night of the Quinkan (USA)
	{ "BHGP" }, // 2199 - Gunstar Future Heroes (EUR)
	{ "BH9X" }, // 2214 - Tony Hawk's American Sk8land (EUR)
	{ "BWIP" }, // 2231 - Win X Club (EUR)
	{ "BQTP" }, // 2232 - My Pet Hotel (EUR)
	{ "B4LJ" }, // 2245 - Sugar Sugar Une - Heart Gaippai! Moegi Gakuen (JPN)
	{ "B3CJ" }, // 2249 - Summon Night Craft Sword Monogatari - Hajimari no Ishi (JPN)
	{ "A4GE" }, // 2260 - ZatchBell! - Electric Arena (USA)
	{ "BLFE" }, // 2264 - 2 in 1 - Dragon Ball Z 1 and 2 (USA)
	{ "BO2J" }, // 2272 - Ochainu no Bouken Jima (JPN)
	{ "BWIE" }, // 2276 - WinX Club (USA)
	{ "BGQE" }, // 2279 - Greg Hastings' Tournament Paintball Max'd (USA)
	{ "BURE" }, // 2298 - Paws & Claws - Pet Resort (USA)
	{ "A3AC" }, // 2303 - Yaoxi Dao (CHN)
	{ "AN8E" }, // 2305 - Tales of Phantasia (USA)
	{ "BZWJ" }, // 2309 - Akagi (JPN)
	{ "BT8P" }, // 2316 - Teenage Mutant Ninja Turtles Double Pack (EUR)
	{ "BYUJ" }, // 2322 - Yggdra Union (JPN)
	{ "AN3E" }, // 2324 - Catz (USA)
	{ "AN8P" }, // 2325 - Tales of Phantasia (EUR)
	{ "AA2C" }, // 2332 - Chaoji Maliou Shijie (CHN)
	{ "BWOP" }, // 2333 - World Poker Tour (EUR)
	{ "BKCS" }, // 2334 - Shinchan - Aventuras en Cineland (ESP)
	{ "BC2J" }, // 2341 - Crayon Shin chan - Densetsu wo Yobu Omake no Miyako Shockgaan (JPN)
	{ "BUOJ" }, // 2345 - Minna no Soft Series - Numpla Advance (JPN)
	{ "AN3J" }, // 2347 - Minna no Soft Series - Kawaii Koneko (JPN)
	{ "BUOE" }, // 2366 - Dr. Sudoku (USA)
	{ "B8SE" }, // 2368 - Spyro Superpack - Season of Ice + Season of Flame (USA)
	{ "U32J" }, // 2369 - Zoku Bokura no Taiyou - Taiyou Shounen Django (v01) (JPN)
	{ "BBMJ" }, // 2388 - B-Legend! Battle B-Daman - Moero! B-Damashi!! (JPN)
	{ "B53E" }, // 2395 - Crash & Spyro Superpack - Ripto's Rampage + The Cortex Conspiracy (USA)
	{ "BH9P" }, // 2399 - Tony Hawk's American Sk8land (EUR)
	{ "BAQP" }, // 2406 - Premier Action Soccer (EUR)
	{ "AB4E" }, // 2432 - Summon Night - Swordcraft Story (USA)
	{ "BBYE" }, // 2436 - Barnyard (USA)
	{ "BDXE" }, // 2438 - Battle B-Daman (USA)
	{ "B7ME" }, // 2446 - Madden NFL 07 (USA)
	{ "BUFE" }, // 2447 - 2 Games in 1 - Dragon Ball Z - Buu's Fury + Dragon Ball GT - Transformation (USA)
	{ "BUOP" }, // 2449 - Dr. Sudoku (EUR)
	{ "BBYX" }, // 2461 - Barnyard (EUR)
	{ "BFEE" }, // 2466 - Dogz - Fashion (USA)
	{ "AN3X" }, // 2468 - Catz (EUR)
	{ "BBME" }, // 2481 - Battle B-Daman - Fire Spirits (USA)
	{ "BQZE" }, // 2487 - Avatar - The Last Airbender (USA)
	{ "BXFE" }, // 2498 - Bratz - Forever Diamondz (USA)
	{ "BT8E" }, // 2500 - Teenage Mutant Ninja Turtles Double Pack (USA)
	{ "B3YE" }, // 2504 - The Legend of Spyro - A New Beginning (USA)
	{ "BSKE" }, // 2505 - Summon Night - Swordcraft Story 2 (USA)
	{ "BHBP" }, // 2513 - Best Friends - Hunde & Katzen (EUR)
	{ "B3YP" }, // 2519 - The Legend Of Spyro - A New Beginning (EUR)
	{ "BXFD" }, // 2520 - Bratz - Forever Diamondz (GER)
	{ "BRLP" }, // 2532 - Rebelstar - Tactical Command (EUR)
	{ "BENP" }, // 2560 - Eragon (EUR)
	{ "BENE" }, // 2561 - Eragon (USA)
	{ "BYUE" }, // 2573 - Yggdra Union - We'll Never Fight Alone (USA)
	{ "BFRP" }, // 2588 - My Animal Centre in Africa (EUR)
	{ "BFQE" }, // 2606 - Mazes of Fate (USA)
	{ "BQZP" }, // 2607 - Avatar - The Legend of Aang (EUR)
	{ "BFEP" }, // 2613 - Dogz Fashion (EUR)
	{ "BC2S" }, // 2631 - Shinchan contra los Munecos de Shock Gahn (ESP)
	{ "BXFP" }, // 2640 - Bratz - Forever Diamondz (EUR)
	{ "BEFP" }, // 2652 - Best Friends - My Horse (EUR)
	{ "BNBE" }, // 2695 - Petz Vet (USA)
	{ "BIME" }, // 2696 - Dogz 2 (USA)
	{ "BQTX" }, // 2710 - Mijn Dierenpension (EUR)
	{ "BIMP" }, // 2720 - Dogz 2 (EUR)
	{ "BIMX" }, // 2727 - Dogz 2 (EUR)
	{ "BHUE" }, // 2730 - Horsez (USA)
	{ "BQTF" }, // 2732 - Lea - Passion Veterinaire (FRA)
	{ "BJPP" }, // 2770 - Harry Potter Collection (EUR)
	{ "BEFE" }, // 2772 - Let's Ride - Friends Forever (USA)
	{ "BHBE" }, // 2774 - Best Friends - Dogs & Cats (USA)
	{ "BYUP" }, // 2781 - Yggdra Union - We'll Never Fight Alone (EUR)
	{ "ACOJ" }, // 2787 - Manga-ka Debut Monogatari (v01) (JPN)
	{ "BFDJ" }, // 2789 - Fruit Mura no Doubutsu Tachi (v02) (JPN)
	// gba scan no. 8
	{ "AYSJ" }, // 0229 - Gakkou wo Tsukurou!! Advance (JPN)
	{ "ASNJ" }, // 0260 - Sansara Naga 1x2 (JPN)
	{ "ACTX" }, // 0265 - Creatures (EUR)
	{ "ASFJ" }, // 0359 - Slot! Pro Advance - Takarabune & Ooedo Sakurafubuki 2 (JPN)
	{ "ABGJ" }, // 0404 - Sweet Cookie Pie (JPN)
	{ "ARNJ" }, // 0615 - Harukanaru Toki no Naka de (JPN)
	{ "AOPJ" }, // 0646 - Oshare Princess (JPN)
	{ "AHVJ" }, // 0664 - Nakayoshi Youchien - Sukoyaka Enji Ikusei Game (JPN)
	{ "AYCE" }, // 0758 - Phantasy Star Collection (USA)
	{ "AYCP" }, // 0877 - Phantasy Star Collection (EUR)
	{ "ATBJ" }, // 0948 - Slot! Pro 2 Advance - GoGo Juggler & New Tairyou (JPN)
	{ "A83J" }, // 1012 - Hamster Monogatari 3 GBA (JPN)
	{ "AEHJ" }, // 1014 - Puzzle & Tantei Collection (JPN)
	{ "BWDJ" }, // 1114 - Wan Nyan Doubutsu Byouin (JPN)
	{ "BKZE" }, // 1138 - Banjo-Kazooie - Grunty's Revenge (USA)
	{ "FZLJ" }, // 1369 - Famicom Mini Series 5 - Zelda no Denzetsu 1 (JPN)
	{ "BPVP" }, // 1738 - Pferd & Pony - Mein Pferdehof (EUR)
	{ "ACTY" }, // 1763 - Creatures (EUR)
	{ "BITJ" }, // 1811 - Onmyou Taisenki Zeroshik (JPN)
	{ "BOVJ" }, // 1848 - Bouken-Ou Beet - Busters Road (JPN)
	{ "BT3J" }, // 1852 - Tantei Jinguuji Saburou Shiroi Kage no Syoujyo (JPN)
	{ "BG8J" }, // 1853 - Ganbare! Dodge Fighters (JPN)
	{ "BLDS" }, // 1919 - 2 Games in 1 - Lizzie McGuire - Disney Princesas (ESP)
	{ "BLDP" }, // 1934 - 2 Games in 1 - Lizzie McGuire - Disney Princess (EUR)
	{ "B8MP" }, // 1993 - Mario Party Advance (EUR)
	{ "BYPP" }, // 2155 - Horse & Pony - Let`s Ride 2 (EUR)
	{ "B8AE" }, // 2174 - Crash Superpack - N-Tranced + Nitro Kart (USA)
	{ "BL9E" }, // 2329 - Let's Ride! Dreamer (USA)
	{ "B34E" }, // 2331 - Let's Ride! - Sunshine Stables (USA)
	{ "BQVP" }, // 2414 - Meine Tierarztpraxis (EUR)
	{ "BHUP" }, // 2480 - Horse and Pony - My Stud Farm (EUR)
	{ "BPVX" }, // 2645 - Pippa Funnell - Stable Adventures (EUR)
	{ "BYPX" }, // 2653 - Pippa Funell 2 (EUR)
	{ "BQVX" }, // 2711 - Mijn Dierenpraktijk (EUR)
	{ "BPVY" }, // 2712 - Paard & Pony - Mijn Manege (EUR)
	{ "BYPY" }, // 2713 - Paard & Pony - Paard in Galop (EUR)
	{ "B54E" }, // 2757 - Crash & Spyro Superpack - The Huge Adventure + Season of Ice (USA)
	// gba scan no. 9
	{ "BKZX" }, // 1199 - Banjo-Kazooie - Grunty's Revenge (EUR)
	{ "BKZI" }, // 1381 - Banjo Kazooie - La Vendetta di Grunty (ITA)
	{ "BAZJ" }, // 1710 - Akachan Doubutsu Sono (JPN)
	{ "BKZS" }, // 1883 - Banjo Kazooie - La Venganza de Grunty (ESP)
	// gba scan no. 11
	{ "A9BP" }, // 0925 - Medabots - Rokusho Version (EUR)
	{ "A3IJ" }, // bokura no taiyou - taiyou action rpg - kabunushi go-yuutai ban (japan) (demo)
};

#endif
