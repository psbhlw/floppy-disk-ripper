
# tools
MKDIR=mkdir
CP=cp
RM=rm -f
RMDIR=rm -rf

RELEASE_DIR=release


all:
	cd fdr; make all; cd ..
	cd fdrc; make all; cd ..

	$(RMDIR) $(RELEASE_DIR)
	$(MKDIR) $(RELEASE_DIR)
	$(MKDIR) $(RELEASE_DIR)/fdr
	$(MKDIR) $(RELEASE_DIR)/fdr/firmware
	$(MKDIR) $(RELEASE_DIR)/fdrc

	$(CP) fdrc/README.md $(RELEASE_DIR)/fdrc/README.md
	$(CP) fdrc/fdrc.exe $(RELEASE_DIR)/fdrc/fdrc.exe

	$(CP) fdr/README.md $(RELEASE_DIR)/fdr/README.md
	$(CP) fdr/fdr.trd $(RELEASE_DIR)/fdr/fdr.trd
	$(CP) fdr/fdr.spg $(RELEASE_DIR)/fdr/fdr.spg

	$(CP) fdr/firmware/README.md $(RELEASE_DIR)/fdr/firmware/README.md
	$(CP) fdr/firmware/ts-bios.rom $(RELEASE_DIR)/fdr/firmware/ts-bios.rom
	$(CP) fdr/firmware/zxevo_fw.bin $(RELEASE_DIR)/fdr/firmware/zxevo_fw.bin
	$(CP) fdr/firmware/zxevo_fw_vdac.bin $(RELEASE_DIR)/fdr/firmware/zxevo_fw_vdac.bin
	@echo OK.

clean:
	cd fdr; make clean; cd ..
	cd fdrc; make clean; cd ..
	$(RMDIR) $(RELEASE_DIR)

