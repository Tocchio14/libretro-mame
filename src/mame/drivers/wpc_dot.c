#include "emu.h"
#include "cpu/m6809/m6809.h"

extern const char layout_pinball[];

class wpc_dot_state : public driver_device
{
public:
	wpc_dot_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }
};


static ADDRESS_MAP_START( wpc_dot_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_UNMAP_HIGH
ADDRESS_MAP_END

static INPUT_PORTS_START( wpc_dot )
INPUT_PORTS_END

static MACHINE_RESET( wpc_dot )
{
}

static DRIVER_INIT( wpc_dot )
{
}

static MACHINE_CONFIG_START( wpc_dot, wpc_dot_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6809, 2000000)
	MCFG_CPU_PROGRAM_MAP(wpc_dot_map)

	MCFG_MACHINE_RESET( wpc_dot )

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_pinball)
MACHINE_CONFIG_END

/*-----------------
/ Gilligan's Island
/------------------*/
ROM_START(gi_l9)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("gilli_l9.rom", 0x00000, 0x40000, CRC(af07a757) SHA1(29c4f4ac2aed5b36e1d22490d656b1c4acba7f4c))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("gi_u18.l2", 0x000000, 0x20000, CRC(ea53e196) SHA1(5dcf3f44d2d658f6a7b130fa9e48d3cd616b4300))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u15.l2", 0x080000, 0x20000, CRC(f8241dc9) SHA1(118a65555b9fff6f94e5e8324ed97d6ddec3d82b))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u14.l2", 0x100000, 0x20000, CRC(0e7a4140) SHA1(c6408794120b5e45a48b35c380333879e1f0be78))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(gi_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("gi_l3.u6", 0x00000, 0x40000, CRC(d4e26140) SHA1(c2a9f02217071768ec1ef9169d2922c0e1585bee))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("gi_u18.l2", 0x000000, 0x20000, CRC(ea53e196) SHA1(5dcf3f44d2d658f6a7b130fa9e48d3cd616b4300))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u15.l2", 0x080000, 0x20000, CRC(f8241dc9) SHA1(118a65555b9fff6f94e5e8324ed97d6ddec3d82b))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u14.l2", 0x100000, 0x20000, CRC(0e7a4140) SHA1(c6408794120b5e45a48b35c380333879e1f0be78))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(gi_l4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("gi_l4.u6", 0x00000, 0x40000, CRC(2313986d) SHA1(6e0dd293b869ea986ac9cb65b020463a86d955d4))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("gi_u18.l2", 0x000000, 0x20000, CRC(ea53e196) SHA1(5dcf3f44d2d658f6a7b130fa9e48d3cd616b4300))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u15.l2", 0x080000, 0x20000, CRC(f8241dc9) SHA1(118a65555b9fff6f94e5e8324ed97d6ddec3d82b))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u14.l2", 0x100000, 0x20000, CRC(0e7a4140) SHA1(c6408794120b5e45a48b35c380333879e1f0be78))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(gi_l6)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("gi_l6.u6", 0x00000, 0x40000, CRC(7b73eef2) SHA1(fade23019600d84492d5a0fc6f4f5be52ec319be))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1",0)
	ROM_LOAD("gi_u18.l2", 0x000000, 0x20000, CRC(ea53e196) SHA1(5dcf3f44d2d658f6a7b130fa9e48d3cd616b4300))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u15.l2", 0x080000, 0x20000, CRC(f8241dc9) SHA1(118a65555b9fff6f94e5e8324ed97d6ddec3d82b))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("gi_u14.l2", 0x100000, 0x20000, CRC(0e7a4140) SHA1(c6408794120b5e45a48b35c380333879e1f0be78))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

/*-----------------
/ Hot Shot
/------------------*/
ROM_START(hshot_p8)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "user2", 0)
	ROM_LOAD("hshot_p8.u6", 0x00000, 0x80000, CRC(26dd6bb2) SHA1(45674885052838b6bd6b3ed0a276a4d9323290c5))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("hshot_l1.u18", 0x000000, 0x20000, CRC(a0e5beba) SHA1(c54a22527d861df54891308752ebdec5829deceb))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("hshot_l1.u14", 0x100000, 0x80000, CRC(a3ccf557) SHA1(a8e518ea115cd1963544273c45d9ae9a6cab5e1f))
ROM_END

