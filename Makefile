# Makefile for kernel module "hellomoto" for testing purposes.
# Written by EXL, 27-Sep-2016.
# Edited by EXL, 27-Oct-2021.

include $(BOOTSTRAP)

ifneq ($(HW_ARCH),i686)

KERNEL_MAKE = cd $(STD_DIRPATH)/linux_build && $(MAKE) ARCH=$(ARCH) M=$(PWD)/driver INSTALL_MOD_PATH=$(BUILDTOP) STD_INCPATH=$(STD_INCPATH)

.PHONY: impl api_build distclean

api_build: $(PROPFILES)

impl:
	$(KERNEL_MAKE) modules
	$(KERNEL_MAKE) modules_install

distclean: clean
	$(KERNEL_MAKE) clean

endif # ifneq ($(HW_ARCH),i686)
