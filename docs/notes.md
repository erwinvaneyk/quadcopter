Protocol:
---------------
- header
- mode
- roll/pitch/jaw
- 2 other
- checksum

Communication:
----------------
- information over periodicity
- levels UP-[-k, k]


Flow:
----------------
to qr:
- joystick output -> J
- persistent keyboard output -> K
- send packet with values V = J + K to QR on change or every x ms (to prevent rogue qr else go into panic->safe mode)
on qr:
- feedback loops to reach V
- expect ping/packet every x ms
- data collection in memory
From qr:
- real-time state