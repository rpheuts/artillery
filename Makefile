all:
	$(MAKE) -C src/bootloader/
	$(MAKE) -C src/

bootloader:
	$(MAKE) -C src/bootloader/

clean:
	$(MAKE) -C src/bootloader/ clean
	$(MAKE) -C src/ clean
	
install: $(TARGET)
	@echo "Installing to PSP: /mnt/psp"
	@echo "Mounting psp..."
	@mount /dev/sdb1 /mnt/psp
	@echo "Cleaning psp..."
	@rm -rf /mnt/psp/psp/game150/*pspfw*
	@echo "Building kxploited version..."
	@make -sC src/bootloader/ kxploit