/*-----------------
/  Hurricane
/------------------*/
ROM_START(hurr_l2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("hurcnl_2.rom", 0x00000, 0x40000, CRC(fda6155f) SHA1(0088155a2582524d8720d71cd3ff82e8733ef434))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("u18.pp", 0x000000, 0x20000, CRC(63944b37) SHA1(045f8046ba5bf1c88b65a80737e2d3d017271c04))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("u15.pp", 0x080000, 0x20000, CRC(93d02c62) SHA1(203cd6b933822d6d3f70c63e051237e3587568f1))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("u14.pp", 0x100000, 0x20000, CRC(51c82899) SHA1(aa6c3d9e7efa3708727b06fb3372638d5245a510))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

/*-----------------
/  Party Zone
/------------------*/
ROM_START(pz_f4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("pzonef_4.rom", 0x00000, 0x40000, CRC(041d7d15) SHA1(d40e7010caa3bc664dc985c748309fe84ae17dac))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("pz_u18.l1", 0x000000, 0x20000, CRC(b7fbba98) SHA1(6533a1474dd335419331d37d4a4447951171412b))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u15.l1", 0x080000, 0x20000, CRC(168bcc52) SHA1(0bae89278cd24950b2e247bba48eaa636f7b566c))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u14.l1", 0x100000, 0x40000, CRC(4d8897ce) SHA1(7a4ac9e849dae93078ddd60adbd34f3930e4cd46))
	ROM_RELOAD( 0x100000 + 0x40000, 0x40000)
ROM_END
ROM_START(pz_l1)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("u6-l1.rom", 0x00000, 0x40000, CRC(48023444) SHA1(0c14f5902c6c0b3466fb4265a2e1fc6a1050f8d7))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("pz_u18.l1", 0x000000, 0x20000, CRC(b7fbba98) SHA1(6533a1474dd335419331d37d4a4447951171412b))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u15.l1", 0x080000, 0x20000, CRC(168bcc52) SHA1(0bae89278cd24950b2e247bba48eaa636f7b566c))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u14.l1", 0x100000, 0x40000, CRC(4d8897ce) SHA1(7a4ac9e849dae93078ddd60adbd34f3930e4cd46))
	ROM_RELOAD( 0x100000 + 0x40000, 0x40000)
ROM_END
ROM_START(pz_l2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("pz_u6.l2", 0x00000, 0x40000, CRC(200455a9) SHA1(d0f9a2227c67ddc73111a120a6a19dc5ac218baa))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("pz_u18.l1", 0x000000, 0x20000, CRC(b7fbba98) SHA1(6533a1474dd335419331d37d4a4447951171412b))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u15.l1", 0x080000, 0x20000, CRC(168bcc52) SHA1(0bae89278cd24950b2e247bba48eaa636f7b566c))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u14.l1", 0x100000, 0x40000, CRC(4d8897ce) SHA1(7a4ac9e849dae93078ddd60adbd34f3930e4cd46))
	ROM_RELOAD( 0x100000 + 0x40000, 0x40000)
ROM_END
ROM_START(pz_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("pzonel_3.rom", 0x00000, 0x40000, CRC(156f158f) SHA1(73a31deee6b299e5f5479b43210a822009e116d0))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("pz_u18.l1", 0x000000, 0x20000, CRC(b7fbba98) SHA1(6533a1474dd335419331d37d4a4447951171412b))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u15.l1", 0x080000, 0x20000, CRC(168bcc52) SHA1(0bae89278cd24950b2e247bba48eaa636f7b566c))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("pz_u14.l1", 0x100000, 0x40000, CRC(4d8897ce) SHA1(7a4ac9e849dae93078ddd60adbd34f3930e4cd46))
	ROM_RELOAD( 0x100000 + 0x40000, 0x40000)
ROM_END

/*--------------------
/ Slugfest baseball
/--------------------*/
ROM_START(sf_l1)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("sf_u6.l1", 0x00000, 0x40000, CRC(ada93967) SHA1(90094d207dafdacfaf7d259c6cc3dc2b552c8588))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("sf_u18.l1", 0x000000, 0x20000, CRC(78092c83) SHA1(7c922dfd8be4bb5e23d4c86b6eb18a29cc034338))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("sf_u15.l1", 0x080000, 0x20000, CRC(adcaeaa1) SHA1(27aa9526c628634c395161f4966d9943bdf1f120))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("sf_u14.l1", 0x100000, 0x20000, CRC(b830b419) SHA1(c59980a78d8cb1d979de21dfc5ad3d671d8486e7))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

/*-----------------
/  Terminator 2: Judgement Day
/------------------*/
ROM_START(t2_l8)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "user2", 0)
	ROM_LOAD("t2_l8.rom", 0x00000, 0x80000, CRC(c00e52e9) SHA1(830c1a7eabf3c8e4fa6242421587b398e21449e8))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u14.l3", 0x100000, 0x20000, CRC(9addc9dc) SHA1(847bb027f6b9167cbbaa13f1af50d61e0c69f01f))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(t2_l6)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("t2_l6.u6", 0x00000, 0x40000, CRC(0d714b35) SHA1(050fd2b3afbecbbd03d58ab206ff6cfac8780a2b))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u14.l3", 0x100000, 0x20000, CRC(9addc9dc) SHA1(847bb027f6b9167cbbaa13f1af50d61e0c69f01f))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(t2_p2f)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("u6-nasty.rom", 0x00000, 0x40000, CRC(add685a4) SHA1(d1ee7eb620864b017495e52ea8fe8db18508c3eb))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("u14-nsty.rom", 0x100000, 0x20000, CRC(b4d64152) SHA1(03a828cef8b067d4da058fd3a1e972265a72f10a))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(t2_l4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("u6-l4.rom", 0x00000, 0x40000, CRC(4d8b894d) SHA1(218b3628e7709c329c2030a5391ded60301aad26))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u14.l3", 0x100000, 0x20000, CRC(9addc9dc) SHA1(847bb027f6b9167cbbaa13f1af50d61e0c69f01f))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(t2_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("u6-l3.rom", 0x00000, 0x40000, CRC(7520398a) SHA1(862881481dc7b617f3b14bbb35d48cffb0ce950e))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u14.l3", 0x100000, 0x20000, CRC(9addc9dc) SHA1(847bb027f6b9167cbbaa13f1af50d61e0c69f01f))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END
