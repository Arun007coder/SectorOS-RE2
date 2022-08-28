#include "apm.h"

int is_apm_connected = 0;
int isPM = 0;
int isAPM = 0;

int check_apm()
{
    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = 0x5300;

    bios32_call(0x15, inreg, outreg);

    if(register_h(outreg->bx) == 'P' && register_l(outreg->bx) == 'M')
    {
        printf("APM is present\n");
        printf("[APM] Version: %d.%d\n", register_h(outreg->ax), register_l(outreg->ax));
        isAPM = 1;
        kfree(inreg);
        kfree(outreg);
        return 1;
    }
    else
    {
        printf("APM is not present\n");
        isAPM = 0;
        kfree(inreg);
        kfree(outreg);
        return 0;
    }
}

int apm_connect_interface()
{
    if(isAPM == 0)
    {
        int ret = check_apm();
        if(ret == 0)
        {
            return 0;
        }
    }

    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = register_hl(0x53, 0x01);
    inreg->bx = 0x00;

    bios32_call(0x15, inreg, outreg);

    uint8_t carry = outreg->eflags & 0x0001;

    if(carry == 1)
    {
        kfree(inreg);
        kfree(outreg);
        
        printf("APM: Failed to connect to interface.\n");
        return 0;
    }

    is_apm_connected = 1;

    kfree(inreg);
    kfree(outreg);
    return 1;
}

int apm_disconnect_interface()
{
    if(isAPM == 0)
    {
        int ret = check_apm();
        if(ret == 0)
        {
            return 0;
        }
    }

    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = register_hl(0x53, 0x04);
    inreg->bx = 0x00;

    bios32_call(0x15, inreg, outreg);

    uint8_t carry = outreg->eflags & 0x0001;

    if(carry == 1)
    {
        if(register_h(outreg->ax) != 0x03)
        {
            printf("[APM] Failed to disconnect from interface.\n");
            kfree(inreg);
            kfree(outreg);
            return 0;
        }
    }

    is_apm_connected = 0;

    kfree(inreg);
    kfree(outreg);
    return 1;
}

int apm_chVersion(uint8_t major, uint8_t minor)
{
    if(isAPM == 0)
    {
        int ret = check_apm();
        if(ret == 0)
        {
            return 0;
        }
    }
    if(!is_apm_connected)
    {
        apm_connect_interface();
    }
    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = register_hl(0x53, 0x0E);
    inreg->bx = 0x00;
    inreg->cx = register_hl(major, minor);

    bios32_call(0x15, inreg, outreg);

    uint8_t carry = outreg->eflags & 0x0001;
    if(carry == 1)
    {
        if(register_h(outreg->ax) != major || register_l(outreg->ax) != minor)
        {
            printf("[APM] Failed to set version.\n");
            kfree(inreg);
            kfree(outreg);
            return 0;
        }
    }

    kfree(inreg);
    kfree(outreg);
    return 1;
}

int apm_chPM(uint8_t isPowerManagementEnabled)
{
    if(isAPM == 0)
    {
        int ret = check_apm();
        if(ret == 0)
        {
            return 0;
        }
    }

    if(!is_apm_connected)
    {
        apm_connect_interface();
    }

    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = register_hl(0x53, 0x08);
    inreg->bx = register_hl(00, 01);
    inreg->cx = register_hl(00, isPowerManagementEnabled);

    bios32_call(0x15, inreg, outreg);

    uint8_t carry = outreg->eflags & 0x0001;
    if(carry == 1)
    {
        printf("[APM] Failed to set power management.\n");
        kfree(inreg);
        kfree(outreg);
        return 0;
    }

    if(isPowerManagementEnabled)
    {
        isPM = 1;
    }
    else
    {
        isPM = 0;
    }

    kfree(inreg);
    kfree(outreg);
    return 1;
}

int apm_setPowerState(uint8_t state)
{
    if(isAPM == 0)
    {
        int ret = check_apm();
        if(ret == 0)
        {
            return 0;
        }
    }

    if(!is_apm_connected)
    {
        apm_connect_interface();
    }

    if(!isPM)
    {
        apm_chPM(1);
    }

    register16_t* inreg = kmalloc(sizeof(register16_t));
    register16_t* outreg = kmalloc(sizeof(register16_t));
    memset(inreg, 0, sizeof(register16_t));
    memset(outreg, 0, sizeof(register16_t));

    inreg->ax = register_hl(0x53, 0x07);
    inreg->bx = register_hl(00, 01);
    inreg->cx = state;

    bios32_call(0x15, inreg, outreg);

    uint8_t carry = outreg->eflags & 0x0001;
    if(carry == 1)
    {
        printf("[APM] Failed to set power state.\n");
        kfree(inreg);
        kfree(outreg);
        return 0;
    }
    kfree(inreg);
    kfree(outreg);
    return 1;
}

void apm_standby()
{
    if(!check_apm())
    {
        return;
    }
    if(!is_apm_connected)
    {
        apm_connect_interface();
    }
    if(!isPM)
    {
        apm_chPM(1);
    }
    apm_setPowerState(0x01);
}

void apm_suspend()
{
    if(!check_apm())
    {
        return;
    }
    if(!is_apm_connected)
    {
        apm_connect_interface();
    }
    else
    {
        apm_disconnect_interface();
        apm_connect_interface();
    }
    if(!isPM)
    {
        apm_chPM(1);
    }
    apm_setPowerState(0x02);
}

void apm_shutdown()
{
    if(!check_apm())
    {
        return;
    }
    if(!is_apm_connected)
    {
        apm_connect_interface();
    }
    if(!isPM)
    {
        apm_chPM(1);
    }
    apm_setPowerState(0x03);
}

void apm_restart()
{
    if(!check_apm())
    {
        return;
    }
    if(!is_apm_connected)
    {
        apm_connect_interface();
    }
    if(!isPM)
    {
        apm_chPM(1);
    }
    apm_setPowerState(0x03);
}

void init_apm()
{
    if(!check_apm())
    {
        return;
    }
    apm_connect_interface();
    apm_chVersion(0x01, 0x01);
    apm_chPM(1);
    apm_standby();
}