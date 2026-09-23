Flash with:
```
esptool.py --chip esp32s3 --port /dev/cu.usbmodemXXXX write_flash 0x0 wifi-signal-monitor-VERSION.bin
```

Insert a FAT32-formatted microSD card before powering on for RSSI logging + history.
