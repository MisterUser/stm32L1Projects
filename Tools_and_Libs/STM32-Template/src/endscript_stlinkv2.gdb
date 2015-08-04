call DAC_Cmd(0x00,0)
shell sleep 1
call DAC_Cmd(0x10,0)
shell sleep 1
call DAC_DMACmd(0x10,0)
shell sleep 1
call DAC_DMACmd(0x00,0)
shell sleep 1
q
y