ROM_START(t2_l2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "user2", 0)
	ROM_LOAD("u6-l2.rom", 0x00000, 0x40000, CRC(efe49c18) SHA1(9f91081c384990eac6e3c57f318a2639626929f9))
	ROM_REGION(0x10000, "cpu2", ROMREGION_ERASEFF)
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("t2_u18.l3", 0x000000, 0x20000, CRC(2280bdd0) SHA1(ea94265cb8291ee427e0a2119d901ba1eb50d8ee))
	ROM_RELOAD( 0x000000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x000000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u15.l3", 0x080000, 0x20000, CRC(dad03ad1) SHA1(7c200f9a6564d751e5aa9b1ba84363b221502770))
	ROM_RELOAD( 0x080000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x080000 + 0x60000, 0x20000)
	ROM_LOAD("t2_u14.l3", 0x100000, 0x20000, CRC(9addc9dc) SHA1(847bb027f6b9167cbbaa13f1af50d61e0c69f01f))
	ROM_RELOAD( 0x100000 + 0x20000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x40000, 0x20000)
	ROM_RELOAD( 0x100000 + 0x60000, 0x20000)
ROM_END

/*--------------
/ Test Fixture DMD generation
/---------------*/
ROM_START(tfdmd_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x2000, "user1", ROMREGION_ERASEFF)
	ROM_REGION(0x20000, "user2", 0)
	ROM_LOAD("u6_l3.rom", 0x00000, 0x20000, CRC(bd43e28c) SHA1(df0a64a9fddbc59e3edde56ae12b68f76e44ba2e))
ROM_END


GAME(1991,	tfdmd_l3,	0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"WPC Test Fixture: DMD (L-3)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	gi_l9,		0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Gilligan's Island (L-9)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	gi_l3,		gi_l9,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Gilligan's Island (L-3)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	gi_l4,		gi_l9,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Gilligan's Island (L-4)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	gi_l6,		gi_l9,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Gilligan's Island (L-6)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1992,	hshot_p8,	0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Midway",				"Hot Shot Basketball (P-8)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	hurr_l2,	0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Hurricane (L-2)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	pz_f4,		0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Party Zone (F-4)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	pz_l1,		pz_f4,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Party Zone (L-1)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	pz_l2,		pz_f4,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Party Zone (L-2)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	pz_l3,		pz_f4,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Bally",				"Party Zone (L-3)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	sf_l1,		0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Slugfest (L-1)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_l8,		0,		wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (L-8)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_l6,		t2_l8,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (L-6)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_p2f,		t2_l8,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (P-2F) Profanity",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_l4,		t2_l8,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (L-4)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_l3,		t2_l8,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (L-3)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
GAME(1991,	t2_l2,		t2_l8,	wpc_dot,	wpc_dot,	wpc_dot,	ROT0,	"Williams",				"Terminator 2: Judgement Day (L-2)",				GAME_NOT_WORKING | GAME_NO_SOUND | GAME_MECHANICAL)
