


static uint32_t gpio_read(uint32_t pin){

    uint32_t reg = pin/32;
    uint32_t bit = pin%32;
    return (gpio_regs_virt->gplev[reg]&(1<<bit)) != 0;
}