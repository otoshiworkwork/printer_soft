echo on
echo .
echo .
echo .
echo .

ics_client.exe --dev_open_serial            127.0.0.1 42100 ics0a000
ics_client.exe --sys_config_buffer          127.0.0.1 42100 --config 2 2000000
ics_client.exe --dev_jump_to_address        127.0.0.1 42100 0x0
ics_client.exe --dev_software_version       127.0.0.1 42100
ics_client.exe --sys_client_wr_rd_buffer    127.0.0.1 42100 --wr_s 0 0 "..\xtpl_app\xtpl_printer\base\001\firmware\prt_core.ifh"
ics_client.exe --dev_config_hardware        127.0.0.1 42100 --pd 0 0 1484513

ics_client.exe --sys_client_wr_rd_buffer    127.0.0.1 42100 --wr_s 0 0 "..\xtpl_app\xtpl_printer\base\001\firmware\prt_soft.ifs"
ics_client.exe --dev_wr_rd_buffer           127.0.0.1 42100 --wr_s 0 0 0x40000000 13270
ics_client.exe --dev_jump_to_address        127.0.0.1 42100 0x40000000
ics_client.exe --dev_software_version       127.0.0.1 42100

ics_client.exe --dev_xtpl_printer           127.0.0.1 42100 --reset
ics_client.exe --dev_xtpl_printer           127.0.0.1 42100 --info


ics_client.exe --dev_close_serial           127.0.0.1 42100 ics0a000
pause