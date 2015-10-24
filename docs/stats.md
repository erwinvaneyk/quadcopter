# stats

periodic() runs:
no mode: 7-8 us
yaw:  218-220 us
full 660 us

while(alive) loop: 36 us
	get_packet is 13 us


process_packet()
	safe mode: 80 us
	manual mode: 245 us
	yaw control: 95 us
	full control: 125 us