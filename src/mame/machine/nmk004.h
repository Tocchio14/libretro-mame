void NMK004_init(void);
void NMK004_irq(const address_space *space, int irq);
READ16_HANDLER( NMK004_r );
WRITE16_HANDLER( NMK004_w );
